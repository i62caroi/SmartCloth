/*
  Irene Casares Rodríguez
  07/05/24

  Si recibe "GET-BARCODe" desde el Arduino Due, "activa" el lector.
  En realidad, el lector siempre está encendido y leyendo, pero el esp32 lo ignora hasta que 
  se le indique que atienda al código leído. 
  
*/


/*
      --------------------------------------------
      |    ESP32-CAM   |    FTDI (Serial al PC)  |
      --------------------------------------------
      |    UOT (Tx)    |        RXD              |    
      |    UOR (Rx)    |        TXD              |
      --------------------------------------------

      --------------------------------------------
      |    ESP32-CAM   |  Arduino Due (Serial1)  |
      --------------------------------------------
      |    IO15 (Tx1)  |      pin 19 (Rx1)       |    
      |    IO14 (Rx1)  |      pin 18 (Tx1)       |
      |      3V3       |        3V3              |
      |      GND       |        GND              | 
      --------------------------------------------

      CARGAR Y EJECUTAR PROGRAMA:
        1. Puentear IO0 a GND (3º por la derecha de la camara)
        2. Resetear
        3. Cargar programa
        4. Retirar puente
        5. Resetear

**********************************************************************

      ------------------------------------------------------
      |    ESP32-CAM PLUS        |  Arduino Due (Serial1)  |
      ------------------------------------------------------
      |    IO17 (Tx1) (4º izq)   |      pin 19 (Rx1)       |    
      |    IO16 (Rx1) (5º izq)   |      pin 18 (Tx1)       |
      |      3V3                 |        3V3              |
      |      GND                 |        GND              | 
      ------------------------------------------------------


      ------------------------------------------------------
      |    Lector Barcode     |      ESP32-CAM PLUS        |
      ------------------------------------------------------
      |      Tx (naranja)     |    pin 19 (Rx software)    |    
      |      Rx (azul)        |    pin 21 (Tx software)    |
      |      3V3 (marron)     |        3V3                 |
      |      GND (gris)       |        GND                 | 
      ------------------------------------------------------

      ¡¡¡¡¡ IMPORTANTE !!!!!
      USAR ARDUINO IDE. EN VSCODE NO FUNCIONA LA TERMINAL.
      
      ARDUINO BOARD CONFIGURATION:
      - Board: "ESP32 Dev Module"
      - Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
      - Flash Mode: "DIO"
      - Upload Speed: "460800"

      Con eso ya se puede subir el programa a la placa sin problemas y 
      sin tener que presionar el botón de reset ni GIO0.

  **********************************************************************
*/


// ----- ESP32 <-- CAM -------
#include <SoftwareSerial.h>

#define RXD2 19 // Tx (naranja lector)
#define TXD2 21 // Rx (azul lector)

SoftwareSerial mySerial(RXD2, TXD2); // RX, TX
#define SerialESP32Cam mySerial // Renombrar mySerial
// ------------------------------


// ----- ESP32 <--> PC ----------
#define SerialPC Serial
// ------------------------------


// ----- ESP32 <--> Due ---------
#define RXD1 16
#define TXD1 17

#define SerialESP32Due Serial1
// ------------------------------

#define LED_BUILTIN 25



void setup() 
{
  // ----- ESP32 - PC ----------
  SerialPC.begin(115200);
  while (!SerialPC);
  delay(100);
  // ---------------------------
  
  // ----- ESP32 - Due ---------
  SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
  while (!SerialESP32Due);
  delay(100);
  // ---------------------------

  // ----- ESP32 - CAM ------
  SerialESP32Cam.begin(9600); // Debe ser 9600 porque así trabaja el lector
  while (!SerialESP32Cam);
  delay(100);
  // ---------------------------

  SerialPC.println("HOLA PC DESDE ESP32");
  SerialESP32Due.println("HOLA DUE");

  pinMode(LED_BUILTIN, OUTPUT); // Pin del LED del ESP32-CAM
  digitalWrite(LED_BUILTIN, HIGH); // Apaga el LED
  
}


void loop() 
{
    // ------- DUE --> ESP32 -----------------------------
    if (SerialESP32Due.available() > 0) { // Due envió algo
        String command = SerialESP32Due.readStringUntil('\n');
        command.trim();

        SerialPC.print("Mensaje recibido: "); SerialPC.println(command);

        if (command == "GET-BARCODE") {
            digitalWrite(LED_BUILTIN, LOW); // Enciende el LED
            SerialPC.println("\nLed ON");
            SerialPC.println("Leyendo codigo de barras...");
            // ------- CAM --> ESP32 -----------------------------
            bool barcodeRead = false;
            unsigned long startMillis = millis();
            while(!barcodeRead && (millis() - startMillis < 5000)) { // Si aún no se ha leído y no han pasado 5 segundos
                if (SerialESP32Cam.available() > 0) { // La cámara leyó algo
                    String barcode = SerialESP32Cam.readStringUntil('\n');
                    barcode.trim();

                    SerialPC.print("\nBarcode leido: "); SerialPC.println(barcode);
                    barcodeRead = true;
                    digitalWrite(LED_BUILTIN, HIGH); // Apaga el LED
                    SerialPC.println("Led OFF");
                }
            }
            if(!barcodeRead) {
                SerialPC.println("\nNo se ha leido ningún código de barras");
                digitalWrite(LED_BUILTIN, HIGH); // Apaga el LED
                SerialPC.println("Led OFF");
            }
            // ---------------------------------------------------
        }
        else{
          SerialPC.println("Command not valid");
          SerialESP32Due.println("Command not valid");
        }
    }
    // ---------------------------------------------------


    delay(200);

}
