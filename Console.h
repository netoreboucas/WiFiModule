#ifndef Console_h
#define Console_h

#include "Arduino.h"

class Console {
  public:
    void init();
    void write(byte b);
    void print(char c);
    void print(String s);
    void println();
    void println(String s);
};

extern Console console;

#endif
