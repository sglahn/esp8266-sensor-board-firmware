#include <ESP8266WiFi.h>

#include "EEPROMConfiguration.h"
#include "httpServer.h"
#include "wifiManager.h"

#define STATUS_LED 2

WiFiServer server(AP_SERVER_PORT);

EEPROMConfiguration *eepromConfig = new EEPROMConfiguration();

void setup()
{
  request_action[REQUEST_TYPE_SET_CONFIG] = "/set_config";
  request_action[REQUEST_TYPE_RESTART] = "/restart";

  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  EEPROM.begin(512);
  if (eepromConfig->isEepromEmpty())
  {
    eepromConfig->writeConfigurationToEeprom(eepromConfig->createDefaultConfiguration());
  }

  Configuration config = eepromConfig->readConfigurationFromEeprom();

  if (!connectToWifi(config)) {
    setupAccessPoint();
  }

  server.begin();
}

void loop()
{
  Configuration config = eepromConfig->readConfigurationFromEeprom();

  ESP.deepSleep(config.sleepInterval * 60000, WAKE_RF_DEFAULT);

  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  while(!client.available())
  {
    delay(1);
  }

  String req = client.readStringUntil('\r');
  Serial.println(req);

  client.flush();

  String response = "";
  int requestType = getRequestType(req);

  if (requestType == REQUEST_TYPE_SET_CONFIG)
  {
    String ssid = getRequestParameter(req, "ssid");
    String password = getRequestParameter(req, "password");
    String identifier = getRequestParameter(req, "identifier");
    String sleepInterval = getRequestParameter(req, "sleepInterval");
    String otaUrl = getRequestParameter(req, "otaUrl");
    String otaUpdateInterval = getRequestParameter(req, "otaUpdateInterval");
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
    response = htmlHead() + htmlConfigurationForm(config.ssid, config.password, config.identifier, config.sleepInterval, config.otaUrl, config.otaUpdateInterval);
  }
  else if (requestType == REQUEST_TYPE_RESTART)
  {
    Serial.println("Restarting...");
    ESP.restart();
  }
  else
  {
    response = htmlHead() + htmlConfigurationForm(config.ssid, config.password, config.identifier, config.sleepInterval, config.otaUrl, config.otaUpdateInterval);
  }

  client.print(response);
  delay(1);
}
