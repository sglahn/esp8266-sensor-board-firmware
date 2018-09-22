#include "Dht22Sensor.h"

Dht22Sensor::Dht22Sensor(int dataPin, int powerPin)
{
    this->powerPin = powerPin;
    dht = new DHT(dataPin, DHTTYPE);

    Serial.println("Initialized DHT22 Sensor, Data Pin: " + String((int)dataPin) + ", Power Pin: " +String((int)powerPin));
}

Dht22SensorResult Dht22Sensor::read(int maxNumberAttemps, int attemps)
{
    digitalWrite(powerPin, HIGH);
    delay(1000);
    dht->begin();

    float humidity = dht->readHumidity();
    float temperature = dht->readTemperature(false);

    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println(String((int)attemps) + " read(s) from DHT sensor failed! Temperatur: " + String((float)temperature) + " Humidity: " + String((float)humidity));
        if (attemps < maxNumberAttemps)
        {
            delay(2000);
            return read(maxNumberAttemps, attemps +1);
        }
        else
        {
            Serial.println("Giving up!");
            temperature = -1;
            humidity = -1;
        }
    }
    Serial.println("DHT22: " + String((float)temperature) + "C");
    Serial.println("DHT22: " + String((float)humidity) + "%");

    digitalWrite(powerPin, LOW);

    return Dht22SensorResult
    {
        humidity,
        temperature,
        attemps
    };
}
