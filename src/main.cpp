#include <ESPmDNS.h>
#include <WiFi.h>
#include "Arduino.h"
#include "Web.h"
#include "WebSocket.h"
#include "config.h"

Web web;
WebSocket webSocket;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.setHostname(hostName);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(hostName);
  WiFi.begin(ssid, password);
  delay(1000);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }

  Serial.println(WiFi.localIP());

  //   MDNS.addService("http", "tcp", 80);

  web.setup();
  webSocket.setup();
}

void loop() {}
