#ifndef __TASK_H__
#define __TASK_H__

#include "Arduino.h"

typedef void (*function)();

class Task {
 public:
  void add(String name, function fn);
  void run(String name);
 private:
};

#endif
