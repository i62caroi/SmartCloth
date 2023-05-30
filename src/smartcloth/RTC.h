
/* ------------- Real Time Clock (RTC) ---------------------

  Se ha utilizado un RTC DS3231 de Az-Delivery [1]

  ------ ESPECIFICACIONES -------------------
    * Alimentación de 3.3V
    * Batería backup de 3V (CR2032)
    * Comunicación I2C
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

  --------------------------------------------------


  [1] https://www.amazon.es/AZDelivery-Reloj-tiempo-DS3231-Parent/dp/B082MN58TN 

-----------------------------------------------------
*/

#ifndef RTC_H
#define RTC_H

#include <DS3231.h>
#include "Variables.h"


// RTC object 
DS3231  rtc(SDA, SCL); // SDA y SCL ya están definidos como 20 y 21 en la librería




/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
void setupRTC();          // Inicializar RTC
bool checkSummerTime();   // Comprobar si hay que cambiar al horario de verano
bool checkWinterTime();   // Comprobar si hay que cambiar al horario de invierno
void adjustSummerTime();  // Ajustar horario de verano 
void adjustWinterTime();  // Ajustar horario de invierno
uint8_t getLastSunday(uint8_t month, uint16_t year);       // Último domingo del mes
uint8_t getDOW(uint8_t day, uint8_t month, uint16_t year); // DOW de día específico
/*-----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
   setupRTC(): Inicializar RTC
-------------------------------------------------------------------------------*/
void setupRTC(){
    // Initialize the rtc object
    rtc.begin();

    // ----- AJUSTAR HORA, SI ES NECESARIO ---------
    // Comprobar si es momento de cambiar la hora
    if (checkSummerTime()) {
        Serial.println(F("SUMMER TIME"));
        adjustSummerTime();
    }
    else if (checkWinterTime()) {
        Serial.println(F("WINTER TIME"));
        adjustWinterTime();
    }
    // --------------------------------------

    Serial.println(F("RTC initialized"));

    today = rtc.getDateStr();
    Serial.print("Hoy es "); Serial.println(today);
}



/*-----------------------------------------------------------------------------
   checkSummerTime():

   Comprobar si es el último domingo de Marzo, cuando se realiza el cambio
   a horario de verano (se adelanta una hora)
-------------------------------------------------------------------------------*/
bool checkSummerTime() {
    // Fecha de hoy
    Time t = rtc.getTime();
    
    uint8_t day = t.date;
    uint8_t month = t.mon;
    uint16_t year = t.year;
    
    // Último domingo de marzo (3)
    uint8_t lastSundayMarch = getLastSunday(3, year);
    
    if (month == 3 && day == lastSundayMarch) return true;
    else return false;
}



/*-----------------------------------------------------------------------------
   checkWinterTime():

   Comprobar si es el último domingo de Octubre, cuando se realiza el cambio
   a horario de invierno (se retrasa una hora)
-------------------------------------------------------------------------------*/
bool checkWinterTime() {
    // Fecha de hoy
    Time t = rtc.getTime();
    
    uint8_t day = t.date;
    uint8_t month = t.mon;
    uint16_t year = t.year;
    
    // Último domingo de octubre (10)
    uint8_t lastSundayOctober = getLastSunday(10, year);
    
    if (month == 10 && day == lastSundayOctober) return true;
    else return false;
}



/*-----------------------------------------------------------------------------
   adjustSummerTime(): Adelantar 1 hora
-------------------------------------------------------------------------------*/
void adjustSummerTime() {
    Time t = rtc.getTime();
    rtc.setTime(t.hour + 1, t.min, t.sec);
}



/*-----------------------------------------------------------------------------
   adjustWinterTime(): Retrasar 1 hora
-------------------------------------------------------------------------------*/
void adjustWinterTime() {
    Time t = rtc.getTime();
    rtc.setTime(t.hour - 1, t.min, t.sec);
}



/*-----------------------------------------------------------------------------
   getLastSunday(): Calcular el último domingo de un mes y año específico

   Utiliza la función getDOW para obtener el día de la semana (DOW) del último día 
   del mes. Luego, resta el DOW de ese día al día máximo del mes para obtener el 
   último domingo.
-------------------------------------------------------------------------------*/
uint8_t getLastSunday(uint8_t month, uint16_t year) {
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



/*-----------------------------------------------------------------------------
   getDOW(): Obtener día de la semana (DOW) de un día específico

    Esta función utiliza el Algoritmo de Zeller para determinar el día
    de la semana de una fecha específica mediante cálculos matemáticos.
  
    El algoritmo toma como entrada el día, mes y año de una fecha y 
    devuelve un número del 0 al 6, de lunes a domingo. Se ha modificado
    la función para que devuelva del 1 al 7.
-------------------------------------------------------------------------------*/
uint8_t getDOW(uint8_t day, uint8_t month, uint16_t year) {
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

  return dow + 1; // Pasar de L(0) - D(6) a L(1) - D(7)
}



#endif
