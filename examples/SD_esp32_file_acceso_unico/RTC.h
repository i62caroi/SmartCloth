/**
 * @file RTC.h
 * @brief Este archivo contiene la configuración y las funciones para trabajar con un módulo RTC DS3231.
 * 
 * @author Irene Casares Rodríguez
 * @date 01/12/2023
 */

/*
   --------- CONEXIÓN RTC CON ARDUINO DUE -----------

      --------------------------
      |  RTC  |  Arduino Due   |
      --------------------------
      |  GND  |  GND           |    
      |  VCC  |  3.3 V         |    
      |  SDA  |  SDA (pin 20)  |   
      |  SCL  |  SCL (pin 21)  |  
      --------------------------

*/

#ifndef RTC_H
#define RTC_H

#include <DS3231.h>


/**
 * @brief Objeto RTC para interactuar con el módulo RTC DS3231.
 */
DS3231  rtc(SDA, SCL); // SDA y SCL ya están definidos como 20 y 21 en la librería


/**
 * @brief Variable para almacenar la fecha actual.
 */
char *today;


/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicializa el módulo RTC DS3231 y muestra la fecha y hora actuales.
 */
/*-----------------------------------------------------------------------------*/
void setupRTC(){
    // Initialize the rtc object
    rtc.begin();

    Serial.print(F("RTC initialized"));

    today = rtc.getDateStr();
    char *time = rtc.getTimeStr();

    Serial.print(F(" => Hoy es ")); Serial.print(today); Serial.print(F(" y son las ")); Serial.println(time);
}



#endif