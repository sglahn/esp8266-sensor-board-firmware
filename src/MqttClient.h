#ifndef MQTTCLIENT_H
#define MQTTCLIENT_H

#include "WString.h"
#include <PubSubClient.h>
#include <HashMap.h>
#include "HardwareSerial.h"

class MqttClient
{
    private:
        String server;
        int port;
        PubSubClient* client;
        WiFiClient wifiClient;
        String name;
        String clientId;
        bool retained;
        String username;
        String password;

     public:
        MqttClient(String mqttBroker, String identifier, String brokerUser, String brokerPassword, bool retained = true)
        {
            String url = mqttBroker;           
            if (url.indexOf(':') > -1) 
            {
                server = url.substring(0, url.indexOf(':')).c_str();
                port = url.substring(url.indexOf(':') +1, url.length()).toInt();
            }
            else {
                server = url;
                port = 1883;
            }
            name = identifier;
            this->retained = retained;
            username = brokerUser;
            password = brokerPassword;
            clientId = name + "-" + WiFi.macAddress();
            client = new PubSubClient(server.c_str(), port, wifiClient);
        }

        void sendData(HashMap<String, String> &data) 
        {
            if (server.length() == 0) 
            {
                return;
            }
            
            Serial.println("Connecting to MQTT broker: " + server + ":" + String(port));
            bool connected = false;
            if (username.length() > 0 && password.length() > 0) 
            {
                connected = client->connect(clientId.c_str(), username.c_str(), password.c_str());
            }
            else 
            {
                connected = client->connect(clientId.c_str());
            }
            if (connected)
            {
                for(unsigned int i=0; i<data.count(); i++)
                {
                    String topic = name + "/" + data.keyAt(i);
                    String payload = data.valueAt(i);
                    if (client->publish(topic.c_str(), payload.c_str(), retained))
                    {
                        Serial.println("Successfully published payload: " + payload + " to topic: " + topic);
                    }
                    else 
                    {
                        Serial.println("Publish of payload " + payload + " failed for topic: " + topic);
                    }
                }
                client->disconnect();
            }
            else
            {
                Serial.println("Connection to MQTT broker failed.");
            }
        }
};

#endif