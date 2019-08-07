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
extern void initParams();

Web *context = NULL;

Web::Web() { context = this; }

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
    DynamicJsonDocument index(2048);
    DynamicJsonDocument doc(1024);
    network.startScanWifi();
    doc = network.getWifiScanData();
    index["ip"] = network.getIp();
    index["ssid"] = "";
    index["password"] = "";
    index["ssids"] = doc;
    if (WiFi.isConnected()) {
      index["ssid"] = config.get_ssid();
      index["password"] = config.get_password();
    }
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
    DynamicJsonDocument connState(256);
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      connState["connect"] = false;
    } else {
      config.set_ssid(conn_ssid);
      config.set_password(conn_password);
      connState["connect"] = true;
    }
    String data = "";
    serializeJson(connState, data);
    request->send(200, "application/json", data);
  });
  server.on("/api/setting", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument setting(1024);
    setting["steps_per_turn"] = config.get_step_per_turn();
    setting["circumference_mm"] = config.get_circumference_mm();
    setting["wheel_distance"] = config.get_wheel_distance();
    String data = "";
    serializeJson(setting, data);
    request->send(200, "application/json", data);
  });
  server.on("/api/setting", HTTP_POST, [](AsyncWebServerRequest *request) {
    String steps_per_turn = "";
    String circumference_mm = "";
    String wheel_distance = "";
    if (request->hasParam("steps_per_turn", true)) {
      steps_per_turn = request->getParam("steps_per_turn", true)->value();
    }
    if (request->hasParam("circumference_mm", true)) {
      circumference_mm = request->getParam("circumference_mm", true)->value();
    }
    if (request->hasParam("wheel_distance", true)) {
      wheel_distance = request->getParam("wheel_distance", true)->value();
    }
    Serial.println(steps_per_turn);
    Serial.println(circumference_mm);
    Serial.println(wheel_distance);
    if (steps_per_turn.toFloat() != 0 && !steps_per_turn.equals("")) {
      config.set_step_per_turn(steps_per_turn);
    }
    if (circumference_mm.toFloat() != 0 && !circumference_mm.equals("")) {
      config.set_circumference_mm(circumference_mm);
    }
    if (wheel_distance.toFloat() != 0 && !wheel_distance.equals("")) {
      config.set_wheel_distance(wheel_distance);
    }
    initParams();
    DynamicJsonDocument setState(256);
    setState["state"] = true;
    String data = "";
    serializeJson(setState, data);
    request->send(200, "application/json", data);
  });
  server.on("/api/version", HTTP_GET, [](AsyncWebServerRequest *request) {
    DynamicJsonDocument version(1024);
    DynamicJsonDocument local(512);
    DynamicJsonDocument remote(512);
    local["ui"] = context->getVersionUI();
    local["firmware"] = context->getVersionFirmware();
    version["local"] = local;
    String tempStr = context->getVersionFromServer();
    if (tempStr != "") {
      DynamicJsonDocument temp(512);
      deserializeJson(temp, tempStr);
      version["remote"] = temp;
    }
    String result = "";
    serializeJson(version, result);
    request->send(200, "application/json", result);
  });
  server.on("/api/version", HTTP_POST, [](AsyncWebServerRequest *request) {
    String mode = "";
    if (request->hasParam("mode", true)) {
      mode = request->getParam("mode", true)->value();
    }
    Serial.println(mode);
    int code = -1;
    String msg = "Parameter error";
    DynamicJsonDocument temp(512);
    temp["code"] = code;
    temp["msg"] = msg;
    if (mode != "") {
      context->needUpdate = mode;
      temp["code"] = 0;
      temp["msg"] = "Start update ";
    }
    String data = "";
    serializeJson(temp, data);
    request->send(200, "application/json", data);
  });
  //配置路径
  server.serveStatic("/", SPIFFS, "/")
      .setDefaultFile("index.html")
      .setCacheControl("max-age=31536000");

  server.begin();
}

String Web::getVersionUI() {
  File file = SPIFFS.open("/version.json");
  if (!file || file.isDirectory()) {
    Serial.println("getVersionUI failed");
  }
  String temp = "";
  while (file.available()) {
    temp += file.readString();
  }
  DynamicJsonDocument versionObj(256);
  deserializeJson(versionObj, temp);
  String result = versionObj["version"];
  return result;
}

String Web::getVersionFirmware() { return VERSION; }

String Web::getVersionFromServer() {
  HTTPClient http;
  String result = "";
  http.begin(URL_VERSION);
  int httpCode = http.GET();
  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      result = http.getString();
    }
  }
  return result;
}
