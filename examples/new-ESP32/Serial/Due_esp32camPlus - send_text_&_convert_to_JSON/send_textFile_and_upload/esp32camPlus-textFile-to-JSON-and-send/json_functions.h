/**
 * @file json_functions.h
 * @brief Este archivo contiene funciones auxiliares para convertir el mensaje a JSON.
 * 
 * @author Irene Casares Rodríguez
 * @date 07/05/2024
 */

/*
    El esp32 va conformando un JSON con la información que recibe del Due por Serial. Cuando recibe FIN-COMIDA,
    cierra el JSON con la MAC del esp32 y lo envía a la base de datos. Repite el proceso con todas las comidas
    que recibe hasta que reciba la línea FIN-TRANSMISION, entonces cierra sesión.
*/


#ifndef JSON_FUNCTIONS_H
#define JSON_FUNCTIONS_H

#define SerialPC Serial
#define SerialESP32Due Serial1

#include <ArduinoJson.h>    // Trabajar con JSON
#include <TimeLib.h>        // Convertir la fecha a UNIX
#include "wifi_functions.h" // Obtener la MAC, pedir token, enviar JSON y cerrar sesión


#define JSON_SIZE_LIMIT 20480 // 20k; 4k --> 4096


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    processJSON();                                                                              // Crear el documento JSON en un ámbito cerado para liberar su memoria al terminar 
void    processJSON_OnePerMeal();

void    addLineToJSON(DynamicJsonDocument& JSONdoc,                                                 // Todo en un JSON, aunque se quede a mitad, y se envía
                        JsonArray& comidas, JsonArray& platos, JsonArray& alimentos, 
                        JsonObject& comida, JsonObject& plato, 
                        String& line);                                              

/*void    addLineToJSON_oneJsonPerMeal(DynamicJsonDocument& JSONdoc,                                  // Genera y envía un JSON por comida. PUEDE SATURAR EL SERVIDOR!!!
                                    JsonArray& comidas, JsonArray& platos, JsonArray& alimentos, 
                                    JsonObject& comida, JsonObject& plato, 
                                    String& line);*/

void    addLineToJSON_oneJsonPerMeal(DynamicJsonDocument& JSONdoc,                                  
                                    JsonArray& comidas, JsonArray& platos, JsonArray& alimentos, 
                                    JsonObject& comida, JsonObject& plato, 
                                    String& line, String &bearerToken);

void    addLineToJSON_print(DynamicJsonDocument& JSONdoc,                                           // Todo en un JSON, aunque se quede a mitad, y solo se imprime
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
/*void  processJSON()
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
    DynamicJsonDocument JSONdoc(JSON_SIZE_LIMIT); 

    // Elementos del documento JSON
    JsonArray comidas = JSONdoc.createNestedArray("comidas");
    JsonObject comida;
    JsonArray platos;
    JsonObject plato;
    JsonArray alimentos;
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
    SerialPC.println("Añadiendo lineas al JSON...");
    String line = "";

    //unsigned long lastReceivedTime = millis(); // Guarda el tiempo actual
    //unsigned long timeout = 3000; // Espera máxima de 3 segundos
    
    // Mientras el Due no indique que ya leyó todo el fichero TXT
    // Si pasan 3 segundos sin recibir mensaje, sale del bucle. Por si se corta la comunicación
    // Serial con Arduino. Si se corta, addLineToJSON() no recibirá "FIN-TRANSMISION" y no subirá el JSON.
    //while ((line != "FIN-TRANSMISION") && (millis() - lastReceivedTime <= timeout)) { 
    while (line != "FIN-TRANSMISION") { // Mientras el Due no indique que ya leyó todo el fichero TXT
        if (SerialESP32Due.available() > 0) { // Comprobar si hay algún mensaje en el Serial
            line = SerialESP32Due.readStringUntil('\n'); 
            line.trim();
            SerialPC.println("Linea recibida: " + line); 

            //lastReceivedTime = millis(); // Actualiza el tiempo de la última recepción
            
            // Si solo se quiere imprimir:
            //addLineToJSON_print(JSONdoc, comidas, platos, alimentos, comida, plato, line); // Aquí se procesa según la línea recibida
            
            // Enviar un JSON con todo:
            //addLineToJSON(JSONdoc, comidas, platos, alimentos, comida, plato, line); // Aquí se procesa según la línea recibida
            // La línea "FIN-TRANSMISION" también la procesa esta función, cerrando el JSON y enviándolo
        
            // Enviar un JSON por comida
            addLineToJSON_oneJsonPerMeal(JSONdoc, comidas, platos, alimentos, comida, plato, line);             
        }
    }
    //}
    // ------------------------------------------------------------

}*/


/*-----------------------------------------------------------------------------*/
/**
 * @brief Crea el documento JSON en un ámbito cerrado para poder liberar la memoria 
 *        RAM que ocupe al salir del mismo (al salir de la función)
 *        En lugar de pedir token cuando vaya a subir la data, lo pide antes de procesar todas las líneas.
 *        Así solo se pide una vez, luego se suben tantos JSON como haga falta y luego se cierra sesión.
 *        De esta forma, no hay que pedir token, subir data y cerrar sesión con cada comida.
 */
/*-----------------------------------------------------------------------------*/
void  processJSON_OnePerMeal()
{

    // ---------- DECLARAR DOCUMENTO JSON -------------------------
    // Se reservan 20KB de RAM para el JSON, aunque acabe siendo más pequeño.
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

    // 1. Pedir token para subir datos
    // 2. Subir todos los JSON que haga falta, uno por comida
    // 3. Cerrar sesión

    // Antes pedíamos token, subíamos data y cerrábamos sesión, todo con cada comida.
    // Creo que es mejor pedir token una vez, subir toda la data que queramos y luego cerrar
    // sesión una vez.


    // ----- PEDIR TOKEN PARA USAR EN TODAS LAS SUBIDAS -----------
    // Token de autenticación pedido al servidor para poder subir información
    String bearerToken;

    fetchTokenFromServer(bearerToken);          // 1. Pedir token de autenticación
    // Si falla la obtención de token, indica el error HTTP y no intenta subir la data

    if(bearerToken != ""){ // Se ha obtenido token
        // ------------- GENERAR JSON ---------------------------------
        SerialPC.println("Añadiendo lineas al JSON...");
        String line = "";

        while (line != "FIN-TRANSMISION") { // Mientras el Due no indique que ya leyó todo el fichero TXT
            
            // Se sale del while después de procesar FIN-TRANSMISION

            if (SerialESP32Due.available() > 0) { // Comprobar si hay algún mensaje del Due
                line = SerialESP32Due.readStringUntil('\n'); 
                line.trim();
                SerialPC.println("Linea recibida: " + line); 

                // Enviar un JSON por comida
                addLineToJSON_oneJsonPerMeal(JSONdoc, comidas, platos, alimentos, comida, plato, line, bearerToken);           
                //  Si es FIN-COMIDA, se sube info (uploadJSONtoServer()) con el token.
                //  Si es FIN-TRANSMISION, se cierra sesión (logoutFromServer()) con el token.
            }
        }
        // ------------------------------------------------------------
    }
    else{
        SerialPC.println("Error al pedir token");
    }
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
        String macAddress = WiFi.macAddress();
        JSONdoc["mac"] = macAddress;

        // Avisar al Due de que ya se tiene el JSON completo
        SerialESP32Due.println("JSON-OK");

        // Enviar JSON a database
        //sendJsonToDatabase(JSONdoc);

        // Enviar JSON a database:
        //  1. Pedir token
        //  2. Enviar JSON
        //  3. Cerrar sesión
        sendJsonToDatabase_fullProcess(JSONdoc);
        
    }
    else
    {
        SerialESP32Due.println("Línea desconocida");
    }
}








/*-----------------------------------------------------------------------------*/
/**
 * @brief Crea un JSON para cada comida, añade su información y lo envía.
 *        De esta forma divide la información en varios paquetes, para no
 *        tener que crear un JSON extremadamente grande para guardar todo
 *        el fichero txt.
 * 
 *        Esta opción hace los 3 pasos (pide token, sube comida y cierra sesión)
 *        para cada comida. Es innecesario, por eso hay otra opción que pide token
 *        una vez, sube todas las comidas y luego cierra sesión. 
 */
/*-----------------------------------------------------------------------------*/
/*void addLineToJSON_oneJsonPerMeal(DynamicJsonDocument& JSONdoc, 
                                JsonArray& comidas, JsonArray& platos, JsonArray& alimentos, 
                                JsonObject& comida, JsonObject& plato, 
                                String& line)
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

        // Agregar la dirección MAC del ESP32 al objeto JSON
        String macAddress = WiFi.macAddress();
        JSONdoc["mac"] = macAddress;

        // ---- MOSTRAR JSON -----
        SerialPC.println();
        serializeJsonPretty(JSONdoc, SerialPC);
        SerialPC.println();
        // -----------------------

        // -------- MEMORIA RAM USADA POR EL JSON ---------------------
        SerialPC.print(F("\n\nMemoria RAM usada: ")); SerialPC.println(JSONdoc.memoryUsage());
        // ------------------------------------------------------------

        // Finalizar el DynamicJsonDocument actual y enviarlo a la base de datos
        // Se envía solo la info de la comida actual. Si hay otro INICIO-COMIDA, se
        // enviará otro JSON y así hasta recibir FIN-TRANSMISION

        // Enviar JSON a database:
        //  1. Pedir token
        //  2. Enviar JSON
        //  3. Cerrar sesión
        sendJsonToDatabase_fullProcess(JSONdoc);

    }
    else if (line == "FIN-TRANSMISION") // El Due ha terminado de enviar el fichero
    {
        SerialPC.println("Transmisión completa");
    }
    else
    {
        SerialPC.println("Línea desconocida");
    }
}*/



/*-----------------------------------------------------------------------------*/
/**
 * @brief Crea un JSON para cada comida, añade su información y lo envía.
 *        De esta forma divide la información en varios paquetes, para no
 *        tener que crear un JSON extremadamente grande para guardar todo
 *        el fichero txt.
 * 
 *        ¿¿¿¿¿PUEDE SATURAR EL SERVIDOR?????
 */
/*-----------------------------------------------------------------------------*/
void addLineToJSON_oneJsonPerMeal(DynamicJsonDocument& JSONdoc, 
                                JsonArray& comidas, JsonArray& platos, JsonArray& alimentos, 
                                JsonObject& comida, JsonObject& plato, 
                                String& line, String &bearerToken)
{

    if (line == "INICIO-COMIDA") 
    {
        SerialPC.println("\n---------------------\nCOMENZANDO NUEVA COMIDA...\n---------------------\n");
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

        // Agregar la dirección MAC del ESP32 al objeto JSON
        String macAddress = WiFi.macAddress();
        JSONdoc["mac"] = macAddress;

        // Finalizar el DynamicJsonDocument actual y enviarlo a la base de datos
        // Se envía solo la info de la comida actual. Si hay otro INICIO-COMIDA, se
        // enviará otro JSON y así hasta recibir FIN-TRANSMISION

        // Enviar JSON a database:
        //  1. Pedir token
        //  2. Enviar JSON
        //  3. Cerrar sesión
        // No se hacen los 3 pasos cada vez, sino que antes de procesar comidas pedimos token,
        // luego para cada comida se envía el JSON y al terminar (FIN-TRANSMISION) se cierra
        // la sesión.

        uploadJSONtoServer(JSONdoc,bearerToken);    // 2. Enviar JSON
        // Si se devuelve SAVED-OK, da igual que falle el logout



        // ---- MOSTRAR JSON -----
        SerialPC.println();
        serializeJsonPretty(JSONdoc, SerialPC);
        SerialPC.println();
        // -----------------------

        // -------- MEMORIA RAM USADA POR EL JSON ---------------------
        SerialPC.print(F("\n\nMemoria RAM usada: ")); SerialPC.println(JSONdoc.memoryUsage()); SerialPC.println("\n");
        // ------------------------------------------------------------

    }
    else if (line == "FIN-TRANSMISION") // El Due ha terminado de enviar el fichero
    {
        logoutFromServer(bearerToken);              // 3. Cerrar sesión

        SerialPC.println("Transmisión completa");
    }
    else
    {
        SerialPC.println("Línea desconocida");
    }
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Añade la línea al JSON según su contenido. Un solo JSON para toda la info.
 *        Esta función solo imprime el JSON por terminal, no lo envía.
 */
/*-----------------------------------------------------------------------------*/
void addLineToJSON_print(DynamicJsonDocument& JSONdoc, 
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
        String macAddress = WiFi.macAddress();
        JSONdoc["mac"] = macAddress;

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