

// Un programa que traduzca las palabras con caracteres especiales (á, Á, ñ, Ñ, ?¿, !¡) a sus 
// equivalentes en HEX para escribirlos en una pantalla TFT (\xE1, \xC1, \xF1, \xD1, \xBF, \xA1)
// y que muestre el texto en la pantalla TFT. Usar la librería RA8876_v2 de los ficheros 
// RA8876_v2.h y RA8876_v2.cpp


#include "Screen.h"
#include "cadenas.h"


void setup() 
{
    // Initialize Serial communication
    Serial.begin(115200);
    delay(100);

    // Initialize TFT display
    setupScreen();
    delay(100);

    
}

void loop() 
{
    // Mostrar ejemplos sin convertir
    tft.setCursor(10, 10);
    for(int i = 0; i < 8; i++){
        tft.println(ejemplos[i]);
        tft.setCursor(10, tft.getCursorY() + 5);
    }    

    
    // Mostrar ejemplos convertidos
    tft.setCursor(10, 270);
    for(int i = 0; i < 8; i++){
        convertSpecialCharactersToHEX_ref(ejemplos[i]);
        tft.println(ejemplos[i]);
        tft.setCursor(10, tft.getCursorY() + 5);
    }

    tft.setCursor(10, tft.getCursorY() + 30);
    tft.println(convertSpecialCharactersToHEX("¡Hola! ¿Qué tal?/¿Cómo vas?..."));
}

