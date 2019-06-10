#include <Arduino.h>
#include <SimpleMap.h>
#include "Task.h"

Task task;

void _go() { Serial.println("go"); }
void _back() { Serial.println("back"); }
void _left() { Serial.println("left"); }
void _right() { Serial.println("right"); }

void setup() {
  Serial.begin(115200);
  task.add("go", _go);
  task.add("back", _back);
  task.add("left", _left);
  task.add("right", _right);
}

void loop() {
  task.run("go");
  delay(1000);
  task.run("back");
  delay(1000);
  task.run("left");
  delay(1000);
  task.run("right");
  delay(1000);
}