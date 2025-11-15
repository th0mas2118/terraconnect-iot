#ifndef MQTT_SERVICE_H
#define MQTT_SERVICE_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

class MQTTService {
private:
    WiFiClient wifiClient;
    PubSubClient* mqttClient;
    
    const char* broker;
    int port;
    const char* username;
    const char* password;
    const char* deviceId;
    
    unsigned long lastReconnectAttempt;
    const unsigned long RECONNECT_INTERVAL = 5000;
    
    // Topics
    String temperatureTopic;
    String humidityTopic;
    String statusTopic;
    String commandTopic;
    
    bool reconnect();

public:
    MQTTService(const char* broker, int port, const char* username, 
                const char* password, const char* deviceId);
    ~MQTTService();
    
    void begin();
    void loop();
    bool isConnected();
    
    // Publish
    bool publishTemperature(float temperature);
    bool publishHumidity(float humidity);
    bool publishStatus(const char* status);
    
    // Callback pour les commandes
    void setCallback(void (*callback)(char*, uint8_t*, unsigned int));
};

#endif