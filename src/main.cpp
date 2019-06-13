#include <ESPmDNS.h>
#include <Thread.h>
#include <ThreadController.h>
#include <WiFi.h>
#include "Arduino.h"
#include "Audio.h"
#include "NFC.h"
#include "Network.h"
#include "Web.h"
#include "WebSocket.h"
#include "config.h"

Web web;
WebSocket webSocket;

Network network;

Audio audio;

NFC nfc;

ThreadController manager = ThreadController();
Thread *audioThread = new Thread();
Thread *nfcThread = new Thread();

void taskHandler(char *data);

/**
 * Audio函数
 */
void audioPlay();
/**
 * NFC函数
 */
void nfcHandler();

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
  audio.setup();

  nfc.setup();

  audioThread->setInterval(0);
  audioThread->onRun(audioPlay);

  nfcThread->setInterval(10);
  nfcThread->onRun(nfcHandler);

  manager.add(nfcThread);
}

void loop() { manager.run(); }

/**
 * 任务回调函数
 */
void taskHandler(char *data) {
  Serial.print("REV:");
  Serial.println(data);
  delay(1000);

  audio.select("/1.mp3");
  // manager.add(audioThread);

  DynamicJsonDocument doc(256);
  doc["state"] = "OJBK";
  webSocket.sendMsg(doc);
}

void audioPlay() {
  if (audio.isFinish()) {
    manager.remove(audioThread);
  } else {
    audio.loop();
  }
}

void nfcHandler() {
  if (nfc.readSuccess()) {
    Serial.println(nfc.getResult());
    manager.remove(nfcThread);
  } else {
    nfc.loop();
  }
}