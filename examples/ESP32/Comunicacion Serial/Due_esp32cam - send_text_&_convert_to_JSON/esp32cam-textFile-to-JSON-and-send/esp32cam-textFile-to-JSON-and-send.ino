/*
  Irene Casares Rodríguez
  12/01/2024

  Este programa recibe el contenido del fichero TXT por Serial desde el Due, lo convierte
  en JSON y lo envía a la base de datos para guardar su información.
  
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
*/

#define RXD1 14
#define TXD1 15

#define SerialPC Serial
#define SerialESP32Due Serial1

#include "json_functions.h" // ya incluye "wifi_functions.h"


void setup() 
{
    SerialPC.begin(115200);
    while (!SerialPC);
    
    // DEBE TENER LA MISMA VELOCIDAD EN BAUDIOS QUE EL ARDUINO (p.ej. 115200)
    SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
    while (!SerialESP32Due);

    SerialPC.println("HOLA PC DESDE ESP32");
    SerialESP32Due.println("HOLA DUE");

    // Configurar el módulo WiFi en modo estación (STA) para que pueda conectarse a una red WiFi
    // existente, pero no pueda aceptar conexiones entrantes como punto de acceso. Esto es necesario
    // para obtener la MAC correcta para este modo.
    WiFi.mode(WIFI_MODE_STA);

    // Obtener y almacenar la dirección MAC una vez al inicio
    macAddress = WiFi.macAddress();

    // Intentar conectarse a la red WiFi
    connectToWiFi();
}


void loop() 
{
    // -------- Recibir lineas y generar JSON -------
    /*if (SerialESP32Due.available() > 0) 
    {
        String line = SerialESP32Due.readStringUntil('\n');
        line.trim();

        SerialPC.print("Linea recibida: "); SerialPC.println(line);

        //addLineToJSON_ONE(line);
        addLineToJSON(line);
    }*/
    // -----------------------------------------------


    // ------ Comprobar Wifi y avisar al Due ---------
    if (SerialESP32Due.available() > 0) 
    {
        String line = SerialESP32Due.readStringUntil('\n');
        line.trim();

        SerialPC.print("Linea recibida: "); SerialPC.println(line);

        if (line == "CHECK-WIFI"){
            if(hayConexionWiFi()){
                SerialPC.println("Tengo Wifi");
                SerialESP32Due.println("WIFI");
            }
            else { 
                SerialPC.println("No tengo wifi");
                SerialESP32Due.println("NO-WIFI");
            }
        }
    }
    // -----------------------------------------------

}
