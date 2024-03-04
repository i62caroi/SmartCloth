
/** 
 * @file Serial_esp32cam.h
 * @brief Comunicación Serial con ESP32-CAM
 *
 * @author Irene Casares Rodríguez
 * @date 05/02/24
 * @version 1.0
 *
 * Se ha utilizado el módulo ESP32-CAM:
 *    SPI Flash: default 32Mbit
 *    RAM: built-in 520 KB+external 4MPSRAM
 *    Bluetooth: Bluetooth 4.2 BR/EDR and BLE standards 
 *    Wi-Fi: 802.11b/g/n/e/i
 *    Support Interface: UART, SPI, I2C, PWM
 *    Image Output Format: JPEG( OV2640 support only ), BMP, GRAYSCALE
 *    Power supply range: 5V
 *
 * @see https://media.digikey.com/pdf/Data%20Sheets/DFRobot%20PDFs/DFR0602_Web.pdf
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


  -------- MENSAJES ARDUINO -> ESP32 --------------

    1. Check wifi:
        "CHECK-WIFI"

    2. Guardar info en base de datos:
        "SAVE:&carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X"
        Esta opción ya está lista para la petición POST

        2.1. Indicar guardado:
        "SAVE"
        2.2. Mandar datos a guardar:
        "INICIO-COMIDA" "INICIO-PLATO" "ALIMENTO,9,54.35" ... --> sendFileToEsp32()

    3. Activar cámara y leer código de barras:
        "GET-BARCODE"

  
  -------- MENSAJES ESP32 -> ARDUINO -------------- 

    1. Hay wifi:
        "WIFI"

    2. No hay wifi:
        "NO-WIFI"

    3. Esperando datos a subir:
        "WAITING-FOR-DATA"

    4. Guardado correctamente:
        "SAVED-OK"

    5. Error en el guardado (petición HTTP POST):
        "ERROR-HTTP:<codigo_error>"

    6. Código de barras leído (se consulta base de datos de alimentos y se obtienen sus valores de carb, lip, prot y kcal por gramo):
        "BARCODE:<codigo_barras_leido>:<carb>;<prot>;<kcal>"


*/

#ifndef SERIAL_ESP32CAM_H
#define SERIAL_ESP32CAM_H

#include "Comida.h" // comidaActual

// Sincronización SmartCloth con web
#define  NO_INTERNET_CONECTION  2
#define  UPLOADING_DATA         3
#define  UPLOADED_DATA          4
#define  HTTP_ERROR             5


#include "SD_functions.h"

#include "debug.h" // SM_DEBUG --> SerialPC
#define SerialDueESP32 Serial1

// --- Fases en las respuestas del ESP32 ---
#define FASE_1_WAITING   1   // WAITING-FOR-DATA
#define FASE_2_AL_SUBIR  2   // SAVED-OK, NO-WIFI y ERROR-HTTP:<codigo_error>


void showDataToUpload(byte option);


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    checkWifiConnection();  // Comprobar conexion a internet
String  waitResponseFromESP32(byte phase);       // Espera la respuesta del ESP32 y la devuelve. En la fase 1 espera WAITING-FOR-DATA, 
                                                // pero en la fase 2 espera SAVED-OK, NO-WIFI o ERROR-HTTP:<codigo_error>
bool    prepareSaving();                        // Indica al ESP32 que se le va a enviar info y espera su respuesta WAITING-FOR-DATA

byte    handleResponseFromESP32(byte option);    // Maneja la respuesta del ESP32 y actúa acorde
/*-----------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Comprueba la conexión WiFi del ESP32.
 * 
 * @return true si hay conexión WiFi, false si no la hay o por TIMEOUT.
 */
/*---------------------------------------------------------------------------------------------------------*/
bool checkWifiConnection() 
{
    #if defined(SM_DEBUG)
    SerialPC.println(F("Comprobando la conexión WiFi del ESP32..."));
    SerialPC.println(F("Cadena enviada al esp32: CHECK-WIFI")); 
    #endif
    
    SerialDueESP32.print(F("CHECK-WIFI")); //Envía la cadena al esp32

    unsigned long timeout = 3000;        // Espera máxima de 3 segundos
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    while (true) { // Bloquea el arduino en este bucle hasta que se reciba respuesta o se pase el TIMEOUT
        // Comprueba si hay datos disponibles en el puerto serie del ESP32
        if (SerialDueESP32.available() > 0) { // Si el esp32 ha respondido
            String msgFromESP32 = SerialDueESP32.readStringUntil('\n');
            msgFromESP32.trim();  // Elimina espacios en blanco al principio y al final
            #if defined(SM_DEBUG)
            SerialPC.print(F("Respuesta del ESP32: ")); SerialPC.println(msgFromESP32);  
            #endif

            if (msgFromESP32 == "WIFI-OK")      return true;
            else if (msgFromESP32 == "NO-WIFI") return false;
        }

        // Comprueba si ha pasado un tiempo límite sin respuesta del esp32
        if (millis() - startTime > timeout) {
            // Tiempo de espera agotado, se considera que no hay conexión WiFi
            #if defined(SM_DEBUG)
            SerialPC.println(F("El ESP32 no ha respondido en 3 segundos. TIMEOUT..."));
            #endif
            return false;
        }
    }

    // Si sale del while (no debería), se considera que no hay conexión WiFi
    return false;
}




/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Espera la respuesta del ESP32.
 * 
 * Esta función espera a que el ESP32 envíe una respuesta que puede ser "WAITING-FOR-DATA", "SAVED-OK", "HTTP-ERROR:<error>" o "NO-WIFI".
 * Bloquea el Arduino hasta que se reciba una respuesta válida.
 * 
 * @return La respuesta recibida del ESP32.
 */
/*---------------------------------------------------------------------------------------------------------*/
String waitResponseFromESP32(byte phase)
{
    String msgFromESP32 = "";
    unsigned long startTime = millis();

    // Con este while bloqueamos el Arduino (intencionadamente) para que espere la respuesta
    if(phase == FASE_1_WAITING){
        while (msgFromESP32 != "WAITING-FOR-DATA") {
            if (SerialDueESP32.available() > 0) {
                msgFromESP32 = SerialDueESP32.readStringUntil('\n');
                msgFromESP32.trim();
                #if defined(SM_DEBUG)
                SerialPC.println(F("Mensaje del ESP32: ")); SerialPC.println(msgFromESP32); 
                #endif
            }
            if (millis() - startTime > 30000) { // Si han pasado 30 segundos sin respuesta
                #if defined SM_DEBUG
                SerialPC.println(F("El ESP32 no ha respondido en 30 segundos. TIMEOUT..."));
                #endif
                return "TIMEOUT";
            }
        }
    }
    else if(phase == FASE_2_AL_SUBIR){
        while ((msgFromESP32 != "SAVED-OK") && (!msgFromESP32.startsWith("ERROR-HTTP:")) && (msgFromESP32 != "NO-WIFI")) {
            if (SerialDueESP32.available() > 0) {
                msgFromESP32 = SerialDueESP32.readStringUntil('\n');
                msgFromESP32.trim();
                #if defined(SM_DEBUG)
                SerialPC.println(F("Mensaje del ESP32: ")); SerialPC.println(msgFromESP32); 
                #endif
            }
            if (millis() - startTime > 30000) { // Si han pasado 30 segundos sin respuesta
                #if defined SM_DEBUG
                SerialPC.println(F("El ESP32 no ha respondido en 30 segundos. TIMEOUT..."));
                #endif
                return "TIMEOUT";
            }
        }
    }

    return msgFromESP32;
}


/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Prepara el envío de datos al ESP32 para su guardado.
 * 
 * Esta función indica al ESP32 que se le va a enviar información y espera a que el ESP32 responda "WAITING-FOR-DATA".
 * 
 * @note Esta función bloquea el Arduino intencionadamente hasta recibir la respuesta del ESP32.
 * 
 * HAY QUE MODIFICAR EL WHILE CON UN TIMEOUT POR SI SE HA PERDIDO LA COMUNICACIÓN CON EL ESP32
 */
/*---------------------------------------------------------------------------------------------------------*/
bool prepareSaving()
{
    // ---- Indicar envío de datos con "SAVE" ------------------------------------
    #if defined(SM_DEBUG)
    SerialPC.println(F("\nIndicando que se quiere guardar..."));
    #endif
    SerialDueESP32.println(F("SAVE")); // Indicar al ESP32 que se le va a enviar información
    // ---------------------------------------------------------------------------

    // ---- Esperar a que el ESP32 responda "WAITING-FOR-DATA" -------------------
    String response = waitResponseFromESP32(FASE_1_WAITING); // En la fase 1, sale del while si recibe WAITING-FOR-DATA
    if(response == "WAITING-FOR-DATA"){ // El ESP32 queda a la espera de la info
        #if defined SM_DEBUG
        SerialPC.println(F("El ESP32 está esperando la info..."));
        #endif
        return true;
    }
    else if(response = "TIMEOUT") // No se recibió nada en 30 segundos
        return false;
    // ---------------------------------------------------------------------------

    return false; // Para cubrir todos los caminos
}



/*---------------------------------------------------------------------------------------------------------*/
/**
 * @brief Maneja la respuesta recibida desde el ESP32 después de enviar una solicitud.
 * 
 * @param option La opción de visualización de datos.
 *               Puede ser SHOW_SCREEN_UPLOAD_DATA para mostrar los datos a subir o
 *               cualquier otro valor para no mostrar los datos.
 */
/*---------------------------------------------------------------------------------------------------------*/
byte handleResponseFromESP32(byte option)
{
    String msgFromESP32 = waitResponseFromESP32(FASE_2_AL_SUBIR); // En la fase 2, sale del while si recibe SAVED-OK, HTTP-ERROR: o NO-WIFI

    if (msgFromESP32 == "SAVED-OK") // JSON subido a la base de datos
    {
        deleteFileESP32(); // Borrar fichero porque ya se ha subido su info
        if(option == SHOW_SCREEN_UPLOAD_DATA) showDataToUpload(UPLOADED_DATA);

        #if defined(SM_DEBUG)
        SerialPC.println(F("JSON ok y borrando fichero TXT..."));
        SerialPC.println(F("\nPaso a Init tras subir la info y borrar TXT..."));
        #endif

        return SAVE_EXECUTED_FULL;
    } 
    else if (msgFromESP32.startsWith("ERROR-HTTP:")) // La subida no ha sido exitosa 
    { 
        if(option == SHOW_SCREEN_UPLOAD_DATA) showDataToUpload(HTTP_ERROR);
        else if(option == NOT_SHOW_SCREEN_UPLOAD_DATA){ // La opción NOT_SHOW_SCREEN_UPLOAD_DATA se usa en el STATE_saved para no mostrar el proceso.
                                                        // Si se ha intentado enviar la lista de la comida pero ha habido error.
            #if defined SM_DEBUG
            SerialPC.println(F("Guardando la lista en el TXT hasta que el ESP32 pueda subir la info..."));
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
        if(option == SHOW_SCREEN_UPLOAD_DATA) showDataToUpload(NO_INTERNET_CONECTION);
        else if(option == NOT_SHOW_SCREEN_UPLOAD_DATA){ // La opción NOT_SHOW_SCREEN_UPLOAD_DATA se usa en el STATE_saved para no mostrar el proceso.
                                                        // Si se ha intentado enviar la lista de la comida pero no hay WiFi.
            #if defined SM_DEBUG
            SerialPC.println(F("Guardando la lista en el TXT hasta que el ESP32 pueda subir la info..."));
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
        if(option == SHOW_SCREEN_UPLOAD_DATA) showDataToUpload(NO_INTERNET_CONECTION);
        else if(option == NOT_SHOW_SCREEN_UPLOAD_DATA){ // La opción NOT_SHOW_SCREEN_UPLOAD_DATA se usa en el STATE_saved para no mostrar el proceso.
                                                        // Si se ha intentado enviar la lista de la comida, pero ha habido error.
            #if defined SM_DEBUG
            SerialPC.println(F("Guardando la lista en el TXT hasta que el ESP32 pueda subir la info..."));
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
}



#endif