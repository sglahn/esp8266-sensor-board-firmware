#include "Dht22Sensor.h"

Dht22Sensor::Dht22Sensor(int pin)
{
    dht = new DHT(pin, DHTTYPE, 15);
}

Dht22SensorResult Dht22Sensor::readSensor()
{
    float humidity = dht->readHumidity();
    float temperature = dht->readTemperature(false);

    if (isnan(humidity) || isnan(temperature))
    {
        Serial.println("Failed to read from DHT sensor!");
        delay(1000);
        return readSensor();
    }

    return Dht22SensorResult
    {
        humidity,
        temperature
    };
}
