/*
    Irene Casares Rodríguez
    12/01/2024

    Funciones para enviar un String como simulación del esp32File del Due al ESP32 
    por comunicación Serial.
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "cadenas.h"
#include <vector>

#define SerialPC Serial
#define SerialDueESP32 Serial1



/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    checkWifiConnection();                              // Preguntar al esp32 si tiene conexión WiFi
void    sendStringSimulationToEsp32(String fileContent);    // Enviar String al esp32 simulando el fichero TXT
int     sendStringSimulationToEsp32_counting(String fileContent);
void    updateStringSimulation(String &successfulMeals, int nComidasContadas, String fileContent);      
void    getIDsComidas(String &successfulMeals, std::vector<int> &vectorIDs);
/*-----------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Comprueba la conexión WiFi del ESP32.
 * 
 * @return true si hay conexión WiFi, false si no la hay.
 */
/*---------------------------------------------------------------------------------------------------------*/
bool checkWifiConnection() {
    SerialPC.println(F("Comprobando la conexión WiFi del ESP32..."));

    SerialPC.println(F("Cadena enviada al esp32: CHECK-WIFI")); 
    SerialDueESP32.print(F("CHECK-WIFI")); //Envía la cadena al esp32

    unsigned long timeout = 3000;  // Espera máxima de 3 segundos
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    while (true) { // Bloquea el arduino en este bucle hasta que se reciba respuesta o se pase el TIMEOUT
        // Comprueba si hay datos disponibles en el puerto serie del ESP32
        if (SerialDueESP32.available() > 0) { // Si el esp32 ha respondido
            String msgFromESP32 = SerialDueESP32.readStringUntil('\n');
            msgFromESP32.trim();  // Elimina espacios en blanco al principio y al final
            SerialPC.println("Respuesta del ESP32: " + msgFromESP32);  

            if (msgFromESP32 == "WIFI-OK") {
                // Respuesta OK, hay conexión WiFi
                SerialPC.println(F("Dice que hay wifi"));
                return true;
            } 
            else if (msgFromESP32 == "NO-WIFI") {
                // Respuesta NO-WIFI, no hay conexión WiFi
                SerialPC.println(F("Dice que NO hay wifi"));
                return false;
            }
        }

        // Comprueba si ha pasado un tiempo límite sin respuesta del esp32
        if (millis() - startTime > timeout) {
            // Tiempo de espera agotado, se considera que no hay conexión WiFi
            SerialPC.println(F("Timeout"));
            return false;
        }
    }

    // Si sale del while (no debería), se considera que no hay conexión WiFi
    return false;
}



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



int sendStringSimulationToEsp32_counting(String fileContent)
{
    int nComidas = 0;
    int start = 0;
    int end = fileContent.indexOf('\n');

    while (end != -1) 
    {
        String line = fileContent.substring(start, end);
        line.trim();

        if(line == "INICIO-COMIDA") nComidas++;

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

    SerialPC.print("Numero de comidas enviadas: "); SerialPC.println(nComidas);

    return nComidas;
}



void updateStringSimulation(String &successfulMeals, int nComidasContadas, String fileContent){
    std::vector<int> successfulMealIDs;
    // Divide la cadena en los IDs individuales
    getIDsComidas(successfulMeals, successfulMealIDs);

    // Si se han subido todas las comidas (hay tantos IDs como comidas ha contado el Arduino)
    // entonces se borra el fichero TXT (la cadena de simulacion) completo
    if(successfulMealIDs.size() == nComidasContadas){
        SerialPC.println("Se borra todo el fichero TXT");
    }
    else{ // Se borran solo las comidas que se han podido subir
        SerialPC.println("se borran solo las comidas que se han podido subir");
    }

}


void getIDsComidas(String &successfulMeals, std::vector<int> &vectorIDs){
    // -- Divide la cadena en los IDs individuales ---
    int start = 0;
    int end = successfulMeals.indexOf(',');
    while (end != -1) {
        vectorIDs.push_back(successfulMeals.substring(start, end).toInt());
        start = end + 1;
        end = successfulMeals.indexOf(',', start);
    }
    vectorIDs.push_back(successfulMeals.substring(start).toInt());
    // ----------------------------------------------
}

#endif