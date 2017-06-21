#include "Console.h"
#include "Display.h"
#include "Logger.h"

void setup() {
  console.init();
  console.println("Inicializando...");
  
  display.init();
  display.write("Inicializando...");
  
  logger.init();

  display.write(">>>> Pronto <<<<");
  console.println("Pronto!");
}

void loop() {
  
}
