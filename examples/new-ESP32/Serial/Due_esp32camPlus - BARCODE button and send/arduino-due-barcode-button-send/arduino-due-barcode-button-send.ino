/*
  Irene Casares Rodríguez
  07/05/24

  Al pulsar el botón (Lector), el Due envía por Serial al ESP32-CAM el mensaje "GEt-BARCODE"
  para que "active" el lector. 
  En realidad, el lector siempre está encendido, pero con el mensaje "GET-BARCODE" el esp32 se dispone
  a "escuchar" el código de barras. El lector está leyendo códigos de barras continuamente, pero se 
  ignoran hasta que se le indique al esp32 desde el Due que atienda al lector.

*/

/*
    BOTON BARCODE conectado de la siguiente forma (indistintamente):
        -----------------
        | GND | Barcode |
        -----------------
        | GND |   53    |       
        -----------------
    ¡IMPORTANTE! El GND junto al D53 en el screw shield no está conectado a nada. Usar alguno
    de los que están junto a 5V o 3.3V.

*/

#include "ISR.h"

#define SerialPC Serial
#define SerialDueESP32 Serial1

const int pinBarcode = 53;   

void setup() {
    SerialPC.begin(115200);
    while (!SerialPC);
    delay(100);

    SerialDueESP32.begin(115200);
    while (!SerialDueESP32);
    delay(100);

    SerialPC.println("Arduino Due");

    // ----- BARCODE  -----
    // Inicializar   
    pinMode(pinBarcode, INPUT_PULLUP); // Entrada con resistencia pull-up interna

    // Interruption   -----
    // FALLING porque su resistencia (la interna del Due) está modo pull-up, así que el botón se activa a la baja
    attachInterrupt(digitalPinToInterrupt(intPinBarcode), ISR_barcode, FALLING); // Interrupción en flanco de bajada
}

void loop() {
    // ----- ENVIAR MENSAJE AL PULSAR BOTÓN -----
    if (pulsandoBarcode) { // Si se pulsa el botón (interrupción)
        SerialPC.println("Pulsando boton barcode");
        SerialDueESP32.println("GET-BARCODE"); // Envía al ESP32-CAM

        pulsandoBarcode = false;
    }

  
    // ----- RECIBIR MENSAJE -----
    if (SerialDueESP32.available() > 0) {
        String msg = SerialDueESP32.readStringUntil('\n');
        msg.trim();

        SerialPC.print("Mensaje recibido: "); SerialPC.println(msg); 
    }

    delay(100);
}
