#ifndef Server_h
#define Server_h

#include "Arduino.h"
#include "ESP8266.h"
#include "Logger.h"

class Server {
  public:
    static String ssid;
    static String pwd;
    static String port;
    void init();
    void loadConf();
    void saveConf();
    void disconnect();
    String connect(int retry = 0);
    boolean stopServer();
    boolean startServer();    
    String getIP();
    String getStatus();
    boolean httpClose();
    
    void processLine(String line);
};

extern Server server;

#endif
