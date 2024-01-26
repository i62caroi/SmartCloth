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

#include "cadenas.h"


String MAC = "08:D1:F9:CB:A1:EC";



/*-----------------------------------------------------------------------------*/
void    processJSON();
void    stringToJSON(DynamicJsonDocument& doc, const String& fileContent);
void    stringToJSON_copia(DynamicJsonDocument& doc, const String& fileContent);

/*-----------------------------------------------------------------------------*/

#define JSON_SIZE_LIMIT 4096

/*-----------------------------------------------------------------------------*/
/**
 * @brief Crea el documento JSON en un ámbito cerrado para poder liberar la memoria 
 *        RAM que ocupe al salir del mismo (al salir de la función)
 */
/*-----------------------------------------------------------------------------*/
void  processJSON()
{
    // ------------------------------------------------------------
    // Versión 6.21.5 de ArduinoJson
    // ------------------------------
    // Se reservan 4KB de RAM para el JSON, aunque acabe siendo más pequeño.
    //
    // La única forma de liberar esa memoria (sin reiniciar el dispositivo) es declarando 
    // el documento JSON dentro de una función para que al terminar la función, al salir 
    // del ámbito donde se declaró el 'doc', se libere su memoria.
    //
    // Si se declarara como global, solo se podría liberar su memoria reiniciando el dispositivo.
    DynamicJsonDocument doc(JSON_SIZE_LIMIT); 
    // ------------------------------------------------------------

    // ------------------------------------------------------------
    // Versión 7 de ArduinoJson
    // ------------------------------
    // Hay una nueva versión 7 de ArduinoJson que se supone que permite trabajar con documentos JSON
    // sin necesidad de indicar el tamaño, sino que el fichero ocupa más memoria conforme va creciendo.
    // He intentado adaptar este programa a esa versión, pero tras muchos fallos me he dado cuenta
    // de que no es que no supiera adaptarlo bien, es que incluso con todo comentado excepto la 
    // linea de #include <ArduinoJson.h>, el programa siempre daba el mismo error de compilación:
    //      In constructor 'ArduinoJson::V701PB2::JsonDocument::JsonDocument(ArduinoJson::V701PB2::JsonDocument&&)':
    //      error: no matching function for call to 'ArduinoJson::V701PB2::JsonDocument::JsonDocument()'
    // Daba ese error aunque no se creara ningún 'JsonDocument'
    // ------------------------------------------------------------


    // Convertir la String (simulación del fichero txt) a JSON pasando la cadena y el documento 
    // donde guardar el JSON:
    //stringToJSON(doc, string3); // Una sola comida con 10 platos de 10 alimentos cada uno (sobredimensionada)
    //stringToJSON(doc, string4); // Una sola comida con 10 platos de 10 alimentos cada uno (sobredimensionada)

    // Convertir la String a JSON convirtiendo solo comidas completas (sin superar JSON-JSON_SIZE_LIMIT)
    stringToJSON_copia(doc, string4); 

    // Mostrar JSON 
    serializeJsonPretty(doc, Serial);

    Serial.print("\n\nMemoria RAM usada: "); Serial.println(doc.memoryUsage());

}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte el contenido del String en un JSON y lo almacena en 'doc'
 * @param doc - documento tipo 'DynamicJsonDocument' donde guardar el JSON generado
 * @param fileContent - String (simulación de fichero txt) a convertir en en JSON
 */
/*-----------------------------------------------------------------------------*/
void stringToJSON(DynamicJsonDocument& doc, const String& fileContent)
{
    JsonArray comidas = doc.createNestedArray("comidas");
    doc["MAC"] = MAC;
    JsonObject comida;
    JsonArray platos;
    JsonObject plato;
    JsonArray alimentos;

    int start = 0;
    int end = fileContent.indexOf('\n');

    // Contar platos completos introducidos en el JSON. Si se quedan alimentos sin meter,
    // no se considera completo
    //int nPlatosJSON = 0;
    
    // Para mantener un seguimiento de la última línea de comida exitosamente añadida al JSON
    // Vamos contando las líneas (lineNumber) y cuando se termina de procesar una comida completa
    // (cuando se lee FIN-COMIDA y no se ha superado el tamaño máximo del JSON), entonces se guarda
    // el número de línea.
    /*int lineNumber = 0;
    int lastSuccessfulComidaLine = -1;
    bool alimentoProcesado = false;
*/
    while (end != -1) 
    {
        String line = fileContent.substring(start, end);
        line.trim();

        //lineNumber++; // Contar línea leída

        if (line == "INICIO-COMIDA") 
        {
            comida = comidas.createNestedObject();
            platos = comida.createNestedArray("platos");
        } 
        else if (line == "INICIO-PLATO") 
        {
            plato = platos.createNestedObject();
            alimentos = plato.createNestedArray("alimentos");

            // Si se ha procesado al menos un alimento (como decir que lo último leído fue ALIMENTO)
            // antes de leer INICIO-PLATO, entonces podemos decir que se ha completado un plato
            //if (alimentoProcesado) nPlatosJSON++; 
            //alimentoProcesado = false;
        } 
        else if (line.startsWith("ALIMENTO")) 
        {
            JsonObject alimento = alimentos.createNestedObject();
            // Aquí asumimos que los datos del alimento están separados por comas
            int firstCommaIndex = line.indexOf(',');
            int secondCommaIndex = line.lastIndexOf(',');
            alimento["grupo"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
            alimento["peso"] = line.substring(secondCommaIndex + 1);

            // Para contar plato completo la próxima línea de INICIO-PLATO
            //alimentoProcesado = true; 
        } 
        else if (line.startsWith("FIN-COMIDA")) 
        {
            int firstCommaIndex = line.indexOf(',');
            int secondCommaIndex = line.lastIndexOf(',');
            comida["fecha"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
            comida["hora"] = line.substring(secondCommaIndex + 1);

            // Si se lee FIN-COMIDA, el último plato está completo
            //if (alimentoProcesado) nPlatosJSON++; 

            // Si se ha procesado al menos un alimento y aún no se ha superado el límite de 
            // tamaño del JSON, entonces la comida está completa --> guardamos el índice de
            // la línea de FIN-COMIDA leída
            //if (alimentoProcesado && measureJson(doc) <= JSON_SIZE_LIMIT) 
              //  lastSuccessfulComidaLine = lineNumber;
        }

        // Si en algún momento superamos el tamaño máximo del JSON, eliminamos lo último guardado
        // en el JSON hasta la última comida completa.
        // De esta forma siempre enviaremos comidas completas, aunque tengamos que enviar otros JSONs
        // después para enviar el resto de la información que tengamos.
        /*if (measureJson(doc) > JSON_SIZE_LIMIT) {
            while (comidas.size() > 0 && measureJson(doc) > JSON_SIZE_LIMIT) {
                comidas.remove(comidas.size() - 1);
            }
            break;
        }*/



        start = end + 1;
        end = fileContent.indexOf('\n', start);
    }


}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte el contenido del String en un JSON 
 *        El String debe contener líneas que representen una comida, un plato o un alimento.
 *        Si el documento JSON se llena durante el procesamiento, se restaura al estado de la 
 *        última comida completa, es decir, que el JSON final solo incluya comidas completas.
 * 
 * @param doc - documento tipo 'DynamicJsonDocument' donde guardar el JSON generado
 * @param fileContent - String (simulación de fichero txt) a convertir en en JSON
 */
/*-----------------------------------------------------------------------------*/
void stringToJSON_copia(DynamicJsonDocument& doc, const String& fileContent)
{
    // Si ArduinoJson ve que lo que va a meter en el JSON haría que superara su capacidad, no lo hace.
    // Entonces, no podemos simplemente comprobar si el JSON se ha pasado de JSON_SIZE_LIMIT, porque nunca
    // lo hará, sino que comprobamos si se va modificando la cantidad de memoria usada por el JSON, 
    // lo que significaría que se están añadiendo elementos. En cuanto deje de aumentar la memoria usada,
    // significará que el JSON ya no admite más elementos.
    // En ese caso, si se ha quedado una comida a medias (no se ha alcanzado FIN-COMIDA), deberemos ignorarla
    // y solo dejar en el JSON comidas completas porque así será más fácil saber qué información se ha subido
    // a la base de datos y cuál está pendiente. De hecho, si se intenta subir una comida que no está completa,
    // no creo que fuera posible guardar la información en la base de datos.
    // Para "ignorar" la comida que se ha quedado a medias, la mejor forma es crear copias del JSON cuando
    // se complete una comida, de forma que cuando haya que "ignorar" una comida no terminada, simplemente
    // se restaure esa copia.

    // Último estado exitoso del JSON (solo comidas completas)
    String lastSuccessfulState;

    // Elementos del documento JSON
    JsonArray comidas = doc.createNestedArray("comidas");
    doc["MAC"] = MAC;
    JsonObject comida;
    JsonArray platos;
    JsonObject plato;
    JsonArray alimentos;

    // Para las cadenas del String, no es necesario para fichero txt
    int start = 0;
    int end = fileContent.indexOf('\n');
    
    size_t lastMemoryUsage = 0;


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

            size_t currentMemoryUsage = doc.memoryUsage();
            // Si el uso de memoria ha cambiado (se ha añadido algo al JSON), guarda una copia del estado del JSON
            if (currentMemoryUsage != lastMemoryUsage) {
                Serial.println("\n\n::::::::::::::::::::::\nGenerando copia\n::::::::::::::::::::::");
                // Guarda una copia del estado del JSON para restaurarla si se pasa de tamaño
                lastSuccessfulState = "";
                serializeJson(doc, lastSuccessfulState);
            }
            // lastMemoryUsage = currentMemoryUsage; // Ya se hace después del if
        }


        size_t currentMemoryUsage = doc.memoryUsage();
        // Si el uso de memoria no ha cambiado (el JSON ya no admite más datos), restaura el JSON al estado de la última comida completa
        if (currentMemoryUsage == lastMemoryUsage) {
            Serial.println("\n\n********************\nMemoria casi llena, no se pueden agregar más líneas\n********************");
            // Restaura el JSON al estado de la última comida completa
            doc.clear(); 
            deserializeJson(doc, lastSuccessfulState); // Restauramos último estado exitoso del JSON (solo comidas completas)
            return; // Salir de la función
        }

        lastMemoryUsage = currentMemoryUsage;

        start = end + 1;
        end = fileContent.indexOf('\n', start);
    }

}





#endif