#ifndef __ShiftStepper_h__
#define __ShiftStepper_h__

#include "Arduino.h"

#define FORWARD 1
#define BACKWARD 0

#define BASE_INTERRUPT_US 50
#define DEFAULT_STEP_PERIOD 3000
#define UCOUNTER_DEFAULT DEFAULT_STEP_PERIOD / BASE_INTERRUPT_US

class ShiftStepper {
 private:
  //判断左右轮
  int motor_offset;
  static int data_pin;
  static int clock_pin;
  static int latch_pin;
  //最后一次的
  static uint8_t lastBits;
  //当前的
  static uint8_t currentBits;
  //剩余的步数
  volatile long _remaining;
  //方向
  byte _dir;
  //当前步
  byte currentStep;

  int microCounter;

 public:
  ShiftStepper(int offset);
  ~ShiftStepper();
  void setup(int _data_pin, int _clock_pin, int _latch_pin);
  void stop();
  boolean ready();
  void turn(long steps, byte direction);
  void trigger();
  void setNextStep();
  void sendBits();
  void release();
  void updateBits(uint8_t bits);
  byte nextStep();
};
#endif
