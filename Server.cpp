#include "Server.h"
Server server;

String Server::ssid = "netoreboucas";
String Server::pwd = "a192168085";
String Server::port = "80";

void Server::init() {
  loadConf();
}

void Server::loadConf() {
  File file = SD.open("ssid");
  if (file && file.size() > 0) ssid = file.readString();
  file.close();
  
  file = SD.open("pwd");
  if (file && file.size() > 0) pwd = file.readString();
  file.close();
  
  file = SD.open("port");
  if (file && file.size() > 0) port = file.readString();
  file.close();
}

void Server::saveConf() {
  SD.remove("ssid");
  File file = SD.open("ssid", FILE_WRITE);
  if (file) file.print(ssid);
  file.close();
  
  SD.remove("pwd");
  file = SD.open("pwd", FILE_WRITE);
  if (file) file.print(pwd);
  file.close();

  SD.remove("port");
  file = SD.open("port", FILE_WRITE);
  if (file) file.print(port);
  file.close();
}

void Server::disconnect() {
  esp8266.println("AT+CIPSERVER=0");
  esp8266.readLineUntil("SERVER CLOSE\r\n", "%ERROR\r\n", 5000);
}

boolean Server::stopServer() {
  esp8266.println("AT+CIPSERVER=0");
  return esp8266.readLineUntil("OK\r\n", "%ERROR\r\n", 5000);
}

boolean Server::startServer() {
  esp8266.println("AT+CIPMUX=1");
  if (esp8266.readLineUntil("OK\r\n", "%ERROR\r\n", 5000)) {
    esp8266.println("AT+CIPSERVER=1," + port);
    return esp8266.readLineUntil("OK\r\n", "%ERROR\r\n", 5000);
  }

  return false;
}

String Server::getIP() {
  esp8266.println("AT+CIFSR");
  if (esp8266.readLineUntil("%.%", "ERROR\r\n", 5000)) {
    String ip = esp8266.readLineUntil_Output;
    ip = ip.substring(ip.indexOf("\"") + 1, ip.lastIndexOf("\""));
    return ip;
  }
  
  return NULL_STR;
}

String Server::getStatus() {
  esp8266.println("AT+CIPSTATUS");
  if (esp8266.readLineUntil("STATUS:%", "ERROR\r\n", 5000)) {
    return esp8266.readLineUntil_Output.substring(7, esp8266.readLineUntil_Output.lastIndexOf('\r'));
  }
  
  return NULL_STR;
}

boolean Server::httpClose() {
  esp8266.println("AT+CIPCLOSE");
  return esp8266.readLineUntil("OK\r\n", "ERROR\r\n", 5000);
}

boolean waitingGet = false;
String remoteIp;

void Server::processLine(String line) {
  if (waitingGet) {
    if (line.startsWith("GET /")) {
      //processHttpRequest(remoteIp, line.substring(4, line.lastIndexOf(' ')));
    } else {
      String status = getStatus();
      if (status == "CONNECT OK") {
        //closeHttpResponse();
      }
    }
  }
  
  waitingGet = line.startsWith("REMOTE IP: ");
  
  if (waitingGet) {
    remoteIp = line.substring(11, line.lastIndexOf('\r'));
  }
}
