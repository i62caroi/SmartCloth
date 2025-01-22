
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
        ----- CONEXIÓN A INTERNET -----
        1) Check wifi:
            "CHECK-WIFI"

        ----- INFO COMIDAS ------------
        2) Guardar info en base de datos al inicio:
            2.1. Indicar guardado:
                "SAVE"
            2.2. Mandar datos a guardar, línea a línea:
                "INICIO-COMIDA" "INICIO-PLATO" "ALIMENTO,<grupo>,<peso>[,<ean>]" "FIN-COMIDA,<fecha>,<hora>"..."FIN-TRANSMISION"

        ----- BARCODE ----------------
            3) Leer código de barras:
                "GET-BARCODE"
                3.1) Cancelar leer código de barras (para que el ESP32 no siga esperándolo si se cancela la acción):
                    "CANCEL-BARCODE"

            4) Buscar producto:
                "GET-PRODUCT:<barcode>"




    -------- MENSAJES ESP32 -> ARDUINO -------------- 
        ----- CONEXIÓN A INTERNET -----
        1) Hay wifi:
            "WIFI-OK"

        2) No hay wifi:
            "NO-WIFI"
        
        ----- INFO COMIDAS ------------
        3) Esperando datos a subir:
            "WAITING-FOR-DATA"

        4) JSON guardado correctamente:
            "SAVED-OK"

        5) Error en el guardado de la comida (petición HTTP POST):
            "HTTP-ERROR:<codigo_error>"

        ----- BARCODE ----------------
            ----- LEER BARCODE -----
            6) Código de barras leído. Buscando información del producto:
                "BARCODE:<barcode>"
        
            7) No se ha podido leer el código de barras (no se ve bien o el lector no responde):
                "NO-BARCODE"
    
            ----- BUSCAR PRODUCTO -----
            8) Información del producto:
                "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
            
            9) No se ha encontrado el producto en OpenFoodFacts (error HTTP 404 not found):
                "NO-PRODUCT"

            10) Error al buscar producto (diferente a no encontrado):
                "HTTP-ERROR:<codigo_error>"

            11) El servidor de OpenFoodFacts no responde:
                "PRODUCT-TIMEOUT"


*/



#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H

#include "Comida.h" // comidaActual
#include "SD_functions.h"
#include "debug.h" // SM_DEBUG --> Comunicación Serial con PC

bool eventOccurred(); // Evento de interrupción en botoneras o báscula

#include "Scale.h" // checkBascula()
#include "ISR.h" // eventOccurred()
#define SerialESP32 Serial1 // Comunicación Serial con ESP32


// --- RESULTADOS DE SUBIR INFO A DATABASE --- 
// Usadas en:
//      prepareSaving()                         -->     WAITING_FOR_DATA, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT y UNKNOWN_ERROR
//      sendMealsFileToESP32ToUpdateWeb()       -->     ALL_MEALS_UPLOADED, MEALS_LEFT y ERROR_READING_MEALS_FILE
//      saveComidaInDatabase_or_MealsFile()     -->     MEAL_UPLOADED, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT y UNKNOWN_ERROR
#define  WAITING_FOR_DATA           1
#define  UPLOADING_DATA             2
#define  ALL_MEALS_UPLOADED         3 // Comidas guardadas en sincronización inicial
#define  MEAL_UPLOADED              3 // Comida guardada al pulsar "Guardar comida"
#define  MEALS_LEFT                 4
#define  ERROR_READING_MEALS_FILE   5
// --- ERRORES ---
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



// -------- MENSAJES DEL ESP32 --------
#define NUM_EXACT_ESP32_MESSAGES      7   // WIFI-OK, NO-WIFI, WAITING-FOR-DATA, SAVED-OK, NO-BARCODE, NO-PRODUCT, PRODUCT-TIMEOUT
#define NUM_PREFIX_ESP32_MESSAGES     3   // HTTP-ERROR:, BARCODE:, PRODUCT:

// Mensajes exactos esperados del ESP32
// Se usa const char* en lugar de String para ahorrar memoria. Así que al comparar con estos mensajes, se debe hacer con equals() y no con ==
const char* EXACT_ESP32_MESSAGES[] = 
{
    "WIFI-OK",              // Conexión a internet
    "NO-WIFI",              // Sin conexión a internet
    "WAITING-FOR-DATA",     // Esperando datos a subir (líneas del TXT de comidas no subidas)
    "SAVED-OK",             // Comida(s) guardada(s) correctamente
    "NO-BARCODE",           // No se ha podido leer el código de barras
    "NO-PRODUCT",           // No se ha encontrado el producto en OpenFoodFacts
    "PRODUCT-TIMEOUT"       // El servidor de OpenFoodFacts no responde
};

// Prefijos de mensajes esperados del ESP32
const char* PREFIX_ESP32_MESSAGES[] = 
{
    "HTTP-ERROR:",          // Error en el guardado de la comida (incluyendo autenticación) o al buscar producto
    "BARCODE:",             // Código de barras leído
    "PRODUCT:"              // Información nutricial del producto
};
// ------------------------------------





/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
// Setup Serial PC
void            setupSerialPC();                                    // Configurar comunicación Serial con PC

// Comunicación Serial Due-ESP32
void            setupSerialESP32();                                 // Configurar comunicación Serial con ESP32
inline bool     hayMsgFromESP32() { return SerialESP32.available() > 0; };      // Comprobar si hay mensajes del ESP32 disponibles
inline bool     isESP32SerialEmpty(){ return !hayMsgFromESP32(); }              // Comprobar si no hay mensajes del ESP32 disponibles
//inline void     readMsgFromSerialESP32(String &msgFromESP32);                   // Leer mensaje del puerto serie Due-ESP32 y guardarlo en msgFromESP32
inline void     sendMsgToESP32(const String &msg);                              // Enviar mensaje al ESP32

// Esperar mensaje de ESP32
bool            processCharacter(String &tempBuffer, String &msgFromDue);                       // Procesar mensaje del ESP32 caracter a caracter
bool            isValidESP32Message(const String &message);                                     // Comprobar si el mensaje del ESP32 es válido
void            waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout);            // Espera la respuesta del ESP32, sea cual sea, y la devuelve en msgFromESP32.
void            waitResponseFromESP32WithEvents(String &msgFromESP32, unsigned long &timeout);  // Espera la respuesta del ESP32 y la devuelve. Atiende a eventos
inline bool     isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout);            // Comprobar si se ha excedido el tiempo de espera
// -------------

bool    checkWifiConnection();                                      // Comprobar conexion a internet
byte    prepareSaving();                                            // Indica al ESP32 que se le va a enviar info y espera su respuesta WAITING-FOR-DATA
byte    askForBarcode(String &barcode);                             // Obtener el código de barras
byte    getProductInfo(String &barcode, String &productInfo);       // Obtener la información del producto a partir de un código de barras
/******************************************************************************/
/******************************************************************************/




/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/


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

    SerialPC.println("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++");
    SerialPC.println(F("Iniciando comunicacion Serial con PC..."));
    SerialPC.println("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
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
 * Lee un mensaje desde el puerto serie con el ESP32 y lo guarda en la variable proporcionada.
 * 
 * @param msgFromESP32 La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
/*inline void readMsgFromSerialESP32(String &msgFromESP32) 
{ 
    msgFromESP32 = SerialESP32.readStringUntil('\n'); 
    msgFromESP32.trim();  // Elimina espacios en blanco al principio y al final
}*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un mensaje al ESP32 a través del puerto serie.
 *
 * Esta función limpia el buffer del ESP32, envía el mensaje especificado
 * y añade un pequeño retraso para asegurar que el ESP32 tenga tiempo de leer el mensaje.
 *
 * @param msg El mensaje que se enviará al ESP32.
 */
/*-----------------------------------------------------------------------------*/
inline void sendMsgToESP32(const String &msg)
{ 
    SerialESP32.println(msg); // Enviar 'msg' del Due al ESP32 
    delay(50);  // Pequeño retraso para asegurar que el ESP32 tenga tiempo de leer el mensaje
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
 * @brief Procesa un carácter recibido del serial del ESP32 y construye un mensaje completo.
 *
 * Esta función lee un carácter del serial del ESP32 y lo acumula en un buffer temporal.
 * Si el carácter leído es un salto de línea ('\n'), se considera que el mensaje está completo.
 * En ese caso, se elimina cualquier espacio en blanco del buffer temporal, y si no está vacío,
 * se asigna el contenido del buffer al mensaje desde el ESP32 y se retorna true.
 *
 * @param tempBuffer Referencia al buffer temporal donde se acumulan los caracteres.
 * @param msgFromESP32 Referencia a la cadena donde se almacenará el mensaje completo del ESP32.
 * @return true Si se ha recibido y procesado un mensaje completo.
 * @return false Si aún no se ha recibido un mensaje completo.
 */
/*-----------------------------------------------------------------------------*/
/*bool processCharacter(String &tempBuffer, String &msgFromESP32) 
{
    char c = SerialESP32.read();  // Lee un carácter del serial del ESP32

    if (c == '\n') 
    {
        tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

        if (tempBuffer.length() > 0)   // Si el buffer temporal no está vacío, se asigna a msgFromESP32. Para evitar asignar cadenas como "\n"
        {
            msgFromESP32 = tempBuffer;
            #if defined(SM_DEBUG)
                SerialPC.print("\n---> Respuesta del ESP32: "); SerialPC.print("\"" + msgFromESP32); SerialPC.println("\"");
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
bool processCharacter(String &tempBuffer, String &msgFromESP32) 
{
    char c = SerialESP32.read();  // Lee un carácter del serial del ESP32

    #if defined(SM_DEBUG)
        SerialPC.print("Caracter leido: "); 
        if(c == '\n') SerialPC.println("'\\n'");
        else SerialPC.println(String(c));
    #endif

    if (c == '\n') 
    {
        tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

        if (tempBuffer.length() > 0 && isValidESP32Message(tempBuffer))   // Verificar que el mensaje es uno de los posibles antes de asignarlo
        {
            msgFromESP32 = tempBuffer; // Asigna el contenido del buffer temporal al mensaje del ESP32
            #if defined(SM_DEBUG)
                SerialPC.print("\n---> Mensaje completo del ESP32: "); 
                SerialPC.print("\"" + msgFromESP32); 
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
 * @brief Verifica si lo recibido del ESP32 es un mensaje válido o basura del Serial.
 *
 * Esta función comprueba si el mensaje recibido coincide con alguno de los mensajes
 * establecidos que debe enviar el ESP32 en cada situación.
 *
 * @param message El mensaje recibido que se va a verificar.
 * @return true si el mensaje es válido, false en caso contrario.
 */
/*---------------------------------------------------------------------------------------------------------*/
bool isValidESP32Message(const String &message) 
{
    // Verificar mensajes exactos
    for (int i = 0; i < NUM_EXACT_ESP32_MESSAGES; i++) 
    {
        if (message.equals(EXACT_ESP32_MESSAGES[i])) return true;
    }
    
    // Verificar prefijos
    for (int i = 0; i < NUM_PREFIX_ESP32_MESSAGES; i++) 
    {
        if (message.startsWith(PREFIX_ESP32_MESSAGES[i])) return true;
    }
    
    return false;
}


/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Espera una respuesta del ESP32 dentro de un tiempo de espera especificado.
 * 
 * Esta función espera hasta que se reciba un mensaje completo del ESP32 o hasta que se exceda el tiempo de espera.
 * Si se recibe un mensaje completo, se almacena en `msgFromESP32`. Si se excede el tiempo de espera, se marca el mensaje como "TIMEOUT".
 * 
 * @param msgFromESP32 Referencia a un String donde se almacenará el mensaje recibido del ESP32.
 * @param timeout Referencia a un unsigned long que especifica el tiempo máximo de espera en milisegundos.
 * 
 * @note Comprueba si hay mensajes del ESP32 disponibles y procesa el mensaje caracter a caracter, así se evita perder mensajes
 *       recibiendo un falso mensaje vacío "" aunque el ESP32 haya enviado un mensaje correcto.
 */
/*---------------------------------------------------------------------------------------------------------*/
void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual
    String tempBuffer = "";  // Buffer temporal para ensamblar el mensaje

    // Espera hasta que se reciba un mensaje o se exceda el tiempo de espera
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromESP32())  // Si el esp32 ha respondido
        {
            if (processCharacter(tempBuffer, msgFromESP32)) 
                return;  // Sale de la función cuando se ha procesado un mensaje completo del ESP32
        }

        delay(50);  // Evita que el bucle sea demasiado intensivo
    }

    // Si se alcanza el tiempo de espera sin recibir un mensaje
    #if defined(SM_DEBUG)
        SerialPC.println(F("\nTIMEOUT. No se ha recibido respuesta del ESP32"));
    #endif

    msgFromESP32 = "TIMEOUT";  // Marca el mensaje como TIMEOUT si no se recibió nada útil
}






/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Espera una respuesta del ESP32 con manejo de eventos.
 *
 * Esta función espera hasta que se reciba un mensaje del ESP32 o hasta que se exceda el tiempo de espera especificado.
 * Durante la espera, también verifica si ocurre algún evento que interrumpa la lectura.
 *
 * @param msgFromESP32 Referencia a una cadena donde se almacenará el mensaje recibido del ESP32.
 *                     Si ocurre una interrupción, se establecerá en "INTERRUPTION".
 *                     Si se excede el tiempo de espera, se establecerá en "TIMEOUT".
 * @param timeout Referencia a un valor de tiempo en milisegundos que especifica el tiempo máximo de espera.
 * 
 * @note Comprueba si hay mensajes del ESP32 disponibles y procesa el mensaje caracter a caracter, así se evita perder mensajes
 *       recibiendo un falso mensaje vacío "" aunque el ESP32 haya enviado un mensaje correcto.
 */
/*---------------------------------------------------------------------------------------------------------*/
void waitResponseFromESP32WithEvents(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual
    String tempBuffer = "";  // Buffer temporal para acumular datos

    // Espera hasta que se reciba un mensaje o se exceda el tiempo de espera
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromESP32())  // Si el esp32 ha respondido
        {
            if (processCharacter(tempBuffer, msgFromESP32)) 
                return;  // Sale de la función cuando se ha procesado un mensaje completo del ESP32
        }

        if(eventOccurred()) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Interrupcion mientras se leia barcode"));
            #endif
            msgFromESP32 = "INTERRUPTION"; // Señalar que se ha interrumpido
            return; // Salir de la función si se detecta interrupción (cancelación manual de la lectura)
        }

        delay(50);  // Evita que el bucle sea demasiado intensivo
    }


    // Si se alcanza el tiempo de espera sin recibir un mensaje
    #if defined(SM_DEBUG)
        SerialPC.println(F("\nTIMEOUT. No se ha recibido respuesta del ESP32"));
    #endif

    msgFromESP32 = "TIMEOUT";  // Marca el mensaje como TIMEOUT si no se recibió nada útil

}




/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Comprueba la conexión WiFi del ESP32.
 *      Mensaje enviado al ESP32: "CHECK-WIFI"
 * 
 * @return true si hay conexión WiFi, false si no la hay o por TIMEOUT.
 */
/*---------------------------------------------------------------------------------------------------------*/
bool checkWifiConnection() 
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
    unsigned long timeout = 5000; // Espera máxima de 5 segundos
                                    // No hace falta esperar más tiempo porque esta es una comprobación directa del ESP32
                                    // y los mensajes que se pueden recibir "WIFI-OK" o "NO-WIFI" son cortos, por lo que no
                                    // debería tardar demasiado waitResponseFromESP32() en recibirlos.
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
            SerialPC.println(F("TIMEOUT. Sin respuesta del ESP32 al comprobar la conexion WiFi"));
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
    // ---- INDICAR ENVÍO DE DATOS -----------------------------
    #if defined(SM_DEBUG)
        SerialPC.println(F("\nIndicando que se quiere guardar..."));
    #endif
    //SerialESP32.println(F("SAVE")); // Indicar al ESP32 que se le va a enviar información
    sendMsgToESP32("SAVE"); // Indicar al ESP32 que se le va a enviar información
    // ---------------------------------------------------------

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32 = "";
    unsigned long timeout = 20000; // Espera máxima de 20 segundos (el ESP32 tiene 10 segundos para autenticarse en la web)
                                    // La cadena a recibir no es muy larga ("WAITING-FOR-DATA" o error), pero como el ESP32 tiene 10 segundos
                                    // para autenticarse y waitResponseFromESP32() comprueba caracter a caracter hasta recibir un salto de línea, 
                                    // se aumenta el tiempo de espera.
                                    // Se comprueba caracter a caracter en lugar de simplemente SerialESP32.available() > 0 porque
                                    // a veces se leía un mensaje vacío "\n" y se salía del bucle antes de pararse a leer el mensaje completo.
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
            SerialPC.println(F("TIMEOUT. Sin respuesta del ESP32 al indicar guardado\n"));
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
    unsigned long timeout = 35000; // Espera máxima de 35 segundos (el ESP32 da 30 segundos para colocar el producto)
                                    // La cadena a recibir puede ser algo larga según el código ("BARCODE:<barcode>") o no ("NO-BARCODE), 
                                    // pero como el ESP32 tiene 30 segundos para leer barcode y waitResponseFromESP32() comprueba caracter 
                                    // a caracter hasta recibir un salto de línea, se aumenta el tiempo de espera.
                                    // Se comprueba caracter a caracter en lugar de simplemente SerialESP32.available() > 0 porque
                                    // a veces se leía un mensaje vacío "\n" y se salía del bucle antes de pararse a leer el mensaje completo.
    waitResponseFromESP32WithEvents(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32. Atiende a interrupciones
    // --------------------------------------

    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    // --- CANCELAR LECTURA ---
    if(msgFromESP32 == "INTERRUPTION") // Interrupción del usuario (pulsar botón para cancelar lectura)
    {
        #ifdef SM_DEBUG
            SerialPC.println(F("\nIndicando al ESP32 que cancele la lectura..."));
        #endif
        
        // --- CANCELAR LECTURA ---
        sendMsgToESP32("CANCEL-BARCODE"); // Cancelar la lectura del barcode
        // ------------------------

        return INTERRUPTION; // Se ha interrumpido la lectura del barcode (evento de interrupción)
    }
    // --- EXITO ------
    else if(msgFromESP32.startsWith("BARCODE:")) // "BARCODE:<barcode>"
    {
        barcode = msgFromESP32.substring(8);
        #ifdef SM_DEBUG
            SerialPC.println("\nCodigo de barras leido: " + barcode);
        #endif
        return BARCODE_READ; // Se ha leído el código de barras
    }
    // -----------------
    // --- "ERRORES" ---
    else if(msgFromESP32 == "NO-BARCODE") 
    {
        #ifdef SM_DEBUG
            SerialPC.println(F("\nNo se ha podido leer el codigo de barras"));
        #endif
        return BARCODE_NOT_READ; // No se ha detectado el código de barras
    }
    else if(msgFromESP32 == "TIMEOUT") // No se recibió nada en 'timeout' segundos en waitResponseFromESP32WithEvents()
    {
        #if defined SM_DEBUG
            SerialPC.println(F("TIMEOUT. Sin respuesta del ESP32 al pedir leer barcode\n"));
        #endif
        return TIMEOUT; // Se considera que no se ha detectado el código de barras
    }
    else // Mensaje desconocido
    {
        #ifdef SM_DEBUG
            SerialPC.println(F("\nMensaje o error desconocido al pedir leer barcode..."));
        #endif
        return UNKNOWN_ERROR; // Se considera que no se ha detectado el código de barras
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
    // ---- PEDIR BUSCAR PRODUCTO ------------------------------
    #ifdef SM_DEBUG
        SerialPC.println(F("\nPidiendo buscar producto..."));
    #endif
    String msgToESP32 = "GET-PRODUCT:" + barcode; 
    sendMsgToESP32(msgToESP32); // Envía la cadena al ESP32
    // ---------------------------------------------------------
    

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 20000; // Espera máxima de 20 segundos (el ESP32 tiene 10 segundos para buscar el producto)
                                    // Como la cadena a recibir es muy larga y waitResponseFromESP32() comprueba caracter a caracter
                                    // hasta recibir un salto de línea, se aumenta el tiempo de espera.
                                    // Se comprueba caracter a caracter en lugar de simplemente SerialESP32.available() > 0 porque
                                    // a veces se leía un mensaje vacío "\n" y se salía del bucle antes de pararse a leer el mensaje completo.
    waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
    // --------------------------------------

    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    // --- EXITO ------
    if(msgFromESP32.startsWith("PRODUCT:")) // "PRODUCT:<barcode>;<nombreProducto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
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
    else if(msgFromESP32.startsWith("HTTP-ERROR:")) 
    {
        #ifdef SM_DEBUG
            String error = msgFromESP32.substring(11);
            SerialPC.println("Error al buscar info del producto (peticion HTTP GET): " + error);
        #endif
        return HTTP_ERROR;
    }
    else if(msgFromESP32 == "TIMEOUT") // No se recibió nada en 'timeout' segundos en waitResponseFromESP32WithEvents()
    {
        #if defined SM_DEBUG
            SerialPC.println(F("TIMEOUT. Sin respuesta del ESP32 al pedir buscar info de producto\n"));
        #endif
        return TIMEOUT; // Se considera que no se ha encontrado el producto
    }
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