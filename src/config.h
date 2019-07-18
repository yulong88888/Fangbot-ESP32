#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "Arduino.h"

// #define ssid "lengmang.net"
// #define password "lengmang"

#define http_username "admin"
#define http_password "admin"

#define JSON_BUFFER_LENGTH 512

#define ap_name "4DclassRobot"

// Audio使能
#define PIN_AUDIO_EN 16

//舵机
#define PIN_SERVO 17

#define PENUP_DELAY 1100
#define PENDOWN_DELAY 2000

// 电机
#define PIN_SHIFT_REG_DATA 32
#define PIN_SHIFT_REG_CLOCK 33
#define PIN_SHIFT_REG_LATCH 26

#define STEPS_PER_TURN "2048.0"

#define CIRCUMFERENCE_MM "230.0f"
#define WHEEL_DISTANCE "82.2f"

class Config {
 private:
 public:
  void setup();
  void set_ssid(String ssid);
  String get_ssid();
  void set_password(String password);
  String get_password();
  void set_step_per_turn(String value);
  String get_step_per_turn();
  void set_circumference_mm(String value);
  String get_circumference_mm();
  void set_wheel_distance(String value);
  String get_wheel_distance();
};

#endif