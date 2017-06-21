#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "WString.h"

#define REQUEST_TYPE_SET_CONFIG 0
#define REQUEST_TYPE_RESTART 1
#define REQUEST_TYPES_SIZE 2

class HttpServer
{
    private:
        String pageTitle = "ESP8266 Configuration";
        String requestAction[REQUEST_TYPES_SIZE];

    public:
        HttpServer();

        // Parses the request for given param name.
        // Returns the paramter value if availabe, otherwise emtpy string.
        String getRequestParameter(String request, String param);

        // Parses the request and eturns the request type
        int getRequestType(String request);

        String htmlHead();
};

#endif
