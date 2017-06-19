#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <EEPROM.h>
#include "HardwareSerial.h"

/*
 * Configuration is stored in EEPROM
*/
struct Configuration
{
    char ssid[32]; // WLAN ssid
    char password[32]; // WLAN password
    char identifier[32]; // Identifier of board
    int sleepInterval; // Deep Sleep interval (Minutes)
    char otaUrl[32]; // Url to download Over-the-Air firmware updates
    int otaUpdateInterval; // Interval of checking for ota updates (hours)
};

class EEPROMConfiguration
{
    private:
        int eeStartAddress = 0;

    public:
        /*
         * Checks if EEPROM is empty ro not.
         */
        bool isEepromEmpty();

        /*
         * Creates valid default configuration for faster testing and development.
         */
        Configuration createDefaultConfiguration();

        /*
         * Reads and returns the configuration from EEPROM.
         */
        Configuration readConfigurationFromEeprom();

        /*
         * Writes the configuration to EEPROM.
         */
        void writeConfigurationToEeprom(Configuration config);
};

#endif
