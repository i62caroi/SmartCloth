
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
#include <DueFlashStorage.h> // Para guardar una flag si se está en horario de verano o invierno
#include "debug.h" // SM_DEBUG --> SerialPC


// ------ OBJETO RTC --------------
DS3231  rtc(SDA, SCL); // SDA y SCL ya están definidos como 20 y 21 en la librería
// --------------------------------


// ----- FLAG VERANO/INVIERNO -----
DueFlashStorage dueFlashStorage; // Crear objeto para guardar en la flash
#define SUMMER_TIME_FLAG_ADDRESS 0 // Dirección en la flash para la bandera del horario de verano
#define IS_SUMMER 1
#define IS_WINTER 0
// --------------------------------




/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
void      setupRTC();          // Inicializar RTC

bool      isSummerTime();   // Comprobar si hay que cambiar al horario de verano
bool      isWinterTime();   // Comprobar si hay que cambiar al horario de invierno

void      adjustSummerTime();  // Ajustar horario de verano 
void      adjustWinterTime();  // Ajustar horario de invierno

uint8_t   getLastSunday(uint8_t month, uint16_t year);       // Último domingo del mes
uint8_t   getDOW(uint8_t day, uint8_t month, uint16_t year); // DOW de día específico
/******************************************************************************/
/******************************************************************************/





/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicializar el RTC.
 *
 * Esta función inicializa el objeto RTC y ajusta la hora si es necesario por
 * el cambio de hora en verano e invierno.
 */
/*-----------------------------------------------------------------------------*/
void setupRTC()
{
    // Initialize the rtc object
    rtc.begin();

    // ----- AJUSTAR HORA, SI ES NECESARIO ---------
    // Comprobar si es momento de cambiar la hora
    uint8_t isRTCinSummerTime = dueFlashStorage.read(SUMMER_TIME_FLAG_ADDRESS); // Leer la bandera de la flash
    if (isSummerTime() && isRTCinSummerTime == IS_SUMMER) // Estamos en fechas de horario de verano, pero el RTC sigue en horario de invierno
    {
        #if defined(SM_DEBUG)
          SerialPC.println(F("SUMMER TIME"));
        #endif //SM_DEBUG
        adjustSummerTime();
        dueFlashStorage.write(SUMMER_TIME_FLAG_ADDRESS, IS_SUMMER); // Modificar la bandera en la flash
    }
    else if (isWinterTime() && isRTCinSummerTime == IS_WINTER) // Estamos en fechas de horario de invierno, pero el RTC sigue en horario de verano
    {
        #if defined(SM_DEBUG)
          SerialPC.println(F("WINTER TIME"));
        #endif //SM_DEBUG
        adjustWinterTime();
        dueFlashStorage.write(SUMMER_TIME_FLAG_ADDRESS, IS_WINTER); // Modificar la bandera en la flash
    }
    // --------------------------------------

    #if defined(SM_DEBUG)
      SerialPC.print(F("RTC initialized"));
    #endif //SM_DEBUG

    char *today = rtc.getDateStr();
    #if defined(SM_DEBUG)
      SerialPC.print(F(" => Hoy es ")); SerialPC.print(today); SerialPC.print(F(" y son las ")); SerialPC.println(rtc.getTimeStr());
    #endif //SM_DEBUG
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprobar si es horario de verano.
 *
 * Esta función comprueba si estamos en período de horario de verano (entre el 
 * último domingo de marzo y el último domingo de octubre)
 *
 * @return true si es horario de verano, false en caso contrario
 */
/*-----------------------------------------------------------------------------*/
bool isSummerTime()
{
    // Fecha de hoy
    Time t = rtc.getTime();
    
    uint8_t day = t.date;
    uint8_t month = t.mon;
    uint16_t year = t.year;
    
    // Último domingo de marzo (3) y último domingo de octubre (10)
    uint8_t lastSundayMarch = getLastSunday(3, year); 
    uint8_t lastSundayOctober = getLastSunday(10, year);
    
    // Si estamos después del último domingo de marzo y antes del último domingo de octubre, es horario de verano
    if ((month > 3 || (month == 3 && day >= lastSundayMarch)) && (month < 10 || (month == 10 && day < lastSundayOctober))) return true;
    else return false;
    /*if (month == 3 && day == lastSundayMarch) return true; // Solo comprueba si hoy es el último domingo de marzo
    else return false;*/
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprobar si es horario de invierno.
 *
 * Esta función comprueba si estamos en período de horario de invierno (entre el 
 * último domingo de octubre y el último domingo de marzo)
 *
 * @return true si es horario de invierno, false en caso contrario
 */
/*-----------------------------------------------------------------------------*/
bool isWinterTime() 
{
    // Fecha de hoy
    Time t = rtc.getTime();
    
    uint8_t day = t.date;
    uint8_t month = t.mon;
    uint16_t year = t.year;
        
    // Último domingo de octubre (10) y último domingo de marzo (3)
    uint8_t lastSundayOctober = getLastSunday(10, year);
    uint8_t lastSundayMarch = getLastSunday(3, year); 
    

    // Si estamos después del último domingo de octubre o antes del último domingo de marzo, es horario de invierno
    if ((month > 10 || (month == 10 && day >= lastSundayOctober)) || (month < 3 || (month == 3 && day < lastSundayMarch))) return true;
    else return false;
    /*if (month == 10 && day == lastSundayOctober) return true; // Solo comprueba si hoy es el último domingo de octubre
    else return false;*/
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Ajustar la hora para el horario de verano.
 *
 * Esta función ajusta la hora sumando 1 hora.
 */
/*-----------------------------------------------------------------------------*/
void adjustSummerTime() 
{
    Time t = rtc.getTime();
    rtc.setTime(t.hour + 1, t.min, t.sec);
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Ajustar la hora para el horario estándar (horario de invierno).
 *
 * Esta función ajusta la hora restando 1 hora.
 */
/*-----------------------------------------------------------------------------*/
void adjustWinterTime() 
{
    Time t = rtc.getTime();
    rtc.setTime(t.hour - 1, t.min, t.sec);
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Calcular el último domingo de un mes y año específico.
 *
 * Esta función calcula el último domingo de un mes y año específico.
 * Utiliza la función getDOW para obtener el día de la semana (DOW) del último día 
 * del mes. Luego, resta el DOW de ese día al día máximo del mes para obtener el 
 * último domingo.
 * Por ejemplo, si el día 31 de marzo es jueves (día 4), entonces 31-4 = 27. El último
 * domingo es el día 27. En el caso en que el último día del mes (p.ej. 31) caiga en Domingo, 
 * entonces se devuelve ese día (31).
 *
 * @param month El mes
 * @param year El año
 * @return El último domingo del mes
 */
/*-----------------------------------------------------------------------------*/
uint8_t getLastSunday(uint8_t month, uint16_t year) 
{
  // Obtener el día máximo del mes
  uint8_t lastDay = 31;
  if (month == 4 || month == 6 || month == 9 || month == 11) { // Abril, Junio, Septiembre y Noviembre
    lastDay = 30;
  } else if (month == 2) { // Febrero
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) { // Año bisiesto
      lastDay = 29;
    } else {
      lastDay = 28;
    }
  }

  // Obtener el DOW del último día del mes
  uint8_t lastDOW = getDOW(lastDay, month, year);

  // Calcular el último domingo restando el DOW del último día al día máximo del mes:
    // Por ejemplo, si el día 31 de marzo es jueves (día 4), entonces 31-4 = 27. El último
    // domingo es el día 27.
    // En el caso en que el último día del mes (p.ej. 31) caiga en Domingo, entonces se 
    // devuelve ese día (31).
  if (lastDOW == 7) return lastDay;
  else return lastDay - lastDOW;

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Obtener el día de la semana (DOW) para una fecha específica.
 *
 * Esta función calcula el día de la semana (DOW) para una fecha específica 
 * utilizando el algoritmo de Zeller.
 * El algoritmo toma como entrada el día, mes y año de una fecha y devuelve un 
 * número del 0 al 6, de lunes a domingo. Se ha modificado la función para que 
 * devuelva del 1 al 7.
 *
 * @param day El día
 * @param month El mes
 * @param year El año
 * @return El día de la semana (DOW)
 */
/*-----------------------------------------------------------------------------*/
uint8_t getDOW(uint8_t day, uint8_t month, uint16_t year) 
{
  // Ajustar Enero y Febrero como meses 13 y 14 del año anterior. Necesario para el algoritmo.
  if (month < 3) {
    month += 12;
    year--;
  }
  //else month = month - 2;

  //uint8_t century = year / 100 + 1;
  uint8_t century = 21; // Queda siglo 21 para largo
  uint8_t yearOfCentury = year % 100;

  // Fórmula del Algoritmo de Zeller
  uint8_t dow = (day + ((13 * (month + 1))/5) + yearOfCentury + (yearOfCentury/4) + (century/4) - (2*century)) % 7;

  return dow + 1; // Pasar de Lunes(0) - Domingo(6) a Lunes(1) - Domingo(7)
}




/******************************************************************************/
/******************************************************************************/


#endif
