/**
 * ###################################
 * ### SD Module v1.2 - ElecFreaks ###
 * ###################################
 * 
 * pin 01 - GND ------------> GND
 * pin 02 - VCC ------------> 5V
 * pin 03 - CS -------------> Pino 53 do Arduino
 * pin 04 - DI -------------> Pino 51 do Arduino
 * pin 05 - DO -------------> Pino 50 do Arduino
 * pin 06 - CK -------------> Pino 52 do Arduino
 */

#ifndef Logger_h
#define Logger_h

#include "Arduino.h"
#include "Console.h"
#include "Constant.h"
#include <SD.h>

#define PIN_CS 53
#define TIME_SIZE_CHECK 60000 // 1 Min
#define MAX_FILE_SIZE 1048576 // 1 Mb

class Logger {
  public:
    void init();
    void clear();
    void listFiles();
    void write(byte b);
    void print(String s);
    void println();
    void println(String s);
    
    void info(String s);
    void debug(String s);
    
  private:
    boolean ok;
    File file;
    long lastSizeCheck;
    void checkFileLength();
    void open();
    void close();
    void clear(File dir);
    void listFiles(File dir, int numTabs);
};

extern Logger logger;

#endif
