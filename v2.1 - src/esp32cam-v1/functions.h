/**
 * @file json_functions.h
 * @brief Este archivo contiene funciones auxiliares para convertir el mensaje a JSON.
 * 
 * @author Irene Casares Rodríguez
 * @date 13/01/2024
 */

#ifndef JSON_FUNCTIONS_H
#define JSON_FUNCTIONS_H

#include "debug.h" // SM_DEBUG --> SerialPC

#define SerialESP32Due Serial1

#include <ArduinoJson.h>
#include <TimeLib.h>
#include "wifi_functions.h" // Para la MAC

#define JSON_SIZE_LIMIT 4096



/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    processJSON();                                                                              // Crear el documento JSON en un ámbito cerado para liberar su memoria al terminar 

void    addLineToJSON(DynamicJsonDocument& JSONdoc,                                                 // Todo en un JSON, aunque se quede a mitad, y se envía
                        JsonArray& comidas, JsonArray& platos, JsonArray& alimentos, 
                        JsonObject& comida, JsonObject& plato, 
                        String& line);                                              

time_t convertTimeToUnix(String &line, int &firstCommaIndex, int &secondCommaIndex); // Convertir fecha de String a formato Unix timestamp
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
    // Se reservan 4KB de RAM para el JSON, aunque acabe siendo más pequeño.
    //
    // La única forma de liberar esa memoria (sin reiniciar el dispositivo) es declarando 
    // el documento JSON dentro de una función para que al terminar la función, al salir 
    // del ámbito donde se declaró el 'doc', se libere su memoria.
    //
    // Si se declarara como global, solo se podría liberar su memoria reiniciando el dispositivo.
    DynamicJsonDocument JSONdoc(JSON_SIZE_LIMIT); 

    // Elementos del documento JSON
    JsonArray comidas = JSONdoc.createNestedArray("comidas");
    JsonObject comida;
    JsonArray platos;
    JsonObject plato;
    JsonArray alimentos;
    // ------------------------------------------------------------


    // ------------- GENERAR JSON ---------------------------------
    #if defined(SM_DEBUG)
    SerialPC.println("Añadiendo lineas al JSON...");
    #endif
    String line = "";
    while (line != "FIN-TRANSMISION") { // Mientras el Due no indique que ya leyó todo el fichero TXT
        if (SerialESP32Due.available() > 0) { // Comprobar si hay algún mensaje en el Serial
            line = SerialESP32Due.readStringUntil('\n'); 
            line.trim();
            #if defined(SM_DEBUG)
            SerialPC.println("Linea recibida: " + line); 
            #endif
            // Enviar JSON
            addLineToJSON(JSONdoc, comidas, platos, alimentos, comida, plato, line); // Aquí se procesa según la línea recibida
        }
    }
    // ------------------------------------------------------------

    // -------- MEMORIA RAM USADA POR EL JSON ---------------------
    #if defined(SM_DEBUG)
    SerialPC.print(F("\n\nMemoria RAM usada: ")); SerialPC.println(JSONdoc.memoryUsage());
    #endif
    // ------------------------------------------------------------

}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Añade la línea al JSON según su contenido. Un solo JSON para toda la info,
 *        aunque una comida se quede a medias.
 */
/*-----------------------------------------------------------------------------*/
void addLineToJSON(DynamicJsonDocument& JSONdoc, 
                    JsonArray& comidas, JsonArray& platos, JsonArray& alimentos, 
                    JsonObject& comida, JsonObject& plato, 
                    String& line)
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
        int firstCommaIndex = line.indexOf(',');
        int secondCommaIndex = line.lastIndexOf(',');
        alimento["grupo"] = line.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
        alimento["peso"] = line.substring(secondCommaIndex + 1).toFloat();
    } 
    else if (line.startsWith("FIN-COMIDA")) 
    {
        int firstCommaIndex = line.indexOf(',');
        int secondCommaIndex = line.lastIndexOf(',');
        time_t timestamp = convertTimeToUnix(line, firstCommaIndex, secondCommaIndex);
        comida["fecha"] = (int)timestamp;
    }
    else if (line == "FIN-TRANSMISION") // El Due ha terminado de enviar el fichero
    {
        // Agregar la dirección MAC del ESP32 al objeto JSON
        String macAddress = WiFi.macAddress();
        JSONdoc["mac"] = macAddress;

        // --- Mostrar JSON ---
        #if defined(SM_DEBUG)
        SerialPC.println(F("\n\n********************\nContenido del JSON:\n********************"));
        serializeJsonPretty(JSONdoc, SerialPC);
        SerialPC.println(F("\n\n********************\nFin del contenido del JSON\n********************"));
        #endif
        // --------------------

        // Avisar de que ya se tiene el JSON completo
        SerialESP32Due.println("JSON-OK");

        #if defined(SM_DEBUG)
        SerialPC.println("JSON completo");
        #endif

        // Enviar JSON a database:
        //  1. Pedir token
        //  2. Enviar JSON
        //  3. Cerrar sesión con token
        sendJsonToDatabase_fullProcess(JSONdoc);
        
    }
    else
    {
        //SerialESP32Due.println("Línea desconocida");
        #if defined(SM_DEBUG)
        SerialPC.println("Línea desconocida");
        #endif
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