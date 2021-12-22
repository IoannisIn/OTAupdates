#include <WiFiManager.h>
#include <strings_en.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>
#include "GitOTA.h"
#include <EasyButton.h>
#include <Arduino.h>
#include <WebServer.h>

#define OTA_button 0  //This buttons needs to be pressed for 3 seconds

WebServer server(80);

WiFiManager wifiManager;

int press_duration = 3000;

EasyButton button(OTA_button);

void handle_root();

void setup() {
  Serial.begin(115200);
  wifiManager.autoConnect();
  button.begin();
  button.onPressedFor(press_duration, onPressesFor3s);
  connect_wifi();
  server.on("/", handle_root);
  server.begin();
  Serial.println("HTTP server started..");
  delay(100);
}

void loop() {
  server.handleClient();  
  button.read();
  repeatedCall();
}

void onPressesFor3s() {
  Serial.println("Pressed for 3s");
  FirmwareVersionCheck();
}

String HTML = "<!DOCTYPE html>\
<html>\
  <body>\
    <h1>Welcome</h1>\
        <p>John</p>\
        <p>&#128522;</p>\
  </body>\
</html>";

void handle_root(){
server.send(200, "text/html", HTML);
}