/*
    Irene Casares Rodríguez
    12/01/2024

    Funciones para enviar un String como simulación del esp32File del Due al ESP32 
    por comunicación Serial.
 */

#ifndef CADENAS_FUNCTIONS_H
#define CADENAS_FUNCTIONS_H

// Simular la lectura del archivo
String string1 = "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "FIN-COMIDA,02.05.2024,17:55:36\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,24.40\n"
                "FIN-COMIDA,02.05.2024,18:25:25\n";
                
                

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
    unsigned long timeout = 10000; // Tiempo de espera máximo de 10 segundos

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

        // ----- ESPERAR SUBIDA A DATABASE ----------------
        // Si se ha llegado al final de la comida, se espera si se ha subido correctamente
        if (line.startsWith("FIN-COMIDA")) 
        {
            unsigned long startTime = millis();

            // Esperar 10 segundos a que el ESP32 responda
            while (SerialDueESP32.available() == 0 && millis() - startTime < timeout);

            // ---------------------------------------------
            // Cuando se recibe mensaje o se pasa el timout, 
            // entonces se comprueba la respuesta
            // ---------------------------------------------

            // ------- RESPUESTA DEL ESP32 --------------------
            if (SerialDueESP32.available() > 0)  // El ESP32 ha respondido
            {
                String response = SerialDueESP32.readStringUntil('\n');
                response.trim(); 

                // ----- COMIDA SUBIDA ----------
                if(response == "SAVED-OK")
                    // Si se recibió SAVED-OK, no se añade a unsavedMeals 
                    SerialPC.println("Comida guardada correctamente\n\n");
                // ------------------------------

                // ----- COMIDA NO SUBIDA -------
                else if (response == "NO-WIFI" || response.startsWith("HTTP-ERROR")) 
                {
                    SerialPC.println("Sin WiFi o error en la petición HTTP\n");
                    // -- AÑADIR A unsavedMeals --
                    // Si no hay conexión WiFi o ha habido un error en la petición HTTP, se añade la comida actual al vector de comidas no subidas
                    saveMealForLaterCadenas(actualMeal, unsavedMeals);
                    // ---------------------------
                }
                else 
                    SerialPC.println("Respuesta desconocida\n");
                // ------------------------------
            } 
            // ------------------------------------------------

            // ----- TIMEOUT DEL ESP32 ------------------------
            else {
                SerialPC.println("Timeout: No se ha recibido respuesta del ESP32\n");
                // -- COMIDA NO SUBIDA. AÑADIR A unsavedMeals --
                // Si no se recibe respuesta del ESP32 en 5 segundos, se asume que no se ha subido la comida y se guarda en 'unsavedMeals'
                saveMealForLaterCadenas(actualMeal, unsavedMeals);
                // ---------------------------------------------
            }
            // ------------------------------------------------


            // ---- REINCIAR actualMeal -----------------------
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