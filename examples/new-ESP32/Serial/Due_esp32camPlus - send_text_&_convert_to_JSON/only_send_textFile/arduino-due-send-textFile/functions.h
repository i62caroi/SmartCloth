/*
    Irene Casares Rodríguez
    12/01/2024

    Funciones para enviar un String como simulación del esp32File del Due al ESP32 
    por comunicación Serial.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "cadenas.h"
#include <vector>

#define SerialPC Serial
#define SerialDueESP32 Serial1



/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    sendStringSimulationToEsp32(String fileContent);    // Enviar String al esp32 simulando el fichero TXT
/*-----------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía el fichero (String de simulación) por Serial al esp32
 */
/*-----------------------------------------------------------------------------*/
void sendStringSimulationToEsp32(String fileContent)
{
    int start = 0;
    int end = fileContent.indexOf('\n');

    while (end != -1) 
    {
        String line = fileContent.substring(start, end);
        line.trim();

        // Envía la línea al ESP32 a través de Serial
        SerialDueESP32.println(line);

        start = end + 1;
        end = fileContent.indexOf('\n', start);
    }

    // Envía la última línea si existe
    if (start < fileContent.length()) {
        String line = fileContent.substring(start);
        line.trim();
        SerialDueESP32.println(line);
    }

    SerialDueESP32.println(F("FIN-TRANSMISION"));
}





#endif