#ifndef Commander_h
#define Commander_h

#include "Arduino.h"
#include "Display.h"
#include "ESP8266.h"
#include "Server.h"
#include "Logger.h"
#include "Util.h"

class Commander {
  public:
    String execute(String command, boolean waitResponse = false);
};

extern Commander commander;

#endif
