
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
      "SAVE-&carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X"
      Esta opción ya está lista para la petición POST

      Proxima opción: "SAVE:<grupoX>-<peso>;<grupoY>-<peso>;<grupoZ>-<peso>;...." 
          Con esta opción se guardarían los pares 'grupoX-peso' en la base de datos y se calcularían los valores 
          de macronutrientes en el servidor al hacer GET. De esta forma, se podría llevar una trazabilidad de
          lo comido exactamente (a nivel grupo).

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
      "BARCODE-<codigo_barras_leido>:<carb>;<prot>;<kcal>"


*/

#ifndef SERIAL_ESP32CAM_H
#define SERIAL_ESP32CAM_H

#define SerialDueESP32 Serial1

#include "random_values.h" // Generar valores aleatorios y conformar query



struct Message {
    String command;
    String command_or_code;
    String data;
};

Message getInfoFromESP32Msg(String msg);
bool saveComidaDatabase();



bool saveComidaDatabase(){
    Serial.println("Avisando al ESP32 para que guarde los datos...");

    // Obtener datos
    String msgToESP32 = generarValores_y_generarQuery();
    /* // Solo para SmartCloth
    String comidaValues = comidaActual.getComidaAllValuesHttpRequest(); // &carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X
    // No incluimos el tiempo del RTC porque ya se obtiene en el servidor
    String command = "SAVE " + comidaValues;
    */

    Serial.print("Mensaje a enviar al esp32: "); Serial.println(msgToESP32); // "SAVE"
    SerialDueESP32.print(msgToESP32); //Envía la cadena al esp32


    //TODO
    //Esperar respuesta OK o FALLO del esp32

    if (SerialDueESP32.available() > 0) {
        String msgFromESP32 = SerialDueESP32.readStringUntil('\n');

        Serial.print("Mensaje recibido: "); Serial.println(msgFromESP32); 
    }

    unsigned long TIMEOUT = 30000;  // Espera máxima de 30 segundos
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    while (true) { // Bloquea el arduino en este bucle hasta que se reciba respuesta o se pase el TIMEOUT
        
        // Comprueba si hay datos disponibles en el puerto serie del ESP32
        if (SerialDueESP32.available() > 0) { // Si el esp32 ha respondido
            
            String responseFromESP32 = SerialDueESP32.readStringUntil('\n');
            responseFromESP32.trim(); // Elimina espacios en blanco y caracteres de nueva línea alrededor
            Serial.print("Respuesta del ESP32: ");  Serial.println(responseFromESP32);
            
            if (responseFromESP32 == "SAVED-OK") {
                // Respuesta OK, el guardado fue exitoso
                Serial.println("La comida se ha guardado correctamente.");
                return true;
            } 
            else if (responseFromESP32 == "NO-WIFI") {
                Serial.println("No se ha guardado la comida porque no hay conexión WiFi");
                return false;
            }
            else{ // si no es ninguno de los anteriores, debemos separarlo

                Message msgReceived = getInfoFromESP32Msg(responseFromESP32);

                if (msgReceived.command == "ERROR-") {
                    // Respuesta ERROR, hubo un error en el ESP32
                    Serial.print("No se ha guardado la comida. ERROR HTTP:"); Serial.println(msgReceived.data); Serial.println();
                    return false;
                }
                else {
                    Serial.print("Mensaje no gestionado: "); Serial.print("\""); Serial.print(responseFromESP32); Serial.println("\""); 
                    return false;
                }
            }
        } 

        // Comprueba si ha pasado un tiempo límite (puedes ajustar el valor)
        if (millis() - startTime > TIMEOUT) {
            // Tiempo de espera agotado, se considera un error
            return false;
        }
    }

    //Devolver true si todo OK o false si fallo al subirlo porque no hubiera wifi o error del servidor o algo
    return true;

}



Message getInfoFromESP32Msg(String msg){
    // Objeto 'Message'
    Message myMessage;
    
    // Busca el carácter '-' en el mensaje 'msg'
    int firstDelimiterIndex = msg.indexOf('-');
    
    if (firstDelimiterIndex != -1) { // Si existe el delimitador en la cadena
        myMessage.command = msg.substring(0, firstDelimiterIndex + 1); // "ERROR-" o "BARCODE-"
        String cad = msg.substring(firstDelimiterIndex + 1); // "HTTP:<codigo_error>" o "<codigo_barras_leido>:<carb>;<prot>;<kcal>""
        
        // Busca el carácter ':' en el resto de la cadena
        int secondDelimiterIndex = cad.indexOf(':');
        if (secondDelimiterIndex != -1) { // Si existe el delimitador en la cadena
            myMessage.command_or_code = cad.substring(0, secondDelimiterIndex + 1); // "HTTP:" o "<codigo_barras_leido>:"
            myMessage.data = cad.substring(secondDelimiterIndex + 1); // "<codigo_error>" o "<carb>;<prot>;<kcal>"
        }

        /*Serial.print("Comando recibido: "); Serial.println(myMessage.command);
        Serial.print("Datos recibidos: "); Serial.println(myMessage.data);
        Serial.println();*/
    }
    /*else {
        myMessage.command = "NO";
        myMessage.data = "msg";
    }*/

    return myMessage;
}





#endif