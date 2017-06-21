#include <ESP8266WiFi.h>

#include "EepromConfiguration.h"
#include "HttpServer.h"
#include "WifiManager.h"
#include "resources.h"

#define STATUS_LED 2



EepromConfiguration* eepromConfig;
HttpServer* httpServer;
WifiManager* wifiManager;
Configuration config;
bool CONFIG_MODE = true;

void setup()
{
  eepromConfig = new EepromConfiguration();
  httpServer = new HttpServer();
  wifiManager = new WifiManager();

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  if (eepromConfig->isEepromEmpty())
  {
    eepromConfig->writeConfigurationToEeprom(eepromConfig->createDefaultConfiguration());
  }
  config = eepromConfig->readConfigurationFromEeprom();

  if (!wifiManager->connectToWifi(config)) {
    wifiManager->setupAccessPoint();
    httpServer->start();
  }
}

void loop()
{
  //ESP.deepSleep(config.sleepInterval * 60000, WAKE_RF_DEFAULT);

  if (CONFIG_MODE)
  {
      String req = httpServer->handleRequest();
      Serial.println(req);

      String response = "";
      int requestType = httpServer->getRequestType(req);

      if (requestType == REQUEST_TYPE_SET_CONFIG)
      {
        String ssid = httpServer->getRequestParameter(req, "ssid");
        String password = httpServer->getRequestParameter(req, "password");
        String identifier = httpServer->getRequestParameter(req, "identifier");
        String sleepInterval = httpServer->getRequestParameter(req, "sleepInterval");
        String otaUrl = httpServer->getRequestParameter(req, "otaUrl");
        String otaUpdateInterval = httpServer->getRequestParameter(req, "otaUpdateInterval");
        if (ssid.length() > 0)
        {
          struct Configuration config;
          strcpy(config.ssid, ssid.c_str());
          strcpy(config.password, password.c_str());
          strcpy(config.identifier, identifier.c_str());
          config.sleepInterval = atoi(sleepInterval.c_str());
          strcpy(config.otaUrl, otaUrl.c_str());
          config.otaUpdateInterval = atoi(otaUpdateInterval.c_str());
          eepromConfig->writeConfigurationToEeprom(config);
        }
        response = httpServer->htmlHead() + htmlConfigurationForm(config.ssid, config.password, config.identifier, config.sleepInterval, config.otaUrl, config.otaUpdateInterval);
      }
      else if (requestType == REQUEST_TYPE_RESTART)
      {
        Serial.println("Restarting...");
        ESP.restart();
      }
      else
      {
        response = httpServer->htmlHead() + htmlConfigurationForm(config.ssid, config.password, config.identifier, config.sleepInterval, config.otaUrl, config.otaUpdateInterval);
      }
      httpServer->sendResponse(response);

      delay(1);
  }
}
