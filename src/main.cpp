#include <Arduino.h>
#include <WiFi.h>
#include <ESP32Ping.h>
#include "secrets.h"
#include "services/dht/DHTService.h"

// Instance du service DHT
DHTService dhtService(DHT_PIN);

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
    Serial.print("[WiFi] Gateway: ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("[WiFi] Signal: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\n[WiFi] ❌ Échec de connexion");
  }
}

// ============================================
// 🌐 Test Ping Simple
// ============================================
void testPing() {
  Serial.println("\n[Ping] Test de connectivité...");

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("❌ WiFi non connecté");
    return;
  }

  // Ping Google DNS
  IPAddress googleDNS(8, 8, 8, 8);
  Serial.print("[Ping] Test 8.8.8.8... ");

  if (Ping.ping(googleDNS, 3)) {
    Serial.println("✅ OK");
    Serial.print("[Ping] Temps de réponse: ");
    Serial.print(Ping.averageTime());
    Serial.println(" ms");
  } else {
    Serial.println("❌ ÉCHEC");
  }
}

// ============================================
// 🏁 Setup
// ============================================
void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n╔════════════════════════════════════╗");
  Serial.println("║  ESP32 - WiFi + Ping + DHT11      ║");
  Serial.println("╚════════════════════════════════════╝");

  // Initialiser le capteur DHT
  dhtService.begin();

  connectWiFi();

  if (WiFi.status() == WL_CONNECTED) {
    delay(1000);
    testPing();
  }

  // Première lecture du capteur DHT
  Serial.println("\n[DHT] Première lecture...");
  if (dhtService.readSensor()) {
    dhtService.printData();
  }
}

// ============================================
// 🔄 Loop
// ============================================
void loop() {
  static unsigned long lastPingTest = 0;
  static unsigned long lastDHTRead = 0;

  // Ping toutes les 30 secondes
//   if (millis() - lastPingTest > 30000) {
//     Serial.println("\n[Auto-Test] Ping toutes les 30s...");

//     if (WiFi.status() == WL_CONNECTED) {
//       testPing();
//     } else {
//       Serial.println("⚠️  WiFi déconnecté - Reconnexion...");
//       connectWiFi();
//     }

//     lastPingTest = millis();
//   }

  // Lecture DHT toutes les 10 secondes
  if (millis() - lastDHTRead > 10000) {
    Serial.println("\n[DHT] Lecture du capteur...");
    if (dhtService.readSensor()) {
      dhtService.printData();
    }

    lastDHTRead = millis();
  }

  delay(1000);
}