#ifndef SCREEN_H
#define SCREEN_H


#include "RA8876_v2.h" // COLORS.h


/* Screen circuit wiring */
#define RA8876_CS        12 
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10 


#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 600

// Screen object 
RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);


/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la pantalla.
 * 
 * Esta función configura la pantalla, estableciendo el modo de salida del pin de retroiluminación,
 * encendiendo la retroiluminación y verificando si se pudo inicializar correctamente el controlador RA8876.
 * Además, establece la dirección de inicio de la imagen del lienzo en la memoria y borra la pantalla.
 * 
 * @note Esta función debe ser llamada antes de utilizar la pantalla.
 */
/*-----------------------------------------------------------------------------*/
void setupScreen()
{
    pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
    digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight

    if (!tft.init()){
        Serial.println(F("Could not initialize RA8876"));
        while(1);
    }

    tft.canvasImageStartAddress(PAGE1_START_ADDR); 
    tft.clearScreen(BLACK); 

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextColor(WHITE);


    Serial.println(F("Screen initialized"));
}




#endif
