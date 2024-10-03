
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
                "INICIO-COMIDA" "INICIO-PLATO" "ALIMENTO,9,54.35" ... --> sendFileToEsp32MealByMeal()

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
            "ERROR-HTTP:<codigo_error>"

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
                "ERROR-HTTP:<codigo_error>"

            11) El servidor de OpenFoodFacts no responde:
                "PRODUCT-TIMEOUT"


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

#include "Comida.h" // comidaActual
#include "SD_functions.h"
#include "debug.h" // SM_DEBUG --> Comunicación Serial con PC

//void    checkBascula();           // Está en Scale.h, pero hay que declararla aquí también para que esté en este ámbito
//bool    interruptionOccurred();   // Está en ISR.h, pero hay que declararla aquí también para que esté en este ámbito
bool eventOccurred();
#include "Scale.h" // checkBascula()
#include "ISR.h" // eventOccurred()
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
// Setup Serial PC
void            setupSerialPC();                                    // Configurar comunicación Serial con PC

// Comunicación Serial Due-ESP32
void            setupSerialESP32();                                 // Configurar comunicación Serial con ESP32
inline bool     hayMsgFromESP32() { return SerialESP32.available() > 0; };      // Comprobar si hay mensajes del ESP32 disponibles
inline bool     isESP32SerialEmpty(){ return !hayMsgFromESP32(); }              // Comprobar si no hay mensajes del ESP32 disponibles
inline void     readMsgFromSerialESP32(String &msgFromESP32);                   // Leer mensaje del puerto serie Due-ESP32 y guardarlo en msgFromESP32
//inline void     limpiarBufferESP32();                                           // Limpiar buffer con el ESP32
inline void     sendMsgToESP32(const String &msg);                              // Enviar mensaje al ESP32

// Esperar mensaje de ESP32
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
inline void readMsgFromSerialESP32(String &msgFromESP32) 
{ 
    msgFromESP32 = SerialESP32.readStringUntil('\n'); 
    msgFromESP32.trim();  // Elimina espacios en blanco al principio y al final
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Limpia el buffer de datos del ESP32.
 *
 * Esta función vacía el buffer de entrada del ESP32 leyendo todos los datos disponibles.
 * Luego, introduce un pequeño retraso para asegurar que el ESP32 tenga tiempo de procesar
 * y leer cualquier mensaje que se envíe después.
 */
/*-----------------------------------------------------------------------------*/
/*inline void limpiarBufferESP32()
{ 
    while(SerialESP32.available() > 0) { SerialESP32.read(); } // Limpiar buffer con el ESP32
    delay(100);  // Pequeño retraso para asegurar que el ESP32 tenga tiempo de leer el mensaje que se envíe después   
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
    //limpiarBufferESP32(); // Limpiar buffer con ESP32
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



/*---------------------------------------------------------------------------------------------------------*/
/**
 * Espera la respuesta del ESP32 durante un tiempo determinado.
 * 
 * @param msgFromESP32 Referencia a un objeto String donde se almacenará el mensaje recibido del ESP32.
 * @param timeout Tiempo máximo de espera en milisegundos.
 */
/*---------------------------------------------------------------------------------------------------------*/
/*void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje no vacío, si se 
    // pasa el tiempo de espera, o si ocurre un evento
    //
    // A veces se detectaba algo en el buffer y se salía del bucle, pero en realidad era una cadena
    // vacía, entonces el Due no se quedaba esperando a que el ESP32 buscara y respondiera con la 
    // información del producto. Por eso, se cambia la condición del bucle para que se quede esperando
    // hasta que se reciba un mensaje no vacío.
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromESP32())  // Si el esp32 ha respondido
        {
            readMsgFromSerialESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
            #if defined(SM_DEBUG)
                SerialPC.println("--> Respuesta del ESP32: " + msgFromESP32);  
            #endif
            
            if (msgFromESP32.length() > 0) // Mensaje no vacío
            {
                #if defined(SM_DEBUG)
                    SerialPC.println("Mensaje no vacío del ESP32");  
                #endif
                return; // Salir de la función si se recibe un mensaje no vacío
            }
        }

        delay(50); // Pequeño retraso para evitar una espera activa intensa
    }

    // Si se alcanza el tiempo de espera sin recibir un mensaje no vacío
    #if defined(SM_DEBUG)
        SerialPC.println(F("TIMEOUT del ESP32"));
    #endif

    // Con TIMEOUT se considera que no hay conexión WiFi
    msgFromESP32 = "TIMEOUT";

}*/
// A veces se leen mensajes vacíos del buffer, aunque el ESP32 los haya enviado correctamente. 
// Por eso se comprobaba que msgFromESP32.length() > 0 para asegurarse de que se había recibido un mensaje no vacío.
// Pero eso parece que no resuelve nada porque puede ser que el ESP32 envíe un mensaje correcto pero se reciba mal,
// así que vamos a ir comprobando caracter a caracter hasta recibir "\n" y entonces considerar que se ha recibido un mensaje.
/*void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje no vacío, si se 
    // pasa el tiempo de espera, o si ocurre un evento
    //
    // A veces se detectaba algo en el buffer y se salía del bucle, pero en realidad era una cadena
    // vacía, entonces el Due no se quedaba esperando a que el ESP32 buscara y respondiera con la 
    // información del producto. Por eso, se cambia la condición del bucle para que se quede esperando
    // hasta que se reciba un mensaje no vacío.
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (hayMsgFromESP32())  // Si el esp32 ha respondido
        {
            // Vamos a ir comprobando caracter a caracter hasta recibir "\n" y entonces 
            // considerar que se ha recibido un mensaje.

            char c = SerialESP32.read(); // Leer un caracter del puerto serie
            if (c == '\n') // Si se recibe un salto de línea
            {
                msgFromESP32.trim(); // Eliminar espacios en blanco al principio y al final
                #if defined(SM_DEBUG)
                    SerialPC.println("Mensaje del ESP32: " + msgFromESP32);  
                #endif

                if (msgFromESP32.length() > 0) // Mensaje no vacío
                {
                    #if defined(SM_DEBUG)
                        SerialPC.println("Mensaje no vacío del ESP32");  
                    #endif
                    return; // Salir de la función si se recibe un mensaje no vacío
                }
            }
            else
            {
                msgFromESP32 += c; // Añadir el caracter leído al mensaje
                #if defined(SM_DEBUG)
                    SerialPC.println("Msg ESP32: " + msgFromESP32);  
                #endif
            }
            
        }

        delay(50); // Pequeño retraso para evitar una espera activa intensa
    }

    // Si se alcanza el tiempo de espera sin recibir un mensaje no vacío
    #if defined(SM_DEBUG)
        SerialPC.println(F("TIMEOUT del ESP32"));
    #endif

    msgFromESP32 = "TIMEOUT";

}*/
// Otra prueba para hacerlo parecido a waitMsgFromDue() del ESP32, porque esa función toma el mensaje del Due
// perfectamente cada vez.
void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(!hayMsgFromESP32() && !isTimeoutExceeded(startTime, timeout));

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromESP32())  // Si el Due ha respondido
    {
        readMsgFromSerialESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
        #if defined(SM_DEBUG)
            SerialPC.println("\n---> Respuesta del ESP32: " + msgFromESP32);  
        #endif
    } 
    else // No se ha recibido respuesta del ESP32
    {
        // Si se alcanza el tiempo de espera sin recibir un mensaje no vacío
        #if defined(SM_DEBUG)
            SerialPC.println(F("TIMEOUT del ESP32"));
        #endif

        msgFromESP32 = "TIMEOUT";
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
/*void waitResponseFromESP32WithEvents(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje no vacío, si se 
    // pasa el tiempo de espera, o si ocurre un evento
    //
    // A veces se detectaba algo en el buffer y se salía del bucle, pero en realidad era una cadena
    // vacía, entonces el Due no se quedaba esperando a que el ESP32 buscara y respondiera con la 
    // información del producto. Por eso, se cambia la condición del bucle para que se quede esperando
    // hasta que se reciba un mensaje no vacío.
    while (!isTimeoutExceeded(startTime, timeout)) 
    {
        if (eventOccurred()) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Interrupcion mientras se leia barcode"));
            #endif
            msgFromESP32 = "INTERRUPTION"; // Señalar que se ha interrumpido
            return;
        }

        if (hayMsgFromESP32())  // Si el esp32 ha respondido
        {
            readMsgFromSerialESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
            
            if (msgFromESP32.length() > 0) {
                #if defined(SM_DEBUG)
                    SerialPC.println("\n--> Respuesta del ESP32: " + msgFromESP32);  
                #endif
                return; // Salir de la función si se recibe un mensaje no vacío
            }
            #if defined(SM_DEBUG)
                SerialPC.println("\nRespuesta vacía del ESP32: " + msgFromESP32);  
            #endif
        }

        delay(50); // Pequeño retraso para evitar una espera activa intensa
    }

    // Si se alcanza el tiempo de espera sin recibir un mensaje no vacío
    #if defined(SM_DEBUG)
        SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
    #endif

    // Se considera que no hay conexión WiFi
    msgFromESP32 = "TIMEOUT";
}*/


// Prueba para hacerlo parecido a waitMsgFromDue() del ESP32, porque esa función toma el mensaje del Due
// perfectamente cada vez.
void waitResponseFromESP32WithEvents(String &msgFromESP32, unsigned long &timeout)
{
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    // Esperar 'timeout' segundos a que el ESP32 responda. Sale si se recibe mensaje o si se pasa el tiempo de espera
    while(!hayMsgFromESP32() && !isTimeoutExceeded(startTime, timeout))
    {
        if(eventOccurred()) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Interrupcion mientras se leia barcode"));
            #endif
            msgFromESP32 = "INTERRUPTION"; // Señalar que se ha interrumpido
            return; // Salir de la función si se detecta interrupción (cancelación manual de la lectura)
        }
        delay(50); // Pequeño retraso para evitar una espera activa intensa
    }

    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    if (hayMsgFromESP32())  // Si el esp32 ha respondido
    {
        readMsgFromSerialESP32(msgFromESP32); // Leer mensaje del puerto serie y guardarlo en msgFromESP32
        #if defined(SM_DEBUG)
            SerialPC.println("\n---> Respuesta del ESP32: " + msgFromESP32);  
        #endif
    } 
    else // No se ha recibido respuesta del ESP32
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32 durante lectura de barcode"));
        #endif

        // Se considera que no se ha detectado barcode
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
bool checkWifiConnection() 
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    //limpiarBufferESP32();
    // ---------------------------------------------------------

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
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    //limpiarBufferESP32();
    // ---------------------------------------------------------

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
    unsigned long timeout = 12000; // Espera máxima de 12 segundos al ESP32 (espera 10 segundos la respuesta del servidor al pedir token)
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
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    //limpiarBufferESP32();
    //delay(100);
    // ---------------------------------------------------------

    // ---- PEDIR LEER BARCODE ---------------------------------
    #ifdef SM_DEBUG
        SerialPC.println(F("\nPidiendo escanear barcode..."));
    #endif
    sendMsgToESP32("GET-BARCODE"); // Envía la cadena al ESP32
    // ---------------------------------------------------------


    // ---- RESPUESTA DEL ESP32 --------------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32 = "";
    unsigned long timeout = 32000; // Espera máxima de 32 segundos (el ESP32 da 30 segundos para colocar el producto)
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
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    //limpiarBufferESP32();
    // ---------------------------------------------------------

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
    unsigned long timeout = 12000; // Espera máxima de 12 segundos (el ESP32 tiene 10 segundos para buscar el producto)
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
    else if(msgFromESP32.startsWith("ERROR-HTTP:")) 
    {
        #ifdef SM_DEBUG
            String error = msgFromESP32.substring(11);
            SerialPC.println("Error al buscar info del producto (peticion HTTP POST): " + error);
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