# TerraConnect IoT

Projet ESP32 avec capteur DHT11 pour monitoring environnemental.

## Matériel

- ESP32 DevKit V1
- Capteur DHT11 (température et humidité)

## Fonctionnalités

- ✅ Connexion WiFi
- ✅ Test de connectivité Internet (ping)
- ✅ Lecture capteur DHT11
- ✅ Monitoring automatique toutes les 10 secondes

## Installation

### Prérequis

- [PlatformIO](https://platformio.org/)
- Câble USB pour ESP32

### Configuration

1. Cloner le repo:
```bash
git clone https://github.com/th0mas2118/terraconnect-iot.git
cd terraconnect-iot
```

2. Copier et configurer les secrets:
```bash
cp src/secrets.h.example src/secrets.h
```

3. Éditer `src/secrets.h` avec vos credentials WiFi:
```cpp
#define WIFI_SSID "Votre_SSID"
#define WIFI_PASSWORD "Votre_Password"
```

4. Compiler et uploader:
```bash
pio run --target upload
```

5. Monitorer les données:
```bash
pio device monitor
```

## Branchement DHT11

```
DHT11 Module       ESP32 DevKit V1
-------------      ----------------
GND (gauche)  -->  GND
DATA (milieu) -->  D4 (GPIO 4)
5V (droite)   -->  3V3
```

## Architecture

```
src/
├── main.cpp                    # Programme principal
├── secrets.h                   # Credentials (non versionnés)
├── secrets.h.example           # Template de configuration
└── services/
    └── dht/
        ├── DHTService.h        # Service DHT11
        └── DHTService.cpp
```

## Licence

MIT
