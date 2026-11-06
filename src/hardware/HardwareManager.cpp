#include <Arduino.h>
#include "HardwareManager.h"
#include "config.h"
static uint32_t offAt=0;
void initHardware(){ pinMode(LED_GREEN_PIN,OUTPUT); pinMode(LED_RED_PIN,OUTPUT); pinMode(BUZZER_PIN,OUTPUT); digitalWrite(LED_GREEN_PIN,LOW); digitalWrite(LED_RED_PIN,LOW); }
static void signal(bool ok){ digitalWrite(LED_GREEN_PIN,ok?HIGH:LOW); digitalWrite(LED_RED_PIN,ok?LOW:HIGH); tone(BUZZER_PIN,ok?BUZZER_SUCCESS_HZ:BUZZER_FAILURE_HZ,BUZZER_DURATION_MS); offAt=millis()+SIGNAL_DURATION_MS; }
void signalSuccess(){signal(true);} void signalFailure(){signal(false);}
void updateHardware(){if(offAt && (int32_t)(millis()-offAt)>=0){digitalWrite(LED_GREEN_PIN,LOW);digitalWrite(LED_RED_PIN,LOW);noTone(BUZZER_PIN);offAt=0;}}
