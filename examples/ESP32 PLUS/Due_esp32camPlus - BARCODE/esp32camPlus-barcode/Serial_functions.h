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
#define SerialPC Serial 
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


#define SERIAL_DUE 1
#define SERIAL_BR  2



/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-------------------------------------------------------------------------------
 Funciones para manejar la comunicación Serial del ESP32 con el Due y el lector
-------------------------------------------------------------------------------*/
void           setupAllSerial();                                                // Inicializar comunicación serie con PC, Due y lector de código de barras

// ----- Funciones auxiliares -----
// Comunicación Serial ESP32-PC
inline bool     hayMsgFromPC(){ return SerialPC.available() > 0; };         // Comprobar si hay mensajes del PC (escritos en terminal)
inline void     readMsgFromSerialPC(String &msgFromPC);                     // Leer mensaje del puerto serie ESP32-PC 

// Comunicación Serial ESP32-Due
inline bool    hayMsgFromDue() { return SerialDue.available() > 0; }                        // Comprobar si hay mensajes del Due disponibles
inline bool    isDueSerialEmpty(){ return !hayMsgFromDue(); }                               // Comprobar que el Serial del Due está vacío (no hay mensajes)
inline void    readMsgFromSerialDue(String &msgFromDue);                                    // Leer mensaje del puerto serie ESP32-Due
void           waitMsgFromDue(String &msgFromESP32, unsigned long &timeout);                // Esperar mensaje del Due durante un tiempo determinado
inline void    sendMsgToDue(const String &msg);                                             // Enviar mensaje al ESP32

// Comunicación Serial ESP32-BR
inline bool     hayMsgFromBR(){ return SerialBR.available() > 0; };                         // Comprobar si hay mensajes del BR (Barcode Reader) disponibles (se ha leído código)
inline void     readMsgFromSerialBR(String &msgFromBR);    
void            waitForBarcode_noProcessingCharacter(String &buffer);                                 // Leer mensaje del puerto serie ESP32-BR (leer el código de barras)
void            waitForBarcode(String &buffer);                                             // Esperar a que se lea un código de barras

inline bool    isTimeoutExceeded(unsigned long startTime, unsigned long timeout){ return millis() - startTime > timeout; }; // Comprobar si se ha excedido el tiempo de espera
bool           processCharacter(String &tempBuffer, String &msg, byte serialPort); // Procesa buffer del Serial (Due o BR) caracter a caracter hasta completar mensaje con "\n"
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
inline void readMsgFromSerialPC(String &msgFromPC) { 
    msgFromPC = SerialPC.readStringUntil('\n');
    msgFromPC.trim(); // Elimina espacios en blanco al principio y al final
}



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
 * @brief Espera un mensaje desde el Due dentro de un tiempo de espera especificado.
 *
 * Esta función espera a recibir un mensaje desde el Due durante un tiempo máximo
 * especificado por el parámetro `timeout`. Si se recibe un mensaje completo antes
 * de que se agote el tiempo, se almacena en `msgFromDue` y la función retorna.
 * Si el tiempo de espera se agota sin recibir un mensaje completo, se asigna
 * "TIMEOUT-DUE" a `msgFromDue`.
 *
 * @param msgFromDue Referencia a una cadena donde se almacenará el mensaje recibido.
 * @param timeout Referencia a un valor que especifica el tiempo máximo de espera en milisegundos.
 * 
 * @note Procesa el mensaje recibido caracter a caracter para asegurar que se recibe un mensaje completo.
 * @note Hasta ahora esta función ha leído perfectamente los mensajes del Due sin procesar caracter a caracter,
 *       pero por seguir la lógica de waitResponseFromESP32() y por si en algún momento se complican las comunicaciones
 *       o aumenta el tamaño de los mensajes enviados por el Due, se va a utilizar un buffer temporal al que se añadan
 *       los caracteres uno a uno en lugar de salirse en cuanto se detecta un solo caracter, pudiendo ser un simple \n,
 *       lo que hacía que se creyera haber recibido un mensaje vacío "".
 */
/*---------------------------------------------------------------------------------------------------------*/
void waitMsgFromDue(String &msgFromDue, unsigned long &timeout) 
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual
    String tempBuffer = "";  // Buffer temporal para ensamblar el mensaje

    // Esperar 'timeout' segundos a que el Due responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromDue())  // Si el Due ha respondido
        {
            if (processCharacter(tempBuffer, msgFromDue, SERIAL_DUE)) 
                return;  // Sale cuando se ha procesado un mensaje completo
        }
        delay(50);  // Evita que el bucle sea demasiado intensivo
    }

    // Si se alcanza el tiempo de espera sin recibir un mensaje
    SerialPC.println(F("TIMEOUT-DUE. No se ha recibido respuesta del Due"));

    msgFromDue = "TIMEOUT-DUE";  // Marca el mensaje como TIMEOUT si no se recibió nada útil
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un mensaje al Due a través del puerto serie.
 *
 * Esta función limpia el buffer del Due, envía el mensaje especificado
 * y añade un pequeño retraso para asegurar que el Due tenga tiempo de leer el mensaje.
 *
 * @param msg El mensaje que se enviará al Due.
 */
/*-----------------------------------------------------------------------------*/
inline void sendMsgToDue(const String &msg)
{ 
      //limpiarBufferDue();     // Limpiar buffer con Due
      SerialDue.println(msg); // Enviar 'msg' del ESP32 al Due
      delay(50);              // Pequeño retraso para asegurar que el Due tenga tiempo de leer el mensaje
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
 * @brief Espera a que se lea un código de barras o se cancele la lectura.
 * 
 * Esta función espera hasta 30 segundos para recibir un código de barras. Si no se recibe
 * ningún código de barras en ese tiempo, se considera que no se ha leído ningún código de barras
 * y el buffer se establece en "-". 
 * 
 * No se hace el procesamiento caracter a caracter como se hace en waitResponseFromESP32() en el Due
 * porque, en este caso, si se hace ese procesamiento no se detecta el código.
 * 
 * @param buffer Referencia a un String donde se almacenará el código de barras leído o el mensaje de cancelación.
 */
/*-----------------------------------------------------------------------------*/
void waitForBarcode_noProcessingCharacter(String &buffer) 
{
    unsigned long startTime = millis();

    // Esperar 30 segundos a que se lea un código de barras. Sale si se recibe mensaje o si se pasa el tiempo de espera
    //      Si no se ha recibido un código de barras en el tiempo de espera, 
    //      se considera que no se ha leído ningún código de barras (buffer = "-")
    while((buffer == BUFFER_EMPTY) && !isTimeoutExceeded(startTime, TIME_TO_READ_BARCODE))
    {
        // ----- PRODUCTO DETECTADO ------------------------
        if (hayMsgFromBR())   // Si se ha recibido un mensaje del lector de códigos de barras
        {
            readMsgFromSerialBR(buffer); 
            SerialPC.print("Leido del BR: |" + buffer); SerialPC.println("|");
            return;  // Sale cuando se ha procesado un mensaje completo
        }
        // --------------------------------------------------

        // CANCELACIÓN: Para este programa de ejemplo vamos a ignorar la cancelación de la lectura de barcode

        delay(50);
    }

    SerialPC.println("\n\nTIMEOUT de lectura barcode\n");
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Espera a que se lea un código de barras o se cancele la lectura.
 * 
 * Esta función espera hasta 30 segundos para recibir un código de barras. Si no se recibe
 * ningún código de barras en ese tiempo, se considera que no se ha leído ningún código de barras
 * y el buffer se establece en "-". 
 * 
 * Utiliza un buffer temporal para procesar al completo el mensaje recibido del lector (processCharacter()), 
 * aunque esto no solía fallar, pero por seguir la lógica de waitResponseFromESP32().
 * 
 * @param buffer Referencia a un String donde se almacenará el código de barras leído o el mensaje de cancelación.
 */
/*-----------------------------------------------------------------------------*/
void waitForBarcode(String &buffer) 
{
    unsigned long startTime = millis();
    String tempBufferBR = "";  // Buffer temporal para ensamblar el mensaje del BR

    // Esperar 30 segundos a que se lea un código de barras. Sale si se recibe mensaje o si se pasa el tiempo de espera
    //      Si no se ha recibido un código de barras en el tiempo de espera, 
    //      se considera que no se ha leído ningún código de barras (buffer = "-")
    while((buffer == BUFFER_EMPTY) && !isTimeoutExceeded(startTime, TIME_TO_READ_BARCODE))
    {
        // ----- PRODUCTO DETECTADO ------------------------
        if (hayMsgFromBR())   // Si se ha recibido un mensaje del lector de códigos de barras
        {
            if (processCharacter(tempBufferBR, buffer, SERIAL_BR)) 
            {
                SerialPC.println("\nLeido del BR: " + buffer);
                return;  // Sale cuando se ha procesado un mensaje completo
            }
            SerialPC.print("Detectado en BR: |" + buffer); SerialPC.println("|");
        }
        // --------------------------------------------------

        // CANCELACIÓN: Para este programa de ejemplo vamos a ignorar la cancelación de la lectura de barcode

        delay(50);
    }

    SerialPC.println("\n\nTIMEOUT de lectura barcode\n");
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Procesa un carácter recibido por el puerto serial y construye un mensaje completo.
 *
 * Esta función lee un carácter del puerto serial especificado y lo acumula en un buffer temporal.
 * Si se recibe un carácter de nueva línea ('\n'), se considera que el mensaje está completo y se 
 * procesa el contenido del buffer temporal.
 *
 * @param tempBuffer Referencia al buffer temporal donde se acumulan los caracteres recibidos.
 * @param msg Referencia a la cadena donde se almacenará el mensaje completo procesado.
 * @param serialPort Puerto serial desde el cual se lee el carácter. Puede ser SERIAL_DUE o SERIAL_BR.
 * @return true si se ha recibido y procesado un mensaje completo, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool processCharacter(String &tempBuffer, String &msg, byte serialPort) 
{
    char c;
    if (serialPort == SERIAL_DUE) c = SerialDue.read();  // Lee un carácter del serial del Due
    else c = SerialBR.read();  // Lee un carácter del serial del lector BR

    if (c == '\n') {
        tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

        if (tempBuffer.length() > 0) {  // Solo procesa si no está vacío
            msg = tempBuffer;
            if (serialPort == SERIAL_DUE) SerialPC.println("\nRespuesta del Due: " + msg);
            else SerialPC.println("\nBuffer BR leido: " + msg);
            return true;  // Mensaje completo procesado
        }
        tempBuffer = "";  // Si estaba vacío, se resetea el buffer
    } 
    else {
        tempBuffer += c;  // Acumula el carácter en el buffer temporal
    }
    return false;  // Aún no se ha recibido un mensaje completo
}


#endif