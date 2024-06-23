
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




#include "SD_functions.h"

#include "debug.h" // SM_DEBUG --> Comunicación Serial con PC
#define SerialESP32 Serial1 // Comunicación Serial con ESP32


void showDataToUpload(byte option);

bool isESP32Connected = false; // Flag para indicar si la comunicación con el ESP32 está establecida


/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
// Funciones para manejar la comunicación Serial con el ESP32
void            setupSerial();                                      // Inicializar comunicación serie con PC y ESP32
void            pingESP32();                                       // Comprobar conexión con ESP32

inline void     sendMsgToESP32(const String &msg);                  // Enviar 'msg' del Due al ESP32
inline bool     hayMsgFromESP32();                                  // Comprobar si hay mensajes del ESP32 disponibles
inline bool     isESP32SerialEmpty(){ return !hayMsgFromESP32(); }  // Comprobar si no hay mensajes del ESP32 disponibles
inline void     readMsgFromSerialESP32(String &msgFromESP32);       // Leer mensaje del puerto serie Due-ESP32 y guardarlo en msgFromESP32

void            waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout);    // Espera la respuesta del ESP32, sea cual sea, y la devuelve en msgFromESP32.
inline bool     isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout);    // Comprobar si se ha excedido el tiempo de espera
// -------------

bool    checkWifiConnection();                                                      // Comprobar conexion a internet

byte    prepareSaving();                                                            // Indica al ESP32 que se le va a enviar info y espera su respuesta WAITING-FOR-DATA

//byte    handleResponseFromESP32AfterUpload(byte option);                          // Maneja la respuesta del ESP32 y actúa acorde

/******************************************************************************/
/******************************************************************************/




/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/


/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la comunicación serial entre el PC y el ESP32.
 *  Esta función inicializa los puertos seriales y establece la velocidad de baudios
 *  en 115200 para ambos, el PC y el ESP32.
 */
/*-----------------------------------------------------------------------------*/
void setupSerial() 
{
    // Inicializar comunicación con PC (Serial)
    #if defined(SM_DEBUG)
        SerialPC.begin(115200);
    #endif // SM_DEBUG
    
    // Inicializar comunicación con ESP32-CAM (Serial1)
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
void pingESP32()
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
        /* // Ya se indica en waitResponseFromESP32()
        #if defined(SM_DEBUG)
            SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
        #endif*/
        
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
}




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
 * @brief Espera la respuesta del ESP32 y la devuelve
 * 
 * @return La respuesta del ESP32.
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
    if(isESP32Connected) // La comunicación con el ESP32 está establecida
    {
        // ---- PREGUNTAR POR WIFI ---------------------------------
        #if defined(SM_DEBUG)
            SerialPC.println(F("Comprobando la conexion WiFi del ESP32..."));
        #endif
        
        sendMsgToESP32("CHECK-WIFI"); // Envía la cadena al ESP32
        // ---------------------------------------------------------

        // ---- RESPUESTA DEL ESP32 --------------------------------
        // ---- ESPERAR RESPUESTA DEL ESP32 -----
        String msgFromESP32;
        unsigned long timeout = 3000; // Espera máxima de 3 segundos
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
    }
    else // La comunicación con el ESP32 no está establecida
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("No se puede comprobar la conexion WiFi porque no hay comunicacion con el ESP32"));
        #endif
        return false; // Se considera que no hay conexión WiFi
    }

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
 * @brief Maneja la respuesta recibida desde el ESP32 después de subir la info a database.
 * 
 * Esta función recibe la respuesta del ESP32 tras subir la información y realiza las acciones necesarias
 * según la respuesta recibida.
 * 
 * @param option La opción para determinar el comportamiento de la función (mostrar o no pantalla de sincronización) 
 * 
 * @return El código de estado idicando el resultado del proceso de subida:
 *         - SAVE_EXECUTED_FULL: La comida se ha guardado localmente y en la base de datos.
 *         - SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP: La comida solo se ha guardado localmente por un error HTTP.
 *         - SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI: La comida solo se ha guardado localmente porque se perdió la conexión WiFi. 
 *         - SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT: La comida solo se ha guardado localmente por TIMEOUT de respuesta del ESP32. 
 *         - SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR: La comida solo se ha guardado localmente por error desconocido. 
 */
/*---------------------------------------------------------------------------------------------------------*/
/*byte handleResponseFromESP32AfterUpload(byte option)
{
    //String msgFromESP32 = waitResponseFromESP32(FASE_3_UPLOADING); // En la fase 3, sale del while si recibe SAVED-OK, HTTP-ERROR: o NO-WIFI
    String msgFromESP32;
    waitResponseFromESP32(msgFromESP32); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32

    if (msgFromESP32 == "SAVED-OK") // JSON guardado en la base de datos
    {
        deleteFileTXT(); // Borrar fichero porque ya se ha subido su info
        if(option == SHOW_SCREEN_UPLOAD_DATA) showDataToUpload(ALL_MEALS_UPLOADED); // State_UPLOAD_DATA

        #if defined(SM_DEBUG)
            SerialPC.println(F("JSON guardado y borrando fichero TXT..."));
            SerialPC.println(F("\nPaso a Init tras subir la info y borrar TXT..."));
        #endif

        return SAVE_EXECUTED_FULL;
    } 
    else if (msgFromESP32.startsWith("ERROR-HTTP:")) // La subida no ha sido exitosa 
    { 
        if(option == SHOW_SCREEN_UPLOAD_DATA) showDataToUpload(HTTP_ERROR);
        else if(option == NOT_SHOW_SCREEN_UPLOAD_DATA){ // STATE_saved --> La opción NOT_SHOW_SCREEN_UPLOAD_DATA se usa en el STATE_saved para no mostrar el proceso.
                                                        // Si se ha intentado subir la lista de la comida pero ha habido error.
            #if defined SM_DEBUG
                SerialPC.println(F("Guardando la lista en el TXT hasta que el ESP32 pueda subir la info...")); // STATE_saved
            #endif

            saveListInTXT(); // Copia lista en TXT y la limpia
        }

        #if defined(SM_DEBUG)
            SerialPC.print(F("Error al subir JSON: ")); SerialPC.println(msgFromESP32);
            SerialPC.println(F("\nPaso a Init tras ERROR al subir la info..."));
        #endif

        return SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP;
    }
    else if (msgFromESP32 == "NO-WIFI") // Se ha cortado la conexión (no debería ocurrir si ya se ha comprobado antes)
    {
        if(option == SHOW_SCREEN_UPLOAD_DATA) showDataToUpload(NO_INTERNET_CONNECTION);
        else if(option == NOT_SHOW_SCREEN_UPLOAD_DATA){ // STATE_saved --> La opción NOT_SHOW_SCREEN_UPLOAD_DATA se usa en el STATE_saved para no mostrar el proceso.
                                                        // Si se ha intentado enviar la lista de la comida pero no hay WiFi.
            #if defined SM_DEBUG
                SerialPC.println(F("Guardando la lista en el TXT hasta que el ESP32 pueda subir la info...")); // STATE_saved
            #endif

            saveListInTXT(); // Copia lista en TXT y la limpia
        }

        #if defined(SM_DEBUG)
            SerialPC.println(F("Se ha cortado la conexión a Internet..."));
            SerialPC.println(F("\nPaso a Init tras fallo de conexión..."));
        #endif        

        return SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI;
    }
    else if (msgFromESP32 == "TIMEOUT") // El ESP32 no respondió en 30 segundos. Actuamos como si no hubiera WiFi
    {
        if(option == SHOW_SCREEN_UPLOAD_DATA) showDataToUpload(NO_INTERNET_CONNECTION);
        else if(option == NOT_SHOW_SCREEN_UPLOAD_DATA){ // STATE_saved --> La opción NOT_SHOW_SCREEN_UPLOAD_DATA se usa en el STATE_saved para no mostrar el proceso.
                                                        // Si se ha intentado enviar la lista de la comida, pero ha habido error.
            #if defined SM_DEBUG
                SerialPC.println(F("Guardando la lista en el TXT hasta que el ESP32 pueda subir la info...")); // STATE_saved
            #endif

            saveListInTXT(); // Copia lista en TXT y la limpia
        }

        #if defined(SM_DEBUG)
            SerialPC.println(F("No se ha subido por TIMEOUT"));
        #endif        

        return SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT;
    }
    else
    { 
        #if defined(SM_DEBUG)
            SerialPC.println(F("No se ha subido por ERROR DESCONOCIDO"));
        #endif   

        return SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR;
    }
}*/



/******************************************************************************/
/******************************************************************************/

#endif