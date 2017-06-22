#include "Console.h"
#include "Display.h"
#include "ESP8266.h"
#include "Logger.h"

void setup() {
  console.init();
  console.println("Inicializando...");
  
  display.init();
  display.write("Inicializando...");
  
  logger.init();
  esp8266.init();

  display.write(">>>> Pronto <<<<");
  logger.info("Pronto!");
}

void loop() {
  if (esp8266.available()) {
    String s = esp8266.readLine();

    s.trim();
    display.write(s);
    //server.processLine(s);
  }
}
