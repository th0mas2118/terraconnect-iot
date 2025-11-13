#include "DHTService.h"

DHTService::DHTService(uint8_t pin, uint8_t type) {
    this->pin = pin;
    this->type = type;
    this->dht = new DHT(pin, type);
    this->lastTemperature = 0.0;
    this->lastHumidity = 0.0;
    this->lastReadTime = 0;
}

DHTService::~DHTService() {
    delete dht;
}

void DHTService::begin() {
    dht->begin();
    Serial.println("[DHT] Capteur initialisé");
    delay(2000); // Attendre que le capteur se stabilise
}

bool DHTService::readSensor() {
    // Limiter la fréquence de lecture
    unsigned long currentTime = millis();
    if (currentTime - lastReadTime < READ_INTERVAL) {
        return true; // Retourner les dernières valeurs
    }

    float h = dht->readHumidity();
    float t = dht->readTemperature();

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
    return dht->computeHeatIndex(lastTemperature, lastHumidity, false);
}

void DHTService::printData() {
    Serial.println("\n========================================");
    Serial.println("       DONNÉES CAPTEUR DHT11");
    Serial.println("========================================");
    Serial.print("Température: ");
    Serial.print(lastTemperature);
    Serial.println(" °C");
    Serial.print("Humidité: ");
    Serial.print(lastHumidity);
    Serial.println(" %");
    Serial.print("Indice de chaleur: ");
    Serial.print(getHeatIndex());
    Serial.println(" °C");
    Serial.println("========================================\n");
}
