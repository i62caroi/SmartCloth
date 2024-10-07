/**
 * @file SD_functions.h
 * @brief Este archivo contiene las definiciones de las funciones utilizadas para interactuar con la tarjeta SD.
 * 
 * @author Irene Casares Rodríguez
 * @date 01/12/2023
 */

/*
  --------- CONEXIÓN SD CON ARDUINO DUE -----------


  Tarjeta SD conectada al bus SPI de la siguiente forma:
  
    (pines de la SD en el JP2 de la TFT, conectados al módulo SD 
     integrado directamente en la pantalla)
     
      ------------------------------------------------------------
      |  SPI  |        SD (JP2 en TFT)   |      Arduino Due      |
      ------------------------------------------------------------
      |  MOSI |      pin 3  (amarillo)   |      pin 4 (SPI)      |    
      |  MISO |      pin 6  (azul)       |      pin 1 (SPI)      |
      |  CLK  |      pin 4  (verde)      |      pin 3 (SPI)      |
      |  CS   |      pin 2  (blanco)     |   pin 13 (digital PWM) | 
      ------------------------------------------------------------

              -----------------------
              |   SD (JP2 en TFT)   |
      -------------------------------
      |  GND  |      pin 5 (negro)  |
      -------------------------------

-----------------------------------------------------
*/

#ifndef SD_FUNCTIONS
#define SD_FUNCTIONS

// incluir esta línea en el includePath de c_cpp_properties.json 
//
// "/Applications/Arduino.app/Contents/Java/libraries/SD/src"
#include <SD.h>
#include <vector> // Para los vectores actualMal y unsavedMeals
#include "Serial_functions.h" // Funciones de interacción con el Serial Due-ESP32


#define SD_CARD_SCS  13 ///< Define el pin CS para la tarjeta SD


// Fichero donde ir escribiendo la info para cuando haya wifi
//char fileTXT[20] = "data/data-esp.txt";
char auxFileTXT[20] = "data/auxFile.txt";

char fileTXT[30] = "data/test.txt";



/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    setupSDcard();                                      // Inicializar tarjeta SD

void    sendFileToESP32();                                  // Enviar fichero TXT al esp32 para un solo JSON
void    sendFileToESP32MealByMeal();                        // Enviar fichero TXT al esp32 esperando subida de un JSON por comida

inline void     addLineToActualMeal(std::vector<String> &actualMeal, String &line){ actualMeal.push_back(line); }; // Añadir línea a 'actualMeal'
inline void     clearActualMeal(std::vector<String> &actualMeal){ actualMeal.clear(); }; // Limpiar 'actualMeal'
inline void     saveMealForLater(std::vector<String> &actualMeal, std::vector<String> &unsavedMeals){ // Guardar 'actualMeal' no subida en vector 'unsavedMeals' para intentar subirla más tarde
                for (const auto& meal : actualMeal) 
                    unsavedMeals.push_back(meal); 
                }; 
inline bool     hayMealsToUploadLater(std::vector<String> &unsavedMeals){ return !unsavedMeals.empty(); }; // Comprobar si hay comidas no subidas en 'unsavedMeals'

// Fichero auxiliar de comidas no subidas
void            sendFileToESP32MealByMeal_unsavedTXT();                             // Enviar fichero TXT al esp32 esperando subida de un JSON por comida. Las comidas no subidas se guardan en un TXT auxiliar
inline void     saveMealToAuxFile(std::vector<String> &actualMeal, File& auxFile){ // Guardar comida actual no subida en fichero auxiliar
                for (const auto& line : actualMeal) 
                    auxFile.println(line); 
                }; 
inline bool     hayMealsToUploadLaterTXT(){ return SD.exists(auxFileTXT); };        // Comprobar si se ha escrito algo en el fichero auxiliar de comidas no subidas
void            updateFileTXTFromAuxFile();                                         // Actualizar fichero TXT con comidas no subidas y que se han guardado en un fichero auxiliar
bool            deleteAuxFileTXT();                                                 // Borrar fichero auxiliar TXT
void            readAuxFileTXT();

bool    deleteFileTXT();                                    // Borrar fichero TXT
void    updateFileTXT(std::vector<String> &unsavedMeals);   // Actualizar fichero TXT con comidas sin guardar
void    readFileTXT();                                      // Leer fichero TXT 
/*-----------------------------------------------------------------------------*/






/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicializar la tarjeta SD.
 * @return true: inicialización correcta  false: fallo al inicializar la SD
 */
/*-----------------------------------------------------------------------------*/
bool setupSDcard()
{
    if(!SD.begin(SD_CARD_SCS)){
        Serial.println(F("SD card failure!"));
        return false;
    }
    else{ 
        Serial.println(F("SD card initialized"));
        return true;
    }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía el fichero TXT por Serial al esp32.
 *          Envía todas las líneas seguidas para formar un solo JSON para todas.
 */
/*-----------------------------------------------------------------------------*/
void sendFileToESP32()
{
    File dataFile = SD.open(fileTXT);

    if (dataFile) 
    {
        // Lee el archivo línea por línea y envía cada línea al ESP32
        while (dataFile.available()) 
        {
            String line = dataFile.readStringUntil('\n');
            line.trim();

            // Envía la línea al ESP32 a través de Serial
            //SerialESP32.println(line);
            sendMsgToESP32(line);
        }
        dataFile.close();

        //SerialESP32.println(F("FIN-TRANSMISION"));
        sendMsgToESP32(F("FIN-TRANSMISION"));

        SerialPC.println(F("\nFichero completo enviado"));
    }
    else 
    {
        SerialPC.println(F("\nError al abrir el archivo data-ESP.txt"));
    }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía el fichero TXT al esp32 comida a comida y va actualizando la
 *        cadena de comidas que NO se han podido subir.
 * 
 *      El problema de esta opción es que si falla la subida de muchas comidas, el vector 'unsavedMeals'
 *      puede llegar a ser muy grande y ocupar mucha memoria. Otra posible solución sería guardar en un 
 *      fichero TXT auxiliar las comidas no subidas y, al final, borrar el TXT original y crear uno nuevo
 *      donde se copien todas las líneas del auxiliar. Con esa solución se elimina el problema de saturar 
 *      la RAM, pero se añade latencia por la escritura en el auxiliar, pero sobre todo por la copia de 
 *      todas las líneas al nuevo.
 */
/*-----------------------------------------------------------------------------*/
void sendFileToESP32MealByMeal()
{
    std::vector<String> actualMeal;   // Vector que guarda los datos de la comida actual leída del fichero TXT
    std::vector<String> unsavedMeals; // Vector que guarda las comidas que no se han podido subir

    File dataFile = SD.open(fileTXT);

    if (dataFile) 
    {
        // Lee el archivo línea por línea y envía cada línea al ESP32
        while (dataFile.available()) 
        {
            // ----- LEER LÍNEA DEL TXT -------------
            String line = dataFile.readStringUntil('\n');
            line.trim();
            //SerialPC.print("Linea del TXT: "); SerialPC.println(line);
            // -------------------------------------

            // ----- AÑADIR A actualMeal -----------
            // Añade la línea al vector de la comida actual 
            // (si la línea es INICIO-COMIDA, el vector debería estar vacío antes de insertarla)
            addLineToActualMeal(actualMeal, line);
            // -------------------------------------

            // ----- ENVIAR A ESP32 ----------------
            // Envía la línea al ESP32 a través de Serial
            //SerialESP32.println(line);
            sendMsgToESP32(line);
            // -------------------------------------

            // ----- ESPERAR SUBIDA A DATABASE ----------------
            // Si se ha llegado al final de la comida, se espera si se ha subido correctamente
            if (line.startsWith("FIN-COMIDA")) 
            {
                // ---- ESPERAR RESPUESTA DEL ESP32 -----
                String msgFromESP32 = "";
                unsigned long timeout = 12000; // Tiempo de espera máximo de 12 segundos para que el esp32 responda (tiene 10 segundos para subir info)
                waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
                // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
                // --------------------------------------

                // ---- ANALIZAR RESPUESTA DEL ESP32 ----
                // --- EXITO: COMIDA SUBIDA ------
                if(msgFromESP32 == "SAVED-OK")
                {
                    // Si se recibió SAVED-OK, no se añade a unsavedMeals 
                    SerialPC.println("Comida guardada correctamente\n");

                    // Se resetea la comida actual (final del if)
                    // No se hace return porque se sigue con el bucle de lectura de otras comidas del fichero TXT
                    // Al terminar el fichero, se comprueba si se han podido subir las comidas y entonces se devuelve un resultado
                }
                // -------------------------------
                // --- ERRORES -------------------
                else
                {
                    // -- AÑADIR a unsavedMeals --
                    // Si no hay conexión WiFi o ha habido un error en la petición HTTP, se añade la comida actual al vector de comidas no subidas
                    saveMealForLater(actualMeal, unsavedMeals);
                    // ---------------------------

                    // No se hace return porque se sigue con el bucle de lectura de otras comidas del fichero TXT
                    // Al terminar el fichero, se comprueba si se han podido subir las comidas y entonces se devuelve un resultado
                    
                    if(msgFromESP32 == "NO-WIFI")                   SerialPC.println(F("Se ha perdido la conexión WiFi al subir una comida en la actualización de SM..."));
                    else if(msgFromESP32.startsWith("HTTP-ERROR"))  SerialPC.println(F("Error HTTP al subir la info a database en la actualización de SM..."));
                    else if(msgFromESP32 == "TIMEOUT")              SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32 en la actualización de SM"));
                    else                                            SerialPC.println("Error desconocido al subir la comida a database en la actualización de SM...\n");
                }
                // -------------------------------


                // ---- REINICIAR actualMeal -----------------------
                // Reseta el vector de la comida actual
                clearActualMeal(actualMeal); // Reseta el vector de la comida actual
                // ------------------------------------------------

            }
            // ------------------------------------------------

        }

        // Cerrar fichero
        dataFile.close();

        // Tras enviar todas las comidas, se envía un mensaje de fin de transmisión
        //SerialESP32.println(F("FIN-TRANSMISION"));
        sendMsgToESP32(F("FIN-TRANSMISION"));

        // ------------------------------------------------------
        // Una vez enviado todo el fichero, se comprueba si ha quedado 
        // algo en 'unsavedMeals' para actualizar el fichero TXT
        // ------------------------------------------------------
        if (hayMealsToUploadLater(unsavedMeals)) // Si no se ha subido todo, se actualiza el fichero TXT (borrar y crear nuevo) 
        {                                        // con las comidas no subidas
            SerialPC.println("\nNo se ha podido subir todo");
            updateFileTXT(unsavedMeals); // Actualizar fichero TXT con comidas sin guardar
            //readFileTXT(); // Debe mostrar las comidas no subidas
        }
        else  // Si se ha subido todo, se borra el fichero TXT
        {
            deleteFileTXT(); // Borrar fichero TXT
            //readFileTXT(); // Debe mostrar que no hay fichero
            SerialPC.println("\nINFO COMPLETA GUARDADA!");
        } 
        // ------------------------------------------------------

    }
    else 
        SerialPC.println(F("\nError al abrir el archivo data-ESP.txt\n"));

}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un archivo de comidas al ESP32 de forma progresiva, enviando cada línea por separado.
 *          Si una comida no se puede subir a la base de datos, se guarda en un archivo auxiliar.
 *          Al final se sustituye el TXT original por el auxiliar con las comidas que aún no se han
 *          podido subir. Se utiliza un fichero auxiliar en lugar del vector de String para evitar
 *          desbordamiento de la memoria RAM, aunque el uso inteso de la SD aumente la latencia del programa.
 */
/*-----------------------------------------------------------------------------*/
void sendFileToESP32MealByMeal_unsavedTXT()
{
    std::vector<String> actualMeal;     // Vector que guarda los datos de la comida actual

    File originalFile = SD.open(fileTXT);   // Fichero original con las comidas a subir a la base de datos
    
    File auxFile;                       // Fichero TXT auxiliar donde guardar las comidas que no se han podido subir a la base de datos
    bool auxFileCreated = false;        // Flag para saber si se ha creado el fichero TXT auxiliar de comidas sin guardar

    if (originalFile) 
    {
        // Lee el archivo línea por línea y envía cada línea al ESP32
        while (originalFile.available()) 
        {
            // ----- LEER LÍNEA DEL TXT -------------
            String line = originalFile.readStringUntil('\n');
            line.trim();
            //SerialPC.print("Linea del TXT: "); SerialPC.println(line);
            // -------------------------------------

            // ----- AÑADIR A actualMeal -----------
            // Añade la línea al vector de la comida actual 
            // (si la línea es INICIO-COMIDA, el vector debería estar vacío antes de insertarla)
            addLineToActualMeal(actualMeal, line);
            // -------------------------------------

            // ----- ENVIAR A ESP32 ----------------
            // Envía la línea al ESP32 a través de Serial
            sendMsgToESP32(line);
            // -------------------------------------

            // ----- ESPERAR SUBIDA A DATABASE ----------------
            // Si se ha llegado al final de la comida, se espera si se ha subido correctamente
            if (line.startsWith("FIN-COMIDA")) 
            {
                 // ---- ESPERAR RESPUESTA DEL ESP32 -----
                String msgFromESP32 = "";
                unsigned long timeout = 12000; // Tiempo de espera máximo de 12 segundos para que el esp32 responda (tiene 10 segundos para subir info)
                waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
                // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
                // --------------------------------------

                // ---- ANALIZAR RESPUESTA DEL ESP32 ----
                // --- EXITO: COMIDA SUBIDA ------
                if(msgFromESP32 == "SAVED-OK")
                {
                    // Si se recibió SAVED-OK, no se añade a unsavedMeals 
                    SerialPC.println("Comida guardada correctamente\n");

                    // Se resetea la comida actual (final del if)
                    // No se hace return porque se sigue con el bucle de lectura de otras comidas del fichero TXT
                    // Al terminar el fichero, se comprueba si se han podido subir las comidas y entonces se devuelve un resultado
                }
                // -------------------------------
                // --- ERRORES -------------------
                else
                {
                    // -- AÑADIR A unsavedMeals --
                    // Si no hay conexión WiFi o ha habido un error en la petición HTTP, se añade la comida actual al TXT auxiliar de comidas no subidas
                    // Crear fichero TXT auxiliar la primera vez que falla una subida de comida
                    if(!auxFileCreated)
                    {
                        auxFile = SD.open(auxFileTXT, FILE_WRITE);  // Crear fichero TXT auxiliar
                        auxFileCreated = true;                      // Fichero auxiliar creado
                    }
                    saveMealToAuxFile(actualMeal, auxFile); // Escribir líneas de la comida no subida (actualMeal) en el TXT auxiliar
                    // ---------------------------

                    // No se hace return porque se sigue con el bucle de lectura de otras comidas del fichero TXT
                    // Al terminar el fichero, se comprueba si se han podido subir las comidas y entonces se devuelve un resultado
                    
                    if(msgFromESP32 == "NO-WIFI")                   SerialPC.println(F("Se ha perdido la conexión WiFi al subir una comida en la actualización de SM..."));
                    else if(msgFromESP32.startsWith("HTTP-ERROR"))  SerialPC.println(F("Error HTTP al subir la info a database en la actualización de SM..."));
                    else if(msgFromESP32 == "TIMEOUT")              SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32 en la actualización de SM"));
                    else                                            SerialPC.println("Error desconocido al subir la comida a database en la actualización de SM...\n");
                }
                // -------------------------------


                // ---- REINICIAR actualMeal -----------------------
                // Reseta el vector de la comida actual
                clearActualMeal(actualMeal); // Reseta el vector de la comida actual
                // ------------------------------------------------

            }
            // ------------------------------------------------

        }

        // Cerrar ficheros
        originalFile.close();
        if(auxFileCreated) auxFile.close(); // Cerrar el archivo auxiliar si se ha creado

        // Tras enviar todas las comidas, se envía un mensaje de fin de transmisión
        sendMsgToESP32(F("FIN-TRANSMISION"));


        // ------------------------------------------------------
        // Una vez enviado todo el fichero, se comprueba si ha quedado 
        // algo en el TXT auxiliar para actualizar el fichero TXT de comidas a enviar al ESP32
        // ------------------------------------------------------
        if(!auxFileCreated)  // Si no se ha creado el fichero auxiliar, significa que se han subido todas las comidas sin problema
        {
            SerialPC.println("\nINFO COMPLETA GUARDADA!");
            deleteFileTXT(); // Borrar fichero TXT
            readFileTXT(); // Debe mostrar que no hay fichero
        }
        // Si no se ha subido todo (se ha creado el fichero auxiliar), se actualiza el fichero TXT (borrar y crear nuevo) con las comidas 
        // no subidas, escritas en el TXT auxiliar
        else if(hayMealsToUploadLaterTXT()) // Comprobar que realmente existe, por si fallara su creación
        {                    
            SerialPC.println("\nNo se ha podido subir todo");
            readAuxFileTXT();                                   // Mostrar el contenido del archivo auxiliar
            updateFileTXTFromAuxFile();                         // Actualizar fichero TXT con comidas sin guardar
            readFileTXT();                                      // Debe mostrar las comidas no subidas
            deleteAuxFileTXT();                                 // Eliminar archivo auxiliar
        }
        // ------------------------------------------------------

    }
    else // No se ha podido abrir el TXT con las comidas a subir a la base de datos
        SerialPC.println(F("\nError al abrir el archivo data-ESP.txt\n"));

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el fichero TXT y crea un nuevo con las comidas que no se han podido subir
 *          y que se han guardado, temporalmente, en un fichero auxiliar.
 * 
 */
/*-----------------------------------------------------------------------------*/
void updateFileTXTFromAuxFile() 
{
    SerialPC.println(F("\nActualizando fichero TXT (ESP32)..."));

    // Borrar fichero
    deleteFileTXT(); // Ya comprueba si existe el TXT original antes de borrarlo

    // Abrir el archivo auxiliar en modo lectura
    File auxFile = SD.open(auxFileTXT, FILE_READ);
    if (!auxFile) {
        SerialPC.println(F("Error al abrir el archivo auxiliar para lectura"));
        return;
    }

    // Crear un nuevo archivo para el archivo original actualizado
    File originalFile = SD.open(fileTXT, FILE_WRITE);
    if (!originalFile) {
        SerialPC.println(F("Error al crear de nuevo el archivo TXT"));
        auxFile.close();
        return;
    }

    // Leer el archivo auxiliar línea por línea y escribir en el nuevo archivo
    while (auxFile.available()) 
    {
        String line = auxFile.readStringUntil('\n');
        originalFile.println(line); 
    }

    // Cerrar los archivos
    auxFile.close();
    originalFile.close();

    SerialPC.println(F("Archivo TXT actualizado con comidas no subidas"));
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el fichero auxiliar TXT que guarda las comidas cuya subida a la database ha fallado.
 * 
 * Esta función borra el fichero TXT auxiliar de la tarjeta SD.
 * 
 * @return true si el fichero se borra correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool deleteAuxFileTXT()
{
    // -------- BORRAR FICHERO ESP32 ------------------------
    if (SD.exists(auxFileTXT))
    {
        SerialPC.println(F("\nBorrando fichero auxiliar TXT (ESP32)..."));
        SD.remove(auxFileTXT);

        if (!SD.exists(auxFileTXT)) 
        {
            SerialPC.println(F("Fichero auxiliar TXT (ESP32) borrado"));
            return true;
        }
        else  
        {
            SerialPC.println(F("Error borrando fichero auxiliar TXT (ESP32)!"));
            return false;
        }
    }
    else{
        SerialPC.println(F("\nNo existe el auxiliar TXT (ESP32)"));
        return true;
    }
    // ------------------------------------------------------

}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Lee el contenido del archivo en la tarjeta SD.
 */
/*-----------------------------------------------------------------------------*/
void readAuxFileTXT()
{
    if (SD.exists(fileTXT)){
        Serial.println(F("\n\nLeyendo fichero auxiliar...\n"));

        File myFile = SD.open(auxFileTXT, FILE_READ);
        
        if (myFile){
            while (myFile.available()) {
                Serial.write(myFile.read());
            }
            myFile.close();
        }
        else{
            Serial.println("\nError abriendo el fichero auxiliar TXT para lectura!");
        }
    }
    else{
        Serial.println("\nNo se encuentra el fichero auxiliar TXT!\n");
    }
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el fichero TXT que guarda las comidas que aún no se ha subido.
 * 
 * Esta función borra un fichero TXT de la tarjeta SD.
 * 
 * @return true si el fichero se borra correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool deleteFileTXT()
{
    // -------- BORRAR FICHERO ESP32 ------------------------
    if (SD.exists(fileTXT))
    {
        SerialPC.println(F("\nBorrando fichero TXT (ESP32)..."));
        SD.remove(fileTXT);

        if (!SD.exists(fileTXT)) 
        {
            SerialPC.println(F("Fichero TXT (ESP32) borrado"));
            return true;
        }
        else  
        {
            SerialPC.println(F("Error borrando fichero TXT (ESP32)!"));
            return false;
        }
    }
    else{
        SerialPC.println(F("\nNo existe el fichero TXT (ESP32)"));
        return true;
    }
    // ------------------------------------------------------

    // En este caso no hace falta crearlo aquí, como sí ocurría con el CSV, porque
    // cuando se vaya a escribir algo al guardar comida, ya se creará.
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el fichero TXT y crea un nuevo con las comidas que no se han podido subir
 * 
 */
/*-----------------------------------------------------------------------------*/
void updateFileTXT(std::vector<String> &unsavedMeals)
{
    // Borrar fichero
    deleteFileTXT(); // Ya comprueba si existe antes de borrarlo

    SerialPC.println(F("\nActualizando fichero TXT (ESP32)..."));

    // Crear nuevo fichero
    File dataFile = SD.open(fileTXT, FILE_WRITE);

    if (dataFile) 
    {
        // Escribir en el fichero las comidas no subidas
        for (const auto& meal : unsavedMeals) {
            dataFile.println(meal);
        }
        dataFile.close();

        SerialPC.println(F("Fichero actualizado!"));
    }
    else 
    {
        SerialPC.println(F("Error al actualizar el archivo data-ESP.txt"));
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Lee el contenido del archivo en la tarjeta SD.
 */
/*-----------------------------------------------------------------------------*/
void readFileTXT()
{
    if (SD.exists(fileTXT)){
        Serial.println(F("\n\nLeyendo fichero...\n"));

        File myFile = SD.open(fileTXT, FILE_READ);
        
        if (myFile){
            while (myFile.available()) {
                Serial.write(myFile.read());
            }
            myFile.close();
        }
        else{
            Serial.println("\nError abriendo el fichero TXT para lectura!");
        }
    }
    else{
        Serial.println("\nNo se encuentra el fichero TXT!\n");
    }
}




#endif
