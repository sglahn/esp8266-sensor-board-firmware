#include <ESP8266WiFi.h>

#include "EepromConfiguration.h"
#include "HttpServer.h"
#include "WifiManager.h"
#include "resources.h"
#include "Dht22Sensor.h"

EepromConfiguration* eepromConfig;
HttpServer* httpServer;
WifiManager* wifiManager;
Configuration config;
Dht22Sensor* dht22Sensor;
bool CONFIG_MODE = false;

void setup()
{
  eepromConfig = new EepromConfiguration();
  httpServer = new HttpServer();
  wifiManager = new WifiManager();
  dht22Sensor = new Dht22Sensor(5);

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  Serial.println("Booting...");
  if (eepromConfig->isEepromEmpty())
  {
    eepromConfig->writeConfigurationToEeprom(eepromConfig->createDefaultConfiguration());
  }
  config = eepromConfig->readConfigurationFromEeprom();

  if (!wifiManager->connectToWifi(config))
  {
      CONFIG_MODE = true;
      wifiManager->setupAccessPoint();
      httpServer->start();
  }
}

void readSensor() {
    Serial.println("Reading sensor data");
    Dht22SensorResult result = dht22Sensor->readSensor();
    Serial.println(String((float)result.temperature) + "C");
    Serial.println(String((float)result.humidity) + "%");
}

void loop()
{
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
  else
  {
      readSensor();

      int sleepTime = 2;//config.sleepInterval;
      if (sleepTime > 0)
      {
          Serial.print("Sleeping  ");
          Serial.print(sleepTime);
          Serial.print(" Minutes");
          ESP.deepSleep(sleepTime * 60000000, WAKE_RF_DEFAULT);
      }
      else {
          delay(1000);
      }
  }
}
