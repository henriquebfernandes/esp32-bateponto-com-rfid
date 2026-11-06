#include <Arduino.h>
#include "HardwareManager.h"
#include "config.h"
static uint32_t offAt=0;
void initHardware(){ pinMode(LED_GREEN_PIN,OUTPUT); pinMode(LED_RED_PIN,OUTPUT); pinMode(BUZZER_PIN,OUTPUT); digitalWrite(LED_GREEN_PIN,LOW); digitalWrite(LED_RED_PIN,LOW); }
static void signal(bool ok, uint16_t frequency, uint16_t duration){ digitalWrite(LED_GREEN_PIN,ok?HIGH:LOW); digitalWrite(LED_RED_PIN,ok?LOW:HIGH); tone(BUZZER_PIN,frequency,duration); offAt=millis()+SIGNAL_DURATION_MS; }
void signalSuccess(){signal(true,BUZZER_SUCCESS_HZ,BUZZER_SUCCESS_DURATION_MS);}
void signalFailure(){signal(false,BUZZER_FAILURE_HZ,BUZZER_FAILURE_DURATION_MS);}
void signalUnknown(){signal(false,BUZZER_UNKNOWN_HZ,BUZZER_UNKNOWN_DURATION_MS);}
void updateHardware(){if(offAt && (int32_t)(millis()-offAt)>=0){digitalWrite(LED_GREEN_PIN,LOW);digitalWrite(LED_RED_PIN,LOW);noTone(BUZZER_PIN);offAt=0;}}
