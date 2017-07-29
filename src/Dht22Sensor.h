#ifndef DHT22SENSOR_H_
#define DHT22SENSOR_H_

#include <DHT.h>
#include "HardwareSerial.h"

#define DHTTYPE DHT22

struct Dht22SensorResult
{
    float humidity; // in percent
    float temperature; // in c
};

class Dht22Sensor
{
    private:
        DHT* dht;

    public:
        Dht22Sensor(int pin);

        Dht22SensorResult readSensor();
};

#endif
