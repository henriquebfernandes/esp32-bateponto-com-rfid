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

File openFile(const char* path, const char* mode) { return LittleFS.open(path, mode); }
bool fileExists(const char* path) { return LittleFS.exists(path); }
bool renameFile(const char* from, const char* to) { return LittleFS.rename(from, to); }
bool removeFile(const char* path) { return LittleFS.remove(path); }
