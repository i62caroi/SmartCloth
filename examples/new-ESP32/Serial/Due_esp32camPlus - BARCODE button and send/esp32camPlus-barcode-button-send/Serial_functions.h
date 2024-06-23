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

// ESP32-CAM
//#define RXD1 14
//#define TXD1 15
// ------------------------------


// ----- ESP32 <-- BR -----------
// BR: Barcode Reader
#include <SoftwareSerial.h>

#define RXD2 19 // Tx (naranja lector)
#define TXD2 21 // Rx (azul lector)

SoftwareSerial mySerial(RXD2, TXD2); // RX, TX
#define SerialBR mySerial // Renombrar mySerial
// ------------------------------


#include "wifi_functions.h"
void          getFoodData(String barcode);
inline bool   hayConexionWiFi();


/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-----------------------------------------------------------------------------*/
void            setupAllSerial();                             // Inicializar comunicación serie con PC, Due y lector de código de barras
void            ackDue();                                     // Indicar que la comunicación está establecida
inline bool     isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout); // Comprobar si se ha excedido el tiempo de espera

// Comunicación Serial ESP32-Due
inline void     sendMsgToDue(const String &msg);              // Enviar 'msg' del ESP32 al Due
inline bool     hayMsgFromDue();                              // Comprobar si hay mensajes del Due disponibles
inline void     readMsgFromSerialDue(String &msgFromDue);     // Leer mensaje del puerto serie ESP32-Due

// Comunicación Serial ESP32-BR
inline bool     hayMsgFromBR();                               // Comprobar si hay mensajes del BR (Barcode Reader) disponibles (se ha leído código)
inline void     readMsgFromSerialBR(String &msgFromBR);       // Leer mensaje del puerto serie ESP32-BR (leer el código de barras)

void            tryGetBarcode();    // Si hay WiFi, intentar obtener el código de barras 
void            getBarcode();       // Leer código de barras 
/*-----------------------------------------------------------------------------*/



/*------------------------------------------------------------------------------
                           DEFINICIÓN FUNCIONES
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

    // ----- ESP32 - BR ------
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

    SerialPC.println(F("Comunicación con Due establecida"));
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
 * @brief Comprueba si hay mensajes del BR (Barcode Reader) disponibles en el puerto serial.
 *          Se considera que hay mensajes disponibles si se ha leído un código de barras.
 * @return true si hay mensajes disponibles, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool hayMsgFromBR() { return SerialBR.available() > 0; }



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
 * @brief Intenta obtener el código de barras y obtener su información de OpenFoodFacts.
 * 
 * Si hay una conexión WiFi disponible, se intentará leer el código de barras y obtener su información de OpenFoodFacts.
 * Si no hay conexión WiFi, se mostrará un mensaje de error y se enviará un mensaje al dispositivo Due indicando la falta de conexión.
 */
/*-----------------------------------------------------------------------------*/
void tryGetBarcode()
{
    if(hayConexionWiFi())
    {
        SerialPC.println("Leyendo codigo de barras...");
        getBarcode(); // Lee barcode con BR y obtiene su info de OpenFoodFacts
    }
    else  
    {
        SerialPC.println("No hay conexión a Internet. No se puede buscar la info del producto.");
        sendMsgToDue(F("NO-WIFI"));
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Obtiene el códido de barras leído por el lector BR (Barcode Reader) y 
 *          envía la información del producto al Due.
 *
 *      Si no se lee ningún código de barras en un tiempo determinado, se envía un 
 *      mensaje de error al Due.
 * 
 * @note Esta función asume que hay una comunicación establecida con el lector de código de barras y el dispositivo Due.
 */
/*-----------------------------------------------------------------------------*/
void getBarcode()
{
    String barcode = "-"; // Cadena inicialmente "vacía"

    unsigned long startMillis = millis();
    unsigned long timeToRead = 10000; // 10 segundos para leer el código de barras

    // A veces, al leer el Serial se lee el código de barras anterior. Limpiar el buffer podría
    // acabar eliminando el código de barras que se está leyendo ahora, pero eso es mejor que enviar
    // datos extraños en la petición a OpenFoodFacts. Si se borra sin querer lo leído ahora, que
    // el usuario lo vuelva a escanear.
    SerialBR.read(); // Lee y descarta cualquier dato pendiente en el buffer    

    while((barcode == "-") && !isTimeoutExceeded(startMillis, timeToRead))  // Si aún no se ha leído y no han pasado 5 segundos
    {
        if (hayMsgFromBR())  // El BR leyó algo
        {
            readMsgFromSerialBR(barcode); // Enviar barcode al Due
            SerialPC.print("\nBarcode leido: "); SerialPC.println(barcode);
        }
    }


    // --- OBTENER INFO DEL PRODUCTO ---
    if(barcode != "-")  // Si se ha leído barcode
    {
        //SerialPC.println("Obtieniendo información del producto...");

        // Avisar al Due de que se ha leído el barcode
        String msgToDue = "BARCODE:" + barcode; 
        sendMsgToDue(msgToDue);
        //SerialPC.println(msgToDue);
        delay(100);


        getFoodData(barcode); // Llama a getFoodData() con el código de barras leído
    } 
    else // Si han pasado 5 segundos sin leer barcode (no se ve bien o el lector no responde)
    {
        SerialPC.println("\nNo se ha leido ningún código de barras");
        sendMsgToDue(F("NO-BARCODE"));
    }
    // ---------------------------------

}





#endif