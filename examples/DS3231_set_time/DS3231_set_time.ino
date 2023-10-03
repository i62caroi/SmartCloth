


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

  Serial.print("1 - Hoy es "); Serial.print(rtc.getDateStr()); Serial.print(F(" y son las ")); Serial.println(rtc.getTimeStr());

  /*
  // The following lines can be uncommented to set the date and time
  rtc.setDOW(2);         // Set Day-of-Week (1 - 7)
  rtc.setDate(3, 10, 2023);   // Set the date to the current day (1 - 31), month (1 - 12), year (2000 - 2099)
  rtc.setTime(13, 19, 15);     // Set the time (24hr format) => hour (0 - 23), min (0 - 59), sec (0 - 59)

  // --------------------------------------
  delay(1000);
*/
  //Serial.print("2 - Hoy es "); Serial.print(rtc.getDateStr()); Serial.print(F(" y son las ")); Serial.println(rtc.getTimeStr());


}



void loop()
{
  /Serial.print("2 - Hoy es "); Serial.print(rtc.getDateStr()); Serial.print(F(" y son las ")); Serial.println(rtc.getTimeStr());
}






