/**
 * @file esp32camPlus-barcode-button-send.ino
 * @brief Si recibe "GET-BARCODE" desde el Arduino Due, "activa" el lector.
 *        En realidad, el lector siempre está encendido y leyendo, pero el esp32 lo ignora hasta que 
 *        se le indique que atienda al código leído. 
 * 
 *        En cuanto lee un código de barras, envía una petición HTTP a la API de OpenFoodFacts con el código
 *        y recibe la información del producto.
 * 
 * @author Irene Casares Rodríguez
 * @date 09/05/2024
 */


/*

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

/*
  [27/05/24 12:37] 
    El entorno de staging de OpenFoodFacts (https://world.openfoodfacts.net) está fallando, devuelve un error 500.
    En la documentación (https://openfoodfacts.github.io/openfoodfacts-server/api/) dicen que usemos el staging si no
    estamos en producción, pero como no funciona, vamos a usar el entorno de producción (https://world.openfoodfacts.org).
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


// ----- WiFi -------------------
#include "wifi-functions.h"
// ------------------------------

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

    // Configurar el módulo WiFi en modo estación (STA) para que pueda conectarse a una red WiFi
    // existente, pero no pueda aceptar conexiones entrantes como punto de acceso. Esto es necesario
    // para obtener la MAC correcta para este modo.
    WiFi.mode(WIFI_MODE_STA);

    // Intentar conectarse a la red WiFi
    connectToWiFi();
  
}


void loop() 
{
    // ------- DUE --> ESP32 -----------------------------
    if (SerialESP32Due.available() > 0) { // Due envió algo
        String command = SerialESP32Due.readStringUntil('\n');
        command.trim();

        SerialPC.print("Mensaje recibido: "); SerialPC.println(command);

        if (command == "GET-BARCODE") {
            if (!hayConexionWiFi()) { // Comprueba la conexión a Internet
                SerialPC.println("No hay conexión a Internet. No se puede buscar la info del producto.");
                return;
            }

            SerialPC.println("Leyendo codigo de barras...");
            // ------- CAM --> ESP32 -----------------------------
            String barcode = "-"; // Cadena inicialmente "vacía"
            unsigned long startMillis = millis();

            // A veces, al leer el Serial se lee el código de barras anterior. Probé a limpiar el buffer,
            // pero podría acabar eliminando el código de barras que se está leyendo ahora. Por eso, se
            // deja en manos del usuario que si se indica en pantalla un producto distinto al escaneado,
            // se vuelva a escanear.

            while((barcode == "-") && (millis() - startMillis < 5000)) { // Si aún no se ha leído y no han pasado 5 segundos
                if (SerialESP32Cam.available() > 0) { // La cámara leyó algo
                    barcode = SerialESP32Cam.readStringUntil('\n');
                    barcode.trim();
                    SerialPC.print("\nBarcode leido: "); SerialPC.println(barcode);
                }
            }
            if(barcode != "-")  // Si se ha leído barcode
                getFoodData(barcode); // Llama a getFoodData() con el código de barras leído
            else // Si han pasado 5 segundos sin leer barcode
                SerialPC.println("\nNo se ha leido ningún código de barras");
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
