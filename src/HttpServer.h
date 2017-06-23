#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "WString.h"
#include <ESP8266WiFi.h>

#define REQUEST_TYPE_SET_CONFIG 0
#define REQUEST_TYPE_RESTART 1
#define REQUEST_TYPES_SIZE 2
//#define AP_SERVER_PORT 80

class HttpServer
{
    private:
        String pageTitle = "ESP8266 Configuration";
        String requestAction[REQUEST_TYPES_SIZE];
        int serverPort = 80;
        WiFiServer* server;
        WiFiClient client;

    public:
        HttpServer();

        void start();

        String handleRequest();

        void sendResponse(String response);

        // Parses the request for given param name.
        // Returns the paramter value if availabe, otherwise emtpy string.
        String getRequestParameter(String request, String param);

        // Parses the request and eturns the request type
        int getRequestType(String request);

        String htmlHead();
};

#endif
