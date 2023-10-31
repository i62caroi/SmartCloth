
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
*/

#ifndef SERIAL_ESP32CAM_H
#define SERIAL_ESP32CAM_H

#include "Comida.h" // comidaActual

#define SerialDueESP32 Serial1

bool saveComidaDatabase(){
    Serial.println("Avisando al ESP32 para que guarde los datos...");

    String comidaValues = comidaActual.getComidaAllValuesHttpRequest(); // &carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X
    // No incluimos el tiempo del RTC porque ya se obtiene en el servidor

    Serial.print("Cadena enviada al esp32: "); Serial.println(comidaValues);
    SerialDueESP32.print(comidaValues); //Envía la cadena al esp32

    //TODO
    //Esperar respuesta OK o FALLO del esp32

    //Devolver true si todo OK o false si fallo al subirlo porque no hubiera wifi o error del servidor o algo
    return true;

}



#endif