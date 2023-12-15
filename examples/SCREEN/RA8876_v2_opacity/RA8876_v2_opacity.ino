/*
  File: RA8876_v2_opacity.ino
  Author: Irene Casares rodríguez
  Date: 09/06/23

  La nueva función 'bteMemoryCopyWithOpacity()' permite hacer la suma ponderada de dos fuentes 
  que estén en la SDRAM, aplicando un nivel de alpha que determine la opacidad/transparencia.


  El chip RA8876 permite dos modos para aplicar esta suma ponderada u opacidad:
    - Modo píxel: solo opera en 8bpp/16bpp y cada píxel tiene su propio valor de opacidad (nivel alpha).
    - Modo imagen: opera en 8bpp/16bpp/24bpp y tiene un solo valor de opacidad (nivel alpha) para toda la imagen.


  Para nuestro caso se ha implementado el modo imagen para aplicar el mismo nivel de transparencia a toda la imagen.
  En este modo, la suma ponderada se obtiene con la siguiente fórmula: 

      Destination = (S0 * (1 - alpha)) + (S1 * alpha)


  Por tanto, cuanto mayor sea el nivel alpha, menor "presencia" tendrá la imagen S0 y más se verá la S1. 


  Por otro lado, la opacidad va de 0.0 (totalmente transparente) a 1.0 (totalmente opaco), teniendo un rango de 0 a 32.


  En este ejemplo se dan diferentes niveles de alpha a la misma suma ponderada de la imagen de 'cocinado' y 
  un rectángulo negro. Estos niveles son 0, 8, 16 y 24, siendo el 'cocinado' cada vez más transparente 
  conforme aumenta el nivel alpha.

*/

#include "RA8876_v2.h"
#include "COLORS.h"

// Screen circuit wiring
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10

#define SD_CARD_SCS   4

RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);


#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 600

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    while (!Serial);
    delay(1000);

    
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

    // Limpiar pantalla mientras se cargan las imágenes
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(BLACK);


    cargarImages(); // Carga imágenes de crudo y cocinado grandes
    showImages(); // Muestra cocinado con diferentes niveles de alpha (transparencia/opacidad)

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void loop() {
     
}





void cargarImages(){
    tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3

    // cociGra
    tft.sdCardDraw16bppBIN256bits(173,131,177,160,"bin/botones/cociGra.bin"); // Cargar cociGra (172x130) en PAGE3 =>  x  =  <guardar(0) + guardar(172) + 1 = 173    ->   y = 131  
          
    // crudoGra
    tft.sdCardDraw16bppBIN256bits(351,131,177,160,"bin/botones/crudoGra.bin"); // Cargar crudoGra (172x130) en PAGE3 =>  x  =  <cociGra(131) + cociGra(177) + 1 = 351  ->   y = 131  
    
    // Regresamos la dirección de inicio del canvas a la PAGE1 
    tft.canvasImageStartAddress(PAGE1_START_ADDR); 
}


void showImages(){
    // Cocinado (S0) vs fondo oscuro en page1 (S1) ==> RA8876_ALPHA_OPACITY_0 --> 0/32 opacidad de S1 --> cocinado con 0/4 opacidad de fondo oscuro (S1) (cocinado normal)
    tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,827,20,PAGE1_START_ADDR,SCREEN_WIDTH,200,100,177,160,RA8876_ALPHA_OPACITY_0);

    // Cocinado (S0) vs fondo oscuro en page1 (S1) ==> RA8876_ALPHA_OPACITY_8 --> 8/32 opacidad de S1 --> cocinado con 1/4 opacidad de fondo oscuro (S1)
    tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,827,20,PAGE1_START_ADDR,SCREEN_WIDTH,500,100,177,160,RA8876_ALPHA_OPACITY_8);

    // Cocinado (S0) vs fondo oscuro en page1 (S1) ==> RA8876_ALPHA_OPACITY_16 --> 16/32 opacidad de S1 --> cocinado con 2/4 opacidad de fondo oscuro (S1)
    tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,827,20,PAGE1_START_ADDR,SCREEN_WIDTH,200,383,177,160,RA8876_ALPHA_OPACITY_16);

    // Cocinado (S0) vs fondo oscuro en page1 (S1) ==> RA8876_ALPHA_OPACITY_24 --> 8/32 opacidad de S1 --> cocinado con 3/4 opacidad de fondo oscuro (S1)
    tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,827,20,PAGE1_START_ADDR,SCREEN_WIDTH,500,383,177,160,RA8876_ALPHA_OPACITY_24);
}








