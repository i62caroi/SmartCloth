#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H


// ----- ESP32 <--> PC ----------
#include "debug.h" // SerialPC 
// ------------------------------


// ----- ESP32 <--> Due ---------
// ESP32-CAM PLUS
#define RXD1 16
#define TXD1 17

#define SerialDue Serial1
// ------------------------------


// ----- ESP32 <-- BR -----------
// BR: Barcode Reader
#include <SoftwareSerial.h>

#define RXD2 19 // Tx (naranja lector)
#define TXD2 21 // Rx (azul lector)

SoftwareSerial mySerial(RXD2, TXD2); // RX, TX
#define SerialBR mySerial // Renombrar mySerial
// ------------------------------



/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-----------------------------------------------------------------------------*/
// Funciones para manejar la comunicación Serial con el Due y el lector
void           setupAllSerial();                                // Inicializar comunicación serie con PC, Due y lector de código de barras
void           ackDue();                                        // Indicar que la comunicación está establecida

inline void    sendMsgToDue(const String &msg);                 // Enviar 'msg' del ESP32 al Due
inline bool    hayMsgFromDue();                                 // Comprobar si hay mensajes del Due disponibles
inline bool    isDueSerialEmpty(){ return !hayMsgFromDue(); }   // Comprobar si no hay mensajes del Due disponibles
inline void    readMsgFromSerialDue(String &msgFromDue);        // Leer mensaje del puerto serie ESP32-Due

inline bool    isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout); // Comprobar si se ha excedido el tiempo de espera
/*-----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
                           IMPLEMENTACIÓN FUNCIONES
/*-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la comunicación serial entre el ESP32 y los demás dispositivos (PC, Due y lector).
 *  Esta función inicializa los puertos seriales y establece la velocidad de baudios
 *  en 115200 para el PC y el Due, y en 9600 para el lector de código de barras.
 */
/*-----------------------------------------------------------------------------*/
void setupAllSerial() 
{
    // Deben tener la misma velocidad de baudios que el PC, el Due y el Barcode Reader, respectivamente
    
    // ----- ESP32 - PC ----------
    #if defined(SM_DEBUG)
        SerialPC.begin(115200);
        delay(100);
    #endif // SM_DEBUG
    // ---------------------------

    // ----- ESP32 - Due ---------
    SerialDue.begin(115200, SERIAL_8N1, RXD1, TXD1); 
    delay(100);
    // ---------------------------

    // ----- ESP32 - CAM ------
    SerialBR.begin(9600); // Debe ser 9600 porque así trabaja el lector
    delay(100);
    // ---------------------------
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Envia una respuesta "PONG" al Due para indicar que la comunicación está establecida.
 * 
 * Esta función envía un mensaje "PONG" al Due para indicar que la comunicación entre el ESP32 y el Due está establecida.
 */
/*-----------------------------------------------------------------------------*/
void ackDue() 
{
    // Enviar respuesta "PONG" al Due para indicar que la comunicación está establecida
    sendMsgToDue("PONG");

    #if defined(SM_DEBUG)
        SerialPC.println(F("Comunicación con Due establecida"));
    #endif
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un mensaje desde el ESP32 al Arduino Due.
 * @param msg El mensaje a enviar.
 */
/*-----------------------------------------------------------------------------*/
inline void sendMsgToDue(const String &msg) { SerialDue.println(msg); }



/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si hay mensajes del Due disponibles en el puerto serial.
 * @return true si hay mensajes disponibles, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool hayMsgFromDue() { return SerialDue.available() > 0; }



/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el Due y lo guarda en la variable proporcionada.
 * 
 * @param msgFromDue La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromSerialDue(String &msgFromDue) { 
    msgFromDue = SerialDue.readStringUntil('\n');
    msgFromDue.trim(); // Elimina espacios en blanco al principio y al final
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