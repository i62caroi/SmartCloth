


// DS3231_Serial_Easy
// Copyright (C)2015 Rinky-Dink Electronics, Henning Karlsen. All right reserved
// web: http://www.RinkyDinkElectronics.com/
//
// A quick demo of how to use my DS3231-library to 
// quickly send time and date information over a serial link
//
// To use the hardware I2C (TWI) interface of the Arduino you must connect
// the pins as follows:
//
// Arduino Uno/2009:
// ----------------------
// DS3231:  SDA pin   -> Arduino Analog 4 or the dedicated SDA pin
//          SCL pin   -> Arduino Analog 5 or the dedicated SCL pin
//
// Arduino Leonardo:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 2 or the dedicated SDA pin
//          SCL pin   -> Arduino Digital 3 or the dedicated SCL pin
//
// Arduino Mega:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA pin
//          SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL pin
//
// Arduino Due:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA1 (Digital 70) pin
//          SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL1 (Digital 71) pin
//
// The internal pull-up resistors will be activated when using the 
// hardware I2C interfaces.
//
// You can connect the DS3231 to any available pin but if you use any
// other than what is described above the library will fall back to
// a software-based, TWI-like protocol which will require exclusive access 
// to the pins used, and you will also have to use appropriate, external
// pull-up resistors on the data and clock signals.
//

#include <DS3231.h>
#include <DueFlashStorage.h> // Para guardar una flag si se está en horario de verano o invierno

// ------ OBJETO RTC --------------
DS3231  rtc(SDA, SCL); // SDA y SCL ya están definidos como 20 y 21 en la librería
// --------------------------------

// ----- FLAG VERANO/INVIERNO -----
DueFlashStorage dueFlashStorage; // Crear objeto para guardar en la flash
#define SUMMER_TIME_FLAG_ADDRESS 0 // Dirección en la flash para la bandera del horario de verano
// --------------------------------



/*******************************************************************************
                          DECLARACIÓN FUNCIONES
*******************************************************************************/
bool      isSummerTime();   // Comprobar si hay que cambiar al horario de verano
bool      isWinterTime();   // Comprobar si hay que cambiar al horario de invierno

void      adjustSummerTime();  // Ajustar horario de verano 
void      adjustWinterTime();  // Ajustar horario de invierno

uint8_t   getLastSunday(uint8_t month, uint16_t year);       // Último domingo del mes
uint8_t   getDOW(uint8_t day, uint8_t month, uint16_t year); // DOW de día específico
/******************************************************************************/



void setup()
{
    // Setup Serial connection
    Serial.begin(115200);
    while (!Serial);
    delay(1000);
    
    // Initialize the rtc object
    rtc.begin();

    char *today = rtc.getDateStr();
    char *actualTime = rtc.getTimeStr();
    Serial.print("1 - Hoy es "); Serial.print(today); Serial.print(F(" y son las ")); Serial.println(actualTime);

    
    // The following lines can be uncommented to set the date and time
    rtc.setDOW(6);         // Set Day-of-Week (1 - 7)
    rtc.setDate(30, 03, 2024);   // Set the date to the current day (1 - 31), month (1 - 12), year (2000 - 2099)
    rtc.setTime(23, 45, 15);     // Set the time (24hr format) => hour (0 - 23), min (0 - 59), sec (0 - 59)

    // --------------------------------------
    delay(1000);

    Serial.print("2 - Hoy es "); Serial.print(rtc.getDateStr()); Serial.print(F(" y son las ")); Serial.println(rtc.getTimeStr());

    delay(1000);
    
    
    // ----- AJUSTAR HORA, SI ES NECESARIO ---------
    // Comprobar si es momento de cambiar la hora
    bool isRTCinSummerTime = dueFlashStorage.read(SUMMER_TIME_FLAG_ADDRESS); // Leer la bandera de la flash
    if (isSummerTime() && !isRTCinSummerTime) // Estamos en fechas de horario de verano, pero el RTC sigue en horario de invierno
    {
        Serial.println(F("SUMMER TIME"));
        adjustSummerTime();
        dueFlashStorage.write(SUMMER_TIME_FLAG_ADDRESS, true); // Modificar la bandera en la flash
    }
    else if (isWinterTime() && isRTCinSummerTime) // Estamos en fechas de horario de invierno, pero el RTC sigue en horario de verano
    {
        Serial.println(F("WINTER TIME"));
        adjustWinterTime();
        dueFlashStorage.write(SUMMER_TIME_FLAG_ADDRESS, false); // Modificar la bandera en la flash
    }
    // --------------------------------------

    actualTime = rtc.getTimeStr();
    Serial.print("\n2 - Hoy es "); Serial.print(today); Serial.print(F(" y son las ")); Serial.println(actualTime);

}



void loop()
{
  //Serial.print("2 - Hoy es "); Serial.print(rtc.getDateStr()); Serial.print(F(" y son las ")); Serial.println(rtc.getTimeStr());
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

  return dow + 1; // Pasar de Lunes(0) - Domingo(6) a Lunes(1) - Domingo(7)
}







