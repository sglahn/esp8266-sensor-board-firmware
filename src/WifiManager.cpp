#include "WifiManager.h"

WifiManager::WifiManager()
{
    // Super secret default password
    apPassword = "password";
}

bool WifiManager::connectToWifi(Configuration config)
{
    Serial.println("Connecting to WLAN");
    if (strlen(config.ssid) == 0) {
        Serial.println("ERROR: No SSID given.");
        return false;
    }

    //WiFi.persistent(false);
    //WiFi.disconnect(true);
    WiFi.begin(config.ssid, config.password);
    for (int i=0; i<10; i++)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("WiFi connected.");
            return true;
        }
        delay(i * 1000);
    }
    Serial.println("WiFi timed out.");
    return false;
}

String WifiManager::createSSID()
{
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    WiFi.softAPmacAddress(mac);
    String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) + String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    macID.toUpperCase();
    return "ESP8266-" + macID;
}

void WifiManager::setupAccessPoint()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(createSSID().c_str(), apPassword);
    Serial.println("Successfully set up access point.");
}
