#include "ESP8266.h"
ESP8266 esp8266;

HardwareSerial& ESP8266::serial = Serial1;

void ESP8266::init() {
  logger.info("Inicializando ESP8266...");
  serial.begin(115200);
  reset();
}

boolean ESP8266::reset() {
  println("AT+RST");
  return readLineUntil("ready\r\n", NULL_STR, 5000);
}

int ESP8266::available() {
  return serial.available();
}

boolean ESP8266::find(char c) {
  return serial.find(c);
}

String ESP8266::read(long timeout) {
  serial.setTimeout(timeout);
  String s = serial.readString();
  
  if (s.length() > 0) {
    logger.debug("---\r\n" + s + "\r\n---");
  }
  
  return s;
}

String ESP8266::readLine(long timeout) {
  serial.setTimeout(timeout);
  String s = serial.readStringUntil('\n');

  if (s.length() == 0)
    return NULL_STR;
    
  s += "\n";

  logger.debug(">>> " + s);
  
  return s;
}

boolean ESP8266::readLineUntil(String success, String error, long timeout, boolean discardOver) {
  boolean found = false;

  String line = readLine(timeout);
  while (line != NULL && !line.equals(NULL_STR)) {
    if (success != NULL && !success.equals(NULL_STR) && (line.equals(success) ||
      (success.startsWith("%") && success.endsWith("%") && line.indexOf(success.substring(1, success.length() - 1)) != -1) ||
      (success.startsWith("%") && line.endsWith(success.substring(1, success.length()))) ||
      (success.endsWith("%") && line.startsWith(success.substring(0, success.length() - 1)))
    )) {
      found = true;
      readLineUntil_Output = line;
      readLineUntil_Output.trim();
      break;
    } else if (error != NULL && !error.equals(NULL_STR) && (line.equals(error) || 
      (error.startsWith("%") && error.endsWith("%") && line.indexOf(error.substring(1, error.length() - 1)) != -1)) ||
      (error.startsWith("%") && line.endsWith(error.substring(1, error.length()))) ||
      (error.endsWith("%") && line.startsWith(error.substring(0, error.length() - 1)))
    ) {
      break;
    }
    
    line = readLine(timeout);
  }
  
  if (discardOver)
    read(500); 
  
  return found;
}

void ESP8266::write(byte b) {
  serial.write(b);
}

void ESP8266::write(byte b[], int l) {
  serial.write(b, l);
}

void ESP8266::print(String s) {
  serial.print(s);
}

void ESP8266::println(String s, int d) {
  serial.println(s);
  delay(d);
}
