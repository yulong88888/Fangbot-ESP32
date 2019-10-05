#include <Adafruit_NeoPixel.h>
#include <ESPmDNS.h>
#include <HTTPUpdate.h>
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

// EEPROM参数
String ssid = "";
String password = "";
float steps_per_mm = 0;
float steps_per_degree = 0;
int wheel_distance = 0;

//任务
Task task;

Web web;
WebSocket webSocket;

Network network;

Audio audio;

NFC nfc;

Config config;

ShiftStepper *left;
ShiftStepper *right;

Servo penServo;

Adafruit_NeoPixel pixels(LED_RGB_NUM, PIN_LED_RGB, NEO_GRB + NEO_KHZ800);

ThreadController manager = ThreadController();
Thread *audioThread = new Thread();
Thread *nfcThread = new Thread();
Thread *shiftStepperThread = new Thread();
Thread *servoThread = new Thread();

void taskHandler(char *data);

String doTaskId = "";

/**
 * 初始化参数
 */
void initParams();
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
/**
 * 设置颜色
 */
void setLED(String color1, String color2);

void setup() {
  pixels.begin();
  setLED("0xFF0000", "0xFF0000");
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  config.setup();
  initParams();
  WiFi.persistent(false);
  WiFi.setHostname(network.getAPName());
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(network.getAPName());

  if (!ssid.equals("")) {
    WiFi.begin((char *)ssid.c_str(), (char *)password.c_str());
    if (WiFi.waitForConnectResult() != WL_CONNECTED) {
      Serial.println("Conn Failed!");
    }
  }

  Serial.println(WiFi.localIP());

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

  //完成初始化
  setLED("0x0000FF", "0x0000FF");
}

void loop() {
  if (web.needUpdate == "") {
    manager.run();
  } else {
    t_httpUpdate_return ret;
    WiFiClient client;
    if (web.needUpdate == "ui") {
      ret = httpUpdate.updateSpiffs(client, URL_BIN_UI);
    }
    if (web.needUpdate == "firmware") {
      ret = httpUpdate.update(client, URL_BIN_FIRMWARE);
    }
    String msg = "";
    switch (ret) {
      case HTTP_UPDATE_FAILED:
        msg = httpUpdate.getLastErrorString().c_str();
        break;
      case HTTP_UPDATE_NO_UPDATES:
        msg = "no updates";
        break;
      case HTTP_UPDATE_OK:
        msg = "update completed";
        ESP.restart();
        break;
    }
    Serial.println(msg);
    web.needUpdate = "";
  }
}

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
    audio.select("/test.mp3");
    manager.add(audioThread);
  }
  if (cmd.equals("led")) {
    Serial.println("led");
    DynamicJsonDocument temp(256);
    deserializeJson(temp, arg);
    setLED(temp["color1"], temp["color2"]);
    taskFinish();
  }

  send["id"] = id;
  send["state"] = "rev";
  webSocket.sendMsg(send);
}

void audioPlay() {
  if (audio.isFinish()) {
    taskFinish();
    manager.remove(audioThread);
    left->setup(PIN_SHIFT_REG_DATA, PIN_SHIFT_REG_CLOCK, PIN_SHIFT_REG_LATCH);
    right->setup(PIN_SHIFT_REG_DATA, PIN_SHIFT_REG_CLOCK, PIN_SHIFT_REG_LATCH);
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

void initParams() {
  ssid = config.get_ssid();
  password = config.get_password();
  wheel_distance = (config.get_wheel_distance()).toFloat();
  steps_per_mm = ((config.get_step_per_turn()).toFloat()) /
                 ((config.get_circumference_mm()).toFloat());
  steps_per_degree = ((wheel_distance * 3.1416) / 360) * steps_per_mm;

  Serial.println(ssid);
  Serial.println(password);
  Serial.println(wheel_distance);
  Serial.println(steps_per_mm);
  Serial.println(steps_per_degree);
}

void setLED(String color1, String color2) {
  pixels.clear();
  pixels.setPixelColor(0, color1.toFloat());
  pixels.show();
  pixels.setPixelColor(1, color2.toFloat());
  pixels.show();
}
