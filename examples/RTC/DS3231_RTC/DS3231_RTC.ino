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

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

void setup()
{
  // Setup Serial connection
  Serial.begin(115200);
  while (!Serial);
  delay(1000);
  
  // Initialize the rtc object
  rtc.begin();

  // Verifica si el módulo RTC está detenido (posible indicio de que se agotó la batería)
  //if (rtc.lostPower()) {
    //Serial.println("La alimentación se perdió. Ajuste la fecha y hora del RTC.");
    
    // AVISAR POR PANTALLA DE QUE LA FECHA ES INCORRECTA Y SE DEBE CORREGIR

    // Establece una fecha y hora predeterminada si la alimentación se perdió
    //rtc.setDate(__DATE__);
    //rtc.setTime(__TIME__);
  //}

  
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(MONDAY);         // Set Day-of-Week (1 - 7)
  //rtc.setDate(29, 5, 2023);   // Set the date to the current day (1 - 31), month (1 - 12), year (2000 - 2099)
  //rtc.setTime(12, 9, 40);     // Set the time (24hr format) => hour (0 - 23), min (0 - 59), sec (0 - 59)


  // ----- AJUSTAR HORA SI ES NECESARIO ---------
  // Comprobar si es momento de adelantar o retrasar el reloj 1 hora
  if (checkSummerTime()) {
    Serial.println("SUMMER TIME");
    //adjustSummerTime();
  }
  else if (checkWinterTime()) {
    Serial.println("WINTER TIME");
    //adjustWinterTime();
  }
  // --------------------------------------

  char *today = rtc.getDateStr();
  Serial.print("Hoy es "); Serial.println(today);

  Serial.print("DOW del 30-5-23: "); Serial.println(getDOW(30,5,2023)); //Martes
  Serial.print("DOW del 14-2-05: "); Serial.println(getDOW(14,2,2005)); //Lunes
  Serial.print("DOW del 23-6-16: "); Serial.println(getDOW(23,6,2016)); // Jueves

  /*Serial.print("DOW del 15-2-16: "); Serial.println(getDOW(15,2,2016)); // L
  Serial.print("DOW del 16-2-16: "); Serial.println(getDOW(16,2,2016)); // M
  Serial.print("DOW del 17-2-16: "); Serial.println(getDOW(17,2,2016)); // X
  Serial.print("DOW del 18-2-16: "); Serial.println(getDOW(18,2,2016)); // J
  Serial.print("DOW del 19-2-16: "); Serial.println(getDOW(19,2,2016)); // V
  Serial.print("DOW del 20-2-16: "); Serial.println(getDOW(20,2,2016)); // S
  Serial.print("DOW del 21-2-16: "); Serial.println(getDOW(21,2,2016)); // D*/
}



void loop()
{
  // Send Day-of-Week
  /*Serial.print(rtc.getDOWStr());
  Serial.print(" ");
  
  // Send date
  Serial.print(rtc.getDateStr());
  Serial.print(" -- ");

  // Send time
  Serial.println(rtc.getTimeStr());
  
  // Wait one second before repeating :)
  delay (1000);*/
}




// ***********+*******************************************************************
// Comprobar si es el último domingo de Marzo, cuando se realiza el cambio
// a horario de verano (se adelanta una hora)
// ***********+*******************************************************************
bool checkSummerTime() {

  Time t = rtc.getTime();
  
  uint8_t day = t.date;
  uint8_t month = t.mon;
  uint16_t year = t.year;
  
  // Último domingo de marzo (3)
  uint8_t lastSundayMarch = getLastSunday(3, year);
  
  if (month == 3 && day == lastSundayMarch) return true;
  else return false;
}

// ***********+*******************************************************************
// Comprobar si es el último domingo de Octubre, cuando se realiza el cambio
// a horario de invierno (se retrasa una hora)
// ***********+*******************************************************************
bool checkWinterTime() {

  Time t = rtc.getTime();
  
  uint8_t day = t.date;
  uint8_t month = t.mon;
  uint16_t year = t.year;
  
  // Último domingo de octubre (10)
  uint8_t lastSundayOctober = getLastSunday(10, year);
  
  if (month == 10 && day == lastSundayOctober) return true;
  else return false;
}



// ***********+*******************************************************************
// Aplicar el adelanto de una hora
// ***********+*******************************************************************
void adjustSummerTime() {
  Time t = rtc.getTime();
  rtc.setTime(t.hour + 1, t.min, t.sec);
}

// ***********+*******************************************************************
// Aplicar el retraso de una hora
// ***********+*******************************************************************
void adjustWinterTime() {
  Time t = rtc.getTime();
  rtc.setTime(t.hour - 1, t.min, t.sec);
}



// ***********+*******************************************************************
// Calcular el último domingo de un mes y año específico
// Utiliza la función getDOW para obtener el día de la semana (DOW) del último día del mes
// Luego, resta el DOW de ese día al día máximo del mes para obtener el último domingo
// ***********+*******************************************************************
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



/* ***********+*******************************************************************
  Esta función utiliza el Algoritmo de Zeller para determinar el día
  de la semana de una fecha específica mediante cálculos matemáticos.
  
  El algoritmo toma como entrada el día, mes y año de una fecha y 
  devuelve un número del 0 al 6, de lunes a domingo. Se ha modificado
  la función para que devuelva del 1 al 7.
// ***********+******************************************************************* */
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

