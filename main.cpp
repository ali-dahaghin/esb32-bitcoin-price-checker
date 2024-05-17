#include <Arduino.h>
#include <WiFi.h>
#include <wifi_service.h>
#include <HttpClient.h>
#include <JsonParser.h>

#define WIFI_SSID "0xA10"
#define WIFI_PASSWORD "password"
const char* HTTP_SERVER_ROOT_CA = "-----BEGIN CERTIFICATE-----\n"
"MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
"RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
"VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
"DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
"ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
"VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
"mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
"IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
"mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
"XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
"dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
"jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
"BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
"DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
"9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
"jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
"Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
"ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
"R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
"-----END CERTIFICATE-----";

WiFiAccessPointInfoDto* wifiInfo;
WiFiService* wifiService;
JsonParser<500> parser;

void setup() {
  Serial.begin(19200);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(3, INPUT);
  
  std::string ssid(WIFI_SSID);
  std::string password(WIFI_PASSWORD);
  wifiInfo = new WiFiAccessPointInfoDto(ssid, password);
  wifiService = new WiFiService(*wifiInfo, true);

  Serial.println("\n\nstarted");

  Serial.println(HTTP_SERVER_ROOT_CA);
}

void loop() {
  int wasHigh = digitalRead(3);
  if (wasHigh) {
    Serial.println("it was higher last time");
  }
  

  bool isConnected = wifiService->check();
  if(isConnected) {
    HTTPClient http;
    http.begin("https://api.kucoin.com/api/v1/market/stats?symbol=BTC-USDT", HTTP_SERVER_ROOT_CA);
    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      char json[500];
      strcpy(json, payload.c_str());
      JsonHashTable hashTable = parser.parseHashTable(json);
      if (hashTable.success()) {
        char* last = hashTable.getHashTable("data").getString("last");
        double lastDouble = atof(last);
        Serial.println(lastDouble);

        if (lastDouble > 22000) {
          digitalWrite(13, HIGH);
        } else {
          digitalWrite(13, LOW);
        }
        
      } else {
        Serial.println("Error on parse response");
      }
    } else {
      Serial.println("Error on HTTP request");
    }
    http.end();
  }

  delay(isConnected ? 5000 : 333);
}