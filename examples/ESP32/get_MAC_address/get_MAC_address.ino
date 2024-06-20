/*
* author: Irene Casares Rodríguez
* date: 19/01/24
*
* brief: Este programa obtiene la MAC del dispositivo y la imprime por terminal.
*/

#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Inicializar WiFi
  WiFi.mode(WIFI_MODE_STA);

  // Imprimir la dirección MAC
  Serial.print("\nDirección MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  // Nada que hacer aquí
}