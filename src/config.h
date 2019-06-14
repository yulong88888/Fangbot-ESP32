#ifndef __CONFIG_H__
#define __CONFIG_H__

// #define ssid "imaker"
// #define password "Wlgf@001"

#define ssid "lengmang.net"
#define password "lengmang"

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

#define STEPS_PER_TURN 2048.0f

#define CIRCUMFERENCE_MM 230.0f
#define WHEEL_DISTANCE 82.2f

#define STEPS_PER_MM STEPS_PER_TURN / CIRCUMFERENCE_MM
#define STEPS_PER_DEGREE ((WHEEL_DISTANCE * 3.1416) / 360) * STEPS_PER_MM

#endif