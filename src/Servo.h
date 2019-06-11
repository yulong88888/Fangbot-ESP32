#ifndef __SERVO_H__
#define __SERVO_H__

#include "Arduino.h"

#define SERVO_PULSES 15

class Servo {
 public:
  typedef enum { UP, DOWN } penState_t;
  void setup(int _ServoPin, int _PenUpDelay, int _PenDownDelay);
  void setPenState(penState_t);
  void servoHandler();
  void setPenUp();
  void setPenDown();
  boolean ready();

 private:
  penState_t PenState;
  int ServoPin;
  int PenUpDelay;
  int PenDownDelay;
  unsigned char ServoPulsesLeft;
  unsigned long NextServoPulse;
};

#endif
