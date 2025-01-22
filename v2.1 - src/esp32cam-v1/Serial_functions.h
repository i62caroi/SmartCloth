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

#define TIMEOUT_MSG_DUE 3000L // Tiempo máximo de espera de mensaje del Due (3 segundos)
// ------------------------------


// ----- ESP32 <-- BR -----------
// BR: Barcode Reader
#include <SoftwareSerial.h>

#define RXD2 19 // Tx (naranja lector)
#define TXD2 21 // Rx (azul lector)

SoftwareSerial mySerial(RXD2, TXD2); // RX, TX
#define SerialBR mySerial // Renombrar mySerial

#define TIME_TO_READ_BARCODE 30000L // 30 segundos para leer el código de barras
#define BR_BUFFER_EMPTY "-" // Buffer del BR vacío
// ------------------------------



// -------- MENSAJES DEL DUE ----------
#define NUM_EXACT_DUE_MESSAGES      7   // CHECK-WIFI, SAVE, INICIO-COMIDA, INICIO-PLATO, FIN-TRANSMISION, GET-BARCODE, CANCEL-BARCODE
#define NUM_PREFIX_DUE_MESSAGES     3   // ALIMENTO, FIN-COMIDA, GET-PRODUCT

// Mensajes exactos esperados del Due
// Se usa const char* en lugar de String para ahorrar memoria. Así que al comparar con estos mensajes, se debe hacer con equals() y no con ==
const char* EXACT_DUE_MESSAGES[] = 
{
    "CHECK-WIFI",       // Comprobar conexión WiFi
    "SAVE",             // Indicar que se va a enviar data del TXT
    "INICIO-COMIDA",    // Iniciar nueva comida en JSON
    "INICIO-PLATO",     // Iniciar nuevo plato en JSON
    "FIN-TRANSMISION",  // Finalizar transmisión de datos
    "GET-BARCODE",      // Leer código de barras
    "CANCEL-BARCODE"    // Cancelar lectura de código de barras
};

// Prefijos de mensajes esperados del Due
const char* PREFIX_DUE_MESSAGES[] = 
{
    "ALIMENTO,",        // Línea de alimento: "ALIMENTO,<grupo>,<peso>" o "ALIMENTO,<grupo>,<peso>,<ean>"
    "FIN-COMIDA,",      // Fin de comida con fecha y hora: "FIN-COMIDA,<fecha>,<hora>"
    "GET-PRODUCT:"      // Buscar producto: "GET-PRODUCT:<barcode>"
};

// ------------------------------------


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
//void           processMsgFromDue(String &msgFromDue);                                       // Procesar mensaje del Due caracter a caracter
void           waitMsgFromDue(String &msgFromESP32, const unsigned long &timeout);                // Esperar mensaje del Due durante un tiempo determinado
inline void    sendMsgToDue(const String &msg);                                             // Enviar mensaje al ESP32

// Comunicación Serial ESP32-BR
inline bool     hayMsgFromBR(){ return SerialBR.available() > 0; };                         // Comprobar si hay mensajes del BR (Barcode Reader) disponibles (se ha leído código)
inline void     readMsgFromSerialBR(String &msgFromBR);                                     // Leer mensaje del puerto serie ESP32-BR (leer el código de barras)
void            waitForBarcode(String &buffer);                                             // Esperar a que se lea un código de barras

inline bool    isTimeoutExceeded(unsigned long startTime, unsigned long timeout){ return millis() - startTime > timeout; }; // Comprobar si se ha excedido el tiempo de espera
bool           processCharacterInMsgFromDue(String &tempBuffer, String &msgFromDue);        // Procesa buffer del SerialDue caracter a caracter hasta completar mensaje con "\n"
bool           isValidDueMessage(const String &message);                                     // Comprobar si el mensaje del Due es válido
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


/*-----------------------------------------------------------------------------*/
/**
 * @brief Procesa los mensajes recibidos desde el dispositivo Due.
 * 
 * Utiliza un búfer temporal para almacenar los caracteres del mensaje y llama a la 
 * función processCharacterInMsgFromDue para procesar cada carácter. La función se 
 * detiene cuando se ha procesado un mensaje completo (lee un '\n').
 * 
 * @param msgFromDue Referencia a la cadena donde se almacenará el mensaje completo procesado.
 * 
 * @note Se usa en lugar de leer con readStringUntil('\n') para asegurar que se para a procesar
 * el mensaje completo y no se pierde nada.
 */
/*-----------------------------------------------------------------------------*/
/*void processMsgFromDue(String &msgFromDue)
{
    String tempBuffer = "";

    while(hayMsgFromDue())
    {
        if(processCharacterInMsgFromDue(tempBuffer, msgFromDue)) 
            return; // Sale cuando se ha procesado un mensaje completo
    }
}*/



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
void waitMsgFromDue(String &msgFromDue, const unsigned long &timeout) 
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual
    String tempBuffer = "";  // Buffer temporal para ensamblar el mensaje

    #if defined(SM_DEBUG)
        SerialPC.println(F(""));
    #endif

    // Esperar 'timeout' segundos a que el Due responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromDue())  // Si el Due ha respondido
        {
            if (processCharacterInMsgFromDue(tempBuffer, msgFromDue)) 
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
 * y el buffer se establece en "-". La función también puede salir si se recibe un mensaje de
 * cancelación de la lectura desde el Due.
 * 
 * Utiliza un buffer temporal para procesar al completo el mensaje recibido del lector (processCharacterInMsgFromDue()), 
 * aunque esto no solía fallar, pero por seguir la lógica de waitResponseFromESP32().
 * 
 * @param buffer Referencia a un String donde se almacenará el código de barras leído o el mensaje de cancelación.
 */
/*-----------------------------------------------------------------------------*/
void waitForBarcode(String &buffer) 
{
    unsigned long startTime = millis();
    String tempBufferDue = "";  // Buffer temporal para ensamblar el mensaje del Due

    // Esperar 30 segundos a que se lea un código de barras. Sale si se recibe mensaje o si se pasa el tiempo de espera
    //      Si no se ha recibido un código de barras en el tiempo de espera, 
    //      se considera que no se ha leído ningún código de barras (buffer = "-")
    while((buffer == BR_BUFFER_EMPTY) && !isTimeoutExceeded(startTime, TIME_TO_READ_BARCODE))
     {
        // ----- PRODUCTO DETECTADO ------------------------
        if (hayMsgFromBR())   // Si se ha recibido un mensaje del lector de códigos de barras
        {
            // El BR no añade '\n' al final del mensaje, así que no podemos procesar caracter a caracter hasta el '\n'.
            // Hasta ahora no ha habido ningún problema con la lectura del barcode usando readMsgFromSerialBR(),
            // así que vamos a asumir que no va a haber problemas y vamos a leer el mensaje directamente.
            readMsgFromSerialBR(buffer); 
            #ifdef SM_DEBUG
                SerialPC.print("Leido del BR: |" + buffer); SerialPC.println("|");
            #endif
            return;  // Sale cuando se ha procesado un mensaje completo
        }
        // --------------------------------------------------

        // ----- CANCELAR LECTURA ---------------------------
        if (hayMsgFromDue()) {  // Si se ha recibido un mensaje del Due
            String msgFromDue;
            if (processCharacterInMsgFromDue(tempBufferDue, msgFromDue)) {
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
 * @param msgFromDue Referencia a la cadena donde se almacenará el mensaje completo procesado.
 * @return true si se ha recibido y procesado un mensaje completo, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
/*bool processCharacterInMsgFromDue(String &tempBuffer, String &msgFromDue) 
{
    char c = SerialDue.read();  // Lee un carácter del serial del Due

    if (c == '\n') {
        tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

        if (tempBuffer.length() > 0) {  // Solo procesa si no está vacío
            msgFromDue = tempBuffer;
            #if defined(SM_DEBUG)
                SerialPC.println("\nMensaje del Due: " + msgFromDue);
            #endif
            return true;  // Mensaje completo procesado
        }
        tempBuffer = "";  // Si estaba vacío, se resetea el buffer
    } 
    else {
        tempBuffer += c;  // Acumula el carácter en el buffer temporal
    }
    return false;  // Aún no se ha recibido un mensaje completo
}*/
bool processCharacterInMsgFromDue(String &tempBuffer, String &msgFromDue) 
{
    char c = SerialDue.read();  // Lee un carácter del serial del ESP32

    #if defined(SM_DEBUG)
        SerialPC.print("Caracter leido: "); 
        if(c == '\n') SerialPC.println("'\\n'");
        else SerialPC.println(String(c));
    #endif

    if (c == '\n') 
    {
        tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

        if (tempBuffer.length() > 0 && isValidDueMessage(tempBuffer))   // Verificar que el mensaje es uno de los válidos antes de asignarlo
        {
            msgFromDue = tempBuffer; // Asigna el contenido del buffer temporal al mensaje del Due
            #if defined(SM_DEBUG)
                SerialPC.print("\n---> Mensaje completo del Due: "); 
                SerialPC.print("\"" + msgFromDue); 
                SerialPC.println("\"");
            #endif
            tempBuffer = "";  // Resetea el buffer temporal
            return true;  // Mensaje completo procesado
        }
        #if defined(SM_DEBUG)
            SerialPC.println("Mensaje incompleto o no reconocido: " + tempBuffer);
        #endif
        tempBuffer = "";  // Si estaba vacío o no era válido, se resetea el buffer
    } 
    else 
    {
        tempBuffer += c;  // Acumula el carácter en el buffer temporal
        #if defined(SM_DEBUG)
            SerialPC.println("Buffer actual: " + tempBuffer);
        #endif
    }
    return false;  // Aún no se ha recibido un mensaje completo válido
}




/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Verifica si lo recibido del Due es un mensaje válido o basura del Serial.
 *
 * Esta función comprueba si el mensaje recibido coincide con alguno de los mensajes
 * establecidos que debe enviar el ESP32 en cada situación.
 *
 * @param message El mensaje recibido que se va a verificar.
 * @return true si el mensaje es válido, false en caso contrario.
 */
/*---------------------------------------------------------------------------------------------------------*/
bool isValidDueMessage(const String &message) 
{
    // Verificar mensajes exactos
    for (int i = 0; i < NUM_EXACT_DUE_MESSAGES; i++) 
    {
         if (message.equals(EXACT_DUE_MESSAGES[i])) return true;
    }
    
    // Verificar prefijos
    for (int i = 0; i < NUM_PREFIX_DUE_MESSAGES; i++) 
    {
        if (message.startsWith(PREFIX_DUE_MESSAGES[i])) return true;
    }
    
    return false;
}


#endif