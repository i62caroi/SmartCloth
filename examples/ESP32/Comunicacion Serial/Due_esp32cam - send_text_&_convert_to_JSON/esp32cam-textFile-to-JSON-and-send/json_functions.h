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
#include <TimeLib.h>


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
void    addLineToJSON_ONE(String line); // Todo en un JSON, aunque se quede a mitad, y se envía


time_t convertTimeToUnix(String &line, int &firstCommaIndex, int &secondCommaIndex); // Convertir fecha de String a formato Unix timestamp
/*-----------------------------------------------------------------------------*/


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
        alimento["grupo"] = line.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
        alimento["peso"] = line.substring(secondCommaIndex + 1).toFloat();
    } 
    else if (line.startsWith("FIN-COMIDA")) 
    {
        int firstCommaIndex = line.indexOf(',');
        int secondCommaIndex = line.lastIndexOf(',');
        //comida["fecha"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
        //comida["hora"] = line.substring(secondCommaIndex + 1);
        // ---
        time_t timestamp = convertTimeToUnix(line, firstCommaIndex, secondCommaIndex);
        comida["fecha"] = (int)timestamp;
        // ---
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
        alimento["grupo"] = line.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
        alimento["peso"] = line.substring(secondCommaIndex + 1).toFloat();
    } 
    else if (line.startsWith("FIN-COMIDA")) 
    {
        int firstCommaIndex = line.indexOf(',');
        int secondCommaIndex = line.lastIndexOf(',');
        //comida["fecha"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
        //comida["hora"] = line.substring(secondCommaIndex + 1);
        // ---
        time_t timestamp = convertTimeToUnix(line, firstCommaIndex, secondCommaIndex);
        comida["fecha"] = (int)timestamp;
        // ---

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





/*-----------------------------------------------------------------------------*/
/**
 * @brief Añade la línea al JSON según su contenido. Un solo JSON para toda la info.
 *        Esta función solo imprime el JSON por terminal, no lo envía.
 */
/*-----------------------------------------------------------------------------*/
void addLineToJSON_print(String line)
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
        alimento["grupo"] = line.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
        alimento["peso"] = line.substring(secondCommaIndex + 1).toFloat();
    } 
    else if (line.startsWith("FIN-COMIDA")) 
    {
        int firstCommaIndex = line.indexOf(',');
        int secondCommaIndex = line.lastIndexOf(',');
        //comida["fecha"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
        //comida["hora"] = line.substring(secondCommaIndex + 1);
        // ---
        time_t timestamp = convertTimeToUnix(line, firstCommaIndex, secondCommaIndex);
        comida["fecha"] = (int)timestamp;
        // ---
    }
    else if (line == "FIN-TRANSMISION") // El Due ha terminado de enviar el fichero
    {
        // Agregar la dirección MAC del ESP32 al objeto JSON
        JSONdoc["MAC"] = macAddress;

        // Mostrar JSON 
        serializeJsonPretty(JSONdoc, SerialPC); 

        // Avisar al Due de que ya se tiene el JSON completo
        SerialESP32Due.println("JSON completo");
        
    }
    else
    {
        SerialESP32Due.println("línea desconocida");
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte una fecha y hora en formato de cadena a un valor de tiempo Unix.
 * 
 * @param firstCommaIndex Índice de la primera coma en la cadena.
 * @param secondCommaIndex Índice de la segunda coma en la cadena.
 * @return time_t Valor de tiempo Unix correspondiente a la fecha y hora especificadas.
 */
/*-----------------------------------------------------------------------------*/
time_t convertTimeToUnix(String &line, int &firstCommaIndex, int &secondCommaIndex)
{
    String fecha = line.substring(firstCommaIndex + 1, secondCommaIndex);
    String hora = line.substring(secondCommaIndex + 1);

    int day = fecha.substring(0,2).toInt();
    int month = fecha.substring(3,5).toInt();
    int year = fecha.substring(6,10).toInt();

    int hour = hora.substring(0,2).toInt();
    int minute = hora.substring(3,5).toInt();
    int second = hora.substring(6,8).toInt();

    tmElements_t tm;
    tm.Day = day;
    tm.Month = month;
    tm.Year = year - 1970;
    tm.Hour = hour;
    tm.Minute = minute;
    tm.Second = second;

    time_t timestamp = makeTime(tm);

    return timestamp;
}



#endif