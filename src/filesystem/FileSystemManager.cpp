#include <Arduino.h>
#include <LittleFS.h>

#include "FileSystemManager.h"

bool initFileSystem() {

    // Never auto-format: a mount failure must not erase employee records.
    if (!LittleFS.begin(false)) {
        Serial.println("Erro ao iniciar LittleFS!");
        return false;
    }

    Serial.println("LittleFS iniciado!");

    return true;
}
