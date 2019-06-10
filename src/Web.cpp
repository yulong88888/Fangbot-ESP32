#include "Web.h"
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <SPIFFSEditor.h>
#include "config.h"

AsyncWebServer server(80);

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

  //允许远程修改SD卡文件
  server.addHandler(new SPIFFSEditor(SD, http_username, http_password));
  //配置路径
  server.serveStatic("/", SD, "/").setDefaultFile("index.htm");
  // 404
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404);
  });

  server.begin();
}
