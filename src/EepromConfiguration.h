#ifndef EEPROMCONFIGURATION_H_
#define EEPROMCONFIGURATION_H_

#include <EEPROM.h>
#include "HardwareSerial.h"

struct Configuration
{
    char firmware[21]; // firmware version
    char ssid[33]; // WLAN ssid
    char password[33]; // WLAN password
    char identifier[11]; // Identifier of board
    int sleepInterval; // Deep Sleep interval (Minutes)
    char otaUrl[33]; // Url to download Over-the-Air firmware updates
    char thingspeakApiKey[17]; // API Key for Thingspeak.com
};

class EepromConfiguration
{
    private:
        int eeStartAddress = 0;
        int eeSize = 512;

    public:
        EepromConfiguration();

        // Checks if EEPROM is empty ro not.
        bool isEepromEmpty();

	    // Erase data in EEPROM
        void eraseEeprom();

        // Creates and returns valid default configuration
        Configuration createDefaultConfiguration();

        // Reads and returns the configuration from EEPROM.
        Configuration readConfigurationFromEeprom();

        // Writes the configuration to EEPROM.
        void writeConfigurationToEeprom(Configuration config);
};

#endif
