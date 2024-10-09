/*
  Irene Casares Rodríguez
  12/01/2024

  Este programa recibe el contenido del fichero TXT por Serial desde el Due, lo convierte
  en JSON y lo envía a la base de datos para guardar su información.
  
*/


/*

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
      |     3V3                        |  cable marrón (3V3)     |
      |     GND                        |  cable gris (GND)       | 
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

  -----------------------------------------------------------------------------------
    1. Recibe "CHECK-WIFI" del Due
    2. Comprueba la conexión WiFi:
        2.1. Hay conexión, responde "WIFI"
        2.2. No hay conexión, responde "NO-WIFI"

    3. Si hay conexión, recibe "SAVE" del Due indicando a va a enviar data del TXT
    4. Responde al Due con "WAITING-FOR-DATA", quedando a la espera de ese data
    5. Cada línea recibida la procesa y modifica el JSON según su contenido
    6. Cuando recibe "FIN-COMIDA", cierra el JSON y lo envía al servidor.
    7. Repite pasos 5 y 6 hasta recibir "FIN-TRANSMISION"
    -----------------------------------------------------------------------------------

  -------- MENSAJES ARDUINO -> ESP32 --------------

    1. Check wifi:
        "CHECK-WIFI"

    2. Guardar info en base de datos al inicio:
        2.1. Indicar guardado:
            "SAVE"
        2.2. Mandar datos a guardar, línea a línea:
            "INICIO-COMIDA" "INICIO-PLATO" "ALIMENTO,9,54.35" ... --> sendFileToEsp32MealByMeal()
        2.3. Finalizar guardado:
            "FIN-TRANSMISION"

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

    4. JSON guardado correctamente:
        "SAVED-OK"

    5. Error en el guardado (petición HTTP POST):
        "ERROR-HTTP:<codigo_error>"

   ******** NOT IMPLEMENTED ********
    6. Código de barras leído (se consulta base de datos de alimentos y se obtienen sus valores de carb, lip, prot y kcal por gramo):
        "BARCODE:<codigo_barras_leido>:<carb>;<prot>;<kcal>"
   *********************************

*/


#include "Serial_functions.h" // Funciones de interación con Seriales ESP32-PC y ESP32-Due
#include "json_functions.h" // ya incluye "wifi_functions.h"


void setup() 
{ 
    // --- COMUNICACIÓN SERIAL ---
    setupAllSerial(); // Inicializar comunicación serie con PC y Due
    delay(100);

    SerialPC.println("HOLA PC DESDE ESP32");
    //SerialDue.println("HOLA DUE");
    sendMsgToDue("HOLA DUE");
    // ---------------------------

    //scanNetworks();

    // --- CONEXIÓN WIFI ---------
    setupWiFi();
    // ---------------------------
}


void loop() 
{

    //if (SerialDue.available() > 0) // Recibido algo del Due
    if(hayMsgFromDue())
    {
        /*String msgFromDUE = SerialDue.readStringUntil('\n');
        msgFromDUE.trim();*/
        String msgFromDue;
        readMsgFromSerialDue(msgFromDue);


        SerialPC.println("Mensaje recibido: " + msgFromDue); 

        // ------ Comprobar Wifi y avisar al Due ---------
        if (msgFromDue == "CHECK-WIFI"){ 
            checkWiFi(); // Comprobar si hay WiFi y responder al Due (WIFI-OK o NO-WIFI)
        // -----------------------------------------------
        // ------ Procesar líneas y generar JSON ---------
        }
        else if (msgFromDue == "SAVE"){
            //processJSON();          // Procesa cada línea y las mete todas en un JSON
            //processJSON_OnePerMeal(); // Procesa cada línea y mete cada comida en un JSON
            processJSON_OnePerMeal_testServer(); // Procesa cada línea y mete cada comida en un JSON. Usa servidor de testeo (FreeHosting)
        }
        // -----------------------------------------------
        else{ 
            // Otras (p.ej. BARCODE)
            SerialPC.println(F("Comando desconocido"));
        }
    }


}
