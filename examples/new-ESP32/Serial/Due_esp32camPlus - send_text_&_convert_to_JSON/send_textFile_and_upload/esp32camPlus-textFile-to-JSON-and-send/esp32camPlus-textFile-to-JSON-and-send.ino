/*
  Irene Casares Rodríguez
  12/01/2024

  Este programa recibe el contenido del fichero TXT por Serial desde el Due, lo convierte
  en JSON y lo envía a la base de datos para guardar su información.
  
*/


/*
  **********************************************************************
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
      |    IO17 (Tx1) (5º izq)   |      pin 19 (Rx1)       |    
      |    IO16 (Rx1) (6º izq)   |      pin 18 (Tx1)       |
      |       5V                 |        5V               |
      |       GND                |        GND              | 
      ------------------------------------------------------

      ------------------------------------------------------------
      |    ESP32-CAM PLUS              |    Barcode Reader       |
      ------------------------------------------------------------
      |  IO19 (Tx2-Software) (4º izq)  |  cable azul (Rx)        |    
      |  IO21 (Rx2-Software) (1º izq)  |  cable naranja (Tx)     |
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

  *********************************************************************+

*/

/*
  // Nuevo ESP32
  #define PWDN_GPIO_NUM -1
  #define RESET_GPIO_NUM   5
  #define XCLK_GPIO_NUM    15
  #define SIOD_GPIO_NUM    22
  #define SIOC_GPIO_NUM    23

  #define Y2_GPIO_NUM 2
  #define Y3_GPIO_NUM 14
  #define Y4_GPIO_NUM 35
  #define Y5_GPIO_NUM 12
  #define Y6_GPIO_NUM 27
  #define Y7_GPIO_NUM 33
  #define Y8_GPIO_NUM 34
  #define Y9_GPIO_NUM 39

  #define VSYNC_GPIO_NUM    18
  #define HREF_GPIO_NUM    36
  #define PCLK_GPIO_NUM    26

  #define LED_GPIO_NUM 25
*/


// ESP32-CAM
//#define RXD1 14
//#define TXD1 15

// ESP32-CAM PLUS
#define RXD1 16
#define TXD1 17

#define SerialPC Serial
#define SerialESP32Due Serial1

#include "json_functions.h" // ya incluye "wifi_functions.h"


void setup() 
{
    SerialPC.begin(115200);
    while (!SerialPC);
    delay(100);
    
    // DEBE TENER LA MISMA VELOCIDAD EN BAUDIOS QUE EL ARDUINO (p.ej. 115200)
    SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
    while (!SerialESP32Due);
    delay(100);

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

    if (SerialESP32Due.available() > 0) // Recibido algo del Due
    {
        String msgFromDUE = SerialESP32Due.readStringUntil('\n');
        msgFromDUE.trim();

        SerialPC.println("Mensaje recibido: " + msgFromDUE); 

        // ------ Comprobar Wifi y avisar al Due ---------
        if (msgFromDUE == "CHECK-WIFI"){
            if(hayConexionWiFi()){ // Si hay WiFi 
                SerialPC.println(F("Tengo Wifi"));
                SerialESP32Due.println("WIFI-OK");
            }
            else { // Si no se obtuvo el token, no se podrá subir info,
                    // así que actuamos como si no hubiera WiFi
                SerialPC.println(F("No tengo wifi"));
                SerialESP32Due.println("NO-WIFI");
            }
        }
        // -----------------------------------------------
        // ------ Procesar líneas y generar JSON ---------
        else if (msgFromDUE == "SAVE"){
            SerialPC.println(F("Esperando data..."));
            SerialESP32Due.println("WAITING-FOR-DATA");
            //processJSON(); // Procesa cada línea que lee del Serial hasta recibir FIN-TRANSMISION
            processJSON_OnePerMeal();
        }
        // -----------------------------------------------
        else{ 
            // Otras (p.ej. BARCODE)
            SerialPC.println(F("Comando desconocido"));
        }
    }


}


/*void loop() 
{
    // -------- Recibir lineas y generar JSON -------
    if (SerialESP32Due.available() > 0) 
    {
        String line = SerialESP32Due.readStringUntil('\n');
        line.trim();

        SerialPC.println("Linea recibida: " + line);

        //addLineToJSON_ONE(line);
        addLineToJSON(line);
    }
    // -----------------------------------------------
}*/