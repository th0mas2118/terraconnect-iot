#ifndef DHT_SERVICE_H
#define DHT_SERVICE_H

#include <Arduino.h>
#include <DHT.h>

class DHTService {
private:
    DHT dht;
    float lastTemperature;
    float lastHumidity;
    unsigned long lastReadTime;
    const unsigned long READ_INTERVAL = 2000; // Min 2s entre lectures DHT11

public:
    DHTService(uint8_t pin, uint8_t type = DHT11);

    void begin();
    bool readSensor();

    float getTemperature();
    float getHumidity();
    float getHeatIndex();

    void printData();
};

#endif
