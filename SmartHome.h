#ifndef SmartHome_h
#define SmartHome_h

#include "Arduino.h"

#define RED    35
#define YELLOW 33
#define GREEN  31

#define RELE_1  22
#define RELE_2  24
#define RELE_3  26
#define RELE_4  28
#define RELE_5  23
#define RELE_6  25
#define RELE_7  27
#define RELE_8  29

class SmartHome {
  public:
    void init();
    boolean isOn(int pin);
    void set(int pin, boolean on);
    void toggle(int pin);
};

extern SmartHome smartHome;

#endif
