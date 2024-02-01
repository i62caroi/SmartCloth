

//#include <JSON.h>
//#include <JSONVar.h>

/**
 * @file text_to_JSON.ino
 * @brief Este programa convierte el contenido de data-esp32.txt en un fichero JSON para enviar por HTTP POST.
 * 
 * @author Irene Casares Rodríguez
 * @date 11/12/2023
 */

//#include "functions.h"
#include "functions_String.h"


void setup()
{
    Serial.begin(115200);
    while (!Serial){};
     
    // Inicializar SD
    /*if (!setupSDcard()){
        Serial.println("Card failed, or not present");
        return;
    }
    delay(100);*/

    // ---- TARJETA SD -----------------
    // Leer fichero
    //readFileESP32();
    
    // Convertir a JSON
    //textFileToJSON();

    // Guardar JSON en fichero .json
    //saveJSON();
    // ---------------------------------

    //Serial.println(ARDUINOJSON_VERSION);

    // ---- NO SD (STRING) -------------
    processJSON();
    // ---------------------------------

}




void loop() {
    
   delay(100);
}


















