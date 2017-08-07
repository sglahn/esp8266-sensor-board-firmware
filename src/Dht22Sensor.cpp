#include "Dht22Sensor.h"

Dht22Sensor::Dht22Sensor(int pin)
{
    dht = new DHT(pin, DHTTYPE, 15);
}

Dht22SensorResult Dht22Sensor::read(int maxNumberAttemps, int attemps)
{
    float humidity = dht->readHumidity();
    float temperature = dht->readTemperature(false);

    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println(String((int)attemps +1) + " reads from DHT sensor failed!");
        if (attemps < maxNumberAttemps)
        {
            delay(500);
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

    return Dht22SensorResult
    {
        humidity,
        temperature,
        attemps
    };
}
