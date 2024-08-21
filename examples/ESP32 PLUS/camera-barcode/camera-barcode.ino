/*
      ¡¡¡¡¡ IMPORTANTE !!!!!
      USAR ARDUINO IDE. EN VSCODE NO FUNCIONA LA TERMINAL.
      
      ARDUINO BOARD CONFIGURATION:
      - Board: "ESP32 Dev Module"
      - Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
      - Flash Mode: "DIO"
      - Upload Speed: "460800"

      Con eso ya se puede subir el programa a la placa sin problemas y 
      sin tener que presionar el botón de reset ni GIO0.
*/
#include <SoftwareSerial.h>

// ESP32-CAM PLUS --> Due
//#define RXD1 16
//#define TXD1 17

// ESP32-CAM PLUS --> cámara
// gris-> GND
// marron -> 3.3
// azul -> RX (camara)
// naranja -> Tx (camara)
#define RXD2 19 // naranja
#define TXD2 21 // azul

SoftwareSerial mySerial(RXD2, TXD2); // RX, TX

#define SerialPC Serial
//#define SerialESP32Due Serial1
#define SerialESP32Cam mySerial



void setup() {
    SerialPC.begin(115200);
    while (!SerialPC);
    delay(100);

    /*SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
    while (!SerialESP32Due);
    delay(100);*/

    SerialESP32Cam.begin(9600);
    while (!SerialESP32Cam);
    delay(100);

    SerialPC.println("Leyendo...");

}


void loop() {
    
    if (SerialESP32Cam.available() > 0) // Cámara leyó algo
    {
        String barcode = SerialESP32Cam.readStringUntil('\n');
        barcode.trim();

        SerialPC.println("Barcode: " + barcode); 

    }

    delay(200);

}












