
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
          de macronutrientes en el servidor al hacer GET. De esta forma, se podría llevar una trazabilidad de
          lo comido exactamente (a nivel grupo).

  2. Activar cámara y leer código de barras:
      "GET BARCODE"

  
  -------- MENSAJES ESP32 -> ARDUINO -------------- 

  1. Guardado correctamente:
      "SAVED OK"
  
  2. Error en el guardado (petición HTTP POST):
      "ERROR HTTP: <codigo_error>"
  
  3. No hay wifi:
      "NO WIFI"

  4. Código de barras leído (se consulta base de datos de alimentos y se obtienen sus valores de carb, lip, prot y kcal por gramo):
      "BARCODE-<codigo_barras_leido>:<carb>-<prot>-<kcal>"


*/

#ifndef SERIAL_ESP32CAM_H
#define SERIAL_ESP32CAM_H

#include "Comida.h" // comidaActual

#define SerialDueESP32 Serial1

bool saveComidaDatabase(){
    Serial.println("Avisando al ESP32 para que guarde los datos...");

    String comidaValues = comidaActual.getComidaAllValuesHttpRequest(); // &carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X
    // No incluimos el tiempo del RTC porque ya se obtiene en el servidor
    String command = "SAVE:" + comidaValues;

    Serial.print("Cadena enviada al esp32: "); Serial.println(command);
    SerialDueESP32.print(command); //Envía la cadena al esp32

    //TODO
    //Esperar respuesta OK o FALLO del esp32

    unsigned long TIMEOUT = 30000;  // Espera máxima de 30 segundos
    unsigned long startTime = millis();  // Obtenemos el tiempo actual

    while (true) { // Bloquea el arduino en este bucle hasta que se reciba respuesta o se pase el TIMEOUT
        // Comprueba si hay datos disponibles en el puerto serie del ESP32
        if (SerialDueESP32.available() > 0) { // Si el esp32 ha respondido
            String response = SerialDueESP32.readStringUntil('\n');
            Serial.print("Respuesta del ESP32: ");  Serial.println(response);
            
            if (response == "SAVED OK") {
                // Respuesta OK, el guardado fue exitoso
                return true;
            } 
            else if (response == "ERROR HTTP: <codigo_error>") {
                // Respuesta ERROR, hubo un error en el ESP32
                return false;
            }
            else if (response == "NO WIFI"){
                // Respuesta fallo en conexion wifi, hubo un error en el ESP32
                return false;
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







#endif