#include "ShiftStepper.h"

int ShiftStepper::data_pin;
int ShiftStepper::clock_pin;
int ShiftStepper::latch_pin;
uint8_t ShiftStepper::lastBits;
uint8_t ShiftStepper::currentBits;

ShiftStepper::ShiftStepper(int offset) {
  motor_offset = offset;
  currentStep = 0;
  microCounter = UCOUNTER_DEFAULT;
}

void ShiftStepper::setup(int _data_pin, int _clock_pin, int _latch_pin) {
  data_pin = _data_pin;
  clock_pin = _clock_pin;
  latch_pin = _latch_pin;
  pinMode(data_pin, OUTPUT);
  pinMode(clock_pin, OUTPUT);
  pinMode(latch_pin, OUTPUT);
  digitalWrite(data_pin, LOW);
  digitalWrite(clock_pin, LOW);
  digitalWrite(latch_pin, LOW);
  lastBits = 0;
  currentBits = 0;
  currentStep = 0;
  release();
}

void ShiftStepper::stop() { _remaining = 0; }

boolean ShiftStepper::ready() { return (_remaining < 0); }

void ShiftStepper::turn(long steps, byte direction) {
  delayCount = 0;
  if (steps < 0) {
    steps = -steps;
    direction = !direction;
  }
  _remaining = steps;
  _dir = direction;
  // lastDirection = direction;
}

void ShiftStepper::trigger() {
  delayCount++;
  if (delayCount < 10) {
    return;
  }
  delayCount = 0;
  setNextStep();
  sendBits();
}

void ShiftStepper::setNextStep() {
  if (_remaining > 0) {
    if (!--microCounter) {
      microCounter = UCOUNTER_DEFAULT;
      _remaining--;
      updateBits(nextStep());
    }
  } else {
    _remaining = -1;
    release();
  }
}

void ShiftStepper::sendBits() {
  if (currentBits != lastBits) {
    lastBits = currentBits;
    digitalWrite(latch_pin, LOW);
    shiftOut(data_pin, clock_pin, MSBFIRST, currentBits);
    digitalWrite(latch_pin, HIGH);
  }
}

void ShiftStepper::release() {
  currentStep = 0;
  updateBits(0);
  sendBits();
}

void ShiftStepper::updateBits(uint8_t bits) {
  currentStep = bits;
  bits &= B1111;
  bits <<= (motor_offset * 4);
  currentBits &= ~(B1111 << (motor_offset * 4));
  currentBits |= bits;
}

byte ShiftStepper::nextStep() {
  switch (currentStep) {
    case B0000:
    case B0001:
      return (_dir == FORWARD ? B0010 : B1000);
    case B0010:
      return (_dir == FORWARD ? B0100 : B0001);
    case B0100:
      return (_dir == FORWARD ? B1000 : B0010);
    case B1000:
      return (_dir == FORWARD ? B0001 : B0100);
    default:
      return B0000;
  }
}
