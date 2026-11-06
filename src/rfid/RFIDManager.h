#ifndef RFID_MANAGER_H
#define RFID_MANAGER_H
#include <Arduino.h>
bool initRFID();
bool pollRFID(String& uid);
#endif
