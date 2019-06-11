#include "WebSocket.h"
#include <ESPAsyncWebServer.h>
#include "config.h"

AsyncWebServer wsServer(8888);
AsyncWebSocket ws("/ws");

dataHandler handler = NULL;

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  //连接
  if (type == WS_EVT_CONNECT) {
    Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
    //断开连接
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
    //发生错误
  } else if (type == WS_EVT_ERROR) {
    Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(),
                  *((uint16_t *)arg), (char *)data);
    //发送PONG
  } else if (type == WS_EVT_PONG) {
    Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len,
                  (len) ? (char *)data : "");
    //数据
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    String msg = "";
    if (info->final && info->index == 0 && info->len == len) {
      if (info->opcode == WS_TEXT) {
        for (size_t i = 0; i < info->len; i++) {
          msg += (char)data[i];
        }
        Serial.printf("%s\n", msg.c_str());
        if (handler) {
          handler((char *)msg.c_str());
        }
      }
    }
  }
}

void WebSocket::setup() {
  ws.onEvent(onWsEvent);
  wsServer.addHandler(&ws);
  wsServer.begin();
}

void WebSocket::onMsg(dataHandler h) { handler = h; }

void WebSocket::sendMsg(ArduinoJson::DynamicJsonDocument &msg) {
  char jsonBuff[JSON_BUFFER_LENGTH];
  serializeJson(msg, jsonBuff, sizeof(jsonBuff));
  ws.textAll(jsonBuff, strlen(jsonBuff));
}
