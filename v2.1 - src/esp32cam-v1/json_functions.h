/**
 * @file json_functions.h
 * @brief Este archivo contiene funciones auxiliares para convertir el mensaje a JSON.
 * 
 * @author Irene Casares Rodríguez
 * @date 07/05/2024
 */

#ifndef JSON_FUNCTIONS_H
#define JSON_FUNCTIONS_H

#include "debug.h" // SM_DEBUG --> SerialPC

#include <ArduinoJson.h>    // Trabajar con JSON
#include <TimeLib.h>        // Convertir la fecha a UNIX
#include "wifi_functions.h" // Obtener la MAC, pedir token, enviar JSON y cerrar sesión

#define JSON_SIZE_LIMIT 20480 // 20k



/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-----------------------------------------------------------------------------*/
void    processJSON_OnePerMeal();                                                                            // Crear el documento JSON en un ámbito cerado para liberar su memoria al terminar 

void    addLineToJSON_oneJsonPerMeal(DynamicJsonDocument& JSONdoc,                                  
                                    JsonArray& comidas, JsonArray& platos, JsonArray& alimentos, 
                                    JsonObject& comida, JsonObject& plato, 
                                    String& line, String &bearerToken);                                             

time_t convertTimeToUnix(String &line, int &firstCommaIndex, int &secondCommaIndex); // Convertir fecha de String a formato Unix timestamp
/*-----------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Crea el documento JSON en un ámbito cerrado para poder liberar la memoria 
 *        RAM que ocupe al salir del mismo (al salir de la función)
 *        En lugar de pedir token cuando vaya a subir la data, lo pide antes de procesar todas las líneas.
 *        Así solo se pide una vez, luego se suben tantos JSON como haga falta y por último se cierra sesión.
 *        De esta forma, no hay que pedir token, subir data y cerrar sesión con cada comida.
 */
/*-----------------------------------------------------------------------------*/
void  processJSON_OnePerMeal()
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    //limpiarBufferDue();
    // ---------------------------------------------------------

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


    // ----- PEDIR TOKEN PARA USAR EN TODAS LAS SUBIDAS -----------
    String bearerToken; // Token de autenticación pedido al servidor para poder subir información
    
    // Si falla la obtención de token, indica el error HTTP y no intenta subir la data

    if(fetchTokenFromServer(bearerToken)) // 1. Pedir token de autenticación. True si se ha obtenido token
    {
        // ------- ESPERAR DATA ----------------------
        // El ESP32 se ha autenticado en la database y está listo para subir datos
        #if defined(SM_DEBUG)
            SerialPC.println(F("Esperando data..."));
        #endif
        sendMsgToDue("WAITING-FOR-DATA");
        // -------------------------------------------

        // ------------- GENERAR JSON ---------------------------------
        #if defined(SM_DEBUG)
            SerialPC.println("Añadiendo lineas al JSON...");
        #endif

        String line = "";
        unsigned long timeout_waitLine = 15000; // Tiempo de espera máximo de 15 segundos para que el Due envíe la línea

        while (line != "FIN-TRANSMISION")  // Mientras el Due no indique que ya leyó todo el fichero TXT
        //while(!line.equals("FIN-TRANSMISION"))
        {
            // Se sale del while después de procesar FIN-TRANSMISION o si no se recibe nada durante 15 segundos

            // ---- ESPERAR RESPUESTA DEL DUE ---------------
            // Esperar hasta 15 segundos a que el Due envíe la línea
            waitMsgFromDue(line, timeout_waitLine); // Espera mensaje del Due y lo devuelve en 'line'
            // Cuando se recibe mensaje o se pasa el timeout, entonces se comprueba la respuesta
            
            // Se comprueba si no hay nada en el Serial y si han pasado más de 'timeout' segundos
            // sin nada en el Serial. Cuando se recibe algo, se reinician los 15 segundos
            // Se hace esta comprobación por si ha habido algún error en la lectura del fichero en el Due o
            // si se ha perdido la comunicación ESP32-Due, para que el ESP32 no se quede esperando indefinidamente.
            // ------------------------------------------------

            // ---- ANALIZAR MENSAJE DEL DUE ----------------
            // --- Fallo en la comunicación ---
            // Si no se recibe nada durante 15 segundos, se cierra sesión
            if(line == "TIMEOUT-DUE")
            { 
                #if defined(SM_DEBUG)
                    SerialPC.println(F("TIMEOUT-DUE. No se ha recibido respuesta del Due"));
                    SerialPC.println(F("Cerrando sesión..."));
                #endif

                logoutFromServer(bearerToken); // Cerrar sesión sin haber subido nada

                break;
            }
            // --------------------------------
            // --- Línea recibida -------------
            else 
            {
                // Reiniciar el tiempo de espera hasta la próxima recepción de línea
                //startTime = millis();

                // La línea se ha leído en waitMsgFromDue() y se ha guardado en 'line'
                #if defined(SM_DEBUG)
                    SerialPC.println("\nLinea recibida en processJSON: " + line); 
                #endif

                // ------ AÑADIR A JSON --------
                // Comprobar linea y conformar JSON. Enviar un JSON por comida
                //  Si es FIN-COMIDA, se sube info (uploadJSONtoServer()) con el token.
                //  Si es FIN-TRANSMISION, se cierra sesión (logoutFromServer()) con el token.
                addLineToJSON_oneJsonPerMeal(JSONdoc, comidas, platos, alimentos, comida, plato, line, bearerToken);           
                // -----------------------------
            }
            // --------------------------------------------------
        }
        // ------------------------------------------------------------
    }
    else
    {
        // ------- FALLO AUTENTICACIÓN ---------------
        // Ha fallado la autenticación del ESP32 en la database. No puede subir info 
        #if defined(SM_DEBUG)
            SerialPC.println(F("Error al pedir token"));
        #endif

        // En fetchTokenFromServer() se muestra el error (NO-WIFI, HTTP-ERROR, etc.)
        // -------------------------------------------
    }
    // ------------------------------------------------------------

}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Crea un JSON para cada comida, añade su información y lo envía.
 *        De esta forma divide la información en varios paquetes, para no
 *        tener que crear un JSON extremadamente grande para guardar todo
 *        el fichero txt.
 * 
 *        ¿PODRÍA SATURAR EL SERVIDOR?
 */
/*-----------------------------------------------------------------------------*/
void addLineToJSON_oneJsonPerMeal(DynamicJsonDocument& JSONdoc, 
                                JsonArray& comidas, JsonArray& platos, JsonArray& alimentos, 
                                JsonObject& comida, JsonObject& plato, 
                                String& line, String &bearerToken)
{
    if (line == "INICIO-COMIDA") 
    {
        #if defined(SM_DEBUG)
            SerialPC.println("\n---------------------\nCOMENZANDO NUEVA COMIDA...\n---------------------\n");
        #endif

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
    else if (line.startsWith("ALIMENTO")) // "ALIMENTO,grupo,peso" o "ALIMENTO,grupo,peso,ean" si es barcode
    {
        JsonObject alimento = alimentos.createNestedObject();

        int firstCommaIndex = line.indexOf(',');
        int secondCommaIndex = line.indexOf(',', firstCommaIndex + 1);

        // Completar JSON según el tipo de alimento indicado por el ID: grupo o barcode (ID 50)
        alimento["grupo"] = line.substring(firstCommaIndex + 1, secondCommaIndex).toInt();
      
        // --- ALIMENTO DE GRUPOS PREESTABLECIDOS ---
        if (alimento["grupo"] != 50) // "ALIMENTO,grupo,peso"
        {
            alimento["peso"] = line.substring(secondCommaIndex + 1).toFloat();
        }
        // ------------------------------------------
        // --- ALIMENTO DE BARCODE ------------------
        else // "ALIMENTO,grupo,peso,ean"
        {
            int thirdCommaIndex = line.indexOf(',', secondCommaIndex + 1);
            alimento["peso"] = line.substring(secondCommaIndex + 1, thirdCommaIndex).toFloat();
            alimento["ean"] = line.substring(thirdCommaIndex + 1);
        } 
        // ------------------------------------------
    } 
    else if (line.startsWith("FIN-COMIDA")) // "FIN-COMIDA,fecha,hora"
    {
        // Finalizar el JSON actual y enviarlo a la base de datos.

        int firstCommaIndex = line.indexOf(',');
        int secondCommaIndex = line.lastIndexOf(',');
        time_t timestamp = convertTimeToUnix(line, firstCommaIndex, secondCommaIndex); // Convertir "fecha,hora" a timestamp
        comida["fecha"] = (int)timestamp;

        // Agregar la dirección MAC del ESP32 al objeto JSON
        String macAddress = WiFi.macAddress();
        JSONdoc["mac"] = macAddress;

        #if defined(SM_DEBUG)
            // ---- MOSTRAR JSON -----
            SerialPC.println();
            serializeJsonPretty(JSONdoc, SerialPC);
            SerialPC.println();
            // -----------------------

            // -------- MEMORIA RAM USADA POR EL JSON ---------------------
            SerialPC.print(F("\n\nMemoria RAM usada: ")); SerialPC.println(JSONdoc.memoryUsage()); SerialPC.println("\n");
            // ------------------------------------------------------------
        #endif

        // ----- ENVIAR JSON AL SERVIDOR ----------------
        // Se envía solo la info de la comida actual. Si hay otro INICIO-COMIDA, se
        // enviará otro JSON y así hasta recibir FIN-TRANSMISION
        uploadJSONtoServer(JSONdoc,bearerToken);    // 2. Enviar JSON al servidor
        // Si se devuelve SAVED-OK, da igual que falle el logout
        // ----------------------------------------------

    }
    else if (line == "FIN-TRANSMISION") // El Due ha terminado de enviar el fichero
    {
        logoutFromServer(bearerToken);              // 3. Cerrar sesión

        #if defined(SM_DEBUG)
            SerialPC.println("Transmisión completa\n");
        #endif
    }
    else // No debería entrar aquí, pero por cubrir todas las opciones
    {
        #if defined(SM_DEBUG)
            SerialPC.println("Línea desconocida procesando JSON");
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