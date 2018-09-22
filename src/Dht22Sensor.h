#ifndef DHT22SENSOR_H_
#define DHT22SENSOR_H_

#include <DHT.h>
#include "HardwareSerial.h"

#define DHTTYPE DHT22

struct Dht22SensorResult
{
    float humidity; // in percent
    float temperature; // in celsius
    int numberOfReadAttemps; // for debugging purposes
};

class Dht22Sensor
{
    private:
        DHT* dht;
        int powerPin;

    public:
        Dht22Sensor(int dataPin, int powerPin);

        Dht22SensorResult read(int maxNumberAttemps = 3, int attemps = 1);
};

#endif
