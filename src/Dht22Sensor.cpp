#include "Dht22Sensor.h"

Dht22Sensor::Dht22Sensor(int pin)
{
    dht = new DHT(pin, DHTTYPE, 15);
    dht->begin();
    delay(1000);
    Serial.println("DHT22 Sensor initialized.");
}

Dht22SensorResult Dht22Sensor::read(int maxNumberAttemps, int attemps)
{
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

    return Dht22SensorResult
    {
        humidity,
        temperature,
        attemps
    };
}
