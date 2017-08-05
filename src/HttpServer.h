#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "WString.h"
#include <ESP8266WiFi.h>
#include "EepromConfiguration.h"
#include "ConfigurationTemplate.h"
#include <ESP8266WebServer.h>

class HttpServer
{
    private:
        int serverPort = 80;
        ESP8266WebServer* server;
        WiFiClient client;

    public:
        typedef std::function<void(void)> HandlerFunction;

    public:
        HttpServer();

        void start();

        String handleRequest();

        void sendResponse(Configuration configuration);

        void addHandler(String uri, HandlerFunction handler);

        String getRequestArgument(String name);
};

#endif
