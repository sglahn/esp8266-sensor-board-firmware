#include "Dht22Sensor.h"

Dht22Sensor::Dht22Sensor(int pin)
{
    dht = new DHT(pin, DHTTYPE, 15);
}

Dht22SensorResult Dht22Sensor::read(int attemps)
{
    float humidity = dht->readHumidity();
    float temperature = dht->readTemperature(false);

    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("Failed to read from DHT sensor!");
        if (attemps < 20)
        {
            delay(3000);
            return read(attemps +1);
        }
        else {
            Serial.println("Giving up!");
            temperature = 0;
            humidity = 0;
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
