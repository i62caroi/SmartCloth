
/** @file RTC.h
 *  @brief Funciones de un Real Time Clock (RTC) 
 *
 *  @author Irene Casares Rodríguez
 *  @date 31/05/23
 *  @version 1.0
 *
 *  Este archivo contiene las funciones relacionadas con la funcionalidad Real Time Clock (RTC).
 *  Incluye la inicialización del RTC, verificación de cambios de horario de verano, ajustar
 *  la hora para el horario de verano e invierno, y calcular el último domingo de un mes.
 *
 *  @note Se ha utilizado un RTC DS3231 de Az-Delivery
 *  @note https://www.amazon.es/AZDelivery-Reloj-tiempo-DS3231-Parent/dp/B082MN58TN Módulo RTC DS3231 de Az-Delivery
 *
 *  @note Se ha utilizado la librería DS32321 de Rinky-Dink Electronics (http://www.rinkydinkelectronics.com/library.php?id=73)
 *
 *  
 */

/*

   ------ ESPECIFICACIONES -------------------
      - Alimentación de 3.3V
      - Batería backup de 3V (CR2032)
      - Comunicación I2C
   -------------------------------------------

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

#define SerialPC Serial


// ------ OBJETO RTC --------------
DS3231  rtc(SDA, SCL); // SDA y SCL ya están definidos como 20 y 21 en la librería
// --------------------------------





/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
inline void     setupRTC(){rtc.begin();};          // Inicializar RTC
inline char*    getRTCDateStr(){ return rtc.getDateStr(); }; // Obtener fecha en formato "dd/mm/aaaa"
inline char*    getRTCTimeStr(){ return rtc.getTimeStr(); }; // Obtener hora en formato "hh:mm:ss"

void            checkRTC(); // Función de prueba para comprobar el funcionamiento del RTC
/******************************************************************************/
/******************************************************************************/





/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/



void checkRTC()
{
    char *fecha = getRTCDateStr();
    char *hora = getRTCTimeStr();

    // ------ TERMINAL --------------------------
    SerialPC.print("  FECHA: "); SerialPC.println(fecha); 
    SerialPC.print("  HORA: "); SerialPC.println(hora);
    // ------------------------------------------
}


/******************************************************************************/
/******************************************************************************/


#endif
