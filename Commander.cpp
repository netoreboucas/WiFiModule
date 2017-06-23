#include "Commander.h"
Commander commander;

String Commander::execute(String command, boolean waitResponse) {
  String response = "NULL";
  logger.debug("Command: " + command);
  
  if (command == "RESET") {
    esp8266.reset();
    response = esp8266.reset() ? "SUCESSO" : "ERRO";
  } else if (command == "CONNECT") {
    response = server.connectToNetwork() ? "SUCESSO" : "ERRO";
  } else if (command == "DISCONNECT") {
    response = server.disconnectFromNetwork() ? "SUCESSO" : "ERRO";
  } else if (command == "START SERVER") {
    response = server.startServer() ? "SUCESSO" : "ERRO";
  } else if (command == "STOP SERVER") {
    response = server.stopServer() ? "SUCESSO" : "ERRO";
  } else if (command == "IP") {
    response = server.getIP();
  } else if (command == "STATUS") {
    response = server.getStatus();
  } else if (command == "SSID?") {
    response = server.ssid;
  } else if (command == "PWD?") {
    response = server.pwd;
    } else if (command == "PORT?") {
    response = server.port;
  } else if (command.startsWith("SSID=")) {
    response = server.ssid = command.substring(4);
  } else if (command.startsWith("PWD=")) {
    response = server.pwd = command.substring(4);
  } else if (command.startsWith("PORT=")) {
    response = server.port = command.substring(5);
  } else if (command == "LOAD CONFIG") {
    server.loadConf();
    response = "SUCESSO";
  } else if (command == "SAVE CONFIG") {
    server.saveConf();
    response = "SUCESSO";
  } else if (command == "LIST FILES") {
    logger.listFiles();
    response = "SUCESSO";
  } else if (command == "CLEAR LOGS") {
    logger.clear();
    response = "SUCESSO";
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
