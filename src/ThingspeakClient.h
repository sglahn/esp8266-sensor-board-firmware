#ifndef THINGSPEAKCLIENT_H
#define THINGSPEAKCLIENT_H

#include <ESP8266HTTPClient.h>
#include "HardwareSerial.h"

class ThingspeakClient
{
    private:
        const char* host = "api.thingspeak.com";
        int port = 80;
        String apiKey;
        WiFiClient client;

    public:
        ThingspeakClient(String key)
        {
            apiKey = key;
        }

        void sendData(String fields[], int numberOfFields) {
            String postStr = apiKey;
            for(int i=0; i<numberOfFields; i++)
            {
                postStr +="&field" + String((int)i+1) + "=";
                postStr += fields[i];
            }
            postStr.replace(" ", "%20");

            if (client.connect(host, port)) {
                client.print("POST /update HTTP/1.1\n");
                client.print("Host: " + String(host) + "\n");
                client.print("Connection: close\n");
                client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
                client.print("Content-Type: application/x-www-form-urlencoded\n");
                client.print("Content-Length: ");
                client.print(postStr.length());
                client.print("\n\n");
                client.print(postStr);
            }
            else {
                Serial.println("Failed to connect to Thingspeak ");
            }
            client.stop();
        }
};

#endif
