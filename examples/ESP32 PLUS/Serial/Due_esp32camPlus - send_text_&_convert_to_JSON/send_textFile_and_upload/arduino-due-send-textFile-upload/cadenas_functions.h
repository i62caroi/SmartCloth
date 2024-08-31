/*
    Irene Casares Rodríguez
    12/01/2024

    Funciones para enviar un String como simulación del esp32File del Due al ESP32 
    por comunicación Serial.
 */

#ifndef CADENAS_FUNCTIONS_H
#define CADENAS_FUNCTIONS_H

#include "Serial_functions.h"

// Simular la lectura del archivo
String string1 = "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,5,53.50\n"
                "ALIMENTO,7,23.50\n"
                "ALIMENTO,50,120.5,8424621001093\n"  // Línea con barcode (Mini Tostas de Trigo)
                "FIN-COMIDA,03.07.2024,08:55:36\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,20,24.40\n"
                "FIN-COMIDA,03.07.2024,18:25:25\n";
                
                

String string2 = "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "FIN-COMIDA,02.05.2024,17:55:36\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,24.40\n"
                "FIN-COMIDA,02.05.2024,18:25:25\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "INICIO-PLATO\n"
                "ALIMENTO,3,32.07\n"
                "ALIMENTO,27,46.65\n"
                "ALIMENTO,10,16.23\n"
                "FIN-COMIDA,02.05.2024,10:39:36\n";



#include <vector>
//#include <string>

#define SerialPC Serial
#define SerialDueESP32 Serial1


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    sendStringSimulationToEsp32(String fileContent);                                        // Enviar String al esp32 simulando el fichero TXT
void    sendStringSimulationToEsp32MealByMeal(String fileContent);                              // Enviar String al esp32 simulando el fichero TXT

void    saveMealForLaterCadenas(std::vector<String> &actualMeal, std::vector<String> &unsavedMeals);   // Guardar 'actualMeal' no subida en vector 'unsavedMeals' para intentar subirla más tarde
/*-----------------------------------------------------------------------------*/
                




/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía el fichero (String de simulación) por Serial al esp32
 */
/*-----------------------------------------------------------------------------*/
void sendStringSimulationToEsp32(String fileContent)
{
    int start = 0;
    int end = fileContent.indexOf('\n');

    while (end != -1) 
    {
        String line = fileContent.substring(start, end);
        line.trim();

        // Envía la línea al ESP32 a través de Serial
        SerialDueESP32.println(line);

        start = end + 1;
        end = fileContent.indexOf('\n', start);
    }

    // Envía la última línea si existe
    if (start < fileContent.length()) {
        String line = fileContent.substring(start);
        line.trim();
        SerialDueESP32.println(line);
    }

    SerialDueESP32.println(F("FIN-TRANSMISION"));
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía el fichero (String de simulación) al esp32 comida a comida
 *          y va actualizando la cadena de comidas que se han podido subir
 */
/*-----------------------------------------------------------------------------*/
void sendStringSimulationToEsp32MealByMeal(String fileContent)
{
    std::vector<String> actualMeal;   // Vector que guarda los datos de la comida actual
    std::vector<String> unsavedMeals; // Vector que guarda las comidas que no se han podido subir

    int start = 0;
    int end = fileContent.indexOf('\n');

    while (end != -1) 
    {
        // ----- LEER LÍNEA DEL TXT -------------
        // Leer línea del String de simulación (tendría que ser con fichero TXT)
        String line = fileContent.substring(start, end);
        line.trim();
        // -------------------------------------

        // ----- AÑADIR A actualMeal -----------
        // Añade la línea al vector de la comida actual 
        // (si la línea es INICIO-COMIDA, el vector debería estar vacío antes de insertarla)
        actualMeal.push_back(line); 
        // -------------------------------------

        // ----- ENVIAR A ESP32 ----------------
        // Envía la línea al ESP32 a través de Serial
        SerialDueESP32.println(line);
        // -------------------------------------

        // ----- INDICAR SUBIDA A DATABASE ----------------
        // Si se ha llegado al final de la comida, se espera si se ha subido correctamente
        if (line.startsWith("FIN-COMIDA")) 
        {
            // ---- ESPERAR RESPUESTA DEL ESP32 ---------------
            // Esperar hasta 10 segundos a que el ESP32 responda
            String responseFromESP32;
            unsigned long timeout_waitUpload = 10000; // Tiempo de espera máximo de 10 segundos para que el esp32 responda tras subir el JSON
            waitResponseFromESP32(responseFromESP32, timeout_waitUpload); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
            // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
            // ------------------------------------------------

            // ---- ANALIZAR RESPUESTA DEL ESP32 --------------
            // ----- EXITO: COMIDA SUBIDA ----------
            if(responseFromESP32 == "SAVED-OK"){
                // Si se recibió SAVED-OK, no se añade a unsavedMeals 
                SerialPC.println("Comida guardada correctamente\n\n");
            }
            // -------------------------------------
            // ----- ERROR: COMIDA NO SUBIDA -------
            else
            { 
                if (responseFromESP32 == "NO-WIFI" || responseFromESP32.startsWith("HTTP-ERROR")) 
                    SerialPC.println("Sin WiFi o error en la petición HTTP\n");
                else if(responseFromESP32 == "TIMEOUT")
                    SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
                else 
                    SerialPC.println("Respuesta desconocida\n");

                // -- AÑADIR A unsavedMeals --
                // Si no se ha subido la comida por error o timeout, se añade la comida actual al vector de comidas no subidas
                saveMealForLaterCadenas(actualMeal, unsavedMeals);
                // ---------------------------
            }
            // -------------------------------------
            // ------------------------------------------------

            // ---- REINICIAR actualMeal ----------------------
            // Reseta el vector de la comida actual
            actualMeal.clear();
            // ------------------------------------------------

        }
        // ------------------------------------------------

        start = end + 1;
        end = fileContent.indexOf('\n', start);
    }

    // Tras enviar todas las comidas, se envía un mensaje de fin de transmisión
    SerialDueESP32.println(F("FIN-TRANSMISION"));

    if (unsavedMeals.empty())  // Si se ha subido todo, se borraría el fichero TXT
        SerialPC.println("Info completa guardada"); 
    // Si no se ha subido todo, se actualiza el fichero TXT (borrar y crear nuevo) con las comidas no subidas
    else 
        SerialPC.println("No se ha podido subir todo");
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda la comida 'actualMeal' en el vector 'unsavedMeals' para intentar subirla más tarde
 */
/*-----------------------------------------------------------------------------*/
void saveMealForLaterCadenas(std::vector<String> &actualMeal, std::vector<String> &unsavedMeals)
{
    // Recorre el vector actualMeal y añade cada línea al vector unsavedMeals
    for (const auto& meal : actualMeal) unsavedMeals.push_back(meal); 

    // Esta línea se supone que hace lo mismo, pero creo que no funcionaba
    //unsavedMeals.insert(unsavedMeals.end(), actualMeal.begin(), actualMeal.end()); 
}


#endif