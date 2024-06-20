/*
    Irene Casares Rodríguez
    12/01/2024

    Este programa lee el fichero TXT de la SD y lo envía por Serial al esp32 para
    que lo convierta a JSON y lo suba a la base de datos
 */

#include "wifi_functions.h" 
#include "cadenas_functions.h" 
#include "SD_functions.h" // incluye Serial_functions.h



void setup()
{
    setupSerial(); // Inicializar comunicación serie con PC y ESP32
    delay(100);
     
    // Inicializar SD
    if (!setupSDcard())
    {
        SerialPC.println(F("Card failed, or not present"));
        return;
    }
    delay(100);

    SerialPC.println(F("ENVIAME \"go\" PARA COMENZAR"));

}

void loop() 
{

    // ---------------- ENVIO ------------------------------------
    // Simulacion de lectura de fichero enviando String estática
    if (SerialPC.available() > 0)  // Si se recibe algo desde el PC ("go")
    {
        String msg = SerialPC.readStringUntil('\n');
        msg.trim();
        if(msg == "go")
        { 
            // Comprobar WiFi
            if(checkWifiConnection()) // Pregunta al esp32 si hay conexión (CHECK-WIFI) y espera su respuesta
            {
                // Indicar inicio de guardado
                SerialPC.println(F("\nIndicando que se quiere guardar..."));
                //SerialDueESP32.println(F("SAVE"));
                sendMsgToESP32("SAVE");
            }
        }
    }

    // -----------------------------------------------------------


    // -------------- RECEPCION ----------------------------------
    //if (SerialDueESP32.available() > 0)  // Si se recibe algo desde el esp32
    if(hayMsgFromESP32())
    {
        /*String msgFromESP32 = SerialDueESP32.readStringUntil('\n');
        msgFromESP32.trim();  */
        String msgFromESP32 = readMsgFromESP32();
        SerialPC.print("\nMensaje recibido en loop: "); SerialPC.println(msgFromESP32); 

        // Esperando datos a subir
        if(msgFromESP32 == "WAITING-FOR-DATA") // El ESP32 se autenticó correctamente y está esperando datos
        {
            SerialPC.println("\nEnviando data...");

            // --- Enviar cadena de simulación al ESP32 ------------
            //sendStringSimulationToEsp32(string2);             // Todas las comidas en un JSON
            //sendStringSimulationToEsp32MealByMeal(string2);   // Un JSON por comida
            // -----------------------------------------------------

            // --- Enviar fichero TXT al ESP32 ---------------------
            //sendFileToESP32();            // Todas las cadenas de golpe --> todas las comidas en un JSON
            sendFileToESP32MealByMeal();  // Enviar comida a comida --> un JSON por comida
            // -----------------------------------------------------

        }
        else if(msgFromESP32 == "NO-TOKEN") // Falló la autenticación del ESP32 y no puede subir datos
        {
            SerialPC.println(F("Fallo en la autenticacion del SmartCloth en la database"));
        }
        /*else if(msg == "NO-WIFI")
        {
            SerialPC.println(F("Se ha perdido la conexion a Internet"));
        }*/

        //else SerialPC.println("Comando desconocido\n");
    }
    // -----------------------------------------------------------
    
}



/*
// Simulacion de lectura de fichero enviando String estática
    if(!msgPC){
        SerialPC.print("PC, dime String a enviar: ");
        msgPC = true;
    }

    if (SerialPC.available() > 0) { // Si se recibe algo desde el PC
        String idString = SerialPC.readStringUntil('\n');
        SerialPC.println(idString);

        if(checkWifiConnection()){ // Pregunta al esp32 si hay conexión (CHECK-WIFI) y espera su respuesta
            // Enviar al ESP32-CAM la cadena indicada
            if(idString == "1") sendStringSimulationToEsp32MealByMeal(string1);
            else if(idString == "2") sendStringSimulationToEsp32MealByMeal(string2);
        }

        msgPC = false;
    }
*/


















