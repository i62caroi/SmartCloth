/**
 * @file Serial_functions.h
 * @brief Este archivo contiene funciones para manejar las comunicaciones Serial entre el ESP32-Due y el ESP32-BarcodeReader, independientemente.
 * 
 * @author Irene Casares Rodríguez
 * @date 18/04/2024
 */

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



#define TIME_TO_READ_BARCODE 30000L // 30 segundos para leer el código de barras
#define BUFFER_EMPTY "-" // Buffer vacío


/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-------------------------------------------------------------------------------
 Funciones para manejar la comunicación Serial del ESP32 con el Due y el lector
-------------------------------------------------------------------------------*/
void           setupAllSerial();                                                // Inicializar comunicación serie con PC, Due y lector de código de barras

// ----- Funciones auxiliares -----
// Comunicación Serial ESP32-PC
#ifdef SM_DEBUG
    inline bool     hayMsgFromPC(){ return SerialPC.available() > 0; };         // Comprobar si hay mensajes del PC (escritos en terminal)
    inline void     readMsgFromSerialPC(String &msgFromPC);                     // Leer mensaje del puerto serie ESP32-PC 
#endif

// Comunicación Serial ESP32-Due
inline bool    hayMsgFromDue() { return SerialDue.available() > 0; }                            // Comprobar si hay mensajes del Due disponibles
inline bool    isDueSerialEmpty(){ return !hayMsgFromDue(); }                                   // Comprobar que el Serial del Due está vacío (no hay mensajes)
inline void    readMsgFromSerialDue(String &msgFromDue);                                        // Leer mensaje del puerto serie ESP32-Due
void           waitMsgFromDue(String &msgFromESP32, unsigned long &timeout);                    // Esperar mensaje del Due durante un tiempo determinado
inline void    limpiarBufferDue(){ while(SerialDue.available() > 0) { SerialDue.read(); } };    // Limpiar buffer con el Due
inline void    sendMsgToDue(const String &msg){ limpiarBufferDue(); SerialDue.println(msg); };  // Limpiar buffer y enviar 'msg' del ESP32 al Due

// Comunicación Serial ESP32-BR
inline bool     hayMsgFromBR(){ return SerialBR.available() > 0; };                         // Comprobar si hay mensajes del BR (Barcode Reader) disponibles (se ha leído código)
inline void     limpiarBufferBR(){ while(SerialBR.available() > 0) { SerialBR.read(); } };  // Limpiar buffer del lector de códigos de barras
inline void     readMsgFromSerialBR(String &msgFromBR);                                     // Leer mensaje del puerto serie ESP32-BR (leer el código de barras)
void            waitForBarcode(String &buffer);                                             // Esperar a que se lea un código de barras

inline bool    isTimeoutExceeded(unsigned long startTime, unsigned long timeout){ return millis() - startTime > timeout; }; // Comprobar si se ha excedido el tiempo de espera
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
 * Lee un mensaje desde el puerto serie con el PC y lo guarda en la variable proporcionada.
 * 
 * @param msgFromPC La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
#ifdef SM_DEBUG
    inline void readMsgFromSerialPC(String &msgFromPC) { 
        msgFromPC = SerialPC.readStringUntil('\n');
        msgFromPC.trim(); // Elimina espacios en blanco al principio y al final
    }
#endif 



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
    } 
    else // No se ha recibido respuesta del Due
    {
        // Se considera que no hay conexión WiFi
        msgFromDue = "TIMEOUT-DUE";;
    }

}




/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el BR y lo guarda en la variable proporcionada.
 * 
 * @param msgFromBR La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromSerialBR(String &msgFromBR) { 
    msgFromBR = SerialBR.readStringUntil('\n');
    msgFromBR.trim(); // Elimina espacios en blanco al principio y al final
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Espera a que se lea un código de barras.
 *        Mientras espera el código, comprueba si el Due le indica que el usuario ha cancelado
 *        la lectura recibiendo el mensaje "CANCEL-BARCODE".
 * 
 * @param barcode Referencia a una cadena de caracteres donde se guardará el código de barras leído.
 * 
 * @note Esta función espera un máximo de 30 segundos para recibir un código de barras. Si no se recibe ningún código
 *       de barras en ese tiempo, se considera que no se ha leído ningún código y se asigna el valor "-" a la variable barcode.
 */
/*-----------------------------------------------------------------------------*/
void waitForBarcode(String &buffer)
{
    unsigned long startTime = millis();  

    // Esperar 30 segundos a que se lea un código de barras. Sale si se recibe mensaje o si se pasa el tiempo de espera
    //      Si no se ha recibido un código de barras en el tiempo de espera, 
    //      se considera que no se ha leído ningún código de barras (buffer = "-")
    while((buffer == BUFFER_EMPTY) && !isTimeoutExceeded(startTime, TIME_TO_READ_BARCODE))
    {
        // ----- PRODUCTO DETECTADO ------------------------
        if (hayMsgFromBR())  // Si se ha recibido un mensaje del lector de códigos de barras
        {
            // Leer buffer, todo su contenido. Luego se procesará el mensaje para obtener el código de barras
            // por si tiene basura o mensajes anteriores
            readMsgFromSerialBR(buffer); // Leer mensaje del puerto serie y guardarlo en 'buffer'
            #if defined(SM_DEBUG)
                SerialPC.print("\nBuffer BR leido: "); SerialPC.println(buffer);
            #endif
        }
        // --------------------------------------------------

        // ----- CANCELAR LECTURA ---------------------------
        // Si el usuario pulsa cualquier botón, se cancela la lectura del código de barras
        if (hayMsgFromDue())  // Si se ha recibido un mensaje del Due
        {
            String msgFromDue;
            readMsgFromSerialDue(msgFromDue); // Leer mensaje del puerto serie y guardarlo en msgFromDue
            #if defined(SM_DEBUG)
                SerialPC.print("\nMensaje del Due leido: "); SerialPC.println(msgFromDue);
            #endif

            // Si el mensaje es "CANCEL-BARCODE", se cancela la lectura del código de barras
            //      Si se recibe algo del Due, no debería ser nada distinto a "CANCEL-BARCODE", pero hacemos
            //      una comprobación por si acaso
            if(msgFromDue == "CANCEL-BARCODE")  buffer = msgFromDue; // Sale del while y de la función
        }
        // --------------------------------------------------
    }

}




#endif