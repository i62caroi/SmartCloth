
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


  MODIFICACION: El Due ya no espera dos mensajes seguidos, sino que pide leer barcode con "GET-BARCODE" y espera "BARCODE:<barcode>". 
  Después, indica al ESP32 que busque la información del producto con "GET-PRODUCT:<barcode>" y el ESP32 le responderá, si lo encuentra, 
  con "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>".

  Además, como hemos dividido el funcionamiento del barcode en varios estados, los mensajes más escalonados permiten buscar en la SD si el
  barcode ya se ha leído antes y, si es así, no buscarlo en OpenFoodFacts.

    1. Due --> ESP32: 
        "GET-BARCODE"
    2. ESP32 --> Due: 
        "BARCODE:<barcode>"
    3. Due busca en SD el barcode
    4.a. El barcode se leyó antes, así que usa como grupo.
    4.b. El barcode no está en SD, Due --> ESP32: 
            "GET-PRODUCT:<barcode>"
        5. ESP32 --> Due: 
            "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"


    BOTON BARCODE conectado de la siguiente forma (indistintamente):
        -----------------
        | GND | Barcode |
        -----------------
        | GND |   53    |       
        -----------------
    ¡IMPORTANTE! El GND junto al D53 en el screw shield no está conectado a nada. Usar alguno
    de los que están junto a 5V o 3.3V.



    -------- MENSAJES ARDUINO -> ESP32 --------------
        ----- CONEXIÓN A INTERNET -----
        1) Check wifi:
            "CHECK-WIFI"

        ----- BARCODE ----------------
        2) Leer código de barras:
            "GET-BARCODE"

        3) Buscar producto:
            "GET-PRODUCT:<barcode>"



    -------- MENSAJES ESP32 -> ARDUINO -------------- 
        ----- CONEXIÓN A INTERNET -----
        1) Hay wifi:
            "WIFI-OK"

        2) No hay wifi:
            "NO-WIFI"

        ----- BARCODE ----------------
            ----- LEER BARCODE -----
            3) Código de barras leído:
                "BARCODE:<barcode>"
        
            4) No se ha podido leer el código de barras (no se ve bien o el lector no responde):
                "NO-BARCODE"
    
            ----- BUSCAR PRODUCTO -----
            5) Información del producto:
                "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
            
            6) No se ha encontrado el producto en OpenFoodFacts (error HTTP 404 not found):
                "NO-PRODUCT"

            7) Error al buscar producto (diferente a no encontrado):
                "ERROR-HTTP:<codigo_error>"

            8) El servidor de OpenFoodFacts no responde:
                "PRODUCT-TIMEOUT"


*/

#ifndef SERIAL_FUNCTIONS_H
#define SERIAL_FUNCTIONS_H

// ----- ESP32 <--> PC ----------
#define SerialPC Serial 
// ------------------------------

// ----- Due <--> ESP32 ---------
#define SerialESP32 Serial1
// ------------------------------

#include "SD_functions.h"



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

// --- ERRORES ---
#define  NO_INTERNET_CONNECTION  6
#define  HTTP_ERROR              7
#define  TIMEOUT                 8
#define  UNKNOWN_ERROR           9







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
inline void     readMsgFromSerialESP32(String &msgFromESP32);                   // Leer mensaje del puerto serie Due-ESP32 y guardarlo en msgFromESP32
inline void     sendMsgToESP32(const String &msg);                              // Enviar mensaje al ESP32

// Esperar mensaje de ESP32
bool            processCharacter(String &tempBuffer, String &msgFromDue);                       // Procesar mensaje del ESP32 caracter a caracter
void            waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout);            // Espera la respuesta del ESP32, sea cual sea, y la devuelve en msgFromESP32.
void            waitResponseFromESP32WithEvents(String &msgFromESP32, unsigned long &timeout);  // Espera la respuesta del ESP32 y la devuelve. Atiende a eventos
inline bool     isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout);            // Comprobar si se ha excedido el tiempo de espera
// -------------

bool    checkWifiConnection();                                      // Comprobar conexion a internet
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
void setupSerialPC() 
{
    // Inicializar comunicación con PC (Serial)
    SerialPC.begin(115200);
    delay(100);

    SerialPC.println(F("Iniciando comunicacion Serial con PC...\n"));
}



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

    SerialPC.println(F("Iniciando comunicacion Serial con ESP32...\n"));
}



/*-----------------------------------------------------------------------------*/
/**
 * Lee un mensaje desde el puerto serie con el ESP32 y lo guarda en la variable proporcionada.
 * 
 * @param msgFromESP32 La variable donde se guardará el mensaje leído.
 */
/*-----------------------------------------------------------------------------*/
inline void readMsgFromSerialESP32(String &msgFromESP32) 
{ 
    msgFromESP32 = SerialESP32.readStringUntil('\n'); 
    msgFromESP32.trim();  // Elimina espacios en blanco al principio y al final
}




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
bool processCharacter(String &tempBuffer, String &msgFromESP32) 
{
    char c = SerialESP32.read();  // Lee un carácter del serial del ESP32

    if (c == '\n') 
    {
        tempBuffer.trim();  // Elimina los espacios en blanco del buffer temporal

        if (tempBuffer.length() > 0)   // Solo procesa si no está vacío
        {
            msgFromESP32 = tempBuffer;
            SerialPC.print("\n---> Respuesta del ESP32: "); SerialPC.print("\"" + msgFromESP32); SerialPC.println("\"");
            return true;  // Mensaje completo procesado
        }
        tempBuffer = "";  // Si estaba vacío, se resetea el buffer
    } 
    else {
        tempBuffer += c;  // Acumula el carácter en el buffer temporal
    }
    return false;  // Aún no se ha recibido un mensaje completo
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
    SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));

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

        // CANCELACIÓN: Para este programa de ejemplo vamos a ignorar la cancelación de la lectura de barcode

        delay(50);  // Evita que el bucle sea demasiado intensivo
    }


    // Si se alcanza el tiempo de espera sin recibir un mensaje
    SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));

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
    SerialPC.println(F("Comprobando la conexion WiFi del ESP32..."));
    
    sendMsgToESP32("CHECK-WIFI"); // Envía la cadena al ESP32
    // ---------------------------------------------------------

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 3000; // Espera máxima de 3 segundos
                                    // No hace falta esperar más tiempo porque esta es una comprobación directa del ESP32
                                    // y los mensajes que se pueden recibir "WIFI-OK" o "NO-WIFI" son cortos, por lo que no
                                    // debería tardar demasiado waitResponseFromESP32() en recibirlos.
    waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
    // --------------------------------------

    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    // --- EXITO ------
    if (msgFromESP32 == "WIFI-OK") // Respuesta OK, hay conexión WiFi
    {
        SerialPC.println(F("Dice que hay wifi"));
        return true;
    } 
    // -----------------
    // --- "ERRORES" ---
    else if (msgFromESP32 == "NO-WIFI") // Respuesta NO-WIFI, no hay conexión WiFi
    {
        SerialPC.println(F("Dice que NO hay wifi"));
        return false;
    }
    else if (msgFromESP32 == "TIMEOUT") // No se ha recibido respuesta del ESP32
    {
        SerialPC.println(F("TIMEOUT. Sin respuesta del ESP32 al comprobar la conexion WiFi"));
        return false; // Se considera que no hay conexión WiFi
    }
    else // Mensaje desconocido
    {
        SerialPC.println(F("Error desconocido al comprobar la conexion WiFi..."));
        return false; // Se considera que no hay conexión WiFi
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
    SerialPC.println(F("\nPidiendo escanear barcode..."));
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
    // Para este programa de ejemplo vamos a ignorar la cancelación de la lectura de barcode

    // --- EXITO ------
    if(msgFromESP32.startsWith("BARCODE:")) // "BARCODE:<barcode>"
    {
        barcode = msgFromESP32.substring(8);
        SerialPC.println("\nCodigo de barras leido: " + barcode);
        return BARCODE_READ; // Se ha leído el código de barras
    }
    // -----------------
    // --- "ERRORES" ---
    else if(msgFromESP32 == "NO-BARCODE") 
    {
        SerialPC.println(F("\nNo se ha podido leer el codigo de barras"));
        return BARCODE_NOT_READ; // No se ha detectado el código de barras
    }
    else if(msgFromESP32 == "TIMEOUT") // No se recibió nada en 'timeout' segundos en waitResponseFromESP32WithEvents()
    {
        SerialPC.println(F("TIMEOUT. Sin respuesta del ESP32 al pedir leer barcode\n"));
        return TIMEOUT; // Se considera que no se ha detectado el código de barras
    }
    else // Mensaje desconocido
    {
        SerialPC.println(F("\nMensaje o error desconocido al pedir leer barcode..."));
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
    SerialPC.println(F("\nPidiendo buscar producto..."));
    String msgToESP32 = "GET-PRODUCT:" + barcode; 
    sendMsgToESP32(msgToESP32); // Envía la cadena al ESP32
    // ---------------------------------------------------------
    

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 15000; // Espera máxima de 15 segundos (el ESP32 tiene 10 segundos para buscar el producto)
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
        //SerialPC.println("\nInformacion del producto: " + msgFromESP32);
        productInfo = msgFromESP32;
        return PRODUCT_FOUND;
    }
    // -----------------
    // --- ERRORES -----
    else if(msgFromESP32 == "NO-PRODUCT") 
    {
        SerialPC.print("\nNo se ha encontrado el producto " + barcode); SerialPC.println(" en OpenFoodFacts");
        return PRODUCT_NOT_FOUND;
    }
    else if(msgFromESP32 == "PRODUCT-TIMEOUT") 
    {
        SerialPC.println("\nEl servidor de OpenFoodFacts no responde");
        return PRODUCT_TIMEOUT;
    }
    else if(msgFromESP32.startsWith("HTTP-ERROR:")) 
    {
        String error = msgFromESP32.substring(11);
        SerialPC.println("Error al buscar info del producto (peticion HTTP GET): " + error);
        return HTTP_ERROR;
    }
    else if(msgFromESP32 == "TIMEOUT") // No se recibió nada en 'timeout' segundos en waitResponseFromESP32WithEvents()
    {
        SerialPC.println(F("TIMEOUT. Sin respuesta del ESP32 al pedir buscar info de producto\n"));
        return TIMEOUT; // Se considera que no se ha encontrado el producto
    }
    else // Mensaje desconocido
    {
        SerialPC.println("Mensaje no reconocido: " + msgFromESP32);
        return UNKNOWN_ERROR;
    }
    // -----------------
    // --------------------------------------
    // ---------------------------------------------------------
}




/******************************************************************************/
/******************************************************************************/

#endif