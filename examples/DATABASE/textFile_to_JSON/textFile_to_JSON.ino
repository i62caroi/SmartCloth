/**
 * @file text_to_JSON.ino
 * @brief Este programa convierte el contenido de data-esp32.txt en un fichero JSON para enviar por HTTP POST.
 * 
 * @author Irene Casares Rodríguez
 * @date 11/12/2023
 */

#include "functions.h"
#include "functions_String.h"

bool crearJson = true;

void setup()
{
    Serial.begin(115200);
    while (!Serial){};
     
    // ---- TARJETA SD -----------------
    // Inicializar SD
    /*if (!setupSDcard()){
        Serial.println("Card failed, or not present");
        return;
    }
    delay(100);

    // Leer fichero
    readFileESP32();
    
    // Convertir fichero TXT a JSON y guardar .jsn en SD
    processJSON_txtFile();
    */
    // ---------------------------------

    // ---- NO SD (STRING) -------------
    processJSON();
    // ---------------------------------

}




void loop() {
    
    /*if(crearJson){
        DynamicJsonDocument doc(JSON_SIZE_LIMIT);
        processJSON(doc);
        crearJson = false;
    }*/

   delay(100);
}


















