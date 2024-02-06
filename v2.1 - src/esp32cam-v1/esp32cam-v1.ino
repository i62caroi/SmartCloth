/*
    Irene Casares Rodríguez
    12/01/2024

    PROGRAMA PRINCIPAL DEL ESP32
      
    Este programa recibe el contenido del fichero TXT por Serial desde el Due, lo convierte
    en JSON y lo envía a la base de datos para guardar su información.
  
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


    -----------------------------------------------------------------------------------
    1. Recibe "CHECK-WIFI" del Due
    2. Comprueba la conexión WiFi:
        2.1. Hay conexión, responde "WIFI"
        2.2. No hay conexión, responde "NO-WIFI"

    3. Si hay conexión, recibe "SAVE" del Due indicando a va a enviar data del TXT
    4. Responde al Due con "WAITING-FOR-DATA", quedando a la espera de ese data
    5. Cada línea recibida la procesa y modifica el JSON según su contenido
    6. Cuando reciba "FIN-TRANSMISION", cierra el JSON y lo envía al servidor
    -----------------------------------------------------------------------------------

  -------- MENSAJES ARDUINO -> ESP32 --------------

    1. Check wifi:
        "CHECK-WIFI"

    2. Guardar info en base de datos al inicio:
        2.1. Indicar guardado:
            "SAVE"
        2.2. Mandar datos a guardar, línea a línea:
            "INICIO-COMIDA" "INICIO-PLATO" "ALIMENTO,9,54.35" ... --> sendFileToEsp32()

   ******** NOT IMPLEMENTED ********
    3. Activar cámara y leer código de barras:
        "GET-BARCODE"
   *********************************

  
  -------- MENSAJES ESP32 -> ARDUINO -------------- 

    1. Hay wifi:
        "WIFI-OK"

    2. No hay wifi:
        "NO-WIFI"

    3. Esperando datos a subir:
        "WAITING-FOR-DATA"

    4. Guardado correctamente:
        "SAVED-OK"

    5. Error en el guardado (petición HTTP POST):
        "ERROR-HTTP:<codigo_error>"

   ******** NOT IMPLEMENTED ********
    6. Código de barras leído (se consulta base de datos de alimentos y se obtienen sus valores de carb, lip, prot y kcal por gramo):
        "BARCODE:<codigo_barras_leido>:<carb>;<prot>;<kcal>"
   *********************************

 */




#define RXD1 14
#define TXD1 15


#include "functions.h" // ya incluye "wifi_functions.h"



void setup() {
    // --- Serial esp32-PC ---
    #if defined(SM_DEBUG)
    SerialPC.begin(115200);
    while (!SerialPC);
    delay(100);
    #endif
    
    // --- Serial esp32-Due ---
    // DEBE TENER LA MISMA VELOCIDAD EN BAUDIOS QUE EL ARDUINO (p.ej. 115200)
    SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
    while (!SerialESP32Due);
    delay(100);

    // Configurar el módulo WiFi en modo estación (STA) para que pueda conectarse a una red WiFi
    // existente, pero no pueda aceptar conexiones entrantes como punto de acceso. Esto es necesario
    // para obtener la MAC correcta para este modo.
    WiFi.mode(WIFI_MODE_STA);

    // Intentar conectarse a la red WiFi
    connectToWiFi();
  
}



void loop() {
  
    if (SerialESP32Due.available() > 0) // Recibido algo del Due
    {
        String msgFromDUE = SerialESP32Due.readStringUntil('\n');
        msgFromDUE.trim();

        #if defined(SM_DEBUG)
        SerialPC.println("Mensaje recibido: " + msgFromDUE); 
        #endif

        // ------ Comprobar Wifi y avisar al Due ---------
        if (msgFromDUE == "CHECK-WIFI"){
            if(hayConexionWiFi()){
                #if defined(SM_DEBUG)
                SerialPC.println(F("Tengo Wifi"));
                #endif
                SerialESP32Due.println("WIFI-OK");
            }
            else { 
                #if defined(SM_DEBUG)
                SerialPC.println(F("No tengo wifi"));
                #endif
                SerialESP32Due.println("NO-WIFI");
            }
        }
        // -----------------------------------------------
        // ------ Procesar líneas y generar JSON ---------
        else if (msgFromDUE == "SAVE"){
            #if defined(SM_DEBUG)
            SerialPC.println(F("Esperando data..."));
            #endif
            SerialESP32Due.println("WAITING-FOR-DATA");
            processJSON(); // Procesa cada línea que lee del Serial hasta recibir FIN-TRANSMISION,
                           // entonces cierra el JSON y lo envía
        }
        // -----------------------------------------------
        else{ 
            // Otras (p.ej. BARCODE)
            #if defined(SM_DEBUG)
            SerialPC.println(F("Comando desconocido"));
            #endif
        }
    }

}



