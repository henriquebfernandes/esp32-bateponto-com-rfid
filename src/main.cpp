#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "Bate Ponto ESP32";
const char* password = "12345678";

WebServer server(80);

void setup() {
  Serial.begin(115200);

  // Cria rede Wi-Fi
  WiFi.softAP(ssid, password);

  Serial.println("Rede criada!");

  // Configura domínio local
  if (MDNS.begin("bateponto")) {
    Serial.println("Dominio: http://bateponto.local");
  }

  // Página inicial
  server.on("/", []() {
    server.send(200, "text/html", "<h1>Hello World!</h1>");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}