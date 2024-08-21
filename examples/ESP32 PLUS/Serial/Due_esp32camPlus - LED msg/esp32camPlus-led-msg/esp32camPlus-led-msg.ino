/* Baud-rates available: 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 57600, or 115200, 256000, 512000, 962100
 *  
 *  Protocols available:
 * SERIAL_5N1   5-bit No parity 1 stop bit
 * SERIAL_6N1   6-bit No parity 1 stop bit
 * SERIAL_7N1   7-bit No parity 1 stop bit
 * SERIAL_8N1 (the default) 8-bit No parity 1 stop bit
 * SERIAL_5N2   5-bit No parity 2 stop bits 
 * SERIAL_6N2   6-bit No parity 2 stop bits
 * SERIAL_7N2   7-bit No parity 2 stop bits
 * SERIAL_8N2   8-bit No parity 2 stop bits 
 * SERIAL_5E1   5-bit Even parity 1 stop bit
 * SERIAL_6E1   6-bit Even parity 1 stop bit
 * SERIAL_7E1   7-bit Even parity 1 stop bit 
 * SERIAL_8E1   8-bit Even parity 1 stop bit 
 * SERIAL_5E2   5-bit Even parity 2 stop bit 
 * SERIAL_6E2   6-bit Even parity 2 stop bit 
 * SERIAL_7E2   7-bit Even parity 2 stop bit  
 * SERIAL_8E2   8-bit Even parity 2 stop bit  
 * SERIAL_5O1   5-bit Odd  parity 1 stop bit  
 * SERIAL_6O1   6-bit Odd  parity 1 stop bit   
 * SERIAL_7O1   7-bit Odd  parity 1 stop bit  
 * SERIAL_8O1   8-bit Odd  parity 1 stop bit   
 * SERIAL_5O2   5-bit Odd  parity 2 stop bit   
 * SERIAL_6O2   6-bit Odd  parity 2 stop bit    
 * SERIAL_7O2   7-bit Odd  parity 2 stop bit    
 * SERIAL_8O2   8-bit Odd  parity 2 stop bit    
*/

//#include <Arduino.h>



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
//#define LED_BUILTIN 4

// ESP32-CAM PLUS
#define RXD1 16
#define TXD1 17
#define LED_BUILTIN 25

#define SerialPC Serial
#define SerialESP32Due Serial1

void setup() {
  SerialPC.begin(115200);
  while (!SerialPC);
  
  // DEBE TENER LA MISMA VELOCIDAD EN BAUDIOS QUE EL ARDUINO (p.ej. 115200)
  SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
  while (!SerialESP32Due);
  
  pinMode(LED_BUILTIN, OUTPUT); // Pin del LED del ESP32-CAM

  SerialPC.print("jelou");
}


void loop() {
  
  if (SerialESP32Due.available() > 0) {
    String command = SerialESP32Due.readStringUntil('\n');

    SerialPC.print("Mensaje recibido: "); SerialPC.println(command);

    if (command == "ON") {
      digitalWrite(LED_BUILTIN, LOW); // Enciende el LED
      SerialPC.println("Led ON");
      SerialESP32Due.println("Led encendido");
    }
    else if (command == "OFF") {
      digitalWrite(LED_BUILTIN, HIGH); // Apaga el LED
      SerialPC.println("Led OFF");
      SerialESP32Due.println("Led apagado");
    }
    else{
      SerialPC.println("Command not valid");
      SerialESP32Due.println("Command not valid");
    }
  }

}
