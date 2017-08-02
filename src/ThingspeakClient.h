#ifndef THINGSPEAKCLIENT_H
#define THINGSPEAKCLIENT_H

#include <ESP8266HTTPClient.h>
#include "HardwareSerial.h"

class ThingspeakClient
{
    private:
        const char* host = "api.thingspeak.com";
        String apiKey;
        HTTPClient client;

    public:
        ThingspeakClient(String key)
        {
            apiKey = key;
        }

        void sendData(float humidity, float temperature)
        {
            String postStr = apiKey;
            postStr +="&field1=";
            postStr += String((float)temperature);
            postStr +="&field2=";
            postStr += String((float)humidity);

            client.begin(host);
            client.addHeader("X-THINGSPEAKAPIKEY", apiKey);
            client.addHeader("Content-Type", "application/x-www-form-urlencoded");

            if(int httpCode = client.POST(postStr) != 200)
            {
                Serial.println("Error sending http request, status code: " + httpCode);
            }
            client.end();
        }
};

#endif
