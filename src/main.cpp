
#include <Arduino.h>

#include "wifi/WifiManager.h"
#include "webserver/WebServerManager.h"
#include "filesystem/FileSystemManager.h"
#include "rfid/RFIDManager.h"
#include "database/DatabaseManager.h"
#include "hardware/HardwareManager.h"

static bool startupFailed = false;

void setup() {

    Serial.begin(115200);
    delay(200);
    Serial.println("Iniciando BatePonto RFID...");

    // Inicializa sistema de arquivos
    if (!initFileSystem()) {
        Serial.println("Falha no sistema de arquivos!");
        startupFailed = true; return;
    }
    if (!initDatabase()) { Serial.println("Falha ao carregar banco de dados."); startupFailed = true; return; }
    initHardware();

    // Inicia a rede antes do leitor para manter o painel acessível se o RC522 falhar.
    if (!initWiFi()) { Serial.println("Falha ao iniciar Wi-Fi AP."); startupFailed = true; return; }

    if (!initRFID()) Serial.println("Falha ao inicializar RC522; painel Wi-Fi continuará ativo.");

    // Inicializa servidor web
    initWebServer();

}

void loop() {

    if (startupFailed) { delay(1000); return; }

    handleWebServer();
    processRFID();
    updateHardware();

}
