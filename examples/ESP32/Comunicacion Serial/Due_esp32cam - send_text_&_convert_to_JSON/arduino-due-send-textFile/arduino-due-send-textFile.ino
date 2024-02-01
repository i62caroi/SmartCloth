/*
    Irene Casares Rodríguez
    12/01/2024

    Este programa lee el fichero TXT de la SD y lo envía por Serial al esp32 para
    que lo convierta a JSON y lo suba a la base de datos
 */

#include "functions.h" 
#include "SD_functions.h"

#define SerialPC Serial
#define SerialDueESP32 Serial1

int nComidasToSend = -1;
int nComidasUploaded = -2;


void setup()
{
    SerialPC.begin(115200);
    while (!SerialPC);

    // DEBE TENER LA MISMA VELOCIDAD EN BAUDIOS QUE EL ESP32 (p.ej. 115200)
    SerialDueESP32.begin(115200);
    while (!SerialDueESP32);
     
    // Inicializar SD
    /*if (!setupSDcard()){
        SerialPC.println(F("Card failed, or not present"));
        return;
    }
    delay(100);*/

    SerialPC.println(F("ENVIAME \"go\" PARA COMENZAR"));

}

void loop() 
{

    // ---------------- ENVIO ------------------------------------
    // Simulacion de lectura de fichero enviando String estática
    if (SerialPC.available() > 0) { // Si se recibe algo desde el PC ("go")
        String msg = SerialPC.readStringUntil('\n');
        msg.trim();
        if(msg == "go"){ 
            if(checkWifiConnection()){ // Pregunta al esp32 si hay conexión y espera su respuesta
                SerialPC.println(F("\nIndicando que se quiere guardar..."));
                SerialDueESP32.println(F("SAVE"));
            }
        }
    }

    // -----------------------------------------------------------


    // -------------- RECEPCION ----------------------------------
    if (SerialDueESP32.available() > 0) { // Si se recibe algo desde el esp32
        String msg = SerialDueESP32.readStringUntil('\n');
        msg.trim();  
        SerialPC.print("Mensaje recibido: "); SerialPC.println(msg); 

        // Esperando datos a subir
        if(msg == "WAITING-FOR-DATA"){
            SerialPC.println("\nEnviando data...");
            // Enviar al ESP32-CAM la cadena 
            sendStringSimulationToEsp32(string1);
        }
    }
    // -----------------------------------------------------------
    
}

/*
void loop() 
{
    // ---------------- ENVIO ------------------------------------
    // Simulacion de lectura de fichero enviando String estática
    if(!msgPC){
        SerialPC.print("PC, dime String a enviar: ");
        msgPC = true;
    }

    if (SerialPC.available() > 0) { // Si se recibe algo desde el PC
        String idString = SerialPC.readStringUntil('\n');
        SerialPC.println(idString);

        // Enviar al ESP32-CAM la cadena indicada
        if(idString == "1") sendStringSimulationToEsp32(string1);
        else if(idString == "2") sendStringSimulationToEsp32(string2);
    }

    // Lectura y envío real del fichero txt 
    if (SerialPC.available() > 0) { // Si se recibe algo desde el PC
        String msgPC = SerialPC.readStringUntil('\n');
        msgPC.trim();

        SerialPC.print("Mensaje recibido: "); SerialPC.println(msgPC);
        if(msgPC == "go"){ 
            // Solo enviar fichero al ESP32-CAM 
            //sendFileToEsp32ONE();

            // Enviar fichero contando comidas
            nComidasToSend = sendFileToEsp32();
        }
    }
    // -----------------------------------------------------------


    // -------------- RECEPCION ----------------------------------
    if (SerialDueESP32.available() > 0) { // Si se recibe algo desde el esp32
        String msg = SerialDueESP32.readStringUntil('\n');

        SerialPC.print("Mensaje recibido: "); SerialPC.println(msg); 

        // Acuse de recibo de comida subida
        if(msg == "JSON - OK"){
            if(nComidasUploaded == -2) nComidasUploaded = 1; // Primera comida
            else nComidasUploaded++;
        }

        // Si se han subido todas las comidas enviadas, se subió toda la info del fichero
        if(nComidasUploaded == nComidasToSend) SerialPC.println("Subidas todas las comidas");
    
    }
    // -----------------------------------------------------------
    
}
*/

















