#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "Arduino.h"
#include <ArduinoJson.h>

class Network {
 public:
  char * getAPName();
  DynamicJsonDocument getWifiScanData();
  void startScanWifi();
  String getIp();

 private:
};

#endif
