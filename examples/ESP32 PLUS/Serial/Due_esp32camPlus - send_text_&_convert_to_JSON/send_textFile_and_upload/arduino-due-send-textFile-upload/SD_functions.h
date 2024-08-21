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
char fileTXT[30] = "data/data-esp.txt";




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    setupSDcard();                                      // Inicializar tarjeta SD

void    sendFileToESP32();                                  // Enviar fichero TXT al esp32 para un solo JSON
void    sendFileToESP32MealByMeal();                        // Enviar fichero TXT al esp32 esperando subida de un JSON por comida

void    saveMealForLater(std::vector<String> &actualMeal, std::vector<String> &unsavedMeals);   // Guardar 'actualMeal' no subida en vector 'unsavedMeals' para intentar subirla más tarde

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
            //SerialDueESP32.println(line);
            sendMsgToESP32(line);
        }
        dataFile.close();

        //SerialDueESP32.println(F("FIN-TRANSMISION"));
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
    std::vector<String> actualMeal;   // Vector que guarda los datos de la comida actual
    std::vector<String> unsavedMeals; // Vector que guarda las comidas que no se han podido subir

    File dataFile = SD.open(fileTXT);

    unsigned long timeout = 10000; // Tiempo de espera máximo de 10 segundos para que el esp32 responda

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
            actualMeal.push_back(line); 
            // -------------------------------------

            // ----- ENVIAR A ESP32 ----------------
            // Envía la línea al ESP32 a través de Serial
            //SerialDueESP32.println(line);
            sendMsgToESP32(line);
            // -------------------------------------

            // ----- ESPERAR SUBIDA A DATABASE ----------------
            // Si se ha llegado al final de la comida, se espera si se ha subido correctamente
            if (line.startsWith("FIN-COMIDA")) 
            {
                unsigned long startTime = millis();

                // Esperar hasta 10 segundos a que el ESP32 responda
                //while (SerialDueESP32.available() == 0 && millis() - startTime < timeout);
                while(isESP32SerialEmpty() && timeoutNotExceeded(startTime, timeout));

                // ---------------------------------------------
                // Cuando se recibe mensaje o se pasa el timout, 
                // entonces se comprueba la respuesta
                // ---------------------------------------------

                // ------- RESPUESTA DEL ESP32 --------------------
                //if (SerialDueESP32.available() > 0)  // El ESP32 ha respondido
                if(hayMsgFromESP32())
                {
                    /*String msgFromESP32 = SerialDueESP32.readStringUntil('\n');
                    msgFromESP32.trim(); */
                    String msgFromESP32 = readMsgFromESP32();
                    SerialPC.print("\nMensaje recibido en sendFile: "); SerialPC.println(msgFromESP32); 
                    
                    // ----- COMIDA SUBIDA ----------
                    if(msgFromESP32 == "SAVED-OK")
                        // Si se recibió SAVED-OK, no se añade a unsavedMeals 
                        SerialPC.println("Comida guardada correctamente\n\n");
                    // ------------------------------

                    // ----- COMIDA NO SUBIDA -------
                    else if (msgFromESP32 == "NO-WIFI" || msgFromESP32.startsWith("HTTP-ERROR")) {
                        SerialPC.println("Sin WiFi o error en la petición HTTP\n");
                        // -- AÑADIR A unsavedMeals --
                        // Si no hay conexión WiFi o ha habido un error en la petición HTTP, se añade la comida actual al vector de comidas no subidas
                        saveMealForLater(actualMeal, unsavedMeals);
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
                    saveMealForLater(actualMeal, unsavedMeals);
                    // ---------------------------------------------
                }
                // ------------------------------------------------


                // ---- REINICIAR actualMeal -----------------------
                // Reseta el vector de la comida actual
                actualMeal.clear();
                // ------------------------------------------------

            }
            // ------------------------------------------------

        }

        // Cerrar fichero
        dataFile.close();

        // Tras enviar todas las comidas, se envía un mensaje de fin de transmisión
        //SerialDueESP32.println(F("FIN-TRANSMISION"));
        sendMsgToESP32(F("FIN-TRANSMISION"));


        // ------------------------------------------------------
        // Una vez enviado todo el fichero, se comprueba si ha quedado 
        // algo en 'unsavedMeals' para actualizar el fichero TXT
        // ------------------------------------------------------
        if (unsavedMeals.empty()) { // Si se ha subido todo, se borraría el fichero TXT
            SerialPC.println("\nINFO COMPLETA GUARDADA!");
            deleteFileTXT(); // Borrar fichero TXT
            //readFileTXT(); // Debe mostrar que no hay fichero

        } 
        // Si no se ha subido todo, se actualiza el fichero TXT (borrar y crear nuevo) con las comidas no subidas
        else { 
            SerialPC.println("\nNo se ha podido subir todo");
            updateFileTXT(unsavedMeals); // Actualizar fichero TXT con comidas sin guardar
            readFileTXT(); // Debe mostrar las comidas no subidas
        }
        // ------------------------------------------------------

    }
    else 
        SerialPC.println(F("\nError al abrir el archivo data-ESP.txt\n"));

}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda la comida 'actualMeal' en el vector 'unsavedMeals' para intentar subirla más tarde
 */
/*-----------------------------------------------------------------------------*/
void saveMealForLater(std::vector<String> &actualMeal, std::vector<String> &unsavedMeals)
{
    // Recorre el vector actualMeal y añade cada línea al vector unsavedMeals
    for (const auto& meal : actualMeal) unsavedMeals.push_back(meal); 

    // Esta línea se supone que hace lo mismo, pero creo que no funcionaba
    //unsavedMeals.insert(unsavedMeals.end(), actualMeal.begin(), actualMeal.end()); 
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra un fichero en el ESP32.
 * 
 * Esta función borra un fichero en el ESP32 utilizando la tarjeta SD.
 * 
 * @return true si el fichero se borra correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool deleteFileTXT()
{
    // -------- BORRAR FICHERO ESP32 ------------------------
    if (SD.exists(fileTXT)){
        SerialPC.println(F("\nBorrando fichero TXT (ESP32)..."));
        SD.remove(fileTXT);

        if (!SD.exists(fileTXT)) {
            SerialPC.println(F("Fichero TXT (ESP32) borrado"));
            return true;
        }
        else  {
            SerialPC.println(F("Error borrando fichero ESP32!"));
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
