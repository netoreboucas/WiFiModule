#include "SmartHome.h"

SmartHome smartHome;

void SmartHome::init() {
  pinMode(RED, OUTPUT);
  pinMode(YELLOW, OUTPUT);
  pinMode(GREEN, OUTPUT);

  pinMode(RELE_1, OUTPUT);
  pinMode(RELE_2, OUTPUT);
  pinMode(RELE_3, OUTPUT);
  pinMode(RELE_4, OUTPUT);
  pinMode(RELE_5, OUTPUT);
  pinMode(RELE_6, OUTPUT);
  pinMode(RELE_7, OUTPUT);
  pinMode(RELE_8, OUTPUT);
}

boolean SmartHome::isOn(int pin) {
  return digitalRead(pin) == HIGH;
}

void SmartHome::set(int pin, boolean on) {
  digitalWrite(pin, on ? HIGH : LOW);
}

void SmartHome::toggle(int pin) {
  set(pin, !isOn(pin));
}
