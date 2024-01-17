/**
 * @file functions.h
 * @brief Este archivo contiene funciones auxiliares para convertir el fichero TXT en un JSON
 * 
 * @author Irene Casares Rodríguez
 * @date 01/12/2023
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "SD_functions.h"
#include <ArduinoJson.h>

DynamicJsonDocument doc(1024);

String apiKeyValue = "apiKeySmartCloth";
String MAC = "08:D1:F9:CB:A1:EC";

/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte el contenido del fichero en un JSON
 */
/*-----------------------------------------------------------------------------*/
void textFileToJSON()
{
    File dataFile = SD.open(fileESP32);

    if (dataFile) 
    {
        JsonArray comidas = doc.createNestedArray("comidas");
        doc["api_key"] = apiKeyValue;
        doc["MAC"] = MAC;
        JsonObject comida;
        JsonArray platos;
        JsonObject plato;
        JsonArray alimentos;

        while (dataFile.available()) 
        {
            String line = dataFile.readStringUntil('\n');
            line.trim();

            if (line == "INICIO-COMIDA") 
            {
                comida = comidas.createNestedObject();
                platos = comida.createNestedArray("platos");
            } 
            else if (line == "INICIO-PLATO") 
            {
                plato = platos.createNestedObject();
                alimentos = plato.createNestedArray("alimentos");
            } 
            else if (line.startsWith("ALIMENTO")) 
            {
                JsonObject alimento = alimentos.createNestedObject();
                // Aquí asumimos que los datos del alimento están separados por comas
                int firstCommaIndex = line.indexOf(',');
                int secondCommaIndex = line.lastIndexOf(',');
                alimento["grupo"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
                alimento["peso"] = line.substring(secondCommaIndex + 1);
            } 
            else if (line.startsWith("FIN-COMIDA")) 
            {
                int firstCommaIndex = line.indexOf(',');
                int secondCommaIndex = line.lastIndexOf(',');
                comida["fecha"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
                comida["hora"] = line.substring(secondCommaIndex + 1);
            }
        }

        dataFile.close();

        //serializeJson(doc, Serial);
        serializeJsonPretty(doc, Serial);

    } 
    else 
    {
        Serial.println("\nError al abrir el archivo data-ESP.txt");
    }

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda el objeto JSON en un fichero .json
 */
/*-----------------------------------------------------------------------------*/
void saveJSON()
{
    
    File jsonFile = SD.open(fileJSON, FILE_WRITE);

    if (jsonFile) 
    {
        // Escribir el JSON en el archivo
        serializeJson(doc, jsonFile);

        // Cerrar el archivo
        jsonFile.close();

        Serial.println("\n\nJSON guardado en jsonFile.json");
    } 
    else 
    {
        Serial.println("\n\nError al abrir el archivo data-ESP.json");
    }
}


#endif