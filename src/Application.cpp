#include <ESP8266WiFi.h>

#include "EepromConfiguration.h"
#include "HttpServer.h"
#include "WifiManager.h"
#include "resources.h"
#include "Dht22Sensor.h"
#include "ThingspeakClient.h"

EepromConfiguration* eepromConfig;
HttpServer* httpServer;
WifiManager* wifiManager;
Dht22Sensor* dht22Sensor;
Configuration config;
ThingspeakClient* thingspeak;
bool CONFIG_MODE = false;

void setup()
{
  eepromConfig = new EepromConfiguration();
  httpServer = new HttpServer();
  wifiManager = new WifiManager();
  thingspeak = new ThingspeakClient(config.thingspeakApiKey);
  dht22Sensor = new Dht22Sensor(5);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  if (eepromConfig->isEepromEmpty())
  {
    eepromConfig->writeConfigurationToEeprom(eepromConfig->createDefaultConfiguration());
  }
  config = eepromConfig->readConfigurationFromEeprom();

  //if (!wifiManager->connectToWifi(config))
  {
      CONFIG_MODE = true;
      wifiManager->setupAccessPoint();
      httpServer->start();
  }
}

void process() {
    Dht22SensorResult result = dht22Sensor->read(0);
    thingspeak->sendData(result.temperature, result.humidity);
    Serial.println("Needed " + String((int)result.numberOfReadAttemps) + " to read DHT22");
}

void loop()
{
  if (CONFIG_MODE)
  {
      String req = httpServer->handleRequest();
      Serial.println(req);

      String response;
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
        response = httpServer->htmlHead() + htmlConfigurationForm(config.ssid, config.password, config.identifier, config.sleepInterval, config.otaUrl, config.otaUpdateInterval, config.thingspeakApiKey);
      }
      else if (requestType == REQUEST_TYPE_RESTART)
      {
        Serial.println("Restarting...");
        ESP.restart();
      }
      else
      {
        response = httpServer->htmlHead() + htmlConfigurationForm(config.ssid, config.password, config.identifier, config.sleepInterval, config.otaUrl, config.otaUpdateInterval, config.thingspeakApiKey);
      }
      httpServer->sendResponse(response);

      delay(1);
  }
  else
  {
      process();

      int sleepTime = config.sleepInterval;
      if (sleepTime > 0)
      {
          Serial.println("Sleeping for " + String((int)sleepTime) + " Minutes");
          ESP.deepSleep(sleepTime * 60000000, WAKE_RF_DEFAULT);
      }
      else {
          delay(60 * 1000);
      }
  }
}
