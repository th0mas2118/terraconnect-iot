#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoOTA.h>
#include <esp_task_wdt.h>
#include "secrets.h"
#include "services/dht/DHTService.h"
#include "services/mqtt/MQTTService.h"

// Configuration Watchdog Timer
#define WDT_TIMEOUT 30 // 30 secondes avant redémarrage

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

  // Configuration OTA
  if (WiFi.status() == WL_CONNECTED) {
    ArduinoOTA.setHostname(DEVICE_ID);
    ArduinoOTA.setPassword("terra2024"); // Changez ce mot de passe

    ArduinoOTA.onStart([]() {
      Serial.println("\n[OTA] Démarrage mise à jour...");
    });
    ArduinoOTA.onEnd([]() {
      Serial.println("\n[OTA] ✅ Mise à jour terminée");
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("[OTA] Progression: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
      Serial.printf("[OTA] ❌ Erreur[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin();
    Serial.println("[OTA] ✅ Service démarré");
  }

  // DHT11
  dhtService.begin();

  // MQTT
  if (WiFi.status() == WL_CONNECTED) {
    mqttService.begin();
    mqttService.setCallback(mqttCallback);
  }

  // Watchdog Timer - Toujours en dernier dans setup()
  Serial.printf("[WDT] Configuration watchdog (%d secondes)\n", WDT_TIMEOUT);
  esp_task_wdt_init(WDT_TIMEOUT, true); // Enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); // Add current thread to WDT watch
  Serial.println("[WDT] ✅ Watchdog activé");

  Serial.println("\n[Setup] ✅ Initialisation terminée\n");
}

// ============================================
// 🔄 Loop
// ============================================
void loop() {
  static unsigned long lastDHTRead = 0;
  static unsigned long lastWDTReset = 0;

  // Réinitialiser le watchdog toutes les 10 secondes
  if (millis() - lastWDTReset >= 10000) {
    esp_task_wdt_reset();
    lastWDTReset = millis();
  }

  // Gérer les mises à jour OTA
  ArduinoOTA.handle();

  // Maintenir la connexion MQTT
  mqttService.loop();

  // Lecture DHT + Publication MQTT toutes les 10 secondes
  if (millis() - lastDHTRead >= 10000) {
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

  // Petit yield pour ne pas monopoliser le CPU (optionnel)
  delay(10);
}