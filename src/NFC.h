#ifndef __NFC_H__
#define __NFC_H__

#include "Arduino.h"

class NFC {
 public:
  void setup();
  void loop();
  String getResult();
  bool readSuccess();

 private:
  int getBytesLength(byte *buffer);
  void dump_byte_array(byte *buffer, byte bufferSize);
  bool readSuccessFlag = false;
};

#endif
