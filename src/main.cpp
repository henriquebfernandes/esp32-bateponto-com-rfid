#include <Arduino.h>

// A maioria das placas ESP32 Dev Module tem o LED interno no pino 2
#define LED_PIN 2

void setup() {
  // Inicializa a comunicação serial a 115200 bps
  Serial.begin(115200);
  
  // Configura o pino do LED como saída
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("ESP32 Inicializado com sucesso!");
}

void loop() {
  // Liga o LED
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED Ligado");
  delay(1000); // Aguarda 1 segundo

  // Desliga o LED
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED Desligado");
  delay(1000); // Aguarda 1 segundo
}
