#include "icons.h"
#include "RA8876_v2.h"

// Screen circuit wiring
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10

#define SD_CARD_SCS   4

RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);


void setup()
{
  Serial.begin(115200);
  delay(1000);
  while (!Serial);

  Serial.println("\nStarting up...");

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
  
  Serial.println("Startup complete...");

  tft.clearScreen(0);

  //char file[30] = "bin/otros/home.bin";
  //tft.sdCardShowPicture16bpp(50,50,128,128,file);

  // ------- BMP ---------
  Serial.println(F("\nProbando BMP ...\n"));
  char fileBMP[30] = "bmp/logo3.bmp";
  uint32_t startTime = millis();
  tft.sdCardDraw24bppBMP(fileBMP, 250, 150);
  Serial.print(F("BMP cargado en ")); Serial.print(millis() - startTime); Serial.println(" ms\n");
  delay(2000);

  // ------- BIN ---------
  tft.clearScreen(0);
  Serial.println(F("\nProbando BIN ..."));
  char fileBIN[30] = "bin/logo.bin";
 startTime = millis();
  tft.sdCardShowPicture16bpp(250,150,482,350,fileBIN);
  Serial.print(F("BIN cargado en ")); Serial.print(millis() - startTime); Serial.println(" ms\n");
  delay(2000);

  // ------- BIN ---------
  tft.clearScreen(0);
  Serial.println(F("\nProbando fichero .H ..."));
  startTime = millis();
  tft.putPicture_16bpp(250,150,500,350,smartcloth_icono); //icons.h
  Serial.print(F("Fichero en .H cargado en ")); Serial.print(millis() - startTime); Serial.println(" ms");
  //delay(2000);

  //tft.sdCardShowPicture16bpp(50,50,300,300,"BMP_1.BMP");



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
