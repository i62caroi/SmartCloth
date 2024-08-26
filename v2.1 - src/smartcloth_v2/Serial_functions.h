
/** 
 * @file Serial_esp32cam.h
 * @brief Comunicación Serial con ESP32-CAM
 *
 * @author Irene Casares Rodríguez
 * @date 20/05/24
 * @version 1.0
 *
 * Se ha utilizado el módulo ESP32-CAM PLUS:
 *    SPI Flash: default 32Mbit
 *    RAM: built-in 520 KB+external 4MPSRAM
 *    Bluetooth: Bluetooth 4.2 BR/EDR and BLE standards 
 *    Wi-Fi: 802.11 b/g/n de hasta 150 Mbps
 *    Support Interface: UART, SPI, I2C, PWM
 *    Camera: OV5640 autofocus
 *    Image Output Format: JXGA
 *    Power supply range: 5V
 *
 * @see Aliexpress: https://acortar.link/Aubtwq
 *
 */


/*
  --------- CONEXIÓN ESP32-CAM CON ARDUINO DUE -----------

  ESP32-CAM conectado por Serial (UART) de la siguiente forma:  
     
      --------------------------------------------
      |    ESP32-CAM   |  Arduino Due (Serial1)  |
      --------------------------------------------
      |    IO15 (Tx1)  |      pin 19 (Rx1)       |    
      |    IO14 (Rx1)  |      pin 18 (Tx1)       |
      |      3V3       |        3V3              |
      |      GND       |        GND              | 
      --------------------------------------------

-----------------------------------------------------
-----------------------------------------------------

 ESP32-CAM PLUS conectado por Serial (UART) de la siguiente forma:  

      ------------------------------------------------------
      |    ESP32-CAM PLUS        |  Arduino Due (Serial1)  |
      ------------------------------------------------------
      |    IO17 (Tx1) (4º izq)   |      pin 19 (Rx1)       |    
      |    IO16 (Rx1) (5º izq)   |      pin 18 (Tx1)       |
      |      3V3                 |        3V3              |
      |      GND                 |        GND              | 
      ------------------------------------------------------

-----------------------------------------------------


    -------- MENSAJES ARDUINO -> ESP32 --------------
        ----- COMPROBAR COMUNICACIÓN --
        1) Comprobar conexión:
            "PING"

        ----- CONEXIÓN A INTERNET -----
        2) Check wifi:
            "CHECK-WIFI"

        ----- INFO COMIDAS ------------
        3) Guardar info en base de datos al inicio:
            3.1. Indicar guardado:
                "SAVE"
            3.2. Mandar datos a guardar, línea a línea:
                "INICIO-COMIDA" "INICIO-PLATO" "ALIMENTO,9,54.35" ... --> sendFileToEsp32MealByMeal()

        ----- BARCODE ----------------
        4) Leer código de barras:
            "GET-BARCODE"



    -------- MENSAJES ESP32 -> ARDUINO -------------- 
        ----- COMPROBAR COMUNICACIÓN --
        1) Comprobación de conexión:
            "PONG"

        ----- CONEXIÓN A INTERNET -----
        2) Hay wifi:
            "WIFI-OK"

        3) No hay wifi:
            "NO-WIFI"
        
        ----- INFO COMIDAS ------------
        4) Esperando datos a subir:
            "WAITING-FOR-DATA"

        5) JSON guardado correctamente:
            "SAVED-OK"

        6) Error en el guardado de la comida (petición HTTP POST):
            "ERROR-HTTP:<codigo_error>"

        ----- BARCODE ----------------
            ----- LEER BARCODE -----
            7) Código de barras leído. Buscando información del producto:
                "BARCODE:<barcode>"
        
            8) No se ha podido leer el código de barras (no se ve bien o el lector no responde):
                "NO-BARCODE"
    
            ----- BUSCAR PRODUCTO -----
            9) Información del producto:
                "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
            
            10) No se ha encontrado el producto en OpenFoodFacts (error HTTP 404 not found):
                "NO-PRODUCT"

            11) Error al buscar producto (diferente a no encontrado):
                "ERROR-HTTP:<codigo_error>"

            12) El servidor de OpenFoodFacts no responde:
                "PRODUCT-TIMEOUT"


*/

#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H

#include "Comida.h" // comidaActual
#include "SD_functions.h"
#include "debug.h" // SM_DEBUG --> Comunicación Serial con PC

void    checkBascula();           // Está en Scale.h, pero hay que declararla aquí también para que esté en este ámbito
bool    interruptionOccurred();   // Está en ISR.h, pero hay que declararla aquí también para que esté en este ámbito
#include "Scale.h" // checkBascula()
#include "ISR.h" // interruptionOccurred()
#define SerialESP32 Serial1 // Comunicación Serial con ESP32


// --- RESULTADOS DE SUBIR INFO A DATABASE --- 
// Usadas en:
//      prepareSaving()                 -->     WAITING_FOR_DATA, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT y UNKNOWN_ERROR
//      sendTXTFileToESP32()            -->     ALL_MEALS_UPLOADED, MEALS_LEFT y ERROR_READING_TXT
//      saveComidaInDatabase_or_TXT()   -->     MEAL_UPLOADED, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT y UNKNOWN_ERROR
//      showDataToUpload()              -->     UPLOADING_DATA, ALL_MEALS_UPLOADED, MEALS_LEFT, ERROR_READING_TXT, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT y UNKNOWN_ERROR
#define  WAITING_FOR_DATA        1
#define  UPLOADING_DATA          2
#define  ALL_MEALS_UPLOADED      3 // Comidas guardadas en sincronización inicial
#define  MEAL_UPLOADED           3 // Comida guardada al pulsar "Guardar comida"
#define  MEALS_LEFT              4
#define  ERROR_READING_TXT       5
#define  NO_INTERNET_CONNECTION  6
#define  HTTP_ERROR              7
#define  TIMEOUT                 8
#define  UNKNOWN_ERROR           9


// --- RESULTADOS DE LEER BARCODE ---
// Barcode leido o no:
#define BARCODE_READ            0
#define BARCODE_NOT_READ        1
// Conexion con ESP32:
//#define NO_ESP32_CONNECTION     2
// Product info found or not:
#define PRODUCT_FOUND           3
#define PRODUCT_NOT_FOUND       4
#define PRODUCT_TIMEOUT         5
#define INTERRUPTION            6

//bool isESP32Connected = false; // Flag para indicar si la comunicación con el ESP32 está establecida


void showDataToUpload(byte option);


/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
// Funciones para manejar la comunicación Serial con el ESP32
void            setupSerialPC();                                    // Configurar comunicación Serial con PC
void            setupSerialESP32();                                 // Configurar comunicación Serial con ESP32
//void            pingESP32();                                        // Comprobar conexión con ESP32

inline void     sendMsgToESP32(const String &msg);                  // Enviar 'msg' del Due al ESP32
inline bool     hayMsgFromESP32();                                  // Comprobar si hay mensajes del ESP32 disponibles
inline bool     isESP32SerialEmpty(){ return !hayMsgFromESP32(); }  // Comprobar si no hay mensajes del ESP32 disponibles
inline void     readMsgFromSerialESP32(String &msgFromESP32);       // Leer mensaje del puerto serie Due-ESP32 y guardarlo en msgFromESP32

void            waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout);            // Espera la respuesta del ESP32, sea cual sea, y la devuelve en msgFromESP32.
void            waitResponseFromESP32WithEvents(String &msgFromESP32, unsigned long &timeout);  // Espera la respuesta del ESP32 y la devuelve. Atiende a eventos
inline bool     isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout);            // Comprobar si se ha excedido el tiempo de espera
// -------------

bool    checkWifiConnection();                                      // Comprobar conexion a internet
byte    prepareSaving();                                            // Indica al ESP32 que se le va a enviar info y espera su respuesta WAITING-FOR-DATA
void    getBarcodeAndProductInfo();                                 // Leer código de barras y buscar información del producto
byte    askForBarcode(String &barcode);                             // Obtener el código de barras
byte    getProductInfo(String &barcode, String &productInfo);       // Obtener la información del producto a partir de un código de barras
/******************************************************************************/
/******************************************************************************/




/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/


bool eventOccurredReadingBarcode(){
    checkBascula();     // Comprueba interrupción de báscula. Lo necesito para ver si hace falta marcar evento
    if(interruptionOccurred()) return true; // Si ha habido interrupción en botoneras (pulsación) o evento en báscula (cambio de peso, no solo interrupción)
    return false;
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la comunicación serial con la PC.
 * 
 * Esta función inicializa la comunicación serial con la PC a una velocidad de 115200 baudios.
 * 
 * @note Esta función debe ser llamada antes de utilizar cualquier función de comunicación serial con la PC.
 */
/*-----------------------------------------------------------------------------*/
#if defined(SM_DEBUG)
void setupSerialPC() 
{
    // Inicializar comunicación con PC (Serial)
    SerialPC.begin(115200);
    delay(100);
    SerialPC.println(F("\nIniciando comunicacion Serial con PC..."));
}
#endif // SM_DEBUG



/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la comunicación serial con el ESP32.
 * 
 * Esta función inicializa la comunicación serial con el ESP32 utilizando el puerto Serial1.
 * Se establece una velocidad de transmisión de 115200 baudios.
 * 
 * @note Esta función debe ser llamada antes de utilizar cualquier función de comunicación con el ESP32.
 */
/*-----------------------------------------------------------------------------*/
void setupSerialESP32() 
{    
    // Inicializar comunicación con ESP32 (Serial1)
    SerialESP32.begin(115200); 
    delay(100);
}


/*-----------------------------------------------------------------------------*/
/**
 * Comprueba la conexión Serial con el ESP32.
 * 
 * Esta función envía un mensaje al ESP32 y espera una respuesta. 
 * Si la respuesta es "PONG", significa que la comunicación con el ESP32 se ha establecido correctamente.
 * Si la respuesta es "TIMEOUT", significa que no se ha recibido respuesta del ESP32.
 * Si la respuesta es cualquier otro mensaje, se considera desconocido y se establece la comunicación como no exitosa.
 */
/*-----------------------------------------------------------------------------*/
/*void pingESP32()
{
    // ---- COMPROBAR CONEXIÓN ---------------------------------
    #if defined(SM_DEBUG)
            SerialPC.println("\nComprobando conexion Serial con ESP32");  
    #endif

    // Enviar mensaje al ESP32 y esperar respuesta
    sendMsgToESP32("PING");
    // ---------------------------------------------------------

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 3000; // Espera máxima de 3 segundos
    waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
    // --------------------------------------
    
    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    // --- EXITO ------
    if (msgFromESP32 == "PONG") // Respuesta OK, el ESP32 está conectado y ready
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("Comunicacion con el ESP32 establecida correctamente"));
        #endif

        isESP32Connected = true; // Comunicación con ESP32 establecida
    } 
    // -----------------
    // --- "ERRORES" ---
    else if (msgFromESP32 == "TIMEOUT") // No se ha recibido respuesta del ESP32
    {
        // Ya se indica "TIMEOUT" en waitResponseFromESP32()
        
        isESP32Connected = false; // Fallo en la comunicación con ESP32
    }
    else // Mensaje desconocido
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("Mensaje desconocido al comprobar la comunicacion con ESP32"));
        #endif
        
        isESP32Connected = false; // Fallo en la comunicación con ESP32
    }
    // -----------------
    // --------------------------------------
    // ---------------------------------------------------------
}*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un mensaje desde el Arduino Due al ESP32.
 * @param msg El mensaje a enviar.
 */
/*-----------------------------------------------------------------------------*/
inline void sendMsgToESP32(const String &msg) { SerialESP32.println(msg); }



/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si hay mensajes del ESP32 disponibles en el puerto serial.
 * @return true si hay mensajes disponibles, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
inline bool hayMsgFromESP32() { return SerialESP32.available() > 0; }



/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el ESP32 y lo guarda en la variable proporcionada.
 * 
 * @param msgFromESP32 La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromSerialESP32(String &msgFromESP32) { 
    msgFromESP32 = SerialESP32.readStringUntil('\n'); 
    msgFromESP32.trim();  // Elimina espacios en blanco al principio y al final
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
    while(isESP32SerialEmpty() && !isTimeoutExceeded(startTime, timeout));

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromESP32())  // Si el esp32 ha respondido
    {
        readMsgFromSerialESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
        #if defined(SM_DEBUG)
            SerialPC.println("Respuesta del ESP32: " + msgFromESP32);  
        #endif
    } 
    else // No se ha recibido respuesta del ESP32
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
        #endif

        // Se considera que no hay conexión WiFi
        msgFromESP32 = "TIMEOUT";;
    }

}


/*---------------------------------------------------------------------------------------------------------*/
/**
 * Espera una respuesta del ESP32 con eventos.
 * 
 * Esta función espera a que el ESP32 responda durante un tiempo determinado. Si se recibe un mensaje, se guarda en la variable 'msgFromESP32'.
 * Si se supera el tiempo de espera, se considera un timeout y se asigna el valor "TIMEOUT" a 'msgFromESP32'.
 * Si ocurre un evento durante la espera, se asigna el valor "INTERRUPTION" a 'msgFromESP32'.
 * 
 * @param msgFromESP32 Referencia a una variable de tipo String donde se guardará el mensaje recibido del ESP32.
 * @param timeout Referencia a una variable de tipo unsigned long que indica el tiempo máximo de espera en milisegundos.
 */
/*---------------------------------------------------------------------------------------------------------*/
void waitResponseFromESP32WithEvents(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje, si se pasa el tiempo de espera, o si ocurre un evento
    while (isESP32SerialEmpty() && !isTimeoutExceeded(startTime, timeout)) {
        if(eventOccurredReadingBarcode()) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Interrupcion mientras se leia barcode"));
            #endif
            msgFromESP32 = "INTERRUPTION"; // Señalar que se ha interrumpido
            return;
        }

        delay(50); // Pequeño retraso para evitar una espera activa intensa
    }

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromESP32())  // Si el esp32 ha respondido
    {
        readMsgFromSerialESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
        #if defined(SM_DEBUG)
            SerialPC.println("Respuesta del ESP32: " + msgFromESP32);  
        #endif
    } 
    else // No se ha recibido respuesta del ESP32
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
        #endif

        // Se considera que no hay conexión WiFi
        msgFromESP32 = "TIMEOUT";;
    }

}




/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Comprueba la conexión WiFi del ESP32.
 *      Mensaje enviado al ESP32: "CHECK-WIFI"
 * 
 * @return true si hay conexión WiFi, false si no la hay o por TIMEOUT.
 */
/*---------------------------------------------------------------------------------------------------------*/
/*bool checkWifiConnection() 
{
    #if defined(SM_DEBUG)
        SerialPC.println(F("Comprobando la conexión WiFi del ESP32..."));
        //SerialPC.println(F("Cadena enviada al esp32: CHECK-WIFI")); 
    #endif
    
    //SerialESP32.print(F("CHECK-WIFI")); //Envía la cadena al esp32
    sendMsgToESP32("CHECK-WIFI"); // Envía la cadena al ESP32

    unsigned long timeout = 3000;        // Espera máxima de 3 segundos
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Bucle while se ejecuta mientras no se recibe una respuesta del ESP32 y no se ha superado el tiempo de espera.
    // Es más eficiente que while(true) porque se detiene en cuanto se recibe la respuesta o se supera el tiempo de espera,
    // pero no ejecuta las líneas de comprobación continuamente si no se ha recibido respuesta del ESP32.

    // Esperar 3 segundos a que el ESP32 responda
    //while ((SerialESP32.available() == 0) && (millis() - startTime < timeout)); 
    while(isESP32SerialEmpty() && isTimeoutExceeded(startTime, timeout));
    
    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
   // if (SerialESP32.available() > 0) { // Si el esp32 ha respondido
    if(hayMsgFromESP32()) // Si el esp32 ha respondido
    {
        //tring msgFromESP32 = SerialESP32.readStringUntil('\n');
        //msgFromESP32.trim();  // Elimina espacios en blanco al principio y al final
        String msgFromESP32;
        readMsgFromSerialESP32(msgFromESP32);

        #if defined(SM_DEBUG)
            SerialPC.println("Respuesta del ESP32: " + msgFromESP32);  
        #endif

        if (msgFromESP32 == "WIFI-OK") {
            // Respuesta OK, hay conexión WiFi
            #if defined(SM_DEBUG)
                SerialPC.println(F("Dice que hay wifi"));
            #endif
            return true;
        } 
        else if (msgFromESP32 == "NO-WIFI") {
            // Respuesta NO-WIFI, no hay conexión WiFi
            #if defined(SM_DEBUG)
                SerialPC.println(F("Dice que NO hay wifi"));
            #endif
            return false;
        }
    } 
    else {
        // No se ha recibido respuesta del ESP32
        #if defined(SM_DEBUG)
            SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
        #endif

        // Se considera que no hay conexión WiFi
        return false;
    }

}*/
bool checkWifiConnection() 
{
    //if(isESP32Connected) // La comunicación con el ESP32 está establecida
    //{
        // ---- PREGUNTAR POR WIFI ---------------------------------
        #if defined(SM_DEBUG)
            SerialPC.println(F("Comprobando la conexion WiFi del ESP32..."));
        #endif
        
        sendMsgToESP32("CHECK-WIFI"); // Envía la cadena al ESP32
        // ---------------------------------------------------------

        // ---- RESPUESTA DEL ESP32 --------------------------------
        // ---- ESPERAR RESPUESTA DEL ESP32 -----
        String msgFromESP32;
        unsigned long timeout = 5000; // Espera máxima de 3 segundos
        waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
        // --------------------------------------

        // ---- ANALIZAR RESPUESTA DEL ESP32 ----
        // --- EXITO ------
        if (msgFromESP32 == "WIFI-OK") // Respuesta OK, hay conexión WiFi
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Dice que hay wifi"));
            #endif
            return true;
        } 
        // -----------------
        // --- "ERRORES" ---
        else if (msgFromESP32 == "NO-WIFI") // Respuesta NO-WIFI, no hay conexión WiFi
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Dice que NO hay wifi"));
            #endif
            return false;
        }
        else if (msgFromESP32 == "TIMEOUT") // No se ha recibido respuesta del ESP32
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
            #endif
            return false; // Se considera que no hay conexión WiFi
        }
        else // Mensaje desconocido
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Error desconocido al comprobar la conexion WiFi..."));
            #endif
            return false; // Se considera que no hay conexión WiFi
        }
        // -----------------
        // --------------------------------------
        // ---------------------------------------------------------
    /*}
    else // La comunicación con el ESP32 no está establecida
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("No se puede comprobar la conexion WiFi porque no hay comunicacion con el ESP32"));
        #endif
        return false; // Se considera que no hay conexión WiFi
    }*/

}




/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Prepara el envío de datos al ESP32 para su guardado.
 *    Mensaje enviado al ESP32: "SAVE"
 * 
 * Esta función indica al ESP32 que se le va a enviar información y espera a que el ESP32 responda "WAITING-FOR-DATA".
 * 
 * @note Esta función bloquea el Arduino intencionadamente hasta recibir la respuesta del ESP32 o hasta TIMEOUT
 * 
 */
/*---------------------------------------------------------------------------------------------------------*/
byte prepareSaving()
{
    // No hace falta comprobar si hay conexión con el ESP32 porque se hace en checkWifiConnection(), antes 
    // de llamar a esta función

    // ---- INDICAR ENVÍO DE DATOS -----------------------------
    #if defined(SM_DEBUG)
        SerialPC.println(F("\nIndicando que se quiere guardar..."));
    #endif
    //SerialESP32.println(F("SAVE")); // Indicar al ESP32 que se le va a enviar información
    sendMsgToESP32("SAVE"); // Indicar al ESP32 que se le va a enviar información
    // ---------------------------------------------------------

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 3000; // Espera máxima de 3 segundos
    waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
    // --------------------------------------

    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    // --- EXITO ------
    if(msgFromESP32 == "WAITING-FOR-DATA") // El ESP32 queda a la espera de la info
    {
        #if defined SM_DEBUG
            SerialPC.println(F("El ESP32 esta esperando la info...\n"));
        #endif
        return WAITING_FOR_DATA; // ESP32 listo para recibir la info
    }
    // -----------------
    // --- ERRORES -----
    else if(msgFromESP32 == "NO-WIFI") // Se ha perdido la conexion WiFi
    {
        #if defined SM_DEBUG
            SerialPC.println(F("Se ha perdido la conexión WiFi...\n"));
        #endif
        return NO_INTERNET_CONNECTION; // No se puede enviar info al ESP32
    }
    else if(msgFromESP32.startsWith("HTTP-ERROR")) // Error HTTP al subir la info al ESP32
    {
        #if defined SM_DEBUG
            SerialPC.println(F("Error HTTP al autenticarse en la database...\n"));
        #endif
        return HTTP_ERROR; // No se puede enviar info al ESP32
    }
    else if(msgFromESP32 == "TIMEOUT") // No se recibió nada en 'timeout' segundos en waitResponseFromESP32()
    {
        #if defined SM_DEBUG
            SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32\n"));
        #endif
        return TIMEOUT; // No se puede enviar info al ESP32
    }   
    else
    {
        #if defined SM_DEBUG
            SerialPC.println(F("Error desconocido al indicar que se quiere guardar...\n"));
        #endif
        return UNKNOWN_ERROR; // No se puede enviar info al ESP32
    }
    // -----------------
    // --------------------------------------
    // ---------------------------------------------------------

}



/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Obtiene el código de barras y la información del producto.
 * 
 * Esta función se utiliza para obtener el código de barras y la información del producto asociada.
 * Si la comunicación con el ESP32 está establecida, se solicita el código de barras y se busca la información del producto en OpenFoodFacts.
 * Si no se ha leído el código de barras, no se buscará información del producto.
 * 
 * @param barcode Referencia a una cadena de caracteres donde se almacenará el código de barras.
 * @param productInfo Referencia a una cadena de caracteres donde se almacenará la información del producto.
 */
/*---------------------------------------------------------------------------------------------------------*/
void getBarcodeAndProductInfo()
{
    String barcode;
    // 1. Obtener barcode
    byte resultFromReadingBarcode = askForBarcode(barcode);

    // 2. Si se ha leído el barcode, buscar información del producto
    if(resultFromReadingBarcode == BARCODE_READ)
    { 
        String productInfo;
        byte resultFromGettingProductInfo = getProductInfo(barcode, productInfo); // Buscar info del producto en OpenFoodFacts
        if(resultFromGettingProductInfo == PRODUCT_FOUND); //showProductInfo(productInfo);
    }
    else
    {
        #ifdef SM_DEBUG
            SerialPC.println(F("\nNo se va a buscar info del producto porque no se ha leido el barcode"));
        #endif
    } 
}



/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Solicita la lectura de un código de barras al ESP32 y espera la respuesta.
 * 
 * @param barcode Referencia a un objeto String donde se almacenará el código de barras leído.
 * @return El resultado de la operación:
 *         - BARCODE_READ si se ha leído correctamente el código de barras.
 *         - BARCODE_NOT_READ si no se ha podido leer el código de barras.
 *         - NO_INTERNET_CONNECTION si no hay conexión a internet.
 */
/*---------------------------------------------------------------------------------------------------------*/
byte askForBarcode(String &barcode)
{
    // ---- PEDIR LEER BARCODE ---------------------------------
    #ifdef SM_DEBUG
        SerialPC.println(F("\nPidiendo escanear barcode..."));
    #endif
    sendMsgToESP32("GET-BARCODE"); // Envía la cadena al ESP32
    // ---------------------------------------------------------

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 30500; // Espera máxima de 10.5 segundos (el tiempo que da el ESP32 para colocar el producto)
    //waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
    waitResponseFromESP32WithEvents(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32. Atiende a interrupciones
    // --------------------------------------

    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    if(msgFromESP32 == "INTERRUPTION") 
        return INTERRUPTION; // Se ha interrumpido la lectura del barcode (evento de interrupción)
    // --- EXITO ------
    else if(msgFromESP32.startsWith("BARCODE:")) 
    {
        barcode = msgFromESP32.substring(8);
        #ifdef SM_DEBUG
            SerialPC.println("\nCodigo de barras leido: " + barcode);
        #endif
        return BARCODE_READ;
    }
    // -----------------
    // --- "ERRORES" ---
    else if(msgFromESP32 == "NO-BARCODE") 
    {
        #ifdef SM_DEBUG
            SerialPC.println("\nNo se ha podido leer el codigo de barras");
        #endif
        return BARCODE_NOT_READ;
    }
    else if(msgFromESP32 == "TIMEOUT") return TIMEOUT; // No se ha recibido respuesta del ESP32
    else // Mensaje desconocido
    {
        #ifdef SM_DEBUG
            SerialPC.println(F("\nError desconocido al pedir leer barcode..."));
        #endif
        return UNKNOWN_ERROR;
    }
    // -----------------
    // --------------------------------------
    // ---------------------------------------------------------
}



/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Obtiene la información de un producto a partir de un código de barras.
 * 
 * Esta función envía una solicitud al ESP32 para buscar la información del producto correspondiente al código de barras proporcionado.
 * Espera la respuesta del ESP32 y analiza la respuesta para determinar si se encontró el producto o si ocurrió algún error.
 * 
 * @param barcode El código de barras del producto.
 * @param productInfo La información del producto encontrada.
 */
/*---------------------------------------------------------------------------------------------------------*/
byte getProductInfo(String &barcode, String &productInfo)
{
    // ---- BUSCAR PRODUCTO ---------------------------------
    #ifdef SM_DEBUG
        SerialPC.println("\nBuscando informacion del producto: " + barcode);
    #endif
    // ---------------------------------------------------------

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 5500; // Espera máxima de 5.5 segundos (el tiempo que se da el ESP32 para buscar el producto)
    waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
    // --------------------------------------

    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    // --- EXITO ------
    if(msgFromESP32.startsWith("PRODUCT:")) 
    {
        #ifdef SM_DEBUG
            SerialPC.println("\nInformacion del producto: " + msgFromESP32);
        #endif
        productInfo = msgFromESP32;
        return PRODUCT_FOUND;
    }
    // -----------------
    // --- ERRORES -----
    else if(msgFromESP32 == "NO-PRODUCT") 
    {
        #ifdef SM_DEBUG
            SerialPC.print("\nNo se ha encontrado el producto " + barcode); SerialPC.println(" en OpenFoodFacts");
        #endif
        return PRODUCT_NOT_FOUND;
    }
    else if(msgFromESP32 == "PRODUCT-TIMEOUT") 
    {
        #ifdef SM_DEBUG
            SerialPC.println("\nEl servidor de OpenFoodFacts no responde");
        #endif
        return PRODUCT_TIMEOUT;
    }
    else if(msgFromESP32 == "NO-WIFI") 
    {
        #ifdef SM_DEBUG
            SerialPC.println("\nNo hay wifi");
        #endif
        return NO_INTERNET_CONNECTION;
    }
    else if(msgFromESP32.startsWith("ERROR-HTTP:")) 
    {
        #ifdef SM_DEBUG
            String error = msgFromESP32.substring(11);
            SerialPC.println("Error al buscar info del producto (peticion HTTP POST): " + error);
        #endif
        return HTTP_ERROR;
    }
    else if(msgFromESP32 == "TIMEOUT") return TIMEOUT; // No se ha recibido respuesta del ESP32
    else // Mensaje desconocido
    {
        #ifdef SM_DEBUG
            SerialPC.println("Mensaje no reconocido: " + msgFromESP32);
        #endif
        return UNKNOWN_ERROR;
    }
    // -----------------
    // --------------------------------------
    // ---------------------------------------------------------
}




/******************************************************************************/
/******************************************************************************/

#endif