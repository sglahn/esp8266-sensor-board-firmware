#include <ESP8266WiFi.h>

#define AP_SERVER_PORT 80
#define AP_PSK "password"

/*
 * Tries to connect to Wifi with credentials given in config. Uses exponential wait.
 * Returns false if connection times out
 * Returns true if connection was successful
*/
bool connectToWifi(Configuration config)
{
  Serial.println("Connecting to WLAN");
  WiFi.begin(config.ssid, config.password);
  for (int i=0; i<10; i++)
  {
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println("WiFi connected.");
      //blinkStatusLed();
      return true;
    }
    delay(i * 1000);
  }
  Serial.println("WiFi timed out.");
  return false;
}

/*
 * Creates Access Point SSID, prefixed by ESP8266-, suffixed by the last parts of Mac Adress to get some uniqueness.
*/
String createSSID()
{
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  return "ESP8266-" + macID;
}

/*
 * Creates an access point.
 */
void setupAccessPoint()
{
  WiFi.mode(WIFI_AP);
  WiFi.softAP(createSSID().c_str(), AP_PSK);
  Serial.println("Successfully set up access point.");
}
