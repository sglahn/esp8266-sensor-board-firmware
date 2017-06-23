#include "HttpServer.h"

HttpServer::HttpServer()
{
    requestAction[REQUEST_TYPE_SET_CONFIG] = "/set_config";
    requestAction[REQUEST_TYPE_RESTART] = "/restart";
    server = new WiFiServer(serverPort);
}

void HttpServer::start()
{
    server->begin();
}

String HttpServer::handleRequest() {
    client = server->available();
    if (!client)
    {
        return "";
    }
    while(!client.available())
    {
        delay(1);
    }
    String request = client.readStringUntil('\r');
    client.flush();
    return request;
}

void HttpServer::sendResponse(String response)
{
    client.print(response);
}

String HttpServer::getRequestParameter(String request, String param)
{
  request.replace(" HTTP/1.1", "");
  int startIndex = request.indexOf(param) + param.length() +1;
  if (startIndex != -1)
  {
    String value = request.substring(startIndex, request.length());
    int andIndex = value.indexOf("&");
    if (andIndex != -1)
    {
       return value.substring(0, andIndex);
    }
    return value;
  }
  return "";
}

int HttpServer::getRequestType(String request)
{
  for (int i=0; i<REQUEST_TYPES_SIZE; i++)
  {
    if (request.indexOf(requestAction[i]) != -1)
      return i;
  }
  return -1;
}

String HttpServer::htmlHead()
{
  return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>";
}
