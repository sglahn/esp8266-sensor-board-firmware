#include <ESP8266WiFi.h>

#include "EepromConfiguration.h"
#include "HttpServer.h"
#include "WifiManager.h"
#include "Dht22Sensor.h"
#include "ThingspeakClient.h"

#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)

String buildVersion = ESCAPEQUOTE(BUILD_VERSION);

EepromConfiguration* eepromConfig;
HttpServer* httpServer;
WifiManager* wifiManager;
Dht22Sensor* dht22Sensor;
Configuration config;
ThingspeakClient* thingspeak;
bool CONFIG_MODE = false;

void restartHandler()
{
    ESP.restart();
}

void configurationPageHandler()
{
    httpServer->sendResponse(config);
}

void saveConfigurationHandler()
{
    strcpy(config.ssid, httpServer->getRequestArgument("ssid").c_str());
    strcpy(config.password, httpServer->getRequestArgument("password").c_str());
    strcpy(config.identifier, httpServer->getRequestArgument("identifier").c_str());
    config.sleepInterval = atoi(httpServer->getRequestArgument("sleepInterval").c_str());
    strcpy(config.thingspeakApiKey, httpServer->getRequestArgument("thingspeakKey").c_str());
    strcpy(config.otaUrl, httpServer->getRequestArgument("otaUrl").c_str());
    eepromConfig->writeConfigurationToEeprom(config);
}

String getFirmwareVersion()
{
    if (buildVersion.equals("BUILD_VERSION"))
    {
        buildVersion = "0.0.1";
    }
    return "ESP8266 Sensor v " + buildVersion;
}

void setup()
{
    eepromConfig = new EepromConfiguration();
    wifiManager = new WifiManager();
    thingspeak = new ThingspeakClient(config.thingspeakApiKey);
    dht22Sensor = new Dht22Sensor(5);
    httpServer = new HttpServer();
    httpServer->addHandler("/", std::bind(&configurationPageHandler));
    httpServer->addHandler("/restart", std::bind(&restartHandler));
    httpServer->addHandler("/set_config", std::bind(&saveConfigurationHandler));

    Serial.begin(9600);
    while (!Serial)
    {
        ;
    }

    if (eepromConfig->isEepromEmpty())
    {
        Configuration defaultConfig = eepromConfig->createDefaultConfiguration();
        strcpy(defaultConfig.firmware, getFirmwareVersion().c_str());
        eepromConfig->writeConfigurationToEeprom(defaultConfig);
    }
    config = eepromConfig->readConfigurationFromEeprom();

    if (!wifiManager->connectToWifi(config))
    {
        CONFIG_MODE = true;
        wifiManager->setupAccessPoint();
        httpServer->start();
    }
}

void process()
{
    Dht22SensorResult result = dht22Sensor->read(0);
    thingspeak->sendData(result.temperature, result.humidity);
    Serial.println("Needed " + String((int)result.numberOfReadAttemps) + " to read DHT22");
}

void loop()
{
    if (CONFIG_MODE)
    {
        httpServer->handleRequest();
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
