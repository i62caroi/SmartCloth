#include <WiFi.h>

void setup() {
  Serial.begin(115200);

  // Inicializar WiFi
  WiFi.mode(WIFI_MODE_STA);

  // Imprimir la dirección MAC
  Serial.print("\nDirección MAC: ");
  Serial.println(WiFi.macAddress());
  
  / /Dirección MAC: 08:D1:F9:CB:A1:EC
  // Dirección MAC: 08:D1:F9:CB:4C:24
}

void loop() {
  // Nada que hacer aquí
}