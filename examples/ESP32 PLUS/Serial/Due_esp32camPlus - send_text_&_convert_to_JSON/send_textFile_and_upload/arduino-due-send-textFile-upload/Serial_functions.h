#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H

#define SerialPC Serial
#define SerialDueESP32 Serial1

/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
// Comunicación Serial Due-ESP32
void            setupSerial();                                      // Inicializar comunicación serie con PC y ESP32
inline void     sendMsgToESP32(const String &msg);                  // Enviar 'msg' del Due al ESP32
inline bool     hayMsgFromESP32();                                  // Comprobar si hay mensajes del ESP32 disponibles
inline void     readMsgFromESP32(String &msgFromESP32);             // Leer mensaje del puerto serie Due-ESP32 y guardarlo en msgFromESP32

void            waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout);            // Espera la respuesta del ESP32, sea cual sea, y la devuelve en msgFromESP32.

inline bool     isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout);            // Comprobar si se ha excedido el tiempo de espera
/*-----------------------------------------------------------------------------*/




/*------------------------------------------------------------------------------
                           IMPLEMENTACIÓN FUNCIONES
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la comunicación serial entre el PC y el ESP32.
 *  Esta función inicializa los puertos seriales y establece la velocidad de baudios
 *  en 115200 para ambos, el PC y el ESP32.
 */
/*-----------------------------------------------------------------------------*/
void setupSerial() {
    // Deben tener la misma velocidad de baudios que el PC y ESP32, respectivamente
    SerialPC.begin(115200);
    SerialDueESP32.begin(115200); 
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un mensaje desde el Arduino Due al ESP32.
 * @param msg El mensaje a enviar.
 */
/*-----------------------------------------------------------------------------*/
inline void sendMsgToESP32(const String &msg) { SerialDueESP32.println(msg); }



/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si hay mensajes del ESP32 disponibles en el puerto serial.
 * @return true si hay mensajes disponibles, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool hayMsgFromESP32() { return SerialDueESP32.available() > 0; }


/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el ESP32 y lo guarda en la variable proporcionada.
 * 
 * @param msgFromESP32 La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromESP32(String &msgFromESP32) { 
    msgFromESP32 = SerialDueESP32.readStringUntil('\n'); 
    msgFromESP32.trim();  // Elimina espacios en blanco al principio y al final
}


/*---------------------------------------------------------------------------------------------------------*/
/**
 * Espera la respuesta del ESP32 durante un tiempo determinado.
 * 
 * @param msgFromESP32 Referencia a un objeto String donde se almacenará el mensaje recibido del ESP32.
 * @param timeout Tiempo máximo de espera en milisegundos.
 */
/*---------------------------------------------------------------------------------------------------------*/
void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(!hayMsgFromESP32() && !isTimeoutExceeded(startTime, timeout));

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromESP32())  // Si el esp32 ha respondido
    {
        readMsgFromESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
        SerialPC.println("Respuesta del ESP32: " + msgFromESP32);  
    } 
    else // No se ha recibido respuesta del ESP32
    {
        //SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));

        // Se considera que no hay conexión WiFi
        msgFromESP32 = "TIMEOUT";;
    }

}


/*-----------------------------------------------------------------------------*/
/**
 * Comprueba si se ha excedido el tiempo de espera.
 * 
 * Esta función compara el tiempo actual con el tiempo de inicio y verifica si se ha excedido el tiempo de espera especificado.
 * 
 * @param startTime El tiempo de inicio en milisegundos.
 * @param timeout El tiempo de espera en milisegundos.
 * @return true si se ha excedido el tiempo de espera, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout) 
{ 
    return millis() - startTime > timeout; 
}


#endif