#include <SPI.h>
#include <MFRC522.h>
#include "RFIDManager.h"
#include "config.h"
static MFRC522 reader(RFID_SS_PIN,RFID_RST_PIN);
static String previousUid; static uint32_t lastSeen=0;
bool initRFID(){ SPI.begin(RFID_SCK_PIN,RFID_MISO_PIN,RFID_MOSI_PIN,RFID_SS_PIN); reader.PCD_Init(); return true; }
bool pollRFID(String& uid){
  if(!reader.PICC_IsNewCardPresent() || !reader.PICC_ReadCardSerial()) { if(millis()-lastSeen>1200) previousUid=""; return false; }
  char hex[3]; String current; for(byte i=0;i<reader.uid.size;i++){snprintf(hex,sizeof(hex),"%02X",reader.uid.uidByte[i]);current+=hex;}
  reader.PICC_HaltA(); reader.PCD_StopCrypto1(); lastSeen=millis();
  if(current==previousUid) return false;
  previousUid=current; uid=current; return true;
}
