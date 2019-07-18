#include "Web.h"
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <SPIFFS.h>
#include <SPIFFSEditor.h>
#include "Network.h"
#include "config.h"

AsyncWebServer server(80);

extern Network network;
extern Config config;

void Web::setup() {
  // SD卡初始化
  boolean sd_success = false;
  do {
    sd_success = SD.begin();
    Serial.println("init sdcard");
    // delay(500);
  } while (!sd_success);

  // SD卡信息
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD_MMC Card Size: %lluMB ", cardSize);
  Serial.printf("Total space: %lluMB ", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB ", SD.usedBytes() / (1024 * 1024));
  Serial.println("");

  SPIFFS.begin();

  //允许远程修改SD卡文件
  server.addHandler(new SPIFFSEditor(SD, http_username, http_password));
  // 404
  server.onNotFound([](AsyncWebServerRequest *request) { request->send(404); });
  //处理配置
  server.on("/api/wifi", HTTP_GET, [](AsyncWebServerRequest *request) {
    String message;
    DynamicJsonDocument index(2048);
    DynamicJsonDocument doc(1024);
    network.startScanWifi();
    doc = network.getWifiScanData();
    index["ip"] = network.getIp();
    index["ssids"] = doc;
    String data = "";
    serializeJson(index, data);
    request->send(200, "application/json", data);
  });
  server.on("/api/wifi", HTTP_POST, [](AsyncWebServerRequest *request) {
    String conn_ssid = "";
    String conn_password = "";
    if (request->hasParam("ssid", true)) {
      conn_ssid = request->getParam("ssid", true)->value();
    }
    if (request->hasParam("password", true)) {
      conn_password = request->getParam("password", true)->value();
    }
    Serial.println(conn_ssid);
    Serial.println(conn_password);
    if (WiFi.isConnected()) {
      WiFi.disconnect();
      delay(100);
    }
    WiFi.begin((char *)conn_ssid.c_str(), (char *)conn_password.c_str());
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      request->send(200, "application/json", "fail");
    } else {
      config.set_ssid(conn_ssid);
      config.set_password(conn_password);
      request->send(200, "application/json", "success");
    }
  });

  //配置路径
  server.serveStatic("/", SPIFFS, "/")
      .setDefaultFile("index.html")
      .setCacheControl("max-age=31536000");

  server.begin();
}
