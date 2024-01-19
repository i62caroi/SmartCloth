/*
    Irene Casares Rodríguez
    12/01/2024

    Funciones para enviar el esp32File con el log de comidas del Due al ESP32 por
    comunicación SerialPC.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "SD_functions.h"

#define SerialPC Serial
#define SerialDueESP32 Serial1


/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía el fichero (String de simulación) por Serial al esp32
 */
/*-----------------------------------------------------------------------------*/
void sendFileToEsp32()
{
    File dataFile = SD.open(fileESP32);

    if (dataFile) 
    {
        while (dataFile.available()) 
        {
            String line = dataFile.readStringUntil('\n');
            line.trim();

            // Envía la línea al ESP32 a través de Serial
            SerialDueESP32.println(line);
        }
        dataFile.close();

        SerialPC.println("\nFichero completo enviado");
    }
    else 
    {
        SerialPC.println("\nError al abrir el archivo data-ESP.txt");
    }
}



#endif