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

String doTaskId = "";

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
/**
 * 完成任务
 */
void taskFinish();

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

  penServo.setup(PIN_SERVO, PENUP_DELAY, PENDOWN_DELAY);
  servoThread->setInterval(0);
  servoThread->onRun(servo);
}

void loop() { manager.run(); }

/**
 * 任务回调函数
 */
void taskHandler(char *data) {
  Serial.print("REV:");
  Serial.println(data);

  DynamicJsonDocument rev(256);
  DynamicJsonDocument send(256);

  deserializeJson(rev, data);
  String cmd = rev["cmd"];
  String arg = rev["arg"];
  String id = rev["id"];
  //设置任务id
  if (doTaskId != "") {
    send["id"] = id;
    send["state"] = "busy";
    webSocket.sendMsg(send);
    return;
  }
  doTaskId = id;

  if (cmd.equals("forward")) {
    Serial.println("forward");
    int temp = arg.toInt();
    if (temp > 0) {
      left->turn(temp * steps_per_mm, FORWARD);
      right->turn(temp * steps_per_mm, BACKWARD);
      manager.add(shiftStepperThread);
    }
  }
  if (cmd.equals("back")) {
    Serial.println("back");
    int temp = arg.toInt();
    if (temp > 0) {
      left->turn(temp * steps_per_mm, BACKWARD);
      right->turn(temp * steps_per_mm, FORWARD);
      manager.add(shiftStepperThread);
    }
  }
  if (cmd.equals("left")) {
    Serial.println("left");
    int temp = arg.toInt();
    if (temp > 0) {
      left->turn(temp * steps_per_degree, FORWARD);
      right->turn(temp * steps_per_degree, FORWARD);
      manager.add(shiftStepperThread);
    }
  }
  if (cmd.equals("right")) {
    Serial.println("right");
    int temp = arg.toInt();
    if (temp > 0) {
      left->turn(temp * steps_per_degree, BACKWARD);
      right->turn(temp * steps_per_degree, BACKWARD);
      manager.add(shiftStepperThread);
    }
  }
  if (cmd.equals("pen")) {
    if (arg.equals("up")) {
      Serial.println("up");
      penServo.setPenUp();
    }
    if (arg.equals("down")) {
      Serial.println("down");
      penServo.setPenDown();
    }
    manager.add(servoThread);
  }
  if (cmd.equals("nfcSing")) {
    Serial.println("nfcSing");
    // audio.select("/1.mp3");
    // manager.add(audioThread);
  }

  send["id"] = id;
  send["state"] = "rev";
  webSocket.sendMsg(send);
}

void audioPlay() {
  if (audio.isFinish()) {
    taskFinish();
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
    taskFinish();
    manager.remove(shiftStepperThread);
    return;
  }
  left->trigger();
  right->trigger();
}

void servo() {
  if (!penServo.ready()) {
    penServo.servoHandler();
  } else {
    taskFinish();
    manager.remove(servoThread);
  }
}

void taskFinish() {
  DynamicJsonDocument send(256);
  send["id"] = doTaskId;
  send["state"] = "ok";
  webSocket.sendMsg(send);
  doTaskId = "";
}