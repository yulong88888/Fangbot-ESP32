#ifndef __WEBSOCKET_H__
#define __WEBSOCKET_H__

#include <ArduinoJson.h>

typedef void (* dataHandler) (char *);

class WebSocket {
 public:
  void setup();
  void onMsg(dataHandler h);
  void sendMsg(ArduinoJson::DynamicJsonDocument &msg);

 private:
};

#endif
