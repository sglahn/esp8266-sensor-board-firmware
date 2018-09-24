#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "WString.h"
#include <ESP8266WiFi.h>
#include "EepromConfiguration.h"
#include "Dht22Sensor.h"
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

        void handleRequest();

        void sendResponse(Configuration configuration);

        void sendResponse(Dht22SensorResult sensorResult);

        void sendResponse(String message);

        void addHandler(String uri, HandlerFunction handler);

        String getRequestArgument(String name);
};

#endif
