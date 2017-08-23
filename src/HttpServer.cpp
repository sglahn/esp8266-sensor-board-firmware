#include "HttpServer.h"

HttpServer::HttpServer()
{
    server = new ESP8266WebServer(serverPort);
}

void HttpServer::start()
{
    server->begin();
}

void HttpServer::addHandler(String uri, HandlerFunction handler)
{
    server->on(uri.c_str(), handler);
}

String HttpServer::handleRequest() {
    server->handleClient();
}

void HttpServer::sendResponse(Configuration configuration)
{
    String page = FPSTR(HTTP_HEAD);
    page += FPSTR(HTTP_SCRIPT);
    page += FPSTR(HTTP_STYLE);
    String body = FPSTR(HTTP_BODY);
    body.replace("{{firmware}}", configuration.firmware);
    body.replace("{{ssid}}", configuration.ssid);
    body.replace("{{password}}", configuration.password);
    body.replace("{{identifier}}", configuration.identifier);
    body.replace("{{sleepInterval}}", String((int)configuration.sleepInterval).c_str());
    body.replace("{{thingspeakApiKey}}", configuration.thingspeakApiKey);
    body.replace("{{otaUrl}}", configuration.otaUrl);
    page += body;
    page += FPSTR(HTTP_FOOT);

    server->sendHeader("Content-Length", String(page.length()));
    server->send(200, "text/html", page);
}

void HttpServer::sendResponse(Dht22SensorResult sensorResult)
{
    String page = "{ \"Temperature\": \"" + String((float)sensorResult.temperature) + "\","
        + "\"Humidity\":" + "\"" + String((float)sensorResult.humidity) + "\"}";
    server->sendHeader("Content-Length", String(page.length()));
    server->send(200, "text/html", page);
}

String HttpServer::getRequestArgument(String name)
{
    return server->arg(name);
}
