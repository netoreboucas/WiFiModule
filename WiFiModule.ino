#include "Console.h"
#include "Commander.h"
#include "Display.h"
#include "ESP8266.h"
#include "Logger.h"
#include "Server.h"
#include "SmartHome.h"

String line1 = NULL_STR;
String line2 = NULL_STR;

void setup() {
  console.init();
  console.println("Inicializando...");
  
  display.init();
  display.setText("Inicializando...");
  
  logger.init();
  esp8266.init();
  server.init();
  smartHome.init();

  display.setText(">>>> Pronto <<<<");
  logger.info("Pronto!");

  server.startServer();
}

void loop() {
  if (esp8266.available()) {
    esp8266.readLine();
  }
  
  if (console.available()) {
    String command = console.readString();
    command.trim();
    display.setText(command);
    
    String response = commander.execute(command);
    if (response == NULL || response.equals(NULL_STR))
      response = "NONE";
    response.trim();
    
    if (response.length() == 0)
      response = "<EMPTY>";
    
    display.setText(command, response);
  }
}
