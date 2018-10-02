#include "EepromConfiguration.h"

EepromConfiguration::EepromConfiguration(int size)
{
    eeStartAddress = 0;
    eeSize = size;
}

bool EepromConfiguration::isEepromEmpty()
{
    EEPROM.begin(eeSize);
    delay(10);
    for(int i=eeStartAddress; i<eeSize; i++)
    {
        int b = EEPROM.read(i);
        if (b != 255)
        {
            Serial.println("EEPROM is non-empty");
            return false;
        }
    }
    EEPROM.end();
    Serial.println("EEPROM is empty");
    return true;
}

void EepromConfiguration::eraseEeprom()
{
    EEPROM.begin(eeSize);
    delay(10);
    for (int i=eeStartAddress; i<eeSize; i++)
    {
        EEPROM.write(i, 255);
    }
    EEPROM.commit();
    EEPROM.end();
}

Configuration EepromConfiguration::createDefaultConfiguration()
{
    return Configuration
    {
        "",
        "",
	    "",
        "",
	    0,
	    "",
        "",
        "",
        ""
    };
}

Configuration EepromConfiguration::readConfigurationFromEeprom()
{
    Configuration config;
    EEPROM.begin(eeSize);
    delay(10);
    EEPROM.get(eeStartAddress, config);
    EEPROM.end();
    Serial.println("Reading configuration from EEPROM:");
    Serial.println(config.firmware);
    Serial.println(config.ssid);
    Serial.println(config.password);
    Serial.println(config.identifier);
    Serial.println(config.sleepInterval);
    Serial.println(config.otaUrl);
    Serial.println(config.mqttBrokerUrl);
    Serial.println(config.mqttUser);
    Serial.println(config.mqttPassword);
    return config;
}

void EepromConfiguration::writeConfigurationToEeprom(Configuration config)
{
    Serial.println("Writing " + String((int)sizeof(config)) + "bytes to EEPROM.");
    EEPROM.begin(eeSize);
    delay(10);
    EEPROM.put(eeStartAddress, config);
    EEPROM.commit();
    EEPROM.end();
}
