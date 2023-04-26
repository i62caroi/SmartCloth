/*
 * CREO QUE NO ESTÁ HECHA
 * 
 * 
  Amount of data for color display is huge, if the operation speed 
  of MPU write is not fast enough, you can see the update scan line 
  on the display is shown like a waterfall. [...] so if you want to get 
  a better display effect, you can take advantage of the BTE function, 
  the image data can be written to the non-display area of the display 
  memory through the MPU interface or DMA interface firstly, and then use 
  BTE memory copy function to duplicate and move the image data to the 
  display memory area, to avoids the bad display 
  effect which is described above.
*/


#include "RA8876_v2.h"

// Screen circuit wiring
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10

#define SD_CARD_SCS   4

RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);


void setup()
{
  Serial.begin(9600);
  delay(1000);
  while (!Serial);

  Serial.println("Starting up...");

  //setting up the screen
  pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
  digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight

  if(SD.begin(SD_CARD_SCS))
   Serial.println("SD card initialized");
   else
   Serial.println("SD card failure");
   
  if (!tft.init())
  {
    Serial.println("Could not initialize RA8876");
  }
  
  //tft.clearScreen(0);
  tft.setTextScale(2);
  tft.selectInternalFont(RA8876_FONT_SIZE_32, RA8876_FONT_ENCODING_8859_1);

  Serial.println("Startup complete...");

  tft.clearScreen(0);
  tft.sdCardShowPicture16bpp(50,50,128,128,"home.bin");
  delay(2000);

}



void loop()
{
  /*tft.clearScreen(0);
  tft.sdCardShowPicture16bpp(0,0,800,600,"wp2.bin");
  delay(2000);

  tft.clearScreen(0);
  tft.sdCardShowPicture16bpp(0,0,800,600,"wp23.bin");
  delay(2000);*/
  
}
