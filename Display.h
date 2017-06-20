/**
 * ################
 * ### JHD 162A ###
 * ################
 * 
 * pin 01 - VSS ------------> GND
 * pin 02 - VCC ------------> 5V
 * pin 03 - VEE ------------> Pino central do Potenciômetro 01 (controle do contraste)
 * pin 04 - RS -------------> Pino 12 do Arduino
 * pin 05 - RW -------------> GND
 * pin 06 - ENABLE ---------> Pino 11 do Arduino
 * pin 07 - DB0 ------------> ~
 * pin 08 - DB1 ------------> ~
 * pin 09 - DB2 ------------> ~
 * pin 10 - DB3 ------------> ~
 * pin 11 - DB4 ------------> Pino 5 do Arduino
 * pin 12 - DB5 ------------> Pino 4 do Arduino
 * pin 13 - DB6 ------------> Pino 3 do Arduino
 * pin 14 - DB7 ------------> Pino 2 do Arduino
 * pin 15 - LED_POSITIVE ---> Pino central do Potenciômetro 02 (controle do brilho)
 * pin 16 - LED_NEGATIVE ---> GND
 * 
 * ########################
 * ### Potenciômetro 01 ###
 * ########################
 * 
 * pino esquerdo - GND
 * pino central -- pin 03 (VEE) do JHD
 * pino direito  - 5v
 * 
 * ########################
 * ### Potenciômetro 02 ###
 * ########################
 * 
 * pino esquerdo - GND
 * pino central -- pin 15 (LED_POSITIVE) do JHD
 * pino direito  - 5v
 */

#ifndef Display_h
#define Display_h

#include "Arduino.h"

#define PIN_RS          12 // pin 04
#define PIN_ENABLE      11 // pin 06
#define PIN_DB4          5 // pin 11
#define PIN_DB5          4 // pin 12
#define PIN_DB6          3 // pin 13
#define PIN_DB7          2 // pin 14

#define LCD_WIDTH       16
#define LCD_HEIGHT       2
 
class Display {
  public:
    void init();
    void test();
};

extern Display display;

#endif
