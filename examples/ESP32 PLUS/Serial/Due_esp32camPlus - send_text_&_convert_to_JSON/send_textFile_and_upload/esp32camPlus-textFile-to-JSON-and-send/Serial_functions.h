#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H


// ESP32-CAM
//#define RXD1 14
//#define TXD1 15

// ESP32-CAM PLUS
#define RXD1 16
#define TXD1 17

#define SerialPC Serial
#define SerialESP32Due Serial1




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
// Comunicación Serial ESP32-Due
void            setupAllSerial();                             // Inicializar comunicación serie con PC y Due
inline void     sendMsgToDue(const String &msg);              // Enviar 'msg' del ESP32 al Due
inline bool     hayMsgFromDue();                              // Comprobar si hay mensajes del Due disponibles
inline void     readMsgFromSerialDue(String &msgFromDue);     // Leer mensaje del puerto serie ESP32-Due

void            waitMsgFromDue(String &msgFromESP32, unsigned long &timeout);   

inline bool    isTimeoutExceeded(unsigned long startTime, unsigned long timeout); // Comprobar si se ha excedido el tiempo de espera
/*-----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
                           IMPLEMENTACIÓN FUNCIONES
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la comunicación serial entre el PC y el Due.
 *  Esta función inicializa los puertos seriales y establece la velocidad de baudios
 *  en 115200 para ambos, el PC y el Due.
 */
/*-----------------------------------------------------------------------------*/
void setupAllSerial() {
    // Deben tener la misma velocidad de baudios que el PC y Due, respectivamente
    SerialPC.begin(115200);
    SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1); 
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un mensaje desde el ESP32 al Arduino Due.
 * @param msg El mensaje a enviar.
 */
/*-----------------------------------------------------------------------------*/
inline void sendMsgToDue(const String &msg) { SerialESP32Due.println(msg); delay(100); }



/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si hay mensajes del Due disponibles en el puerto serial.
 * @return true si hay mensajes disponibles, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool hayMsgFromDue() { return SerialESP32Due.available() > 0; }



/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el Due y lo guarda en la variable proporcionada.
 * 
 * @param msgFromDue La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromSerialDue(String &msgFromDue) { 
    msgFromDue = SerialESP32Due.readStringUntil('\n');
    msgFromDue.trim(); // Elimina espacios en blanco al principio y al final
}


/*---------------------------------------------------------------------------------------------------------*/
/**
 * Espera un mensaje del Due durante un tiempo determinado.
 * 
 * @param msgFromESP32 Referencia a un objeto String donde se almacenará el mensaje recibido del Due.
 * @param timeout Tiempo máximo de espera en milisegundos.
 */
/*---------------------------------------------------------------------------------------------------------*/
void waitMsgFromDue(String &msgFromDue, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el Due responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(!hayMsgFromDue() && !isTimeoutExceeded(startTime, timeout));

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromDue())  // Si el Due ha respondido
    {
        readMsgFromSerialDue(msgFromDue); // Leer mensaje del puerto serie y guardarlo en msgFromDue
        SerialPC.println("Respuesta del Due: " + msgFromDue);  
    } 
    else // No se ha recibido respuesta del Due
    {
        SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del Due"));

        // Se considera que no hay conexión WiFi
        msgFromDue = "TIMEOUT";;
    }

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
inline bool isTimeoutExceeded(unsigned long startTime, unsigned long timeout) {
    return millis() - startTime > timeout;
}



#endif