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
            processJSON(); // Procesa cada línea que lee del Serial hasta recibir FIN-TRANSMISION
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