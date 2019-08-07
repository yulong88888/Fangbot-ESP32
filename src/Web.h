#ifndef __WEB_H__
#define __WEB_H__

#include <HTTPClient.h>
#include "Arduino.h"

class Web {
 public:
  Web();
  void setup();
  String needUpdate = "";

 private:
  String getVersionUI();
  String getVersionFirmware();
  String getVersionFromServer();
};

#endif
