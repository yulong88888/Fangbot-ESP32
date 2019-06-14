#include <ESPmDNS.h>
#include <Thread.h>
#include <ThreadController.h>
#include <WiFi.h>
#include "Arduino.h"
#include "Audio.h"
#include "NFC.h"
#include "Network.h"
#include "ShiftStepper.h"
#include "Task.h"
#include "Web.h"
#include "WebSocket.h"
#include "config.h"
#include "servo.h"

//任务
Task task;

Web web;
WebSocket webSocket;

Network network;

Audio audio;

NFC nfc;

float steps_per_mm = STEPS_PER_MM;
float steps_per_degree = STEPS_PER_DEGREE;
int wheel_distance = WHEEL_DISTANCE;
ShiftStepper *left;
ShiftStepper *right;

Servo penServo;

ThreadController manager = ThreadController();
Thread *audioThread = new Thread();
Thread *nfcThread = new Thread();
Thread *shiftStepperThread = new Thread();
Thread *servoThread = new Thread();

void taskHandler(char *data);

/**
 * Audio函数
 */
void audioPlay();
/**
 * NFC函数
 */
void nfcHandler();
/**
 * 车轮函数
 */
void shiftStepper();
/**
 * 舵机函数
 */
void servo();

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.setHostname(network.getAPName());
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(network.getAPName());
  network.startScanWifi();
  network.getWifiScanData();
  WiFi.begin(ssid, password);
  delay(1000);

  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }

  Serial.println(WiFi.localIP());

  //   MDNS.addService("http", "tcp", 80);

  webSocket.onMsg(taskHandler);

  web.setup();
  webSocket.setup();

  audio.setup();
  audioThread->setInterval(0);
  audioThread->onRun(audioPlay);

  nfc.setup();
  nfcThread->setInterval(10);
  nfcThread->onRun(nfcHandler);

  //车轮
  left = new ShiftStepper(0);
  right = new ShiftStepper(1);
  left->setup(PIN_SHIFT_REG_DATA, PIN_SHIFT_REG_CLOCK, PIN_SHIFT_REG_LATCH);
  right->setup(PIN_SHIFT_REG_DATA, PIN_SHIFT_REG_CLOCK, PIN_SHIFT_REG_LATCH);

  shiftStepperThread->setInterval(0);
  shiftStepperThread->onRun(shiftStepper);

  // manager.add(shiftStepperThread);
  // left->turn(10000 * steps_per_mm, FORWARD);
  // right->turn(10000 * steps_per_mm, BACKWARD);

  penServo.setup(PIN_SERVO, PENUP_DELAY, PENDOWN_DELAY);
  servoThread->setInterval(0);
  servoThread->onRun(servo);

  manager.add(servoThread);
}

void loop() { manager.run(); }

/**
 * 任务回调函数
 */
void taskHandler(char *data) {
  Serial.print("REV:");
  Serial.println(data);
  delay(1000);

  audio.select("/1.mp3");
  // manager.add(audioThread);

  DynamicJsonDocument doc(256);
  doc["state"] = "OJBK";
  webSocket.sendMsg(doc);
}

void audioPlay() {
  if (audio.isFinish()) {
    manager.remove(audioThread);
  } else {
    audio.loop();
  }
}

void nfcHandler() {
  if (nfc.readSuccess()) {
    Serial.println(nfc.getResult());
    manager.remove(nfcThread);
  } else {
    nfc.loop();
  }
}

void shiftStepper() {
  if (left->ready() && right->ready()) {
    // taskFinish();
    manager.remove(shiftStepperThread);
    return;
  }
  left->trigger();
  right->trigger();
}

bool flag = false;

void servo() {
  if (!penServo.ready()) {
    penServo.servoHandler();
  } else {
    if (flag) {
      penServo.setPenUp();
    } else {
      penServo.setPenDown();
    }
    flag = !flag;
    // taskFinish();
    // manager.remove(servoThread);
  }
}