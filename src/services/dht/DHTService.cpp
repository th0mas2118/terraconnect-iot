#include "DHTService.h"

DHTService::DHTService(uint8_t pin, uint8_t type)
    : dht(pin, type), lastTemperature(0.0), lastHumidity(0.0), lastReadTime(0) {
}

void DHTService::begin() {
    dht.begin();
    Serial.println("[DHT] Capteur initialisé");
    delay(2000); // Attendre que le capteur se stabilise
}

bool DHTService::readSensor() {
    // Limiter la fréquence de lecture
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime < READ_INTERVAL) {
        return true; // Retourner les dernières valeurs
    }

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    // Vérifier si la lecture a échoué
    if (isnan(h) || isnan(t)) {
        Serial.println("[DHT] ❌ Échec de lecture");
        return false;
    }

    lastHumidity = h;
    lastTemperature = t;
    lastReadTime = currentTime;

    return true;
}

float DHTService::getTemperature() {
    return lastTemperature;
}

float DHTService::getHumidity() {
    return lastHumidity;
}

float DHTService::getHeatIndex() {
    return dht.computeHeatIndex(lastTemperature, lastHumidity, false);
}

void DHTService::printData() {
    Serial.printf("[DHT] %.1f°C, %.1f%%, Indice: %.1f°C\n",
                  lastTemperature, lastHumidity, getHeatIndex());
}
