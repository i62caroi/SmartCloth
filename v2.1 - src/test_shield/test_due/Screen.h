/*
 
  ------------------------------------------------------------------------------------------------------------
  --------- CONEXIÓN TFT/LCD CON ARDUINO DUE -----------------------------------------------------------------

  Alimentación y referencia a tierra de la TFT: 
    (pines de conexión en el JP1 de la TFT)
                      ---------------------------------
                      |           TFT (JP1)           |
      -------------------------------------------------
      |  VDD (3.3 V)  |  pines 3, 4, 37 y 38          |   (rojo)
      |  GND          |  pines 1, 2, 13, 31, 39 y 40  |   (negro)
      -------------------------------------------------
  

  TFT conectada al bus SPI del Arduino de la siguiente forma:
    (pines de conexión en el JP1 de la TFT)
      ---------------------------------------------------------
      |  SPI  |       TFT (JP1)     |       Arduino Due       |
      ---------------------------------------------------------
      |  MOSI |    pin 7 (marron)   |      pin 4 (SPI)        |    
      |  MISO |    pin 6 (naranja)  |      pin 1 (SPI)        |
      |  CLK  |    pin 8 (blanco)   |      pin 3 (SPI)        |
      |  CS   |    pin 5 (morado)   |   pin 12 (digital PWM)  | 
      ---------------------------------------------------------

  Otras conexiones de la TFT al Arduino:
    (pines de conexión en el JP1 de la TFT)
      ----------------------------------------------------------------------
      |              TFT (JP1)                    |      Arduino Due       |
      ----------------------------------------------------------------------
      |  pin 11 (Reset) (2º naranja)              |  pin 11 (digital PWM)  |
      |  pin 14 (Backlight) (2º blanco moraito)   |  pin 10 (digital PWM)  |
      ----------------------------------------------------------------------

----------------------------------------------------------------------------------------------------------------------
*/


#ifndef SCREEN_H
#define SCREEN_H


void    checkBascula();           // Está en Scale.h, pero hay que declararla aquí también para que esté en este ámbito
bool    interruptionOccurred();   // Está en ISR.h, pero hay que declararla aquí también para que esté en este ámbito

#include "ISR.h" 
#include "RA8876_v2.h" // COLORS.h
#include "SD.h"

#define SerialPC Serial

// ----- Screen circuit wiring -------
#define RA8876_CS        12 
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10 
// -----------------------------------


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600

// Screen object 
RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);


// ---- FICHEROS ------
char  fileCocinadoGrande[30]  = "bin/botones/coci_gra.bin";
// --------------------

/******************************************************************************/
bool    setupScreen();                    // Inicializar pantalla
void    checkScreen();                    // Comprobar pantalla
/******************************************************************************/




bool setupScreen()
{
    pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
    digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight

    return tft.init(); // Inicializar pantalla
}



void checkScreen()
{
    if(initSD){
      // ----- CARGAR IMAGEN 'COCINADO' DESDE SD -----
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Establecer la dirección de inicio de la imagen de la pantalla
      tft.clearScreen(BLACK);
      tft.sdCardDraw16bppBIN256bits(173,131,177,160,fileCocinadoGrande); // Cargar cociGra (172x130) en PAGE3 =>  x = 173   y = 131 
      // ----------------------------------------------

      // ----- DIBUJAR PANTALLA CON IMAGEN ------------
      tft.canvasImageStartAddress(PAGE1_START_ADDR);
      tft.clearScreen(VERDE_PEDIR); // Limpiar pantalla
      tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,800,350,PAGE1_START_ADDR,SCREEN_WIDTH,300,200,177,160,0);
      // ----------------------------------------------

      delay(1500);
      tft.setCursor(50,100); tft.println("1) PANTALLA OK. OBTENCION IMAGENES SD OK.");
    }
    else{
      tft.setCursor(50,100); tft.println("1) PANTALLA OK. ERROR AL INICIALIZAR SD");
    }
}


/******************************************************************************/
/******************************************************************************/


#endif
