#include <ESP8266WiFi.h>

#include "EepromConfiguration.h"
#include "HttpServer.h"
#include "WifiManager.h"
#include "Dht22Sensor.h"
#include "ThingspeakClient.h"

// To read VCC
ADC_MODE(ADC_VCC);

#define TEXTIFY(A) #A
#define ESCAPEQUOTE(A) TEXTIFY(A)

#define SENSOR_PIN 13
#define SENSOR_POWER_PIN 12
#define CONFIG_INDICATOR_LED_PIN 5
#define EEPROM_SIZE 512
#define SERIAL_BAUD_RATE 115200

String buildVersion = ESCAPEQUOTE(BUILD_VERSION);

EepromConfiguration* eepromConfig;
HttpServer* httpServer;
WifiManager* wifiManager;
Dht22Sensor* dht22Sensor;
Configuration config;
ThingspeakClient* thingspeak;
bool CONFIG_MODE = true;

String getFirmwareVersion()
{
    if (buildVersion.equals("BUILD_VERSION"))
    {
        buildVersion = "0.1";
    }
    return buildVersion;
}

void restartHandler()
{
    Serial.println("Restarting..");
    httpServer->sendResponse("Restarting...");
    ESP.restart();
}

void configurationPageHandler()
{
    Serial.println("Delivering configuration page.");
    httpServer->sendResponse(config);
}

void saveConfigurationHandler()
{
    Serial.println("Updating configuration.");
    strcpy(config.ssid, httpServer->getRequestArgument("ssid").c_str());
    strcpy(config.password, httpServer->getRequestArgument("password").c_str());
    strcpy(config.identifier, httpServer->getRequestArgument("identifier").c_str());
    config.sleepInterval = atoi(httpServer->getRequestArgument("sleepInterval").c_str());
    strcpy(config.thingspeakApiKey, httpServer->getRequestArgument("thingspeakKey").c_str());
    strcpy(config.otaUrl, httpServer->getRequestArgument("otaUrl").c_str());
    eepromConfig->writeConfigurationToEeprom(config);
    httpServer->sendResponse(config);
}

void sensorDataHandler()
{
    Serial.println("Reading sensor.");
    httpServer->sendResponse(dht22Sensor->read(2));
}

void systemInfoHandler()
{
    Serial.println("Delivering system data");
    httpServer->sendResponse("{ \"VCC\": \"" + String(ESP.getVcc()) + "\", " +
        "\"SDK Version\": \"" + String(ESP.getSdkVersion()) + "\", " +
        "\"Free Heap\": \"" + String(ESP.getFreeHeap()) + "\", " +
        "\"Firmware\": \"" + String(getFirmwareVersion()) + "\", " +
        "\"Reset Reason\": \"" + String(ESP.getResetReason()) + "\"}");
}

void handleFirmwareUpdate() {
    Serial.println("Firmware update detected.");
    strcpy(config.firmware, getFirmwareVersion().c_str());
    eepromConfig->writeConfigurationToEeprom(config);
}

void setup()
{
    pinMode(CONFIG_INDICATOR_LED_PIN, OUTPUT);
    pinMode(SENSOR_POWER_PIN, OUTPUT);
    pinMode(SENSOR_PIN, INPUT);

    digitalWrite(CONFIG_INDICATOR_LED_PIN, HIGH);
    delay(500);
    digitalWrite(CONFIG_INDICATOR_LED_PIN, LOW);

    Serial.begin(SERIAL_BAUD_RATE);
    while (!Serial)
    {
        ;
    }

    Serial.println("-----------");
    Serial.println("ESP Sensor Board");
    Serial.println("-----------");
    Serial.println("Firmware Version: " + getFirmwareVersion());
    Serial.println("Reset Reason: " + ESP.getResetReason());
    delay(2000);

    eepromConfig = new EepromConfiguration(EEPROM_SIZE);
    wifiManager = new WifiManager();
    dht22Sensor = new Dht22Sensor(SENSOR_PIN, SENSOR_POWER_PIN);
    httpServer = new HttpServer();
    httpServer->addHandler("/", std::bind(&configurationPageHandler));
    httpServer->addHandler("/restart", std::bind(&restartHandler));
    httpServer->addHandler("/set_config", std::bind(&saveConfigurationHandler));
    httpServer->addHandler("/data", std::bind(&sensorDataHandler));
    httpServer->addHandler("/info", std::bind(&systemInfoHandler));

    // Reset triggerd by manual push of reset button?
    CONFIG_MODE = ESP.getResetReason().equals("External System");

    if (eepromConfig->isEepromEmpty())
    {
        Configuration defaultConfig = eepromConfig->createDefaultConfiguration();
        strcpy(defaultConfig.firmware, getFirmwareVersion().c_str());
        eepromConfig->writeConfigurationToEeprom(defaultConfig);
    }
    config = eepromConfig->readConfigurationFromEeprom();

    if (!getFirmwareVersion().equals(String(config.firmware))) {
        handleFirmwareUpdate();
    }

    thingspeak = new ThingspeakClient(config.thingspeakApiKey);
    
    if (CONFIG_MODE || !wifiManager->connectToWifi(config))
    {
        digitalWrite(CONFIG_INDICATOR_LED_PIN, HIGH);
        CONFIG_MODE = true;
        wifiManager->setupAccessPoint();
        httpServer->start();
    }
}

void process()
{
    int maxReads = 3;
    Dht22SensorResult result = dht22Sensor->read(maxReads);
    if (result.temperature != -1 && result.humidity != -1)
    {
        String data[] = { getFirmwareVersion(),
            config.identifier,
            String((float)result.temperature),
            String((float)result.humidity),
            String((int)result.numberOfReadAttemps) };
        thingspeak->sendData(data, 5);
    }
    Serial.println("Needed " + String((int)result.numberOfReadAttemps) + " attemps to read DHT22");
}

void loop()
{
    if (CONFIG_MODE)
    {
        httpServer->handleRequest();
        delay(500);
    }
    else
    {
        process();

        int sleepTime = config.sleepInterval;
        if (sleepTime > 0)
        {
            Serial.println("Sleeping for " + String((int)sleepTime) + " Minute(s)");
            ESP.deepSleep(sleepTime * 60000000, WAKE_RF_DEFAULT);
        }
        else {
            delay(60 * 1000);
        }
    }
}
