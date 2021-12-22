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

#include "index.h"  //Web page header file

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
  server.on("/", handleRoot);      //This is display page
  server.on("/readADC", handleADC);//To get update of ADC Value only
 
  server.begin();                  //Start server
  Serial.println("HTTP server started");
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

void handleRoot() {
 String s = MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
 
void handleADC() {
 int a = analogRead(34);
 String adcValue = String(a);
 
 server.send(200, "text/plane", adcValue); //Send ADC value only to client ajax request
}