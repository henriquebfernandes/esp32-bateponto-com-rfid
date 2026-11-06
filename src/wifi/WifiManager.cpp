
#include <Arduino.h>
#include <WiFi.h>
#include <ESPmDNS.h>

#include "WifiManager.h"
#include "config.h"

bool initWiFi() {

    // Inicia modo Access Point
    WiFi.mode(WIFI_AP);

    if (!WiFi.softAP(WIFI_SSID, WIFI_PASSWORD)) return false;

    Serial.println("Wi-Fi iniciado!");

    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());

    // Inicia mDNS
    if (MDNS.begin(MDNS_HOSTNAME)) {

        Serial.println("mDNS iniciado!");

        Serial.print("Dominio: http://");
        Serial.print(MDNS_HOSTNAME);
        Serial.println(".local");

    } else {

        Serial.println("Erro ao iniciar mDNS!");

    }
    return true;

}
