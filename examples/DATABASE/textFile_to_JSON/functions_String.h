/**
 * @file functions_String.h
 * @brief Este archivo contiene funciones auxiliares para convertir un String que simula el fichero TXT en un JSON.
 * 
 * @author Irene Casares Rodríguez
 * @date 01/02/2024
 */

#ifndef FUNCTIONS_STRING_H
#define FUNCTIONS_STRING_H

#include <TimeLib.h>
#include <ArduinoJson.h>
#include "functions.h" // Para convertTimeToUnix()
#include "cadenas.h"

#define JSON_SIZE_LIMIT 4096


/*-----------------------------------------------------------------------------*/
void    processJSON();                                                                  // Crear el documento JSON en un ámbito cerado para liberar su memoria al terminar  
void    stringToJSON(DynamicJsonDocument& doc, const String& fileContent);              // Generar JSON con todo lo que entre, aunque una comida se quede a mitad
void    stringToJSON_lastState(DynamicJsonDocument& doc, const String& fileContent);    // Generar JSON solo comidas completas, restaurando el último estado
void    stringToJSON_lastState_v2(DynamicJsonDocument& doc, const String& fileContent); // Intento de generar varios JSON solo con comidas completas y que no se pasen de tamaño
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Crea el documento JSON en un ámbito cerrado para poder liberar la memoria 
 *        RAM que ocupe al salir del mismo (al salir de la función)
 */
/*-----------------------------------------------------------------------------*/
void  processJSON()
{
    // ---------- DECLARAR DOCUMENTO JSON -------------------------
    // ------------------------------
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
    // ------------------------------

    // ------------------------------
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
    // ------------------------------
    // ------------------------------------------------------------


    // ------------- GENERAR JSON ---------------------------------
    // Convertir la String (simulación del fichero txt) a JSON pasando la cadena y el documento 
    // donde guardar el JSON. Mete todo lo que puede en el JSON, aunque alguna comida se quede a medias:
    //stringToJSON(doc, string3); // Una sola comida con 10 platos de 10 alimentos cada uno (sobredimensionada)
    //stringToJSON(doc, string4); // Una sola comida con 10 platos de 10 alimentos cada uno (sobredimensionada)

    // Convertir la String a JSON convirtiendo solo comidas completas (sin superar JSON-JSON_SIZE_LIMIT)
    stringToJSON_lastState(doc, string4);       // Si hace falta, restaura el último estado del JSON solo con comidas completas
    //stringToJSON_lastState_v2(doc, string4);  // Intento de generar varios JSON que no se pasen de tamaño y solo tengan comidas completas
    // ------------------------------------------------------------

    // ------------- MOSTRAR JSON ---------------------------------
    Serial.println(F("\n\n********************\nContenido del JSON:\n********************"));
    serializeJsonPretty(doc, Serial);
    Serial.println(F("\n\n********************\nFin del contenido del JSON\n********************"));
    // ------------------------------------------------------------

    // -------- MEMORIA RAM USADA POR EL JSON ---------------------
    Serial.print(F("\n\nMemoria RAM usada: ")); Serial.println(doc.memoryUsage());
    // ------------------------------------------------------------

}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte el contenido del String en un JSON y lo almacena en 'doc'. 
 * Incluye toda la info que quepa en el JSON, aunque alguna comida se quede a medias.
 * @param doc - documento tipo 'DynamicJsonDocument' donde guardar el JSON generado
 * @param fileContent - String (simulación de fichero txt) a convertir en en JSON
 */
/*-----------------------------------------------------------------------------*/
void stringToJSON(DynamicJsonDocument& doc, const String& fileContent)
{
    String MAC = "08:D1:F9:CB:A1:EC";

    // Elementos del documento JSON
    JsonArray comidas = doc.createNestedArray("comidas");
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
            // Creo que el RTC está dando la hora GMT +0h
            time_t timestamp = convertTimeToUnix(line, firstCommaIndex, secondCommaIndex);
            comida["fecha"] = (int)timestamp;
            // ---
        }


        start = end + 1;
        end = fileContent.indexOf('\n', start);
    }


}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte el contenido del String en un JSON. Solo mantiene en el JSON
 * comidas completas. Si se llena el JSON en mitad de una comida, restaura la 
 * última versión que solo tenía comidas completas.
 * 
 * @param doc - documento tipo 'DynamicJsonDocument' donde guardar el JSON generado
 * @param fileContent - String (simulación de fichero txt) a convertir en en JSON
 */
/*-----------------------------------------------------------------------------*/
void stringToJSON_lastState(DynamicJsonDocument& doc, const String& fileContent)
{
    String MAC = "08:D1:F9:CB:A1:EC";

    // Último estado exitoso del JSON (solo comidas completas)
    String lastSuccessfulState;

    // Elementos del documento JSON
    JsonArray comidas = doc.createNestedArray("comidas");
    doc["MAC"] = MAC;
    JsonObject comida;
    JsonArray platos;
    JsonObject plato;
    JsonArray alimentos;

    size_t lastMemoryUsage = 0;

    // Necesitamos saber la línea de fin de la última comida completa que se pudo incluir en el JSON
    // para saber desde dónde comenzar el siguiente
    int lineIndex = 0;          // Indice de la linea actual
    int lastComidaIndex = 0;    // Indice de fin de la última comida completa (FIN-COMIDA)

    // Para las cadenas del String, no es necesario para fichero txt
    int start = 0;
    int end = fileContent.indexOf('\n');

    while (end != -1) 
    {
        String line = fileContent.substring(start, end);
        line.trim();
        lineIndex++;
        
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
            alimento["grupo"] = line.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
            alimento["peso"] = line.substring(secondCommaIndex + 1).toInt();
        } 
        else if (line.startsWith("FIN-COMIDA")) 
        {
            int firstCommaIndex = line.indexOf(',');
            int secondCommaIndex = line.lastIndexOf(',');
            //comida["fecha"] = line.substring(firstCommaIndex + 1, secondCommaIndex);
            //comida["hora"] = line.substring(secondCommaIndex + 1);
            // ---
            // Creo que el RTC está dando la hora GMT +0h
            time_t timestamp = convertTimeToUnix(line, firstCommaIndex, secondCommaIndex);
            comida["fecha"] = (int)timestamp;
            // ---

            size_t currentMemoryUsage = doc.memoryUsage();
            // Si el uso de memoria ha cambiado (se ha añadido algo al JSON), guarda una copia del estado del JSON
            if (currentMemoryUsage != lastMemoryUsage) {
                Serial.println(F("\n\n::::::::::::::::::::::\nGenerando copia\n::::::::::::::::::::::"));
                // Guarda una copia del estado del JSON para restaurarla si se pasa de tamaño
                lastSuccessfulState = "";
                serializeJson(doc, lastSuccessfulState);
                lastComidaIndex = lineIndex; // Actualiza el índice de la línea de fin de la última comida completa
                Serial.print(F("Indice de fin de última comida: ")); Serial.println(lastComidaIndex);
            }
        }


        size_t currentMemoryUsage = doc.memoryUsage();
        // Si el uso de memoria no ha cambiado (el JSON ya no admite más datos), restaura el JSON al estado de la última comida completa
        if (currentMemoryUsage == lastMemoryUsage) {
            Serial.println(F("\n\n********************\nMemoria casi llena, no se pueden agregar más líneas\n********************"));
            // Restaura el JSON al estado de la última comida completa
            doc.clear(); 
            deserializeJson(doc, lastSuccessfulState); // Restauramos último estado exitoso del JSON (solo comidas completas)
            return; // Salir de la función
        }

        lastMemoryUsage = currentMemoryUsage;

        start = end + 1;
        end = fileContent.indexOf('\n', start);

        // Si end es -1, estamos en la última línea
        if (end == -1) {
            lastComidaIndex = -1; // Se pone a -1 para indicar que se ha terminado, no quedarían comidas por enviar
            Serial.print(F("Indice de fin de última comida: ")); Serial.println(lastComidaIndex);
        }
    }

}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte una cadena de texto en formato JSON. Intento de generar varios JSON
 * para que incluyan solo comidas completas y no se pasen de tamaño.
 * 
 * @param doc El documento JSON donde se almacenará la información.
 * @param fileContent El contenido del archivo de texto a convertir.
 */
/*-----------------------------------------------------------------------------*/
void stringToJSON_lastState_v2(DynamicJsonDocument& doc, const String& fileContent)
{
    String MAC = "08:D1:F9:CB:A1:EC";

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

    // Necesitamos saber la línea de fin de la última comida completa que se pudo incluir en el JSON
    // para saber desde dónde comenzar el siguiente
    int lineIndex = 0;           // Indice de la linea actual
    int lastComidaIndex = -1;    // Indice de fin de la última comida completa (FIN-COMIDA). Empieza en -1 para que 'start' comience en 0

    size_t lastMemoryUsage;
    bool nextJSON;

    while (end != -1) 
    {
        
        // Borra el contenido del JSON para comenzar a llenarlo de nuevo
        doc.clear();
        lastMemoryUsage = doc.memoryUsage(); // 0

        nextJSON = false;

        while (!nextJSON) // Se deja de analizar lineas para el mismo JSON cuando haga falta otro 
        {
            String line = fileContent.substring(start, end);
            line.trim();
            lineIndex++;
            
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
                // Creo que el RTC está dando la hora GMT +0h
                time_t timestamp = convertTimeToUnix(line, firstCommaIndex, secondCommaIndex);
                comida["fecha"] = (int)timestamp;
                // ---

                size_t currentMemoryUsage = doc.memoryUsage();
                // Si el uso de memoria ha cambiado (se ha añadido algo al JSON), guarda una copia del estado del JSON
                if (currentMemoryUsage != lastMemoryUsage) 
                {
                    Serial.println(F("\n\n::::::::::::::::::::::\nGenerando copia\n::::::::::::::::::::::"));
                    // Guarda una copia del estado del JSON para restaurarla si se pasa de tamaño
                    lastSuccessfulState = "";
                    serializeJson(doc, lastSuccessfulState);
                    Serial.println(F("\n\n********************\nContenido del JSON de copia:\n********************"));
                    serializeJsonPretty(doc, Serial);
                    Serial.println(F("\n\n********************\nFin del contenido del JSON de copia\n********************"));
                    
                    lastComidaIndex = lineIndex; // Actualiza el índice de la línea de fin de la última comida completa
                    Serial.print(F("Indice de fin de última comida: ")); Serial.println(lastComidaIndex);
                }
            }


            size_t currentMemoryUsage = doc.memoryUsage();
            // Si el uso de memoria no ha cambiado (el JSON ya no admite más datos), restaura el JSON al estado de la última comida completa
            if (currentMemoryUsage == lastMemoryUsage) 
            {
                Serial.println(F("\n\n********************\nMemoria casi llena, no se pueden agregar más líneas\n********************"));
                // Restaura el JSON al estado de la última comida completa
                doc.clear(); 
                //deserializeJson(doc, lastSuccessfulState); // Restauramos último estado exitoso del JSON (solo comidas completas)
                DeserializationError error = deserializeJson(doc, lastSuccessfulState);
                if (error) {
                Serial.print(F("deserializeJson() failed with code "));
                Serial.println(error.c_str());
                }
                currentMemoryUsage = doc.memoryUsage();
                
                // Imprime el contenido del JSON en el Serial Monitor
                Serial.println(F("\n\n********************\nContenido del JSON:\n********************"));
                serializeJsonPretty(doc, Serial);
                Serial.println(F("\n\n********************\nFin del contenido del JSON\n********************"));

                nextJSON = true; // Se necesita otro JSON
                
                //return;
            }

            lastMemoryUsage = currentMemoryUsage;

            start = end + 1;
            end = fileContent.indexOf('\n', start);
            //start = (lastComidaIndex + 1); // Comienza desde la línea posterior al fin de la última comida completa 
            //end = fileContent.indexOf('\n', start);

            // Si end es -1, estamos en la última línea
            if (end == -1) {
                lastComidaIndex = -100; // Se pone a -100 para indicar que se ha terminado, no quedarían comidas por enviar
                Serial.print(F("Indice de fin de última comida: ")); Serial.println(lastComidaIndex);
                //return; // Salir de la función porque ya no hay más info que enviar
            }

        } 
        

    } 

}





#endif