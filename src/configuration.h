#include <EEPROM.h>
#include "HardwareSerial.h"

/*
 * Configuration is stored in EEPROM
*/
struct Configuration
{
  // WLAN ssid
  char ssid[32];
  // WLAN password
  char password[32];
  // Identifier of board
  char identifier[32];
  // Deep Sleep interval (Minutes)
  int sleepInterval;
  // Url to download Over-the-Air firmware updates
  char otaUrl[32];
  // Interval of checking for ota updates (hours)
  int otaUpdateInterval;
};

int eeStartAddress = 0;

/*
 * Checks if EEPROM is empty ro not.
 */
bool isEepromEmpty()
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

/*
 * Creates valid default configuration for faster testing and development.
 */
Configuration createDefaultConfiguration()
{
  return Configuration {
    "",
    "",
    "",
    0,
    "",
    0
  };
}

/*
 * Reads and returns the configuration from EEPROM.
 */
Configuration readConfigurationFromEeprom()
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

/*
 * Writes the configuration to the EEPROM.
 */
void writeConfigurationToEeprom(Configuration config)
{
  EEPROM.put(eeStartAddress, config);
  EEPROM.commit();
}
