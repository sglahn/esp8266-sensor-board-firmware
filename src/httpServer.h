#include "WString.h"
#include "resources.h"

#define REQUEST_TYPE_SET_CONFIG 0
#define REQUEST_TYPE_RESTART 1
#define REQUEST_TYPES_SIZE 2
String request_action[REQUEST_TYPES_SIZE];

String PAGE_TITLE = "ESP8266 Configuration";

/*
 * Parses the request for given param name.
 * Returns the paramter value if availabe, otherwise emtpy string.
 */
String getRequestParameter(String request, String param)
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

/*
 * Parses the request and eturns the request type
 */
int getRequestType(String request)
{
  for (int i=0; i<REQUEST_TYPES_SIZE; i++)
  {
    if (request.indexOf(request_action[i]) != -1)
      return i;
  }
  return -1;
}

String htmlHead()
{
  return "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>";
}
