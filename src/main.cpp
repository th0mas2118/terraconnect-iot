#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"
#include "services/dht/DHTService.h"
#include "services/mqtt/MQTTService.h"

// Instances des services
DHTService dhtService(DHT_PIN);
MQTTService mqttService(MQTT_SERVER, MQTT_PORT, MQTT_USER, MQTT_PASSWORD, DEVICE_ID);

// Callback pour les messages MQTT reçus
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT] 📥 Message reçu [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);
  
  // TODO: Gérer les commandes (relay control, etc.)
}

// ============================================
// 📡 Connexion WiFi
// ============================================
void connectWiFi() {
  Serial.println("\n[WiFi] Connexion en cours...");
  Serial.print("[WiFi] SSID: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n[WiFi] ✅ Connecté!");
    Serial.print("[WiFi] IP: ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\n[WiFi] ❌ Échec de connexion");
  }
}

// ============================================
// 🏁 Setup
// ============================================
void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║  ESP32 - DHT11 + MQTT             ║");
  Serial.println("╚════════════════════════════════════╝");

  // WiFi
  connectWiFi();

  // DHT11
  dhtService.begin();

  // MQTT
  if (WiFi.status() == WL_CONNECTED) {
    mqttService.begin();
    mqttService.setCallback(mqttCallback);
  }

  Serial.println("\n[Setup] ✅ Initialisation terminée\n");
}

// ============================================
// 🔄 Loop
// ============================================
void loop() {
  static unsigned long lastDHTRead = 0;

  // Maintenir la connexion MQTT
  mqttService.loop();

  // Lecture DHT + Publication MQTT toutes les 10 secondes
  if (millis() - lastDHTRead > 10000) {
    Serial.println("\n[DHT] Lecture du capteur...");
    
    if (dhtService.readSensor()) {
      float temp = dhtService.getTemperature();
      float hum = dhtService.getHumidity();
      
      // Affichage local
      dhtService.printData();
      
      // Publication MQTT si connecté
      if (mqttService.isConnected()) {
        mqttService.publishTemperature(temp);
        mqttService.publishHumidity(hum);
      } else {
        Serial.println("[MQTT] ⚠️  Non connecté, données non publiées");
      }
    }

    lastDHTRead = millis();
  }

  delay(100);
}