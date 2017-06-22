#include "Console.h"
Console console;

HardwareSerial& Console::serial = Serial;

void Console::init() {
  serial.begin(115200);
}

int Console::available() {
  return serial.available();
}

String Console::readString() {
  return serial.readString();
}

void Console::write(byte b) {
  serial.write(b);
}

void Console::print(char c) {
  serial.print(c);
}

void Console::print(String s) {
  serial.print(s);
}

void Console::println() {
  serial.println();
}

void Console::println(String s) {
  serial.println(s);
}
