
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


#include "debug.h"
#define SerialESP32 Serial1 // Comunicación Serial con ESP32


bool isESP32Connected = false; // Flag para indicar si la comunicación con el ESP32 está establecida

// Barcode leido o no:
#define BARCODE_READ            0
#define BARCODE_NOT_READ        1
// Conexion con ESP32:
#define NO_ESP32_CONNECTION     2
// Product info found or not:
#define PRODUCT_FOUND           3
#define PRODUCT_NOT_FOUND       4
#define PRODUCT_TIMEOUT         5
// Los siguientes son los mismos que los de subir datos a la database:
#define NO_INTERNET_CONNECTION  6
#define HTTP_ERROR              7
#define TIMEOUT                 8
#define UNKNOWN_ERROR           9



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

inline bool     isTimeoutExceeded(unsigned long &startTime, unsigned long &timeout);    // Comprobar si se ha excedido el tiempo de espera
void            waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout);    // Espera la respuesta del ESP32, sea cual sea, y la devuelve en msgFromESP32.

byte            askForBarcode(String &barcode);                                 // Obtener el código de barras
byte            getProductInfo(String &barcode, String &productInfo);           // Obtener la información del producto a partir de un código de barras
void            getBarcodeAndProductInfo();                                     // Obtener el código de barras y la información del producto

void            showProductInfo(String &productInfo); // Muestra la información del producto
// -------------
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

    // Limpiar Serial con ESP32
    //while (SerialESP32.available() > 0) SerialESP32.read();

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
        /*#if defined(SM_DEBUG)
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

    // Limpiar Serial con ESP32
    //while (SerialESP32.available() > 0) SerialESP32.read();
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
    SerialPC.println(F("\nPulsando boton barcode..."));
    sendMsgToESP32("GET-BARCODE"); // Envía la cadena al ESP32
    // ---------------------------------------------------------

    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 10500; // Espera máxima de 10.5 segundos (el tiempo que da el ESP32 para colocar el producto)
    waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
    // --------------------------------------

    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    // --- EXITO ------
    if(msgFromESP32.startsWith("BARCODE:")) 
    {
        barcode = msgFromESP32.substring(8);
        SerialPC.println("Codigo de barras leido: " + barcode);
        return BARCODE_READ;
    }
    // -----------------
    // --- "ERRORES" ---
    else if(msgFromESP32 == "NO-BARCODE") 
    {
        SerialPC.println("\nNo se ha podido leer el codigo de barras");
        return BARCODE_NOT_READ;
    }
    else if(msgFromESP32 == "NO-WIFI") 
    {
        SerialPC.println("\nNo hay wifi");
        return NO_INTERNET_CONNECTION;
    }
    else if(msgFromESP32 == "TIMEOUT") return TIMEOUT; // No se ha recibido respuesta del ESP32
    else // Mensaje desconocido
    {
        SerialPC.println(F("\nError desconocido al pedir leer barcode..."));
        return BARCODE_NOT_READ;
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
    SerialPC.println("\nBuscando informacion del producto: " + barcode);
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
        SerialPC.println("\nInformacion del producto: " + msgFromESP32);
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
    else if(msgFromESP32.startsWith("ERROR-HTTP:")) 
    {
        String error = msgFromESP32.substring(11);
        SerialPC.println("Error al buscar info del producto (peticion HTTP POST): " + error);
        return HTTP_ERROR;
    }
    else if(msgFromESP32 == "NO-WIFI") 
    {
        SerialPC.println("No hay wifi");
        return NO_INTERNET_CONNECTION;
    }
    else if(msgFromESP32 == "TIMEOUT") return TIMEOUT; // No se ha recibido respuesta del ESP32
    else // Mensaje desconocido
    {
        SerialPC.println("Mensaje no reconocido: " + msgFromESP32);
        return UNKNOWN_ERROR;
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
    if(isESP32Connected) // La comunicación con el ESP32 está establecida
    {   
        String barcode;
        // 1. Obtener barcode
        byte resultFromReadingBarcode = askForBarcode(barcode);

        // 2. Si se ha leído el barcode, buscar información del producto
        if(resultFromReadingBarcode == BARCODE_READ)
        { 
            String productInfo;
            byte resultFromGettingProductInfo = getProductInfo(barcode, productInfo); // Buscar info del producto en OpenFoodFacts
            if(resultFromGettingProductInfo == PRODUCT_FOUND) showProductInfo(productInfo);
        }
        else SerialPC.println(F("No se va a buscar informacion del producto porque no se ha leido el barcode"));

    }
    else // La comunicación con el ESP32 no está establecida
        SerialPC.println(F("\nNo se va a leer barcode porque no hay conexion con ESP32"));

}




/*-----------------------------------------------------------------------------*/
/**
 * Muestra la información del producto.
 * 
 * Esta función toma una cadena de información del producto y muestra los detalles del producto en el monitor serie.
 * 
 * @param productInfo La cadena de información del producto que contiene los detalles del producto.
 */
/*-----------------------------------------------------------------------------*/
void showProductInfo(String &productInfo) 
{
    String cad = productInfo.substring(8); // Elimina el prefijo "PRODUCT:"

    int idx_nombre = cad.indexOf(';');
    int idx_carb = cad.indexOf(';', idx_nombre + 1);
    int idx_lip = cad.indexOf(';', idx_carb + 1);
    int idx_prot = cad.indexOf(';', idx_lip + 1);
    int idx_kcal = cad.indexOf(';', idx_prot + 1);

    String barcode = cad.substring(0, idx_nombre);
    String nombre_producto = cad.substring(idx_nombre + 1, idx_carb);
    String carb_1g = cad.substring(idx_carb + 1, idx_lip);
    String lip_1g = cad.substring(idx_lip + 1, idx_prot);
    String prot_1g = cad.substring(idx_prot + 1, idx_kcal);
    String kcal_1g = cad.substring(idx_kcal + 1);

    SerialPC.println("Codigo: " + barcode);
    SerialPC.println("Nombre: " + nombre_producto);
    SerialPC.println("Carb_1g: " + carb_1g);
    SerialPC.println("Lip_1g: " + lip_1g);
    SerialPC.println("Prot_1g: " + prot_1g);
    SerialPC.println("Kcal_1g: " + kcal_1g);
}

/******************************************************************************/
/******************************************************************************/

#endif