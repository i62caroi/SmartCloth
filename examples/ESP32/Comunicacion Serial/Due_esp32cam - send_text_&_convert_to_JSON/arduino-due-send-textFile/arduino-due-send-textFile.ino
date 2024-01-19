/*
    Irene Casares Rodríguez
    12/01/2024

    Este programa lee el fichero TXT de la SD y lo envía por Serial al esp32 para
    que lo convierta a JSON y lo suba a la base de datos
 */

#include "functions.h"
#include "functions_String.h"

#define SerialPC Serial

void setup()
{
    SerialPC.begin(115200);
    while (!SerialPC);

    // DEBE TENER LA MISMA VELOCIDAD EN BAUDIOS QUE EL ESP32 (p.ej. 115200)
    SerialDueESP32.begin(115200);
    while (!SerialDueESP32);

    SerialPC.println("HOLA PC DESDE DUE");  
    SerialDueESP32.println("HOLA ESP32");
     
    // Inicializar SD
    /*if (!setupSDcard()){
        SerialPC.println("Card failed, or not present");
        return;
    }
    delay(100);*/
}


void loop() {
  if (SerialPC.available() > 0) {
    SerialPC.print("PC, dime String a enviar: ");
    String idString = SerialPC.readStringUntil('\n');
    SerialPC.println(idString);

    // Enviar al ESP32-CAM la cadena indicada
    if(idString == "1") sendStringSimulationtoEsp32(string1);
    else if(idString == "2") sendStringSimulationtoEsp32(string2);
  }

  if (SerialDueESP32.available() > 0) {
    String msg = SerialDueESP32.readStringUntil('\n');

    SerialPC.print("Mensaje recibido: "); SerialPC.println(msg); 
  }

}


















