#include <Thread.h>
#include "Arduino.h"

Thread myThread = Thread();

void niceCallback() { Serial.println("0.0"); }

void setup() {
  Serial.begin(115200);
  myThread.onRun(niceCallback);
  myThread.setInterval(2000);
}

void loop() {
  if (myThread.shouldRun()) myThread.run();
}