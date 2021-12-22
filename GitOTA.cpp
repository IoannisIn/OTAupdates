#include "GitOTA.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "cert.h"
#include <Arduino.h>

extern String FirmwareVer = {
  "8.0"
};

extern const char * ssid = "Arkasa";
extern const char * password = "@@@@@@@@";

extern unsigned long previousMillis = 0; // will store last time LED was updated
extern unsigned long previousMillis2 = 0;
extern const long interval = 1800000;
extern const long mini_interval = 1000;

#define URL_fw_Version "https://raw.githubusercontent.com/IoannisIn/OTAupdates/main/version.txt"
#define URL_fw_Bin "https://raw.githubusercontent.com/IoannisIn/OTAupdates/main/firmware.bin"

void repeatedCall() {
  static int num = 0;
  unsigned long currentMillis = millis();
  if ((currentMillis - previousMillis) >= interval) {
    // Check for new firmware every 1800000ms or 30 minutes
    previousMillis = currentMillis;
    if (FirmwareVersionCheck()) {
      firmwareUpdate();
    }
  }
  if ((currentMillis - previousMillis2) >= mini_interval) {
    previousMillis2 = currentMillis;
    // Show current firmware version and check if wifi is connected
    Serial.print(" Active fw version:");
    Serial.println(FirmwareVer);
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("wifi connected");
    }
    //else
    //{
    //  connect_wifi();
    //}
  }
}

void connect_wifi() {
  Serial.print("Active firmware version:");
  Serial.println(FirmwareVer);
  //Serial.println("Waiting for WiFi");
  //WiFi.begin(ssid, password);
  //while (WiFi.status() != WL_CONNECTED) {
  //  delay(500);
  //  Serial.print(".");
  //}

  //Serial.println("");
  //Serial.println("WiFi connected");
  //Serial.println("IP address: ");
  //Serial.println(WiFi.localIP());
}


void firmwareUpdate(void) {
  WiFiClientSecure client;
  client.setCACert(rootCACertificate);
  
  t_httpUpdate_return ret = httpUpdate.update(client, URL_fw_Bin);

  switch (ret) {
    case HTTP_UPDATE_FAILED:
      Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
      break;

    case HTTP_UPDATE_NO_UPDATES:
      Serial.println("HTTP_UPDATE_NO_UPDATES");
      break;

    case HTTP_UPDATE_OK:
      Serial.println("HTTP_UPDATE_OK");
      break;
  }
}
int FirmwareVersionCheck(void) {
  String payload;
  int httpCode;
  String fwurl = "";
  fwurl += URL_fw_Version;
  fwurl += "?";
  fwurl += String(rand());
  Serial.println(fwurl);
  WiFiClientSecure * client = new WiFiClientSecure;

  if (client)
  {
    client -> setCACert(rootCACertificate);

    // Add a scoping block for HTTPClient https to make sure it is destroyed before WiFiClientSecure *client is
    HTTPClient https;

    if (https.begin( * client, fwurl))
    { // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      delay(100);
      httpCode = https.GET();
      delay(100);
      if (httpCode == HTTP_CODE_OK) // if version received
      {
        payload = https.getString(); // save received version
      } else {
        Serial.print("error in downloading version file:");
        Serial.println(httpCode);
      }
      https.end();
    }
    delete client;
  }

  if (httpCode == HTTP_CODE_OK) // if version received
  {
    payload.trim();
    if (payload.equals(FirmwareVer)) {
      Serial.printf("\nDevice already on latest firmware version: %s\n", FirmwareVer);
      return 0;
    }
    else if (payload < FirmwareVer){
      
      Serial.printf("\nIn server there is a previous firmware version: %s\n", FirmwareVer);      
      return 0;
      
    } 
    else
    {
      Serial.println(payload);
      Serial.println("New firmware detected");
      Serial.println("Firmware update Starting..");
      firmwareUpdate();
      return 1;
    }
  }
  return 0;
}
