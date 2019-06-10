#include "Task.h"
#include <SimpleMap.h>

SimpleMap<String, function> *taskMap =
    new SimpleMap<String, function>([](String &a, String &b) -> int {
      if (a == b)
        return 0;  // a and b are equal
      else if (a > b)
        return 1;  // a is bigger than b
      else
        return -1;  // a is smaller than b
    });

void Task::add(String name, function fn) { taskMap->put(name, fn); }

void Task::run(String name) {
  function cb = taskMap->get(name);
  cb();
}
