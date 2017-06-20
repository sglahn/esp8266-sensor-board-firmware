#include "EEPROMConfiguration.h"

bool EEPROMConfiguration::isEepromEmpty()
{
    for(int i=eeStartAddress; i<512; i++)
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

void EEPROMConfiguration::eraseEeprom() 
{
    for (int i=eeStartAddress; i<EEPROM.length(); i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
}

Configuration EEPROMConfiguration::createDefaultConfiguration()
{
    return Configuration
    {
        "", 
	"", 
	"", 
	0, 
	"", 
	0
    };
}

Configuration EEPROMConfiguration::readConfigurationFromEeprom()
{
    Configuration config;
    EEPROM.get(eeStartAddress, config);
    Serial.println("Reading configuration from EEPROM:");
    Serial.println(config.ssid);
    Serial.println(config.password);
    Serial.println(config.identifier);
    Serial.println(config.sleepInterval);
    Serial.println(config.otaUrl);
    Serial.println(config.otaUpdateInterval);
    return config;
}

void EEPROMConfiguration::writeConfigurationToEeprom(Configuration config)
{
    EEPROM.put(eeStartAddress, config);
    EEPROM.commit();
}

