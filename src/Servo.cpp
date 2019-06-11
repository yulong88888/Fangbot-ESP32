#include "servo.h"

void Servo::setup(int _ServoPin, int _PenUpDelay, int _PenDownDelay) {
  ServoPin = _ServoPin;
  PenUpDelay = _PenUpDelay;
  PenDownDelay = _PenDownDelay;
  pinMode(ServoPin, OUTPUT);
}

void Servo::setPenState(penState_t state) {
  PenState = state;
  ServoPulsesLeft = SERVO_PULSES;
  NextServoPulse = 0;
}

void Servo::servoHandler() {
  if (ServoPulsesLeft) {
    if (micros() >= NextServoPulse) {
      ServoPulsesLeft--;
      digitalWrite(ServoPin, HIGH);
      if (PenState == UP) {
        NextServoPulse = micros() + (12000 - PenUpDelay);
        delayMicroseconds(PenUpDelay);
      } else {
        NextServoPulse = micros() + (12000 - PenDownDelay);
        delayMicroseconds(PenDownDelay);
      }
      digitalWrite(ServoPin, LOW);
    }
  }
}

void Servo::setPenUp() { setPenState(UP); }

void Servo::setPenDown() { setPenState(DOWN); }

boolean Servo::ready() { return ServoPulsesLeft == 0; }
