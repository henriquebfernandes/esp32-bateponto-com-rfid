#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H
#include <Arduino.h>
bool initRFID();
bool isRFIDConnected();
bool pollRFID(String& uid);
#endif
