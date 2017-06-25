#include "Server.h"
Server server;

String Server::ssid = "...";
String Server::pwd = "...";
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

boolean Server::connectToNetwork() {
  esp8266.println("AT+CWJAP=\"" + ssid + "\",\"" + pwd + "\"");
  return esp8266.readLineUntil("OK\r\n", "%ERROR\r\n", 5000);
}

boolean Server::disconnectFromNetwork() {
  esp8266.println("AT+CWQAP");
  return esp8266.readLineUntil("OK\r\n", "%ERROR\r\n", 5000);
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
  if (esp8266.readLineUntil("%.%", "%ERROR\r\n", 5000, true)) {
    String ip = esp8266.readLineUntil_Output;
    ip = ip.substring(ip.indexOf("\"") + 1, ip.lastIndexOf("\""));
    return ip;
  }
  
  return NULL_STR;
}

String Server::getStatus() {
  esp8266.println("AT+CIPSTATUS");
  if (esp8266.readLineUntil("STATUS:%", "%ERROR\r\n", 5000)) {
    return esp8266.readLineUntil_Output.substring(7, esp8266.readLineUntil_Output.lastIndexOf('\r'));
  }
  
  return NULL_STR;
}

void Server::processLine(String line) {
  if (line.startsWith("+IPD,")) {
    line = line.substring(5); // remove "+IPD,"
    
    String channel = line.substring(0, line.indexOf(","));
    line = line.substring(line.indexOf(":") + 1); // remove "<channel>,123,:"
    
    String method = line.substring(0, line.indexOf(" "));
    line = line.substring(method.length() + 1); // remove "<method> "
    
    String url = line.substring(0, line.lastIndexOf(" "));

    processHttpRequest(channel, method, url);
  }
}

void Server::processHttpRequest(String channel, String method, String url) {
  logger.info("HTTP REQUEST: Channel[" + channel + "] Method[" + method + "] Url[" + url + "]");
  display.setText(method, url);
  
  esp8266.read(1000);
  createHttpResponseBuffer(channel, url);
  sendHttpResponseBuffer(channel);
  closeHttpResponse(channel);
}

void Server::closeHttpResponse(String channel) {
  esp8266.println("AT+CIPCLOSE=" + channel);
  esp8266.readLineUntil("OK\r\n", "%ERROR\r\n", 5000);
}

#define MAX_CIPSEND_LENGTH 2048

void Server::sendHttpResponseBuffer(String channel) {
  File response = SD.open("R_" + channel + ".txt");
  
  if (response) {
    int count = 0;
    
    while (response.available() > 0) {
      int length = min(MAX_CIPSEND_LENGTH, response.available());
      byte buffer[length];
      
      esp8266.println("AT+CIPSEND=" + channel + "," + String(length));
      if (esp8266.find('>')) {
        response.readBytes(buffer, length);
        esp8266.write(buffer, length);
        count += length;
        if (!esp8266.readLineUntil("%SEND OK%", "%SEND FAIL%", 10000)) {
          break;
        }
      }
    }
    
    response.close();
  }
}

void Server::createHttpResponseBuffer(String channel, String url) {
  SD.remove("R_" + channel + ".txt");
  File response = SD.open("R_" + channel + ".txt", FILE_WRITE);
  if (!response) return;
  
  if (url.startsWith("/led")) {
    boolean responseOnlyStatus = false;
    
    if (url == "/led/on/red") {
      smartHome.set(RED, true);
      responseOnlyStatus = true;
    } else if (url == "/led/on/yellow") {
      smartHome.set(YELLOW, true);
      responseOnlyStatus = true;
    } else if (url == "/led/on/green") {
      smartHome.set(GREEN, true);
      responseOnlyStatus = true;
    } else if (url == "/led/on/all") {
      smartHome.set(RED, true);
      smartHome.set(YELLOW, true);
      smartHome.set(GREEN, true);
      responseOnlyStatus = true;
    } else if (url == "/led/off/red") {
      smartHome.set(RED, false);
      responseOnlyStatus = true;
    } else if (url == "/led/off/yellow") {
      smartHome.set(YELLOW, false);
      responseOnlyStatus = true;
    } else if (url == "/led/off/green") {
      smartHome.set(GREEN, false);
      responseOnlyStatus = true;
    } else if (url == "/led/off/all") {
      smartHome.set(RED, false);
      smartHome.set(YELLOW, false);
      smartHome.set(GREEN, false);
      responseOnlyStatus = true;
    }
    
    if (responseOnlyStatus) {
      response.print(F("["));
      
      if (smartHome.isOn(RED)) {
        response.print(F(  "{ \"id\":\"red\", \"action\":\"/led/off/red\", \"classI\":\"fa-thumbs-o-up\" },"));
      } else {
        response.print(F(  "{ \"id\":\"red\", \"action\":\"/led/on/red\", \"classI\":\"fa-thumbs-o-down\" },"));
      }
      
      if (smartHome.isOn(YELLOW)) {
        response.print(F(  "{ \"id\":\"yellow\", \"action\":\"/led/off/yellow\", \"classI\":\"fa-thumbs-o-up\" },"));
      } else {
        response.print(F(  "{ \"id\":\"yellow\", \"action\":\"/led/on/yellow\", \"classI\":\"fa-thumbs-o-down\" },"));
      }
      
      if (smartHome.isOn(GREEN)) {
        response.print(F(  "{ \"id\":\"green\", \"action\":\"/led/off/green\", \"classI\":\"fa-thumbs-o-up\" }"));
      } else {
        response.print(F(  "{ \"id\":\"green\", \"action\":\"/led/on/green\", \"classI\":\"fa-thumbs-o-down\" }"));
      }
      
      response.print(F("]"));
    } else {
      response.print(F("<html>"));
      response.print(F(  "<head>"));
      
      writeHead(response);
      
      response.print(F(    "<script>"));
      response.print(F(      "$(function() {"));
      response.print(F(        "$('.btn').click(function() {"));
      response.print(F(          "var $btn = $(this);"));
      response.print(F(          "var $i = $btn.find('i');"));
      response.print(F(          "var bkpClassB = $btn.attr('class');"));
      response.print(F(          "var bkpClassI = $i.attr('class');"));
      response.print(F(          "$btn.attr('class', 'btn btn-lg btn-block btn-default disabled');"));
      response.print(F(          "$i.attr('class', 'fa fa-cog fa-spin');"));
      response.print(F(          "$.ajax({"));
      response.print(F(            "dataType: 'json',"));
      response.print(F(            "url: $(this).attr('action'),"));
      response.print(F(            "timeout: 5000,"));
      response.print(F(            "success: function(data) {"));
      response.print(F(              "$btn.attr('class', bkpClassB);"));
      response.print(F(              "$i.attr('class', bkpClassI);"));
      response.print(F(              "$.each(data, function() {"));
      response.print(F(                "if (this.action != null) $('#' + this.id).attr('action', this.action);"));
      response.print(F(                "if (this.classI != null) $('#' + this.id).find('i').removeClass().addClass('fa').addClass(this.classI);"));
      response.print(F(              "});"));
      response.print(F(            "},"));
      response.print(F(            "error: function() {"));
      response.print(F(              "$btn.attr('class', bkpClassB);"));
      response.print(F(              "$i.attr('class', bkpClassI);"));
      response.print(F(            "}"));
      response.print(F(          "});"));
      response.print(F(        "});"));
      response.print(F(      "});"));
      response.print(F(    "</script>"));
      response.print(F(  "</head>"));
      response.print(F(  "<body style='padding-bottom: 70px;'>"));
      response.print(F(    "<div class='container'>"));
      response.print(F(      "<h1><i class='fa fa-home' onclick='window.location=\"/\";'></i> Smart Home :-)</h1><br/>"));
      
      if (smartHome.isOn(RED)) {
        response.print(F(      "<button id='red' action='/led/off/red' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(        "<i class='fa fa-thumbs-o-up'></i>"));
        response.print(F(      "</button><br/>"));
      } else {
        response.print(F(      "<button id='red' action='/led/on/red' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(        "<i class='fa fa-thumbs-o-down'></i>"));
        response.print(F(      "</button><br/>"));
      }
      
      if (smartHome.isOn(YELLOW)) {
        response.print(F(      "<button id='yellow' action='/led/off/yellow' type='button' class='btn btn-lg btn-block btn-warning'>"));
        response.print(F(        "<i class='fa fa-thumbs-o-up'></i>"));
        response.print(F(      "</button><br/>"));
      } else {
        response.print(F(      "<button id='yellow' action='/led/on/yellow' type='button' class='btn btn-lg btn-block btn-warning'>"));
        response.print(F(        "<i class='fa fa-thumbs-o-down'></i>"));
        response.print(F(      "</button><br/>"));
      }
      
      if (smartHome.isOn(GREEN)) {
        response.print(F(      "<button id='green' action='/led/off/green' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(        "<i class='fa fa-thumbs-o-up'></i>"));
        response.print(F(      "</button><br/>"));
      } else {
        response.print(F(      "<button id='green' action='/led/on/green' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(        "<i class='fa fa-thumbs-o-down'></i>"));
        response.print(F(      "</button><br/>"));
      }
      
      response.print(F(      "<hr/>"));
      
      response.print(F(      "<button id='all' action='/led/on/all' type='button' class='btn btn-lg btn-block btn-default'>"));
      response.print(F(        "<i class='fa fa-thumbs-o-up'></i> Ligar todos os LEDs"));
      response.print(F(      "</button><br/>"));
      response.print(F(      "<button id='all' action='/led/off/all' type='button' class='btn btn-lg btn-block btn-default'>"));
      response.print(F(        "<i class='fa fa-thumbs-o-down'></i> Desligar todos os LEDs"));
      response.print(F(      "</button>"));
      
      response.print(F(    "</div>"));
      
      writeFooter(response);
      
      response.print(F(  "<body>"));
      response.print(F("</html>"));
    }
  } else if (url.startsWith("/rele")) {
    boolean responseOnlyStatus = false;
    
    if (url == "/rele/on/r1") {
      smartHome.set(RELE_1, true);
      responseOnlyStatus = true;
    } else if (url == "/rele/on/r2") {
      smartHome.set(RELE_2, true);
      responseOnlyStatus = true;
    } else if (url == "/rele/on/r3") {
      smartHome.set(RELE_3, true);
      responseOnlyStatus = true;
    } else if (url == "/rele/on/r4") {
      smartHome.set(RELE_4, true);
      responseOnlyStatus = true;
    } else if (url == "/rele/on/r5") {
      smartHome.set(RELE_5, true);
      responseOnlyStatus = true;
    } else if (url == "/rele/on/r6") {
      smartHome.set(RELE_6, true);
      responseOnlyStatus = true;
    } else if (url == "/rele/on/r7") {
      smartHome.set(RELE_7, true);
      responseOnlyStatus = true;
    } else if (url == "/rele/on/r8") {
      smartHome.set(RELE_8, true);
      responseOnlyStatus = true;
    } else if (url == "/rele/on/all") {
      smartHome.set(RELE_1, true);
      smartHome.set(RELE_2, true);
      smartHome.set(RELE_3, true);
      smartHome.set(RELE_4, true);
      smartHome.set(RELE_5, true);
      smartHome.set(RELE_6, true);
      smartHome.set(RELE_7, true);
      smartHome.set(RELE_8, true);
      responseOnlyStatus = true;
    } else if (url == "/rele/off/r1") {
      smartHome.set(RELE_1, false);
      responseOnlyStatus = true;
    } else if (url == "/rele/off/r2") {
      smartHome.set(RELE_2, false);
      responseOnlyStatus = true;
    } else if (url == "/rele/off/r3") {
      smartHome.set(RELE_3, false);
      responseOnlyStatus = true;
    } else if (url == "/rele/off/r4") {
      smartHome.set(RELE_4, false);
      responseOnlyStatus = true;
    } else if (url == "/rele/off/r5") {
      smartHome.set(RELE_5, false);
      responseOnlyStatus = true;
    } else if (url == "/rele/off/r6") {
      smartHome.set(RELE_6, false);
      responseOnlyStatus = true;
    } else if (url == "/rele/off/r7") {
      smartHome.set(RELE_7, false);
      responseOnlyStatus = true;
    } else if (url == "/rele/off/r8") {
      smartHome.set(RELE_8, false);
      responseOnlyStatus = true;
    } else if (url == "/rele/off/all") {
      smartHome.set(RELE_1, false);
      smartHome.set(RELE_2, false);
      smartHome.set(RELE_3, false);
      smartHome.set(RELE_4, false);
      smartHome.set(RELE_5, false);
      smartHome.set(RELE_6, false);
      smartHome.set(RELE_7, false);
      smartHome.set(RELE_8, false);
      responseOnlyStatus = true;
    }
    
    if (responseOnlyStatus) {
      response.print(F("["));
      
      if (smartHome.isOn(RELE_1)) {
        response.print(F(  "{ \"id\":\"r1\", \"action\":\"/rele/off/r1\", \"classB\":\"btn-success\", \"classI\":\"fa-thumbs-o-up\" },"));
      } else {
        response.print(F(  "{ \"id\":\"r1\", \"action\":\"/rele/on/r1\", \"classB\":\"btn-danger\", \"classI\":\"fa-thumbs-o-down\" },"));
      }
      
      if (smartHome.isOn(RELE_2)) {
        response.print(F(  "{ \"id\":\"r2\", \"action\":\"/rele/off/r2\", \"classB\":\"btn-success\", \"classI\":\"fa-thumbs-o-up\" },"));
      } else {
        response.print(F(  "{ \"id\":\"r2\", \"action\":\"/rele/on/r2\", \"classB\":\"btn-danger\", \"classI\":\"fa-thumbs-o-down\" },"));
      }
      
      if (smartHome.isOn(RELE_3)) {
        response.print(F(  "{ \"id\":\"r3\", \"action\":\"/rele/off/r3\", \"classB\":\"btn-success\", \"classI\":\"fa-thumbs-o-up\" },"));
      } else {
        response.print(F(  "{ \"id\":\"r3\", \"action\":\"/rele/on/r3\", \"classB\":\"btn-danger\", \"classI\":\"fa-thumbs-o-down\" },"));
      }
      
      if (smartHome.isOn(RELE_4)) {
        response.print(F(  "{ \"id\":\"r4\", \"action\":\"/rele/off/r4\", \"classB\":\"btn-success\", \"classI\":\"fa-thumbs-o-up\" },"));
      } else {
        response.print(F(  "{ \"id\":\"r4\", \"action\":\"/rele/on/r4\", \"classB\":\"btn-danger\", \"classI\":\"fa-thumbs-o-down\" },"));
      }
      
      if (smartHome.isOn(RELE_5)) {
        response.print(F(  "{ \"id\":\"r5\", \"action\":\"/rele/off/r5\", \"classB\":\"btn-success\", \"classI\":\"fa-thumbs-o-up\" },"));
      } else {
        response.print(F(  "{ \"id\":\"r5\", \"action\":\"/rele/on/r5\", \"classB\":\"btn-danger\", \"classI\":\"fa-thumbs-o-down\" },"));
      }
      
      if (smartHome.isOn(RELE_6)) {
        response.print(F(  "{ \"id\":\"r6\", \"action\":\"/rele/off/r6\", \"classB\":\"btn-success\", \"classI\":\"fa-thumbs-o-up\" },"));
      } else {
        response.print(F(  "{ \"id\":\"r6\", \"action\":\"/rele/on/r6\", \"classB\":\"btn-danger\", \"classI\":\"fa-thumbs-o-down\" },"));
      }
      
      if (smartHome.isOn(RELE_7)) {
        response.print(F(  "{ \"id\":\"r7\", \"action\":\"/rele/off/r7\", \"classB\":\"btn-success\", \"classI\":\"fa-thumbs-o-up\" },"));
      } else {
        response.print(F(  "{ \"id\":\"r7\", \"action\":\"/rele/on/r7\", \"classB\":\"btn-danger\", \"classI\":\"fa-thumbs-o-down\" },"));
      }
      
      if (smartHome.isOn(RELE_8)) {
        response.print(F(  "{ \"id\":\"r8\", \"action\":\"/rele/off/r8\", \"classB\":\"btn-success\", \"classI\":\"fa-thumbs-o-up\" }"));
      } else {
        response.print(F(  "{ \"id\":\"r8\", \"action\":\"/rele/on/r8\", \"classB\":\"btn-danger\", \"classI\":\"fa-thumbs-o-down\" }"));
      }
      
      response.print(F("]"));
    } else {
      response.print(F("<html>"));
      response.print(F(  "<head>"));
      
      writeHead(response);
      
      response.print(F(    "<script>"));
      response.print(F(      "$(function() {"));
      response.print(F(        "$('.btn').click(function() {"));
      response.print(F(          "var $btn = $(this);"));
      response.print(F(          "var $i = $btn.find('i');"));
      response.print(F(          "var bkpClassB = $btn.attr('class');"));
      response.print(F(          "var bkpClassI = $i.attr('class');"));
      response.print(F(          "$btn.attr('class', 'btn btn-lg btn-block btn-default disabled');"));
      response.print(F(          "$i.attr('class', 'fa fa-cog fa-spin');"));
      response.print(F(          "$.ajax({"));
      response.print(F(            "dataType: 'json',"));
      response.print(F(            "url: $(this).attr('action'),"));
      response.print(F(            "timeout: 5000,"));
      response.print(F(            "success: function(data) {"));
      response.print(F(              "$btn.attr('class', bkpClassB);"));
      response.print(F(              "$i.attr('class', bkpClassI);"));
      response.print(F(              "$.each(data, function() {"));
      response.print(F(                "if (this.action != null) $('#' + this.id).attr('action', this.action);"));
      response.print(F(                "if (this.classB != null) $('#' + this.id).attr('class', 'btn btn-lg btn-block ' + this.classB);"));
      response.print(F(                "if (this.classI != null) $('#' + this.id).find('i').attr('class', 'fa ' + this.classI);"));
      response.print(F(              "});"));
      response.print(F(            "},"));
      response.print(F(            "error: function() {"));
      response.print(F(              "$btn.attr('class', bkpClassB);"));
      response.print(F(              "$i.attr('class', bkpClassI);"));
      response.print(F(            "}"));
      response.print(F(          "});"));
      response.print(F(        "});"));
      response.print(F(      "});"));
      response.print(F(    "</script>"));
      response.print(F(  "</head>"));
      response.print(F(  "<body style='padding-bottom: 70px;'>"));
      response.print(F(    "<div class='container'>"));
      response.print(F(      "<h1><i class='fa fa-home' onclick='window.location=\"/\";'></i> Smart Home :-)</h1><br/>"));
    
      response.print(F(      "<div class='row'>"));
      response.print(F(        "<div class='col-xs-4'>"));
      if (smartHome.isOn(RELE_1)) {
        response.print(F(          "<button id='r1' action='/rele/off/r1' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-up'></i> <span class='badge'>1</span>"));
      } else {
        response.print(F(          "<button id='r1' action='/rele/on/r1' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-down'></i> <span class='badge'>1</span>"));
      }
      response.print(F(          "</button>"));
      response.print(F(        "</div>"));
      response.print(F(        "<div class='col-xs-4'>"));
      if (smartHome.isOn(RELE_2)) {
        response.print(F(          "<button id='r2' action='/rele/off/r2' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-up'></i> <span class='badge'>2</span>"));
      } else {
        response.print(F(          "<button id='r2' action='/rele/on/r2' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-down'></i> <span class='badge'>2</span>"));
      }
      response.print(F(          "</button>"));
      response.print(F(        "</div>"));
      response.print(F(        "<div class='col-xs-4'>"));
      if (smartHome.isOn(RELE_3)) {
        response.print(F(          "<button id='r3' action='/rele/off/r3' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-up'></i> <span class='badge'>3</span>"));
      } else {
        response.print(F(          "<button id='r3' action='/rele/on/r3' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-down'></i> <span class='badge'>3</span>"));
      }
      response.print(F(          "</button>"));
      response.print(F(        "</div>"));
      response.print(F(      "</div><br/>"));
      
      response.print(F(      "<div class='row'>"));
      response.print(F(        "<div class='col-xs-4'>"));
      if (smartHome.isOn(RELE_4)) {
        response.print(F(          "<button id='r4' action='/rele/off/r4' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-up'></i> <span class='badge'>4</span>"));
      } else {
        response.print(F(          "<button id='r4' action='/rele/on/r4' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-down'></i> <span class='badge'>4</span>"));
      }
      response.print(F(          "</button>"));
      response.print(F(        "</div>"));
      response.print(F(        "<div class='col-xs-4'>"));
      if (smartHome.isOn(RELE_5)) {
        response.print(F(          "<button id='r5' action='/rele/off/r5' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-up'></i> <span class='badge'>5</span>"));
      } else {
        response.print(F(          "<button id='r5' action='/rele/on/r5' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-down'></i> <span class='badge'>5</span>"));
      }
      response.print(F(          "</button>"));
      response.print(F(        "</div>"));
      response.print(F(        "<div class='col-xs-4'>"));
      if (smartHome.isOn(RELE_6)) {
        response.print(F(          "<button id='r6' action='/rele/off/r6' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-up'></i> <span class='badge'>6</span>"));
      } else {
        response.print(F(          "<button id='r6' action='/rele/on/r6' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-down'></i> <span class='badge'>6</span>"));
      }
      response.print(F(          "</button>"));
      response.print(F(        "</div>"));
      response.print(F(      "</div><br/>"));
      
      response.print(F(      "<div class='row'>"));
      response.print(F(        "<div class='col-xs-4'>"));
      if (smartHome.isOn(RELE_7)) {
        response.print(F(          "<button id='r7' action='/rele/off/r7' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-up'></i> <span class='badge'>7</span>"));
      } else {
        response.print(F(          "<button id='r7' action='/rele/on/r7' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-down'></i> <span class='badge'>7</span>"));
      }
      response.print(F(          "</button>"));
      response.print(F(        "</div>"));
      response.print(F(        "<div class='col-xs-4'>"));
      if (smartHome.isOn(RELE_8)) {
        response.print(F(          "<button id='r8' action='/rele/off/r8' type='button' class='btn btn-lg btn-block btn-success'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-up'></i> <span class='badge'>8</span>"));
      } else {
        response.print(F(          "<button id='r8' action='/rele/on/r8' type='button' class='btn btn-lg btn-block btn-danger'>"));
        response.print(F(            "<i class='fa fa-thumbs-o-down'></i> <span class='badge'>8</span>"));
      }
      response.print(F(          "</button>"));
      response.print(F(        "</div>"));
      response.print(F(      "</div>"));
      
      response.print(F(      "<hr/>"));
      
      response.print(F(      "<button id='all' action='/rele/on/all' type='button' class='btn btn-lg btn-block btn-default'>"));
      response.print(F(        "<i class='fa fa-thumbs-o-up'></i> Ligar todos os Reles"));
      response.print(F(      "</button><br/>"));
      response.print(F(      "<button id='all' action='/rele/off/all' type='button' class='btn btn-lg btn-block btn-default'>"));
      response.print(F(        "<i class='fa fa-thumbs-o-down'></i> Desligar todos os Reles"));
      response.print(F(      "</button>"));
      
      response.print(F(    "</div>"));
      
      writeFooter(response);
      
      response.print(F(  "<body>"));
      response.print(F("</html>"));
    }
  } else if (url == "/") {
    response.print(F("<html>"));
    response.print(F(  "<head>"));
    
    writeHead(response);
    
    response.print(F(  "</head>"));
    response.print(F(  "<body style='padding-bottom: 70px;'>"));
    response.print(F(    "<div class='container'>"));
    response.print(F(      "<h1><i class='fa fa-home' onclick='window.location=\"/\";'></i> Smart Home :-)</h1><br/>"));
    
    response.print(F(      "<a href='/led' class='btn btn-lg btn-block btn-default'>"));
    response.print(F(        "<i class='fa fa-lightbulb-o'></i> LEDs"));
    response.print(F(      "</a><br/>"));
    response.print(F(      "<a href='/rele' class='btn btn-lg btn-block btn-default'>"));
    response.print(F(        "<i class='fa fa-cogs'></i> Reles"));
    response.print(F(      "</a>"));
    
    response.print(F(    "</div>"));
    
    writeFooter(response);
    
    response.print(F(  "<body>"));
    response.print(F("</html>"));
  }
  
  response.close();
}

void Server::writeHead(File response) {
  response.print(F("<meta name='viewport' content='width=device-width'>"));
  response.print(F("<link rel='stylesheet' href='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css'>"));
  response.print(F("<link rel='stylesheet' href='http://maxcdn.bootstrapcdn.com/font-awesome/4.7.0/css/font-awesome.min.css'>"));
  response.print(F("<link rel='shortcut icon' href='http://arduino.cc/favicon.ico'>"));
  response.print(F("<style>"));
  response.print(F(  ".btn:focus { outline: none; }"));
  response.print(F("</style>"));
  response.print(F("<script src='https://code.jquery.com/jquery-3.2.1.min.js'></script>"));
  response.print(F("<script src='https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/js/bootstrap.min.js'></script>"));
}

void Server::writeFooter(File response) {
  response.print(F("<div id='footer' class='container'>"));
  response.print(F(  "<nav class='navbar navbar-default navbar-fixed-bottom'>"));
  response.print(F(    "<div class='navbar-inner navbar-content-center'>"));
  response.print(F(      "<p style='margin: 15px;'>Desenvolvido por <b>Neto Rebou&ccedil;as</b></p>"));
  response.print(F(    "</div>"));
  response.print(F(  "</nav>"));
  response.print(F("</div>"));
}
