#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#define SerialPC Serial
#define SerialDueESP32 Serial1

#include "Serial_functions.h"


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    checkWifiConnection();                                                                  // Preguntar al esp32 si tiene conexión WiFi
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
   // SerialDueESP32.print(F("CHECK-WIFI")); //Envía la cadena al esp32
    sendMsgToESP32("CHECK-WIFI");

    unsigned long timeout = 3000;  // Espera máxima de 3 segundos
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Espera a recibir la respuesta del ESP32. Dos enfoques:

    // Enfoque 1: bucle while se ejecuta mientras no se recibe una respuesta del ESP32 y no se ha
    // superado el tiempo de espera. Es más eficiente que el enfoque 2 porque se detiene en cuanto
    // se recibe la respuesta o se supera el tiempo de espera, pero no ejecuta las líneas de comprobación
    // continuamente si no se ha recibido respuesta del ESP32.

    // Esperar 3 segundos a que el ESP32 responda
    //while ((SerialDueESP32.available() == 0) && (millis() - startTime < timeout)); 
    while(isESP32SerialEmpty() && timeoutNotExceeded(startTime, timeout));
    
    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromESP32()) { // Si el esp32 ha respondido
        /*String msgFromESP32 = SerialDueESP32.readStringUntil('\n');
        msgFromESP32.trim();*/  // Elimina espacios en blanco al principio y al final
        String msgFromESP32 = readMsgFromESP32();
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
    else {
        // No se ha recibido respuesta del ESP32
        SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));

        // Se considera que no hay conexión WiFi
        return false;
    }


    // Enfoque 2: bucle while infinito que incluye todas las comprobaciones del tipo de mensaje recibido.
    // Como se ejecuta hasta que se recibe una respuesta del ESP32 o se supera el tiempo de espera,
    // obliga a ejecutar todas esas líneas aunque no se haya recibido respuesta del ESP32. Por tanto, puede ser 
    // poco eficiente si la respuesta del ESP32 tarda mucho en llegar
    /*while (true) { // Bloquea el arduino en este bucle hasta que se reciba respuesta o se pase el TIMEOUT
        // Comprueba si hay datos disponibles en el puerto serie del ESP32
        if (SerialDueESP32.available() > 0) { // Si el esp32 ha respondido
            String msgFromESP32 = SerialDueESP32.readStringUntil('\n');
            msgFromESP32.trim();      // Elimina espacios en blanco al principio y al final
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
    */


}









#endif