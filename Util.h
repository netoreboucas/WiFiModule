#ifndef Util_h
#define Util_h

#include "Arduino.h"

class Util {
  public:
    int freeMemory();
};

extern Util util;

#endif
