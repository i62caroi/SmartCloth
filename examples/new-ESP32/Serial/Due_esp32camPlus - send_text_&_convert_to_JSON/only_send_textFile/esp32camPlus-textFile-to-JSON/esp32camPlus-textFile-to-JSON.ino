/*
  Irene Casares Rodríguez
  12/01/2024

  Este programa recibe el contenido del fichero TXT por Serial desde el Due y lo convierte
  en JSON.
  
*/


/*
      --------------------------------------------
      |    ESP32-CAM   |    FTDI (Serial al PC)  |
      --------------------------------------------
      |    UOT (Tx)    |        RXD              |    
      |    UOR (Rx)    |        TXD              |
      --------------------------------------------

  *********************************************************************+
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

  *********************************************************************+

      ------------------------------------------------------
      |    ESP32-CAM PLUS        |  Arduino Due (Serial1)  |
      ------------------------------------------------------
      |    IO17 (Tx1) (4º izq)   |      pin 19 (Rx1)       |    
      |    IO16 (Rx1) (5º izq)   |      pin 18 (Tx1)       |
      |      3V3                 |        3V3              |
      |      GND                 |        GND              | 
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

  *********************************************************************+

*/

// ESP32-CAM
//#define RXD1 14
//#define TXD1 15

// ESP32-CAM PLUS
#define RXD1 16
#define TXD1 17

#define SerialPC Serial
#define SerialESP32Due Serial1

#include "functions.h"


void setup() 
{
  SerialPC.begin(115200);
  while (!SerialPC);
  
  // DEBE TENER LA MISMA VELOCIDAD EN BAUDIOS QUE EL ARDUINO (p.ej. 115200)
  SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
  while (!SerialESP32Due);

  SerialPC.println("HOLA PC DESDE ESP32");
  SerialESP32Due.println("HOLA DUE");
}


void loop() 
{
    if (SerialESP32Due.available() > 0) 
    {
        String msgFromDUE = SerialESP32Due.readStringUntil('\n');
        msgFromDUE.trim();

        SerialPC.println("\nMensaje recibido: " + msgFromDUE); 

        if (msgFromDUE == "SAVE"){
            #if defined(SM_DEBUG)
            SerialPC.println(F("\nEsperando data..."));
            #endif
            SerialESP32Due.println("WAITING-FOR-DATA");
        }
        else{ 
            lineToJSON(msgFromDUE);
        }

        
    }

}
