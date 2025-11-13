#ifndef CONFIG_H
#define CONFIG_H

// Timeouts
#define WIFI_TIMEOUT_MS 20000
#define MQTT_RECONNECT_DELAY 5000

// Pins
#define DHT_PIN 4
#define RELAY_PIN 2
#define LED_PIN 2

// Intervals (ms)
#define SENSOR_READ_INTERVAL 10000
#define MQTT_KEEPALIVE 60

#endif