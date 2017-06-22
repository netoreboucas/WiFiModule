#include "Commander.h"
Commander commander;

String Commander::execute(String command, boolean waitResponse) {
  String response = "NULL";
  logger.debug("Command: " + command);
  
  if (command == "RESET") {
    esp8266.reset();
    response = "RESET";
  } else if (command == "STOP SERVER") {
    if (server.stopServer()) {
      response = "SUCCESS";
    } else {
      response = "ERROR";
    }
  } else if (command == "START SERVER") {
    if (server.startServer()) {
      response = "SUCCESS";
    } else {
      response = "ERROR";
    }
  } else if (command == "IP") {
    response = server.getIP();
  } else if (command == "STATUS") {
    response = server.getStatus();
  } else if (command == "HTTP CLOSE") {
    response = server.httpClose() ? "CLOSED" : "NONE";
  } else if (command == "SSID?") {
    response = server.ssid;
  } else if (command == "PWD?") {
    response = server.pwd;
    } else if (command == "PORT?") {
    response = server.port;
  } else if (command.startsWith("SSID=")) {
    server.ssid = command.substring(4);
    response = server.ssid;
  } else if (command.startsWith("PWD=")) {
    server.pwd = command.substring(4);
    response = server.pwd;
  } else if (command.startsWith("PORT=")) {
    server.port = command.substring(5);
    response = server.port;
  } else if (command == "LOAD CONFIG") {
    server.loadConf();
    response = "CONFIG LOADED";
  } else if (command == "SAVE CONFIG") {
    server.saveConf();
    response = "CONFIG SAVED";
  } else if (command == "LIST FILES") {
    logger.listFiles();
    response = "LISTED";
  } else if (command == "CLEAR LOGS") {
    logger.clear();
    response = "CLEARED";
  } else if (command == "FREE MEMORY") {
    response = String(util.freeMemory()) + " bytes";
  } else if (command.startsWith("AT")) {
    esp8266.println(command);
    response = waitResponse ? esp8266.read(1000) : "NONE";
  } else {
    response = "Comando invalido";
  }

  logger.debug("Response: " + response);  
  return response;
}
