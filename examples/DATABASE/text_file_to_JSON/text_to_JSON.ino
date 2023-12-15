/**
 * @file text_to_JSON.ino
 * @brief Este programa convierte el contenido de data-esp32.txt en un fichero JSON para enviar por HTTP POST.
 * 
 * @author Irene Casares Rodríguez
 * @date 11/12/2023
 */

#include "SD_functions.h"
#include "functions.h"


void setup()
{
    Serial.begin(115200);
    while (!Serial){};
     
    // Inicializar SD
    if (!setupSDcard()){
        Serial.println("Card failed, or not present");
        return;
    }
    delay(100);

    // Leer fichero
    readFileESP32();
    
    // Convertir a JSON
    textToJSON();

    // Guardar JSON en fichero .json
    //saveJSON();

}




void loop() {
    
   delay(100);
}
















