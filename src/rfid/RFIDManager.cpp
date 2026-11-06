#include <SPI.h>
#include <MFRC522.h>
#include "RFIDManager.h"
#include "config.h"
static MFRC522 reader(RFID_SS_PIN,RFID_RST_PIN);
static String previousUid; static uint32_t lastSeen=0;
static bool readerConnected=false;
static uint32_t lastStatusCheck=0;

static bool detectReader() {
  byte first=reader.PCD_ReadRegister(reader.VersionReg);
  delayMicroseconds(100);
  byte second=reader.PCD_ReadRegister(reader.VersionReg);
  return first==second && first!=0x00 && first!=0xFF;
}

static void initializeReaderBus() {
  SPI.end();
  SPI.begin(RFID_SCK_PIN,RFID_MISO_PIN,RFID_MOSI_PIN,RFID_SS_PIN);
  reader.PCD_Init();
  delay(10);
}

static void refreshReaderStatus(uint32_t now) {
  if((uint32_t)(now-lastStatusCheck)<RFID_STATUS_CHECK_INTERVAL_MS) return;
  lastStatusCheck=now;

  if(readerConnected && !detectReader()) {
    readerConnected=false;
  }

  if(!readerConnected) {
    // Tenta recuperar o barramento e o RC522 mesmo se a primeira leitura SPI falhar.
    initializeReaderBus();
    readerConnected=detectReader();
    if(readerConnected) {
      previousUid="";
      lastSeen=now;
    }
  }
}

bool initRFID(){
  initializeReaderBus();
  lastStatusCheck=millis();
  readerConnected=detectReader();
  return readerConnected;
}
bool isRFIDConnected(){return readerConnected;}
bool pollRFID(String& uid){
  uint32_t now=millis();
  refreshReaderStatus(now);
  if(!readerConnected) return false;
  if(!reader.PICC_IsNewCardPresent() || !reader.PICC_ReadCardSerial()) { if((uint32_t)(now-lastSeen)>=RFID_REPEAT_GUARD_MS) previousUid=""; return false; }
  char current[sizeof(reader.uid.uidByte)*2+1];
  if(reader.uid.size>sizeof(reader.uid.uidByte)){reader.PICC_HaltA();reader.PCD_StopCrypto1();return false;}
  for(byte i=0;i<reader.uid.size;i++) snprintf(current+i*2,sizeof(current)-i*2,"%02X",reader.uid.uidByte[i]);
  current[reader.uid.size*2]='\0';
  reader.PICC_HaltA(); reader.PCD_StopCrypto1();
  if(previousUid==current && (uint32_t)(now-lastSeen)<RFID_REPEAT_GUARD_MS) return false;
  lastSeen=now;
  previousUid=current; uid=current; return true;
}
