#ifndef WIFIMANAGER_H_
#define WIFIMANAGER_H_

#include <ESP8266WiFi.h>
#include "EepromConfiguration.h"

class WifiManager
{
    public:
        WifiManager();

        // Tries to connect to Wifi with credentials given in config. Uses exponential wait.
        // Returns false if connection times out
        // Returns true if connection was successful
        bool connectToWifi(Configuration config);

        void disconnect();

        // Creates an access point.
        void setupAccessPoint();

    private:
        char* apPassword;

        // Creates Access Point SSID, prefixed by ESP8266-, suffixed by the last parts of Mac Adress to get some uniqueness.
        String createSSID();
};

#endif
