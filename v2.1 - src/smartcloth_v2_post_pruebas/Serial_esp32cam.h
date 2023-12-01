
/** 
 * @file Serial_esp32cam.h
 * @brief Comunicación Serial con ESP32-CAM
 *
 * @author Irene Casares Rodríguez
 * @date 30/10/23
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

  1. Guardar info en base de datos:
      "SAVE:&carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X"
      Esta opción ya está lista para la petición POST

      Proxima opción: "SAVE:<grupoX>-<peso>;<grupoY>-<peso>;<grupoZ>-<peso>;...." 
          Con esta opción se guardarían los pares 'grupoX-peso' en la base de datos y se calcularían los valores 
          de macronutrientes en el servidor al hacer GET para la wev. De esta forma, se podría llevar una trazabilidad 
          de lo comido exactamente (a nivel grupo).

  2. Activar cámara y leer código de barras:
      "GET-BARCODE"

  
  -------- MENSAJES ESP32 -> ARDUINO -------------- 

  1. Guardado correctamente:
      "SAVED-OK"
  
  2. Error en el guardado (petición HTTP POST):
      "ERROR-HTTP:<codigo_error>"
  
  3. No hay wifi:
      "NO-WIFI"

  4. Código de barras leído (se consulta base de datos de alimentos y se obtienen sus valores de carb, lip, prot y kcal por gramo):
      "BARCODE:<codigo_barras_leido>:<carb>;<prot>;<kcal>"


*/

#ifndef SERIAL_ESP32CAM_H
#define SERIAL_ESP32CAM_H

#include "Comida.h" // comidaActual

#define SerialDueESP32 Serial1

// --- RESPUESTAS AL GUARDAR EN DATABASE ---
#define  SAVED_OK     1
#define  ERROR_HTTP   2
#define  NO_WIFI      3
#define  TIMEOUT      4
#define  UNKNOWN      5


// --- MENSAJE RECIBIDO DEL ESP32CAM ---
struct Message {
  String command;
  String data;
};


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
// -- Guardar comida en database ---
int saveComidaDatabase();
// -- Obtener partes del mensaje del ESP32 ---
Message getQuery(String msg);
/*-----------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------
   saveComidaDatabase(): Envía al ESP32CAM los valores de la comida con el comando "SAVE" para que los
                         guarde en la base de datos. Después espera la respuesta del ESP32CAM, la procesa
                         separando la parte del comando de los datos adjuntos, si los hay, y devuelve un valor
                         según la respuesta dada:
                          - SAVED-OK --> guardado correcto --> return SAVED_OK
                          - ERROR-HTTP:<codigo_error> --> error HTTP en la petición --> ERROR_HTTP (no procesamos cada error por separado)
                          - NO-WIFI --> no hay conexión a internet --> 
          Parámetros:
                - period --> período de tiempo que se quiere esperar pero "escuchando" a las interrupciones.
          Return:
                - true --> ha habido interrupcion durante algún delay
                - false --> no ha habido interrupción durante ningún delay
----------------------------------------------------------------------------------------------------------*/
int saveComidaDatabase(){
    Serial.println("Avisando al ESP32 para que guarde los datos...");

    String comidaValues = comidaActual.getComidaAllValuesHttpRequest(); // &carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X
    // No incluimos el tiempo del RTC porque ya se obtiene en el servidor
    String command = "SAVE:" + comidaValues;

    Serial.print("Cadena enviada al esp32: "); Serial.println(command);
    SerialDueESP32.print(command); //Envía la cadena al esp32


    unsigned long timeout = 30000;  // Espera máxima de 30 segundos
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    while (true) { // Bloquea el arduino en este bucle hasta que se reciba respuesta o se pase el TIMEOUT
        // Comprueba si hay datos disponibles en el puerto serie del ESP32
        if (SerialDueESP32.available() > 0) { // Si el esp32 ha respondido
            String responseFromESP32 = SerialDueESP32.readStringUntil('\n');
            Serial.print("Respuesta del ESP32: ");  Serial.println(responseFromESP32);

            Message msgReceived = getQuery(responseFromESP32);
            
            if (msgReceived.command == "SAVED-OK") {
                // Respuesta OK, el guardado fue exitoso
                return SAVED_OK;
            } 
            else if (msgReceived.command == "ERROR-HTTP:") {
                // Respuesta ERROR HTTP, hubo un error en el ESP32
                // ¿Tratamos diferente cada error o simplemente decimos que no se ha guardado?
                return ERROR_HTTP;
            }
            else if (msgReceived.command == "NO-WIFI"){
                // Respuesta fallo en conexion wifi, hubo un error en el ESP32
                return NO_WIFI;
            }
            else {
                // Respuesta desconocida
                return UNKNOWN;
            }
        }

        // Comprueba si ha pasado un tiempo límite sin respuesta del esp32
        if (millis() - startTime > timeout) {
            // Tiempo de espera agotado, se considera que ha fallado el guardado
            return TIMEOUT;
        }
    }

    //Devolver true si todo OK o false si fallo o error al subirlo 
    return UNKNOWN;

}




Message getQuery(String msg){
    // Objeto 'Message'
    Message myMessage;
    
    // Busca el carácter ':' en el mensaje
    int delimiterIndex = msg.indexOf(':');
    
    // Si existe el delimitador en la cadena:
    if (delimiterIndex != -1) { 
        myMessage.command = msg.substring(0, delimiterIndex + 1); // "ERROR-HTTP:" o "BARCODE:"
        myMessage.data = msg.substring(delimiterIndex + 1); // "<codigo_error>" o "<codigo_barras_leido>:<carb>;<prot>;<kcal>"
        
        Serial.print("Comando recibido: "); Serial.println(myMessage.command);
        Serial.print("Datos recibidos: "); Serial.println(myMessage.data);
        Serial.println();
    }
    // Si no hay ':' puede ser SAVED-OK o NO-WIFI, así que se guardan como comando (command) 
    // y se dejan en blanco los datos adjuntos (data)
    else { 
        myMessage.command = msg;
        myMessage.data = "";
    }

    return myMessage;
}





#endif