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
inline bool     isESP32SerialEmpty(){ return !hayMsgFromESP32(); }  // Comprobar si no hay mensajes del ESP32 disponibles
inline String   readMsgFromESP32();                                 // Leer mensaje del puerto serie Due-ESP32

inline bool     timeoutNotExceeded(unsigned long startTime, unsigned long timeout); // Comprobar si se ha excedido el tiempo de espera
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
 * @brief Lee un mensaje desde el puerto serie Due-ESP32 y lo devuelve como un String
 * @return String - El mensaje leído desde el puerto serie.
 */
/*-----------------------------------------------------------------------------*/
inline String readMsgFromESP32() { 
    String msg = SerialDueESP32.readStringUntil('\n');
    msg.trim();
    return msg;
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si se ha excedido el tiempo de espera.
 * 
 * Esta función compara el tiempo transcurrido desde el inicio con el tiempo de espera especificado.
 * 
 * @param startTime El tiempo de inicio en milisegundos.
 * @param timeout El tiempo de espera en milisegundos.
 * @return true si el tiempo transcurrido es menor que el tiempo de espera, false de lo contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool timeoutNotExceeded(unsigned long startTime, unsigned long timeout) {
    return millis() - startTime < timeout;
}


#endif