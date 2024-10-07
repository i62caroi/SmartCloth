#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#define SerialPC Serial
#define SerialESP32 Serial1

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
bool checkWifiConnection() 
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    //limpiarBufferESP32();
    // ---------------------------------------------------------

    // ---- PREGUNTAR POR WIFI ---------------------------------
    SerialPC.println(F("Comprobando la conexion WiFi del ESP32..."));    
    sendMsgToESP32("CHECK-WIFI"); // Envía la cadena al ESP32
    // ---------------------------------------------------------

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 3000; // Espera máxima de 3 segundos
                                    // No hace falta esperar más tiempo porque esta es una comprobación directa del ESP32
                                    // y los mensajes que se pueden recibir "WIFI-OK" o "NO-WIFI" son cortos, por lo que no
                                    // debería tardar demasiado waitResponseFromESP32() en recibirlos.
    waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
    // --------------------------------------

    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    // --- EXITO ------
    if (msgFromESP32 == "WIFI-OK") // Respuesta OK, hay conexión WiFi
    {
        SerialPC.println(F("Dice que hay wifi"));
        return true;
    } 
    // -----------------
    // --- "ERRORES" ---
    else if (msgFromESP32 == "NO-WIFI") // Respuesta NO-WIFI, no hay conexión WiFi
    {
        SerialPC.println(F("Dice que NO hay wifi"));
        return false;
    }
    else if (msgFromESP32 == "TIMEOUT") // No se ha recibido respuesta del ESP32
    {
        SerialPC.println(F("TIMEOUT. Sin respuesta del ESP32 al comprobar la conexion WiFi"));
        return false; // Se considera que no hay conexión WiFi
    }
    else // Mensaje desconocido
    {
        SerialPC.println(F("Error desconocido al comprobar la conexion WiFi..."));
        return false; // Se considera que no hay conexión WiFi
    }
    // -----------------
    // --------------------------------------
    // ---------------------------------------------------------

}









#endif