#include "RA8876_v2.h"
#include "bug.h"
#include "pic16bpp_word.h"
#include "crudo.h"
#include "SmartCloth_icono.h"

// Screen circuit wiring
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10

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
  
  if (!tft.init())
  {
    Serial.println("Could not initialize RA8876");
  }
  
  tft.clearScreen(0);
  tft.setTextScale(2);
  tft.selectInternalFont(RA8876_FONT_SIZE_32, RA8876_FONT_ENCODING_8859_1);

  Serial.println("Startup complete...");

  //tft.canvasImageStartAddress(PAGE2_START_ADDR);
  
  //printImage16bpp(0,0,16,16,bug);
  //printImage16bpp(50,50,128,128,pic16bpp_word);
  
  //tft.putPicture_16bpp(0,0,16,16,bug);
  //tft.putPicture_16bpp(50,50,128,128,pic16bpp_word);
  //tft.putPicture_16bpp(50,50,189,181,crudo);
  tft.putPicture_16bpp(50,50,500,350,smartcloth_icono);
}



void loop()
{
  
}




/*
void printImage16bpp(uint16_t x,uint16_t y,uint16_t width, uint16_t height, const unsigned short *data){
//void printImage16bpp(uint16_t x,uint16_t y,uint16_t width, uint16_t height, const unsigned short data[]){
  
  //int pixel = 0;
  //tft.setCanvasWindow(x,y,width,height);
  //tft.setCursor(x, y);

  for (uint16_t i = y; i <= (height+y); i++) {
  //for (uint16_t i = x; i <= (width+x); i++) {
    for (uint16_t j = x; j <= (width+x); j++){
    //for (uint16_t j = y; j <= (height+y); j++){
      //tft.drawPixel(i, j, bug[pixel]);
      //pixel++;

      //tft.drawPixel(i, j, *data);
      tft.drawPixel(j, i, *data); // Elemento del  vector apuntado por 'data' se coloca en x,y (j,i)
      data++;

  Serial.print("Imagen impresa en pantalla "); 
}*/
