

#include <DS3231.h>

// ------ OBJETO RTC --------------
DS3231  rtc(SDA, SCL); // SDA y SCL ya están definidos como 20 y 21 en la librería
// --------------------------------


void setup()
{
  // Inicializar la conexión serie
  Serial.begin(115200);
  while (!Serial);
  delay(1000);
  
  // Inicializar el objeto RTC
  rtc.begin();

  Serial.println("\nRTC inicializado\n");

}




void loop()
{
  Serial.print(F("Hoy es ")); Serial.print(rtc.getDateStr()); 
  Serial.print(F(" y son las ")); Serial.println(rtc.getTimeStr());

  delay(1000);
}


