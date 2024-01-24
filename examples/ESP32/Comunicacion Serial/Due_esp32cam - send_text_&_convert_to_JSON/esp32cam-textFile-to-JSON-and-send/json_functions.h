/**
 * @file json_functions.h
 * @brief Este archivo contiene funciones auxiliares para convertir el mensaje a JSON.
 * 
 * @author Irene Casares Rodríguez
 * @date 13/01/2024
 */

#ifndef JSON_FUNCTIONS_H
#define JSON_FUNCTIONS_H

#define SerialPC Serial
#define SerialESP32Due Serial1

#include "wifi_functions.h"

#include <ArduinoJson.h>


// El JSONdoc debe crearse fuera de la función para no crear uno nuevo cada vez que se procese
// una línea. Los objetos y arrays (comidas, comida, platos, plato y alimentos) se debe declarar
// fuera para que estén disponibles en todas las opciones de línea (dentro de todos los IF)

// Crear el documento JSON
DynamicJsonDocument JSONdoc(1024); 

JsonArray comidas = JSONdoc.createNestedArray("comidas");
JsonObject comida;
JsonArray platos;
JsonObject plato;
JsonArray alimentos;

/*-----------------------------------------------------------------------------*/
/**
 * @brief Añade la línea al JSON según su contenido. Un solo JSON para toda la info.
 */
/*-----------------------------------------------------------------------------*/
void addLineToJSON_ONE(String line)
{
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
        // Agregar la dirección MAC del ESP32 al objeto JSON
        JSONdoc["MAC"] = macAddress;

        // Mostrar JSON 
        serializeJsonPretty(JSONdoc, SerialPC); 

        // Avisar al Due de que ya se tiene el JSON completo
        SerialESP32Due.println("JSON completo");

        // Enviar JSON a database
        sendJsonDocToDatabase(JSONdoc);
        
    }
    else
    {
        SerialESP32Due.println("línea desconocida");
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Crea un JSON para cada comida, añade su información y lo envía.
 *        De esta forma divide la información en varios paquetes, para no
 *        tener que crear un JSON extremadamente grande para guardar todo
 *        el fichero txt.
 */
/*-----------------------------------------------------------------------------*/
void addLineToJSON(String line)
{
    if (line == "INICIO-COMIDA") 
    {
        // Limpiar el DynamicJsonDocument antes de reutilizarlo para otra comida
        JSONdoc.clear();

        comidas = JSONdoc.createNestedArray("comidas");
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

        // Agregar la dirección MAC del ESP32 al objeto JSON
        JSONdoc["MAC"] = macAddress;

        // Mostrar JSON 
        serializeJsonPretty(JSONdoc, SerialPC); 

        // Finalizar el DynamicJsonDocument actual y enviarlo a la base de datos
        // Se envía solo la info de la comida actual. Si hay otro INICIO-COMIDA, se
        // enviará otro JSON y así hasta recibir FIN-TRANSMISION
        sendJsonDocToDatabase(JSONdoc);
    }
    else if (line == "FIN-TRANSMISION") // El Due ha terminado de enviar el fichero
    {
        SerialESP32Due.println("Transmisión completa");
    }
    else
    {
        SerialESP32Due.println("línea desconocida");
    }
}






#endif