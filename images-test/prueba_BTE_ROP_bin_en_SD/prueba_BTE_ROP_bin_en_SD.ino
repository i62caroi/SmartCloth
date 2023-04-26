
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

  // Imagen normal
  tft.clearScreen(0);
  tft.sdCardShowPicture16bpp(50,50,128,128,"home.bin");
  delay(2000);

  // ------ BTE con ROP (operaciones lógicas entre imágenes) -------
  // Muestra la imagen (Source 0) tal y como es. Sin operación lógica (S0 => RA8876_BTE_ROP_CODE_12)
  tft.clearScreen(0);
  tft.sdCardShowPicture16bppBteMpuWriteWithROP(PAGE1_START_ADDR, tft.getWidth(), 50, 50, PAGE1_START_ADDR, tft.getWidth(), 50, 50, 
                                           128, 128,RA8876_BTE_ROP_CODE_12,"home.bin");
  delay(2000);

  // Muestra la imagen (Source 0) invertida en colores (~S0 ==> RA8876_BTE_ROP_CODE_3)
  tft.clearScreen(0);
  tft.sdCardShowPicture16bppBteMpuWriteWithROP(PAGE1_START_ADDR, tft.getWidth(), 50, 50, PAGE1_START_ADDR, tft.getWidth(), 50, 50, 
                                           128, 128,RA8876_BTE_ROP_CODE_3,"home.bin");
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
