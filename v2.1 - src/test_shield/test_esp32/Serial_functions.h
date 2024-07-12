#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H

#define SerialPC Serial

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
// ------------------------------

/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-----------------------------------------------------------------------------*/
// Funciones para manejar la comunicación Serial con el Due y el lector
void            setupAllSerial();                                // Inicializar comunicación serie con PC, Due y lector de código de barras
void            ackDue();                                        // Indicar que la comunicación está establecida

inline void     sendMsgToDue(const String &msg) { SerialDue.println(msg); };
inline bool     hayMsgFromDue() { return SerialDue.available() > 0; };
inline void     readMsgFromSerialDue(String &msgFromDue) { msgFromDue = SerialDue.readStringUntil('\n'); msgFromDue.trim(); };
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
    SerialPC.begin(115200);
    delay(100);
    // ---------------------------

    // ----- ESP32 - Due ---------
    SerialDue.begin(115200, SERIAL_8N1, RXD1, TXD1); 
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

    SerialPC.println(F("Comunicación con Due establecida"));
}





#endif