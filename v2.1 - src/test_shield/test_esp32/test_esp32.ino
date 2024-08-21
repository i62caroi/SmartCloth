
/*  
    **********************************************************************

      -------------------------------------------------
      |    ESP32-CAM PLUS   |  Arduino Due (Serial1)  |
      -------------------------------------------------
      |    IO17 (Tx1)       |      pin 19 (Rx1)       |    
      |    IO16 (Rx1)       |      pin 18 (Tx1)       |
      |      3V3            |        3V3              |
      |      GND            |        GND              | 
      -------------------------------------------------

      ------------------------------------------------------------
      |    ESP32-CAM PLUS              |    Barcode Reader       |
      ------------------------------------------------------------
      |  IO21 (Tx2-Software) (4º izq)  |  cable azul (Rx)        |    
      |  IO19 (Rx2-Software) (1º izq)  |  cable naranja (Tx)     |
      |     3V3                        |        3V3              |
      |     GND                        |        GND              | 
      ------------------------------------------------------------

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



#include "Serial_functions.h" // incluye debug.h
#include <WiFi.h>



void setup() 
{
    // --- COMUNICACIÓN SERIAL ---
    setupAllSerial();
    // ---------------------------


    // --- OBTENER MAC -----------
    // Inicializar WiFi
    WiFi.mode(WIFI_MODE_STA);

    // Imprimir la dirección MAC
    SerialPC.print("\nDirección MAC: ");
    SerialPC.println(WiFi.macAddress());
    // ---------------------------

  
}



void loop() 
{  
    if(hayMsgFromDue()) // Recibido algo del Due
    {
        // ------ Leer mensaje del Due ---------
        String msgFromDue;
        readMsgFromSerialDue(msgFromDue); // Leer mensaje del Serial

        SerialPC.println("\nMensaje recibido: " + msgFromDue); 
        // -------------------------------------

        // ------ Comprobar comunicación Serial ----------
        if (msgFromDue == "PING") ackDue(); // Responder al Due (PONG) indicando comunicación establecida
        // -----------------------------------------------

        else SerialPC.println(F("Comando desconocido"));
    }

}



