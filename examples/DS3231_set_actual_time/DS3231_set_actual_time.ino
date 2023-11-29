
// Establecer fecha y hora en RTC tomando los valores actuales del sistema 
//

#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);



/*-----------------------------------------------------------------------------*/
/**
 * @brief Configuración inicial del programa.
 *
 * Esta función se ejecuta una vez cuando el programa comienza. Se utiliza para 
 * inicializar la conexión serie y el objeto RTC. Luego obtiene la fecha y hora 
 * de compilación y las establece en el RTC.
 */
/*-----------------------------------------------------------------------------*/

void setup()
{
  // Inicializar la conexión serie
  Serial.begin(115200);
  while (!Serial);
  delay(1000);
  
  // Inicializar el objeto RTC
  rtc.begin();

  Serial.print("1 - Hoy es "); Serial.print(rtc.getDateStr()); Serial.print(F(" y son las ")); Serial.println(rtc.getTimeStr());


  // Obtener la fecha y hora de compilación
  const char* compileDate = __DATE__;
  const char* compileTime = __TIME__;


  // Descomponer la fecha y hora de compilación en componentes
  // Fecha de compilación
  char month[4];
  int day, year;
  sscanf(compileDate, "%s %d %d", month, &day, &year);
  // Hora de compilacion
  int hour, minute, second;
  sscanf(compileTime, "%d:%d:%d", &hour, &minute, &second);

  // Corregir desfase de segundos (iba 26 segundos por detrás)
  adjustTime(second, minute, hour);

  // Convertir el mes a un número
  int monthNumber = convertMonthToNumber(month);
  

  // Establecer la fecha y hora en el RTC
  int dow = getDOW(day, monthNumber, year);
  Serial.print("Día de la semana: "); Serial.println(dow);
  rtc.setDOW(dow);                        // Establecer el día de la semana Lunes-Domingo (1 - 7)
  rtc.setDate(day, monthNumber, year);   // Establecer fecha a día (1 - 31), mes (1 - 12), año (2000 - 2099) actuales
  rtc.setTime(hour, minute, second);     // Establecer la hora (formato 24hr) => hora (0 - 23), min (0 - 59), seg (0 - 59)
  

}






/*-----------------------------------------------------------------------------*/
/**
 * @brief Bucle principal del programa.
 *
 * Esta función se ejecuta repetidamente en un bucle. Se utiliza para imprimir 
 * la fecha y hora actuales en la consola serie.
 */
/*-----------------------------------------------------------------------------*/

void loop()
{
  Serial.print(F("Hoy es ")); Serial.print(rtc.getDateStr()); 
  Serial.print(F(" y son las ")); Serial.println(rtc.getTimeStr());

  delay(1000);
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Ajustar la hora para corregir el desfase.
 *
 * Esta función toma como entrada la hora, minuto y segundo por referencia y 
 * ajusta el tiempo sumando 14 segundos para corregir el desfase. Si los segundos 
 * son 60 o más, suma un minuto, y si los minutos son 60 o más, suma una hora. 
 * Si la hora es 24 o más, resta 24 para pasar al día siguiente.
 *
 * @param second El segundo actual
 * @param minute El minuto actual
 * @param hour La hora actual
 */
/*-----------------------------------------------------------------------------*/
void adjustTime(int& second, int& minute, int& hour) 
{
  // Corregir el desfase de segundos
  second += 14;
  if (second >= 60) {
    second -= 60;
    minute++;
    if (minute >= 60) {
      minute -= 60;
      hour++;
      if (hour >= 24) {
        hour -= 24;
      }
    }
  }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Convertir el nombre del mes a un número.
 *
 * Esta función toma como entrada el nombre de un mes y devuelve su número 
 * correspondiente.
 *
 * @param month El nombre del mes
 * @return El número del mes
 */
/*-----------------------------------------------------------------------------*/
int convertMonthToNumber(char* month) 
{
  int monthNumber;

  if (strcmp(month, "Jan") == 0) monthNumber = 1;
  else if (strcmp(month, "Feb") == 0) monthNumber = 2;
  else if (strcmp(month, "Mar") == 0) monthNumber = 3;
  else if (strcmp(month, "Apr") == 0) monthNumber = 4;
  else if (strcmp(month, "May") == 0) monthNumber = 5;
  else if (strcmp(month, "Jun") == 0) monthNumber = 6;
  else if (strcmp(month, "Jul") == 0) monthNumber = 7;
  else if (strcmp(month, "Aug") == 0) monthNumber = 8;
  else if (strcmp(month, "Sep") == 0) monthNumber = 9;
  else if (strcmp(month, "Oct") == 0) monthNumber = 10;
  else if (strcmp(month, "Nov") == 0) monthNumber = 11;
  else if (strcmp(month, "Dec") == 0) monthNumber = 12;

  return monthNumber;
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
