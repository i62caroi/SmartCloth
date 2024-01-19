/**
 * @file functions_String.h
 * @brief Este archivo contiene funciones auxiliares para convertir un String que simula el fichero TXT en un JSON.
 * 
 * @author Irene Casares Rodríguez
 * @date 10/01/2024
 */

#ifndef FUNCTIONS_STRING_H
#define FUNCTIONS_STRING_H

#include <ArduinoJson.h>

DynamicJsonDocument doc(1024);

String apiKeyValue = "apiKeySmartCloth";
String MAC = "08:D1:F9:CB:A1:EC";

// Simular la lectura del archivo
String string1 = "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "FIN-COMIDA,05.12.2023,17:55:36\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,24.40\n"
                "FIN-COMIDA,05.12.2023,18:25:25\n";
                
                

String string2 = "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "FIN-COMIDA,05.12.2023,17:55:36\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,24.40\n"
                "FIN-COMIDA,05.12.2023,18:25:25\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "INICIO-PLATO\n"
                "ALIMENTO,3,32.07\n"
                "ALIMENTO,27,46.65\n"
                "ALIMENTO,10,16.23\n"
                "FIN-COMIDA,20.12.2023,10:39:36\n";

/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte el contenido del String en un JSON
 */
/*-----------------------------------------------------------------------------*/
void stringToJSON(String fileContent)
{
    JsonArray comidas = doc.createNestedArray("comidas");
    doc["api_key"] = apiKeyValue;
    doc["MAC"] = MAC;
    JsonObject comida;
    JsonArray platos;
    JsonObject plato;
    JsonArray alimentos;

    int start = 0;
    int end = fileContent.indexOf('\n');

    while (end != -1) 
    {
        String line = fileContent.substring(start, end);
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

        start = end + 1;
        end = fileContent.indexOf('\n', start);
    }


    // Mostrar JSON 
    serializeJsonPretty(doc, Serial);


}




#endif