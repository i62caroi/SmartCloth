#include <Wire2.h>

#include <Wire.h>

#include "RA8876_v2.h"
#include "COLORS.h"

#include "pic16bpp_word.h"
#include "pic16bpp_byte.h"
#include "bw.h"
#include "pattern6.h"
#include "pattern11.h"
#include "bug1.h"


// Screen circuit wiring
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10

#define SD_CARD_SCS   4


RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);

#define SCREEN_WIDTH  1024
#define SCREEN_HEIGHT 600

bool done = false;

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


}



void clearPage1(){
  //clear page1
  tft.canvasImageStartAddress(PAGE1_START_ADDR);
  tft.canvasImageWidth(SCREEN_WIDTH);
  tft.setCanvasWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.clearScreen(BLUE);
  //tft.fillRect(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, BLUE);
}


void loop() {
   //unsigned short i;
    if(!done){

        
        /*
        
        La cantidad de datos para la pantalla a color es enorme. Si la velocidad de operación de escritura 
        de MPU no es lo suficientemente rápida, se puede ver que la línea de escaneo de actualización en la 
        pantalla se muestra como una cascada. 

        En otros casos, se pueden necesitar efectos dinámicos para la pantalla, como una imagen de fondo 
        estática (como papel tapiz), y se cambia el texto o la imagen de primer plano. Para este efecto 
        en el caso de codificación regular, el programador debe volver a escribir los datos de fondo y 
        luego actualizar el texto de primer plano o los datos de imagen. 
        Si ejecutamos directamente el contenido actual de la memoria de visualización, provocará el parpadeo 
        de la pantalla al actualizar el datos de fondo. Si actualiza el texto de primer plano o los datos de 
        la imagen directamente sin volver a escribir los datos de fondo, se superpondrá la imagen.
        
        Por lo tanto, si se desea obtener un mejor efecto de visualización, se puede aprovechar la función BTE: 
        en primer lugar, los datos de la imagen se pueden escribir en el área no mostrada de la memoria de 
        visualización a través de la interfaz MPU o la interfaz DMA, y luego usar la función de copia de 
        memoria BTE para duplicar y mover los datos de la imagen al área de memoria de visualización, para evitar 
        el efecto de visualización deficiente que se describe anteriormente.


        => Tras las pruebas realizadas, se ha visto que usar MEMORY COPY evita la cascada más que MPU WRITE.

        */

        // --------- bteMemoryCopyDemo() --------------------------------
        //clear page1
        clearPage1();
  
        //clear page2
        tft.canvasImageStartAddress(PAGE2_START_ADDR);
        tft.clearScreen(RED);
        //tft.drawSquareFill(0, 0, SCREEN_WIDTH-1, SCREEN_HEIGHT-1, RED);
        
        //write picture to page2
        tft.putPicture_16bpp(50,50,128,128,pic16bpp_word);
        //tft.sdCardDraw256bitsBIN(250,150,482,350,"bin/logo.bin");
        //delay(200);
        
        //write string to page1
        tft.canvasImageStartAddress(PAGE1_START_ADDR);
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextColor(WHITE,BLACK,RA8876_TEXT_TRANS_ON);
        tft.putString(0,0,"BTE memory copy,copy page2 picture to page1 display");
        
        //copy page2 picture to page1 
        tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH, 50,50,128,128); 
        tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH, (50+128),50,128,128);
        tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH, (50+128+128),50,128,128);
        //tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,250,150,PAGE1_START_ADDR,SCREEN_WIDTH,250,150,482,350);
        //tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,250,150,PAGE1_START_ADDR,SCREEN_WIDTH, (250+128),150,128,128);
        //tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,250,150,PAGE1_START_ADDR,SCREEN_WIDTH, (250+128+128),150,128,128);
        
        //tft.mainImageStartAddress(PAGE2_START_ADDR);
        delay(2000);
        //while(1);

        // ------------------------------------------------------------



        // ------------- bteMemoryCopyROPDemo() -----------------------
        //write string to page1
        tft.canvasImageStartAddress(PAGE1_START_ADDR);
        tft.putString(0,178,"BTE memory copy with ROP, copy page2 picture to page1 display"); 

        //copy page2 picture to page1 with ROP
        tft.bteMemoryCopyWithROP(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,50,228,
                                  PAGE1_START_ADDR,SCREEN_WIDTH, 50,228,128,128,RA8876_BTE_ROP_CODE_1); // ~S0・~S1 or ~ ( S0+S1 ) 

        tft.bteMemoryCopyWithROP(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,(50+128),228,
                                  PAGE1_START_ADDR,SCREEN_WIDTH, (50+128),228,128,128,RA8876_BTE_ROP_CODE_2); // ~S0・S1

        tft.bteMemoryCopyWithROP(PAGE2_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,(50+128+128),228,
                                  PAGE1_START_ADDR,SCREEN_WIDTH, (50+128+128),228,128,128,RA8876_BTE_ROP_CODE_3);  // ~S0
        
        delay(2000);
        // ------------------------------------------------------------



        // ------------- bteMemoryCopyChromaDemo() --------------------
        //write string to page1 (ya estamos en PAGE1)
        tft.canvasImageStartAddress(PAGE1_START_ADDR);
        tft.putString(0,356,"BTE memory copy with ChromaKey, copy page2 picture to page1 display"); 

        //copy page2 picture to page1 with Chroma
        tft.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,50,50, PAGE1_START_ADDR,SCREEN_WIDTH,50,406,128,128,0xf800);
        tft.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,50,50, PAGE1_START_ADDR,SCREEN_WIDTH,50+128,406,128,128,0xf800);
        tft.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,50,50, PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,406,128,128,0xf800);
        delay(2000); 
        //while(1);

        clearPage1();
        // ------------------------------------------------------------

        

        // ------------- bteMPUWriteROPDemo() ------------------------- 
       // clearPage1();

        //write string to page1 (ya estamos en PAGE1)
        tft.canvasImageStartAddress(PAGE1_START_ADDR);
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextColor(WHITE,BLACK,RA8876_TEXT_TRANS_ON);
        tft.putString(0,0,"BTE MPU write with ROP, write picture to page1, format byte");

        tft.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,
                                      50,50,128,128,RA8876_BTE_ROP_CODE_4,pic16bpp_byte);
        tft.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,50,PAGE1_START_ADDR,SCREEN_WIDTH,
                                      50+128,50,128,128,RA8876_BTE_ROP_CODE_5,pic16bpp_byte);                         
        tft.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,50,PAGE1_START_ADDR,SCREEN_WIDTH,
                                      50+128+128,50,128,128,RA8876_BTE_ROP_CODE_6,pic16bpp_byte); 

        delay(2000);

        tft.putString(0,178,"BTE MPU write with ROP, write picture to page1, format word");        

        tft.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50,228,PAGE1_START_ADDR,SCREEN_WIDTH,
                                      50,228,128,128,RA8876_BTE_ROP_CODE_7,pic16bpp_word);
        tft.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,228,PAGE1_START_ADDR,SCREEN_WIDTH,
                                      50+128,228,128,128,RA8876_BTE_ROP_CODE_8,pic16bpp_word);                         
        tft.bteMpuWriteWithROP(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,228,PAGE1_START_ADDR,SCREEN_WIDTH,
                                      50+128+128,228,128,128,RA8876_BTE_ROP_CODE_9,pic16bpp_word); 

        delay(2000);
        // ------------------------------------------------------------



        // ------------- bteMPUWriteChromaDemo() ------------------------- 

        tft.putString(0,356,"BTE MPU write with Chroma Key, write picture to page1, format byte,word"); 

        tft.bteMpuWriteWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH, 50,406,128,128,0xf800,pic16bpp_byte);
        tft.bteMpuWriteWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH, 50+128,406,128,128,0xf800,pic16bpp_word);
        
        delay(2000);
        //while(1);
        
        // ------------------------------------------------------------



        // ------------- bteMPUWriteColorExpansionDemo() -----------------------
        clearPage1();

        //write string to page1 (ya estamos en PAGE1)
        tft.canvasImageStartAddress(PAGE1_START_ADDR);
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextColor(WHITE,BLACK,RA8876_TEXT_TRANS_ON);
        tft.putString(0,0,"BTE MPU write with color expansion, write black and white picture data to page1");

        tft.bteMpuWriteColorExpansion(PAGE1_START_ADDR,SCREEN_WIDTH,50,50,128,128,BLACK,WHITE,bw);
        tft.bteMpuWriteColorExpansion(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,50,128,128,WHITE,BLACK,bw);
        tft.bteMpuWriteColorExpansion(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,50,128,128,YELLOW,CYAN,bw);

        delay(2000);

        tft.canvasImageStartAddress(PAGE1_START_ADDR);
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextColor(WHITE,BLACK,RA8876_TEXT_TRANS_ON);
        tft.putString(0,178,"BTE MPU write with color expansion with chroma key, write black and white picture data to page1");
        
        tft.bteMpuWriteColorExpansionWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH,50,228,128,128,BLACK,WHITE,bw);
        tft.bteMpuWriteColorExpansionWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH,50+128,228,128,128,WHITE,BLACK,bw);
        tft.bteMpuWriteColorExpansionWithChromaKey(PAGE1_START_ADDR,SCREEN_WIDTH,50+128+128,228,128,128,YELLOW,BLACK,bw);
        delay(2000);
        //while(1);
        
        //clear page1
        //clearPage1();

        // Lo de PATTERN se ha ignorado porque no nos sirve.
        // ------------------------------------------------------------


      done = true;
    }
}


