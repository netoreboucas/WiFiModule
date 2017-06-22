/**
 * #############################################
 * ### ESP8266 - Open Smart Serial Wifi v1.0 ###
 * #############################################
 * 
 * pin 01 - GND ------------> GND
 * pin 02 - VCC ------------> 3.3V
 * pin 03 - TX -------------> Pino 19 (RX1) do Arduino
 * pin 04 - RX -------------> Pino 18 (TX1) do Arduino
 */

#ifndef ESP8266_h
#define ESP8266_h

#include "Arduino.h"
#include "Logger.h"

#define DEFAULT_TIMEOUT 100
#define DEFAULT_DELAY   100

class ESP8266 {
  public:
    void init();
    void reset();
    
    int available();

    String read(long timeout = DEFAULT_TIMEOUT);
    String readLine(long timeout = DEFAULT_TIMEOUT);
    boolean readLineUntil(String success, String error, long timeout = DEFAULT_TIMEOUT, boolean discardOver = true);
    
    void write(byte b);
    void print(String s);
    void println(String s, int d = DEFAULT_DELAY);
    
  private:
    static HardwareSerial& serial;
    String readLineUntil_Output;
};

extern ESP8266 esp8266;

#endif
