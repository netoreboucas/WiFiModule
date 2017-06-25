#ifndef Server_h
#define Server_h

#include "Arduino.h"
#include "Display.h"
#include "ESP8266.h"
#include "Logger.h"
#include "SmartHome.h"

class Server {
  public:
    static String ssid;
    static String pwd;
    static String port;
    
    void init();
    void loadConf();
    void saveConf();
    
    boolean connectToNetwork();
    boolean disconnectFromNetwork();
    
    boolean startServer();    
    boolean stopServer();
    
    String getIP();
    String getStatus();
    
    void processLine(String line);

  private:
    void processHttpRequest(String channel, String method, String url);
    void closeHttpResponse(String channel);
    
    void sendHttpResponseBuffer(String channel);
    void createHttpResponseBuffer(String channel, String url);
    void writeHead(File response);
    void writeFooter(File response);
};

extern Server server;

#endif
