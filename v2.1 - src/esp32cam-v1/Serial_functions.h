/**
 * @file Serial_functions.h
 * @brief Este archivo contiene funciones para manejar las comunicaciones Serial entre el ESP32-Due y el ESP32-BarcodeReader, independientemente.
 * 
 * @author Irene Casares Rodríguez
 * @date 18/04/2024
 */


 /*
    --------------------------------------------------------
    Estrategias para asegurar la correcta lectura de datos:
    --------------------------------------------------------

        1.	Protocolo de comunicación con encabezado y longitud:
                Una técnica común es enviar un encabezado o marca de inicio junto con la longitud del mensaje. Esto te permite saber cuántos bytes debes esperar antes de leer el mensaje completo.
                Ejemplo de protocolo simple:
                    •	El ESP32 envía algo como: LEN:<longitud><mensaje>\n.
                    •	El Due primero lee la longitud (<longitud>) del mensaje, y entonces sabe cuántos bytes debe esperar para que el mensaje esté completo.
                Este enfoque permite a tu programa saber cuándo esperar más bytes antes de procesar el mensaje completo.

        2.	Verificar mensajes incompletos:
                Si recibes mensajes vacíos, podrías intentar implementar un sistema de retransmisión o un mecanismo que verifique si el mensaje recibido es válido (completo o no). Por ejemplo, 
                podrías enviar un código de confirmación desde el Due al ESP32 cuando se recibe el mensaje completo y correcto, de modo que si el ESP32 no recibe este código, pueda retransmitir el mensaje.
        
        3.	Buffer circular o cola de mensajes:
                Implementar un buffer circular o cola de mensajes en el ESP32 y el Due podría ayudar a manejar de manera más eficiente los datos recibidos, asegurando que se procesen todos los bytes en el 
                orden correcto, y se eviten problemas de sobresaturación.
        
        4.	Reintentos automáticos:
                Puedes agregar un contador de reintentos en el Due. Si el Due no recibe una respuesta adecuada en un tiempo razonable o recibe un mensaje vacío, podría enviar un comando de reintento al ESP32, 
                solicitando que reenvíe el último mensaje.
        
        5.	Esperar datos adicionales:
                Si lees un mensaje parcial o vacío, en lugar de descartar la lectura inmediatamente, puedes esperar un poco más (aumentar el tiempo de espera) para asegurarte de que se hayan recibido todos los bytes. 
                De esta manera, evitas procesar datos antes de que todos hayan llegado.
        
        6.	Monitorización de tiempo real (debugging):
                Como parte del proceso de depuración, asegúrate de que ambos dispositivos (Due y ESP32) están bien sincronizados en términos de tiempos de espera y respuesta. Puedes aumentar los tiempos de espera 
                temporalmente para ver si se reduce la pérdida de datos.
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
#ifdef SM_DEBUG
    inline bool     hayMsgFromPC(){ return SerialPC.available() > 0; };         // Comprobar si hay mensajes del PC (escritos en terminal)
    inline void     readMsgFromSerialPC(String &msgFromPC);                     // Leer mensaje del puerto serie ESP32-PC 
#endif

// Comunicación Serial ESP32-Due
inline bool    hayMsgFromDue() { return SerialDue.available() > 0; }                        // Comprobar si hay mensajes del Due disponibles
inline bool    isDueSerialEmpty(){ return !hayMsgFromDue(); }                               // Comprobar que el Serial del Due está vacío (no hay mensajes)
inline void    readMsgFromSerialDue(String &msgFromDue);                                    // Leer mensaje del puerto serie ESP32-Due
void           waitMsgFromDue(String &msgFromESP32, unsigned long &timeout);                // Esperar mensaje del Due durante un tiempo determinado
//inline void    limpiarBufferDue();                                                          // Limpiar buffer con el ESP32
inline void    sendMsgToDue(const String &msg);                                             // Enviar mensaje al ESP32

// Comunicación Serial ESP32-BR
inline bool     hayMsgFromBR(){ return SerialBR.available() > 0; };                         // Comprobar si hay mensajes del BR (Barcode Reader) disponibles (se ha leído código)
//inline void     limpiarBufferBR(){ while(SerialBR.available() > 0) { SerialBR.read(); } };  // Limpiar buffer del lector de códigos de barras
inline void     readMsgFromSerialBR(String &msgFromBR);                                     // Leer mensaje del puerto serie ESP32-BR (leer el código de barras)
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
/*void waitMsgFromDue(String &msgFromDue, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el Due responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(!hayMsgFromDue() && !isTimeoutExceeded(startTime, timeout));

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromDue())  // Si el Due ha respondido
    {
        readMsgFromSerialDue(msgFromDue); // Leer mensaje del puerto serie y guardarlo en msgFromDue
        #if defined(SM_DEBUG)
            SerialPC.println("\nRespuesta del Due: " + msgFromDue);  
        #endif
    } 
    else // No se ha recibido respuesta del Due
    {
        msgFromDue = "TIMEOUT-DUE";;
    }

}*/

// Hasta ahora esta función ha leído perfectamente los mensajes del Due, pero por seguir la lógica de waitResponseFromESP32()
// y por si en algún momento se complican las comunicaciones o aumenta el tamaño de los mensajes enviados por el Due,
// vamos a utilizar un buffer temporal al que se añadan los caracteres uno a uno en lugar de salirse en cuanto se detecta un solo caracter, 
// pudiendo ser un simple \n, lo que hacía que se creyera haber recibido un mensaje vacío "".
/*void waitMsgFromDue(String &msgFromDue, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual
    String tempBuffer = "";  // Buffer temporal para ensamblar el mensaje

    // Esperar 'timeout' segundos a que el Due responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromDue())  // Si el Due ha respondido
        {
            char c = SerialDue.read();  // Lee un carácter del serial
            if (c == '\n') 
            {
                tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

                if (tempBuffer.length() > 0)  // Solo procesa si no está vacío
                {
                    msgFromDue = tempBuffer;
                    #if defined(SM_DEBUG)
                        SerialPC.println("\nRespuesta del Due: " + msgFromDue);  
                    #endif
                    return;  // Sale de la función con un mensaje válido
                }
                tempBuffer = "";  // Si estaba vacío, se resetea el buffer
            }
            else 
            {
                tempBuffer += c;  // Acumula el carácter en el buffer temporal
            }
        }

        delay(50);  // Evita que el bucle sea demasiado intensivo

    }

    // Si se alcanza el tiempo de espera sin recibir un mensaje
    #if defined(SM_DEBUG)
        SerialPC.println(F("TIMEOUT-DUE. No se ha recibido respuesta del ESP32"));
    #endif

    msgFromESP32 = "TIMEOUT-DUE";  // Marca el mensaje como TIMEOUT si no se recibió nada útil

}*/


// Misma versión que la anterior pero encapsulando la parte de procesar caracter a caracter
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
    #if defined(SM_DEBUG)
        SerialPC.println(F("TIMEOUT-DUE. No se ha recibido respuesta del Due"));
    #endif

    msgFromDue = "TIMEOUT-DUE";  // Marca el mensaje como TIMEOUT si no se recibió nada útil
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Limpia el buffer de datos del Due.
 *
 * Esta función vacía el buffer de entrada del Due leyendo todos los datos disponibles.
 * Luego, introduce un pequeño retraso para asegurar que el Due tenga tiempo de procesar
 * y leer cualquier mensaje que se envíe después.
 */
/*-----------------------------------------------------------------------------*/
/*inline void limpiarBufferDue()
{ 
  while(SerialDue.available() > 0) { SerialDue.read(); } // Limpiar buffer con el Due
  delay(100);  // Pequeño retraso para asegurar que el Due tenga tiempo de leer el mensaje que se envíe después    
}*/



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
/*void waitForBarcode(String &buffer)
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

        delay(50);
    }

}*/


// Hasta ahora esta función ha leído perfectamente los mensajes del Due, pero por seguir la lógica de waitResponseFromESP32()
// y por si en algún momento se complican las comunicaciones o aumenta el tamaño de los mensajes enviados por el Due,
// vamos a utilizar un buffer temporal al que se añadan los caracteres uno a uno en lugar de salirse en cuanto se detecta un solo caracter, 
// pudiendo ser un simple \n, lo que hacía que se creyera haber recibido un mensaje vacío "".
// La funcion processCharacter() encapsula la parte de procesar caracter a caracter
void waitForBarcode(String &buffer) 
{
    unsigned long startTime = millis();
    String tempBufferBR = "";  // Buffer temporal para ensamblar el mensaje del BR
    String tempBufferDue = "";  // Buffer temporal para ensamblar el mensaje del Due

    // Esperar 30 segundos a que se lea un código de barras. Sale si se recibe mensaje o si se pasa el tiempo de espera
    //      Si no se ha recibido un código de barras en el tiempo de espera, 
    //      se considera que no se ha leído ningún código de barras (buffer = "-")
    while((buffer == BUFFER_EMPTY) && !isTimeoutExceeded(startTime, TIME_TO_READ_BARCODE))
     {
        // ----- PRODUCTO DETECTADO ------------------------
        if (hayMsgFromBR()) {  // Si se ha recibido un mensaje del lector de códigos de barras
            if (processCharacter(tempBufferBR, buffer, SERIAL_BR)) 
                return;  // Sale cuando se ha procesado un mensaje completo
        }
        // --------------------------------------------------

        // ----- CANCELAR LECTURA ---------------------------
        if (hayMsgFromDue()) {  // Si se ha recibido un mensaje del Due
            String msgFromDue;
            if (processCharacter(tempBufferDue, msgFromDue, SERIAL_DUE)) {
                if (msgFromDue == "CANCEL-BARCODE") {
                    buffer = msgFromDue;  // Sale del while y de la función
                    return;
                }
            }
        }
        delay(50);
    }
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
            #if defined(SM_DEBUG)
                if (serialPort == SERIAL_DUE) SerialPC.println("\nRespuesta del Due: " + msg);
                else SerialPC.println("\nBuffer BR leido: " + msg);
            #endif
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