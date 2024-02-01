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
#include <TimeLib.h>

DynamicJsonDocument doc(1024);

#define JSON_SIZE_LIMIT 4096


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    processJSON_txtFile();                      // Crear el documento JSON en un ámbito cerado para liberar su memoria al terminar  
void    textFileToJSON(DynamicJsonDocument& doc);   // Generar JSON con todo lo que entre, aunque una comida se quede a mitad
void    saveJSON(DynamicJsonDocument& doc);         // Guardar JSON generado en un fichero .jsn en la SD

time_t convertTimeToUnix(String &line, int &firstCommaIndex, int &secondCommaIndex);    // Convertir las cadenas de fecha y hora a Unix timestamp para ahorrar espacio en JSON
/*-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
/**
 * @brief Crea el documento JSON en un ámbito cerrado para poder liberar la memoria 
 *        RAM que ocupe al salir del mismo (al salir de la función)
 */
/*-----------------------------------------------------------------------------*/
void  processJSON_txtFile()
{
    // ---------- DECLARAR DOCUMENTO JSON -------------------------
    // Se reservan 4KB de RAM para el JSON, aunque acabe siendo más pequeño.
    //
    // La única forma de liberar esa memoria (sin reiniciar el dispositivo) es declarando 
    // el documento JSON dentro de una función para que al terminar la función, al salir 
    // del ámbito donde se declaró el 'doc', se libere su memoria.
    //
    // Si se declarara como global, solo se podría liberar su memoria reiniciando el dispositivo.
    DynamicJsonDocument doc(JSON_SIZE_LIMIT); 
    // ------------------------------------------------------------


    // ------------- GENERAR JSON ---------------------------------
    textFileToJSON(doc); // Convertir fichero TXT en JSON
    // ------------------------------------------------------------

    // ------------- MOSTRAR JSON ---------------------------------
    Serial.println(F("\n\n********************\nContenido del JSON:\n********************"));
    serializeJsonPretty(doc, Serial);
    Serial.println(F("\n\n********************\nFin del contenido del JSON\n********************"));
    // ------------------------------------------------------------

    // -------- MEMORIA RAM USADA POR EL JSON ---------------------
    Serial.print(F("\n\nMemoria RAM usada: ")); Serial.println(doc.memoryUsage());
    // ------------------------------------------------------------

    // -------- GUARDAR JSON EN SD --------------------------------
    saveJSON(doc); //Guardar JSON en fichero .jsn 
    // ------------------------------------------------------------

}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte el contenido del fichero en un JSON con todo lo que entre,
 * aunque alguna comida se quede a medias.
 */
/*-----------------------------------------------------------------------------*/
void textFileToJSON(DynamicJsonDocument& doc)
{
    File dataFile = SD.open(fileESP32);

    if (dataFile) 
    {
        String MAC = "08:D1:F9:CB:A1:EC";

        JsonArray comidas = doc.createNestedArray("comidas");
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
        }

        dataFile.close();

        //serializeJson(doc, Serial);
        serializeJsonPretty(doc, Serial);

    } 
    else 
    {
        Serial.println(F("\nError al abrir el archivo data-ESP.txt"));
    }

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda el objeto JSON en un fichero .json
 */
/*-----------------------------------------------------------------------------*/
void saveJSON(DynamicJsonDocument& doc)
{
    File jsonFile = SD.open(fileJSON, FILE_WRITE);

    if (jsonFile) 
    {
        // Escribir el JSON en el archivo
        serializeJson(doc, jsonFile);

        // Cerrar el archivo
        jsonFile.close();

        Serial.println(F("\n\nJSON guardado en jsonFile.json"));
    } 
    else 
    {
        Serial.println(F("\n\nError al abrir el archivo data-ESP.json"));
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