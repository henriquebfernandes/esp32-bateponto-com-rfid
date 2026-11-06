
#include <Arduino.h>

#include "wifi/WifiManager.h"
#include "webserver/WebServerManager.h"
#include "filesystem/FileSystemManager.h"
#include "rfid/RFIDManager.h"
#include "database/DatabaseManager.h"
#include "hardware/HardwareManager.h"

void setup() {

    Serial.begin(115200);

    // Inicializa sistema de arquivos
    if (!initFileSystem()) {
        Serial.println("Falha no sistema de arquivos!");
        return;
    }
    if (!initDatabase()) { Serial.println("Falha ao carregar banco de dados."); return; }
    initHardware();
    if (!initRFID()) { Serial.println("Falha ao inicializar RC522."); return; }

    // Inicializa Wi-Fi
    if (!initWiFi()) { Serial.println("Falha ao iniciar Wi-Fi AP."); return; }

    // Inicializa servidor web
    initWebServer();

}

void loop() {

    handleWebServer();
    processRFID();
    updateHardware();

}
