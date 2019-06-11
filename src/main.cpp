#include <ESPmDNS.h>
#include <WiFi.h>
#include "Arduino.h"
#include "Web.h"
#include "WebSocket.h"
#include "config.h"
#include "Network.h"

Web web;
WebSocket webSocket;

Network network;

void taskHandler(char *data);

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.setHostname(network.getAPName());
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(network.getAPName());
  network.startScanWifi();
  network.getWifiScanData();
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

  webSocket.onMsg(taskHandler);

  web.setup();
  webSocket.setup();
}

void loop() {}

/**
 * 任务回调函数
 */
void taskHandler(char *data) {
  Serial.print("REV:");
  Serial.println(data);
  delay(1000);

  DynamicJsonDocument doc(256);
  doc["state"] = "OJBK";
  webSocket.sendMsg(doc);
}
