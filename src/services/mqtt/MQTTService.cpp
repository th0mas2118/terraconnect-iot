#include "MQTTService.h"

MQTTService::MQTTService(const char* broker, int port, const char* username, 
                         const char* password, const char* deviceId) {
    this->broker = broker;
    this->port = port;
    this->username = username;
    this->password = password;
    this->deviceId = deviceId;
    this->lastReconnectAttempt = 0;
    
    // Construction des topics
    this->temperatureTopic = "sensors/" + String(deviceId) + "/temperature";
    this->humidityTopic = "sensors/" + String(deviceId) + "/humidity";
    this->statusTopic = "devices/" + String(deviceId) + "/status";
    this->commandTopic = "devices/" + String(deviceId) + "/command";
    
    mqttClient = new PubSubClient(wifiClient);
}

MQTTService::~MQTTService() {
    delete mqttClient;
}

void MQTTService::begin() {
    mqttClient->setServer(broker, port);
    Serial.println("[MQTT] Client configuré");
    Serial.print("[MQTT] Broker: ");
    Serial.print(broker);
    Serial.print(":");
    Serial.println(port);
}

bool MQTTService::reconnect() {
    Serial.print("[MQTT] Tentative de connexion... ");
    
    // Générer un client ID unique
    String clientId = "ESP32-" + String(deviceId);
    
    if (mqttClient->connect(clientId.c_str(), username, password)) {
        Serial.println("✅ Connecté!");
        
        // S'abonner au topic de commandes
        mqttClient->subscribe(commandTopic.c_str());
        Serial.print("[MQTT] Abonné à: ");
        Serial.println(commandTopic);
        
        // Publier un message de statut
        publishStatus("online");
        
        return true;
    } else {
        Serial.print("❌ Échec, code: ");
        Serial.println(mqttClient->state());
        return false;
    }
}

void MQTTService::loop() {
    if (!mqttClient->connected()) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
            lastReconnectAttempt = now;
            
            if (WiFi.status() == WL_CONNECTED) {
                reconnect();
            } else {
                Serial.println("[MQTT] ⚠️  WiFi déconnecté");
            }
        }
    } else {
        mqttClient->loop();
    }
}

bool MQTTService::isConnected() {
    return mqttClient->connected();
}

bool MQTTService::publishTemperature(float temperature) {
    if (!mqttClient->connected()) return false;
    
    String payload = String(temperature, 2);
    bool result = mqttClient->publish(temperatureTopic.c_str(), payload.c_str());
    
    if (result) {
        Serial.print("[MQTT] 📤 Température publiée: ");
        Serial.print(payload);
        Serial.println(" °C");
    }
    
    return result;
}

bool MQTTService::publishHumidity(float humidity) {
    if (!mqttClient->connected()) return false;
    
    String payload = String(humidity, 2);
    bool result = mqttClient->publish(humidityTopic.c_str(), payload.c_str());
    
    if (result) {
        Serial.print("[MQTT] 📤 Humidité publiée: ");
        Serial.print(payload);
        Serial.println(" %");
    }
    
    return result;
}

bool MQTTService::publishStatus(const char* status) {
    if (!mqttClient->connected()) return false;
    
    bool result = mqttClient->publish(statusTopic.c_str(), status, true); // retained
    
    if (result) {
        Serial.print("[MQTT] 📤 Statut publié: ");
        Serial.println(status);
    }
    
    return result;
}

void MQTTService::setCallback(void (*callback)(char*, uint8_t*, unsigned int)) {
    mqttClient->setCallback(callback);
}