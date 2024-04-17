/**
 * @file functions_String.h
 * @brief Este archivo contiene funciones auxiliares para recibir el mensaje del Due y convertirlo en JSON.
 * 
 * @author Irene Casares Rodríguez
 * @date 12/01/2024
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#define SerialESP32Due Serial1

#include <ArduinoJson.h>

// Crear el documento JSON
DynamicJsonDocument doc(1024);

JsonArray comidas = doc.createNestedArray("comidas");
JsonObject comida;
JsonArray platos;
JsonObject plato;
JsonArray alimentos;


/*-----------------------------------------------------------------------------*/
/**
 * @brief Añade la línea al JSON según su contenido
 */
/*-----------------------------------------------------------------------------*/

void lineToJSON(String line){
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
    else if (line == "FIN-TRANSMISION") // El Due ha terminado de enviar el fichero
    {
        // Mostrar JSON 
        serializeJsonPretty(doc, Serial); 

        SerialESP32Due.println("JSON completo");
    }
    else
    {
        SerialESP32Due.println("línea desconocida");
    }
}






#endif