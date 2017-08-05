#include "EepromConfiguration.h"

EepromConfiguration::EepromConfiguration()
{
    EEPROM.begin(512);
}

bool EepromConfiguration::isEepromEmpty()
{
    for(int i=eeStartAddress; i<eeSize; i++)
    {
        int b = EEPROM.read(i);
        if (b != 255)
        {
            Serial.println("EEPROM is non-empty");
            return false;
        }
    }
    Serial.println("EEPROM is empty");
    return true;
}

void EepromConfiguration::eraseEeprom()
{
    for (int i=eeStartAddress; i<eeSize; i++)
    {
        EEPROM.write(i, 255);
    }
    EEPROM.commit();
}

Configuration EepromConfiguration::createDefaultConfiguration()
{
    return Configuration
    {
        "",
	    "",
        "",
	    0,
	    "",
        ""
    };
}

Configuration EepromConfiguration::readConfigurationFromEeprom()
{
    Configuration config;
    EEPROM.get(eeStartAddress, config);
    Serial.println("Reading configuration from EEPROM:");
    Serial.println(config.ssid);
    Serial.println(config.password);
    Serial.println(config.identifier);
    Serial.println(config.sleepInterval);
    Serial.println(config.otaUrl);
    Serial.println(config.thingspeakApiKey);
    return config;
}

void EepromConfiguration::writeConfigurationToEeprom(Configuration config)
{
    EEPROM.put(eeStartAddress, config);
    EEPROM.commit();
}
