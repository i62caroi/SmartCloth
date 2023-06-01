/*
-------------- MEMORIA SDRAM --------------------------------------------------------------------
  * Memoria SDRAM tipo W9812G6KH-6 [3]:
	  		- 2 M words x 4 banks x 16 bits (128 Mbits)
	  		- 166 MHz de frecuencia de reloj
	  		- modo auto-refresh
	  		- 64 ms de tiempo de refresh
	  		- CAS Latency: 2 y 3
	  		- Velocidad de acceso CL3 (3 ciclos de reloj)
        - Row address: A0-A11
	  		- Column address: A0-A8

  	  Esto se corresponde con los siguientes valores de Datasheet 8.1.2 SDRAM Connection del RA8876:
        Density: 128 Mb (4 banks) 
        Addressing => row: [A0-A11]
                   => column: [A0-A8]  
  -------------------------------------------------------------------------------------------------
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


    
    
    loadPictures(); // CARGAR IMÁGENES EN PÁGINAS 2 Y 3 PARA LUEGO COPIARLAS RÁPIDAMENTE



    arranque();  // PAGE2   (OK)
    delay(3000);
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(BLACK);
    delay(500);
    
    pantalla_inicial(); // PAGE2  (OK)
    delay(3000);
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(BLACK);
    delay(500);

    select_Grupo();   // PAGE3 (~OK)
    delay(3000);
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(BLACK);
    delay(500);

    add_Plato();      // PAGE3 (~OK)
    delay(3000);
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(BLACK);
    delay(500);

    delete_Plato();   // PAGE3 (~OK)
    delay(3000);
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(BLACK);
    delay(500);

    save_Comida();    // PAGE4 (~OK)
    delay(3000);
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(BLACK);
    delay(500);

    crudo_cocinado(); // PAGE4 (OK)
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void loop() {
     
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------- CARGA DE IMÁGENES EN MEMORIA --------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loadPictures(){
  /*
    ------------------ POSICIONES DE IMAGENES EN LAS PAGINAS ----------------------------------------------------------------------------------------------------
    PAGE 2:
        S -> (0,0)    M -> (96,0)   A -> (201,0)    R -> (306,0)    T -> (392,0)    C -> (497,0)    L -> (583,0)   O -> (669,0)   H  -> (775,0)   Log -> (841,0)
        brain1  ->  (0,170)   brain2  ->  (121,170)
        
    PAGE 3:
        grupo1 -> (0,0)   grupo2 -> (131,0)   grupo3 -> (262,0)   grupo4 -> (393,0)   mano -> (524,0)    anadir -> (645,0)    borrar -> (818,0)
        guardar -> (0,131)   cociGra -> (173,131)   crudoGra -> (351,131)    reloj1 -> (529,131)    
        logFull ->  (0,292)
        
    PAGE 4:
        reloj2 -> (0,0)    reloj3 -> (261,0) reloj4 -> (522,0)
        
    --------------------------------------------------------------------------------------------------------------------------------------------------------------
  */

    // ----------------- CARGA INICIAL -------------------------------------------------------------------
      // -------- LOGFULL Y RELOJ1 -------
      tft.canvasImageStartAddress(PAGE3_START_ADDR);
      tft.clearScreen(BLACK);
      tft.sdCardDraw16bppBIN256bits(0,292,324,67,"bin/carga/logFull.bin"); // logFull (324x67) => x = 0   ->   y = <brain2(170) + brain2(108) + 1 = 279   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(529,131,260,410,"bin/carga/reloj1.bin"); // reloj1 (260x410) => x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = 131   ----- Escribiendo en page3
      tft.canvasImageStartAddress(PAGE1_START_ADDR); 
      tft.clearScreen(WHITE);
      tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,279,PAGE1_START_ADDR,SCREEN_WIDTH,680,513,324,67); // logFull (324x67) -> x = 1024 - logFull(324) - 20 = 680  -> y = 600 - logFull(67) - 20 = 513
      tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // reloj1 (260x410) -> x = 512 +/- 130 = 382     y = 300 +/- 205 = 95

      // -------- RELOJ2, RELOJ3 Y RELOJ4 -------
      tft.canvasImageStartAddress(PAGE4_START_ADDR);
      tft.clearScreen(BLACK);
      tft.sdCardDraw16bppBIN256bits(0,0,260,410,"bin/carga/reloj2.bin");   // reloj2 (260x410) =>  ----- Escribiendo en page4
      tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // x = 512 +/- 130 = 382     y = 300 +/- 205 = 95
      tft.sdCardDraw16bppBIN256bits(261,0,260,410,"bin/carga/reloj3.bin"); // reloj3 (260x410) => x = <reloj2(0) + reloj2(260) + 1 = 261     ->   y = 0   ----- Escribiendo en page4
      tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,261,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // x = 512 +/- 130 = 382     y = 300 +/- 205 = 95
      tft.sdCardDraw16bppBIN256bits(522,0,260,410,"bin/carga/reloj4.bin"); // reloj4 (260x410) => x = <reloj3(261) + reloj3(260) + 1 = 522   ->   y = 0   ----- Escribiendo en page4
      tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,522,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // x = 512 +/- 130 = 382     y = 300 +/- 205 = 95
    // ------------- FIN CARGA INICIAL --------------------------------------------------------------




    // ----------------- ARRANQUE -------------------------------------------------------------------
      tft.canvasImageStartAddress(PAGE2_START_ADDR);
      // -------- LETRAS S-M-A-R-T -------
      tft.sdCardDraw16bppBIN256bits(0,0,95,159,"bin/arranque/S.bin");      // S  (95x159)  =>  x  =  <S  =  0                                ->    y = 0   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(96,0,104,159,"bin/arranque/M.bin");    // M  (104x154) =>  x  =  <M  =  <S(0)    + S(95)   + 1  =  96    ->    y = 0   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(201,0,104,159,"bin/arranque/A.bin");   // A  (104x159) =>  x  =  <A  =  <M(96)   + M(104)  + 1  =  201   ->    y = 0   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(306,0,85,159,"bin/arranque/R.bin");    // R  (85x159)  =>  x  =  <R  =  <A(201)  + A(104)  + 1  =  306   ->    y = 0   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(392,0,104,159,"bin/arranque/T.bin");   // T1 (104x159) =>  x  =  <T1 =  <R(306)  + R(85)   + 1  =  392   ->    y = 0   ----- Escribiendo en page2

    // -------- RELOJ1 (CARGA) -------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // x = 512 +/- 130 = 382     y = 300 +/- 205 = 95

      // -------- LETRAS C-L-O-H -------
      tft.sdCardDraw16bppBIN256bits(497,0,85,159,"bin/arranque/C.bin");    // C  (85x159)  =>  x  =  <C  =  <T1(392) + T1(104) + 1  =  497   ->    y = 0   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(583,0,85,159,"bin/arranque/L.bin");    // L  (85x159)  =>  x  =  <L  =  <C(497)  + C(85)   + 1  =  583   ->    y = 0   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(669,0,85,159,"bin/arranque/O.bin");    // O  (85x159)  =>  x  =  <O  =  <L(583)  + L(85)   + 1  =  669   ->    y = 0   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(755,0,85,159,"bin/arranque/H.bin");    // H  (85x159)  =>  x  =  <H  =  <O(669)  + O(85)   + 1  =  755   ->    y = 0   ----- Escribiendo en page2

    // -------- RELOJ2 (CARGA) -------
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // x = 512 +/- 130 = 382     y = 300 +/- 205 = 95

      // --------- LOGO ---------
      tft.sdCardDraw16bppBIN256bits(841,0,162,169,"bin/arranque/Log.bin"); // log (162x169) => x  = <log =  <H(755)  + H(85)   + 1  =  841   ->    y = 0   ----- Escribiendo en page2
    // -------------- FIN ARRANQUE -----------------------------------------------------------------------



    // ----------------- PANTALLA INICIAL ----------------------------------------------------------------
      tft.sdCardDraw16bppBIN256bits(0,170,120,108,"bin/inicial/brain1.bin");    // brain1 (120x108) => x = 0 ->   y = <Log(0) + Log(169) + 1 = 170   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(121,170,120,108,"bin/inicial/brain2.bin");  // brain2 (120x108) => x = <brain2  = <brain1(0)  + brain1(120) + 1 = 121  ->  y = 170   ----- Escribiendo en page2
    
    // -------- RELOJ3 (CARGA) ------- 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,261,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // x = 512 +/- 130 = 382     y = 300 +/- 205 = 95
    // ------------- FIN PANTALLA INICIAL ----------------------------------------------------------------



    // ----------------- ESCOGER GRUPO -------------------------------------------------------------------
      tft.canvasImageStartAddress(PAGE3_START_ADDR);
      // -------- GRUPOS  -------
      tft.sdCardDraw16bppBIN256bits(0,0,130,125,"bin/grupo/assets/grupo1.bin");     // grupo1 (130x125))  =>  x  =  <grupo1  =  0                                      ->   y = 0  ----- Escribiendo en page3
      tft.sdCardDraw16bppBIN256bits(131,0,130,125,"bin/grupo/assets/grupo2.bin");   // grupo2 (130x125))  =>  x  =  <grupo2  =  <grupo1(0)   + grupo1(130) + 1 = 131   ->   y = 0  ----- Escribiendo en page3
      tft.sdCardDraw16bppBIN256bits(262,0,130,125,"bin/grupo/assets/grupo3.bin");   // grupo3 (130x125))  =>  x  =  <grupo3  =  <grupo2(131) + grupo2(130) + 1 = 262   ->   y = 0  ----- Escribiendo en page3
      tft.sdCardDraw16bppBIN256bits(393,0,130,125,"bin/grupo/assets/grupo4.bin");   // grupo4 (130x125))  =>  x  =  <grupo4  =  <grupo3(262) + grupo3(130) + 1 = 393   ->   y = 0  ----- Escribiendo en page3

    // -------- RELOJ4 (CARGA) ------- 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,522,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // x = 512 +/- 130 = 382     y = 300 +/- 205 = 95

      // -------- MANOS (tb para botones) ---------
      // Estas imágenes dejan residuos de rojo o verde al aplicar el chroma porque no todos los píxeles
      // que se quieren eliminar tienen el color que se quiere filtrar (no es un rojo o verde puro, sino
      // que está mezclado con blanco al encontrarse en el borde de la figura)
      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,"bin/grupo/assets/manoG.bin");    /// manoG (120x129)  =>  x  =  <manoG  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  ----- Escribiendo en page3
      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,"bin/grupo/assets/manoR.bin");    // manoR (120x129)   =>  x  =  <manoR  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  ----- Escribiendo en page3
      // Esta otra imagen con la mano roja y el fondo blanco se filtra mejor. Aunque queden residuos de
      // blanco en el borde de la figura, no queda mal.
      tft.sdCardDraw16bppBIN256bits(524,0,120,129,"bin/grupo/assets/handW.bin");    // handW (120x129)   =>  x  =  <handW  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  ----- Escribiendo en page3
    // --------------- FIN ESCOGER GRUPO -----------------------------------------------------------------



    // ----------------- AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO ----------------------------------------
    // -------- BOTONES  -------
      tft.sdCardDraw16bppBIN256bits(645,0,172,130,"bin/botones/assets/anadir.bin");       // anadir   (172x130)  =>  x  =  <anadir   =  <manoR(524) + manoR(120) + 1 = 645      ->   y = 0    ----- Escribiendo en page3
      tft.sdCardDraw16bppBIN256bits(818,0,172,130,"bin/botones/assets/borrar.bin");       // borrar   (172x130)  =>  x  =  <borrar   =  <anadir(645) + anadir(172) + 1 = 818    ->   y = 0    ----- Escribiendo en page3

    // -------- RELOJ1 (CARGA) ------- 
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // x = 512 +/- 130 = 382     y = 300 +/- 205 = 95

      // -------- BOTONES  -------
      tft.sdCardDraw16bppBIN256bits(0,131,172,130,"bin/botones/assets/guardar.bin");      // guardar  (172x130)  =>  x  =   0  ->   y = <borrar(0) + borrar(130) + 1 = 131  ----- Escribiendo en page3
      tft.sdCardDraw16bppBIN256bits(173,131,177,160,"bin/botones/assets/cociGra.bin");    // cociGra  (172x130)  =>  x  =  <cociGra  =  <guardar(0) + guardar(172) + 1 = 173    ->   y = 131  ----- Escribiendo en page3
      tft.sdCardDraw16bppBIN256bits(351,131,177,160,"bin/botones/assets/crudoGra.bin");   // crudoGra (172x130)  =>  x  =  <crudoGra =  <cociGra(131) + cociGra(177) + 1 = 351  ->   y = 131  ----- Escribiendo en page3
    
    // -------- RELOJ2 (CARGA) -------
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // x = 512 +/- 130 = 382     y = 300 +/- 205 = 95

    // --------- FIN AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO --------------------------------------------



    // Regresamos la dirección de inicio del canvas a la PAGE1
    tft.canvasImageStartAddress(PAGE1_START_ADDR);

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void arranque(){ // OK
    // Fondo blanco en page1
    tft.clearScreen(WHITE);
    delay(300);

    // S M A R T C L O T H ==> S T O T M L R A C H (orden de aparición)

    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,40,150,95,159);       // S (95x159) 
    delay(300);

    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,428,150,104,159);   // T1 (104x159)
    delay(300);

    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,669,0,PAGE1_START_ADDR,SCREEN_WIDTH,702,150,85,159);    // O (85x159)
    delay(300);
    
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,787,150,104,159);   // T2 (104x159) 
    delay(300);

    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,96,0,PAGE1_START_ADDR,SCREEN_WIDTH,135,150,104,159);    // M (104x154) 
    delay(300);
    
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,583,0,PAGE1_START_ADDR,SCREEN_WIDTH,617,150,85,159);    // L (85x159) 
    delay(300);

    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,306,0,PAGE1_START_ADDR,SCREEN_WIDTH,343,150,85,159);    // R (85x159) 
    delay(300);

    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,201,0,PAGE1_START_ADDR,SCREEN_WIDTH,239,150,104,159);   // A (104x159) 
    delay(300);

    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,497,0,PAGE1_START_ADDR,SCREEN_WIDTH,532,150,85,159);    // C (85x159) 
    delay(300);

    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,755,0,PAGE1_START_ADDR,SCREEN_WIDTH,891,150,85,159);    // H (85x159) 
    delay(300);   

    // LOGO
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,841,0,PAGE1_START_ADDR,SCREEN_WIDTH,417,350,162,169);   // Logo (162x169) ==> debajo
    delay(300);
    
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void pantalla_inicial(){ // PAGE2 (OK)

    // ----------- TEXTO ------------------------------------------------------------------------------------
    // Fondo rojo en page1
    tft.clearScreen(RED);

    // ----- INTERNAL FIXED 12x24 X3 --------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(130, 90);
    tft.println("COLOQUE UN RECIPIENTE");
    tft.setCursor(150, tft.getCursorY() + tft.getTextSizeY()-10);
    tft.print("EN LA ZONA DE PESADA"); 
      
  /*
    // ------ EXTERNAL ARIAL 8x16 escala x4 ------
    tft.initExternalFontRom(0, RA8876_FONT_ROM_GT30L16U2W);

    tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);

    tft.setTextForegroundColor(WHITE);
    //tft.ignoreTextBackground();
    tft.setTextScale(RA8876_TEXT_W_SCALE_X4, RA8876_TEXT_H_SCALE_X4); 
    tft.setCursor(100, 90);
    tft.println("COLOQUE UN RECIPIENTE");
    tft.setCursor(130, tft.getCursorY() + tft.getTextSizeY());
    tft.print("EN LA ZONA DE PESADA"); 
  */
  // --------------------------------------------------------------------------------------------------------


    // -------- CIRCULO ---------------
    //PAG 2 (OK) ==> Círculo (65 pixeles de diametro)
    tft.fillCircle(512,380,65,WHITE);
    delay(1000);

    // ------- CUADRADO REDONDEADO ----
    tft.fillRoundRect(447,315,577,445,10,WHITE); // x = 512 +/- 65   y = 380 +/- 65
    delay(1000);


    // ----------- PALITOS ----------------------------------------------------------------------------------
    //PAG 5 ==> palitos alrededor cuadrado. 4 pixeles entre barra y barra
    // Palitos izquierda 
    tft.fillRect(430, 366, 447, 374, WHITE); // Arriba (17x8)
    tft.fillRect(422, 378, 447, 386, WHITE); // Central (25x8)
    tft.fillRect(430, 390, 447, 398, WHITE); // Abajo (17x8)
    // Palitos derecha 
    tft.fillRect(577, 366, 594, 374, WHITE); // Arriba (17x8)
    tft.fillRect(577, 378, 602, 386, WHITE); // Central (25x8)
    tft.fillRect(577, 390, 594, 398, WHITE); // Abajo (17x8)
    // Palitos arriba 
    tft.fillRect(498, 298, 506, 315, WHITE); // Izquierda (8x17)
    tft.fillRect(510, 290, 518, 315, WHITE); // Central (8x25)
    tft.fillRect(522, 298, 530, 315, WHITE); // Derecha (8x17)
    // Palitos abajo 
    tft.fillRect(498, 445, 506, 462, WHITE); // Izquierda (8x17)
    tft.fillRect(510, 445, 518, 470, WHITE); // Central (8x25)
    tft.fillRect(522, 445, 530, 462, WHITE); // Derecha (8x17)
    delay(1000);
    // ------------------------------------------------------------------------------------------------------


    // ----------- BRAINS -----------------------------------------------------------------------------------
    // BRAIN1 --> centrar en el cuadrado blanco
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); 
    delay(1000);

    // BRAIN2 (rojo)
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,121,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); 
    // ------------------------------------------------------------------------------------------------------

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void select_Grupo(){ // PAGE3 (OK)

    // ----- TEXTO (INTERNAL FIXED 12x24 X3) --------------------------------------------------------------
    // Fondo rojo en page1
    tft.clearScreen(RED);

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(130, 50);
    tft.println("SELECCIONE UN GRUPO");
    tft.setCursor(255, tft.getCursorY() + tft.getTextSizeY()-10);
    tft.print("DE ALIMENTOS"); 
    delay(1000);
    // ----------------------------------------------------------------------------------------------------


    // ------------ LINEAS --------------------------------------------------------------------------------
    tft.fillRoundRect(0,250,220,258,3,WHITE);
    tft.fillRoundRect(0,450,512,458,3,WHITE);
    delay(1000);
    // ----------------------------------------------------------------------------------------------------

    
    // ------------ GRUPOS --------------------------------------------------------------------------------
    // 50 pixeles entre cuadro y cuadro

    // ------ Grupo 1 ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125); 
    delay(800);

    // ------ Grupo 2 ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125); // 396 = 236(X inicio de g1) + 130(width g1) + 30(espacio)
    delay(800);

    // ------ Grupo 3 ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); // 556 = 396(X inicio de g1) + 130(width g2) + 30(espacio)
    delay(800);

    // ------ Grupo 4 ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125); // 716 = 556(X inicio de g1) + 130(width g2) + 50(espacio)
    delay(800);
    // ----------------------------------------------------------------------------------------------------


    // ------------ CUADRADO REDONDEADO (PULSACION) -------------------------------------------------------
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados redondeados, separados por 1 píxel en cada dirección, para simular un grosor mayor.
    tft.drawRoundRect(556,288,680,413,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(555,287,681,414,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(554,286,682,415,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(553,285,683,416,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(552,284,684,417,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(551,283,685,418,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(550,282,686,419,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(549,281,687,420,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(548,280,688,421,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(547,279,689,422,20,YELLOW); // Alrededor de grupo3
    tft.drawRoundRect(546,278,690,423,20,YELLOW); // Alrededor de grupo3
    // ----------------------------------------------------------------------------------------------------


    // ------------ MANO -----------------------------------------------------------------------------------
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,556,370,120,129,WHITE); // handW 
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,556,370,120,129,RED); // manoR
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,556,370,120,129,GREEN_HAND); // manoG
    // ----------------------------------------------------------------------------------------------------

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void add_Plato(){ // Tb PAGE3, pero más abajo
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    // Fondo rojo en page1
    tft.clearScreen(RED);

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(30, 30);
    tft.println("\xBF""EST\xC1"" SEGURO DE QUE QUIERE");
    tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-30);
    tft.print("A\xD1""ADIR UN NUEVO PLATO\x3F"""); 
    delay(1000);
    // ----------------------------------------------------------------------------------------------------


    // ------------ LINEA --------------------------------------------------------------------------------
    tft.fillRoundRect(252,220,764,228,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // ----- TEXTO (CONFIRMACIÓN) -------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setCursor(150, 270);
    tft.println("PARA CONFIRMAR, PULSE DE NUEVO");
    tft.setCursor(400, tft.getCursorY() + tft.getTextSizeY()+10);
    tft.print("EL BOT\xD3""N"); 
    // ----------------------------------------------------------------------------------------------------


    // ------------ BOTÓN AÑADIR --------------------------------------------------------------------------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); 
    delay(800);
    // ----------------------------------------------------------------------------------------------------

    
     // ------------ CUADRADO REDONDEADO (PULSACION) -------------------------------------------------------
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados redondeados, separados por 1 píxel en cada dirección, para simular un grosor mayor.
    tft.drawRoundRect(425,413,590,535,20,YELLOW); // Alrededor de botón
    tft.drawRoundRect(424,412,591,536,20,YELLOW); 
    tft.drawRoundRect(423,411,592,537,20,YELLOW); 
    tft.drawRoundRect(422,410,593,538,20,YELLOW); 
    tft.drawRoundRect(421,409,594,539,20,YELLOW); 
    tft.drawRoundRect(420,408,595,540,20,YELLOW); 
    tft.drawRoundRect(419,407,596,541,20,YELLOW); 
    tft.drawRoundRect(418,406,597,542,20,YELLOW); 
    tft.drawRoundRect(417,405,598,543,20,YELLOW); 
    tft.drawRoundRect(416,404,599,544,20,YELLOW); 
    tft.drawRoundRect(415,403,600,545,20,YELLOW); 
    // ----------------------------------------------------------------------------------------------------


    // ------------ MANO -----------------------------------------------------------------------------------
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,WHITE); // handW 
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,RED); // manoR
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,GREEN_HAND); // manoG
    // ----------------------------------------------------------------------------------------------------


    delay(2000);

    // ----- TEXTO (PLATO AÑADIDO) -------------------------------------------------------------------------
    tft.clearScreen(RED);
    // ------ LINEA ---------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------ TEXTO ---------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setCursor(170, 258);
    tft.println("NUEVO PLATO A\xD1""ADIDO");
    // ------ LINEA ---------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // ----------------------------------------------------------------------------------------------------
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void delete_Plato(){ // Tb PAGE3, pero más a la derecha
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    // Fondo rojo en page1
    tft.clearScreen(RED);

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(30, 30);
    tft.println("\xBF""EST\xC1"" SEGURO DE QUE QUIERE");
    tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-30);
    tft.print("BORRAR EL PLATO ACTUAL\x3F"""); 
    delay(1000);
    // ----------------------------------------------------------------------------------------------------


    // ------------ LINEA --------------------------------------------------------------------------------
    tft.fillRoundRect(252,220,764,228,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // ----- TEXTO (CONFIRMACIÓN) -------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setCursor(150, 270);
    tft.println("PARA CONFIRMAR, PULSE DE NUEVO");
    tft.setCursor(400, tft.getCursorY() + tft.getTextSizeY()+10);
    tft.print("EL BOT\xD3""N"); 
    // ----------------------------------------------------------------------------------------------------


    // ------------ BOTÓN ELIMINAR -------------------------------------------------------------------------------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); 
    delay(800);
    // ----------------------------------------------------------------------------------------------------


     // ------------ CUADRADO REDONDEADO (PULSACION) -------------------------------------------------------
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados redondeados, separados por 1 píxel en cada dirección, para simular un grosor mayor.
    tft.drawRoundRect(425,413,590,535,20,YELLOW); // Alrededor de botón
    tft.drawRoundRect(424,412,591,536,20,YELLOW); 
    tft.drawRoundRect(423,411,592,537,20,YELLOW); 
    tft.drawRoundRect(422,410,593,538,20,YELLOW); 
    tft.drawRoundRect(421,409,594,539,20,YELLOW); 
    tft.drawRoundRect(420,408,595,540,20,YELLOW); 
    tft.drawRoundRect(419,407,596,541,20,YELLOW); 
    tft.drawRoundRect(418,406,597,542,20,YELLOW); 
    tft.drawRoundRect(417,405,598,543,20,YELLOW); 
    tft.drawRoundRect(416,404,599,544,20,YELLOW); 
    tft.drawRoundRect(415,403,600,545,20,YELLOW); 
    // ----------------------------------------------------------------------------------------------------


    // ------------ MANO -----------------------------------------------------------------------------------
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,WHITE); // handW 
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,RED); // manoR
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,GREEN_HAND); // manoG
    // ----------------------------------------------------------------------------------------------------


    delay(2000);

    // ----- TEXTO (PLATO BORRADO) -------------------------------------------------------------------------
    tft.clearScreen(RED);
    // ------ LINEA ---------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------ TEXTO ---------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setCursor(100, 258);
    tft.println("PLATO ACTUAL ELIMINADO");
    // ------ LINEA ---------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // ----------------------------------------------------------------------------------------------------
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void save_Comida(){ // Tb PAGE3, pero más a la derecha
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    // Fondo rojo en page1
    tft.clearScreen(RED);

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(30, 20);
    tft.println("\xBF""EST\xC1"" SEGURO DE QUE QUIERE");
    tft.setCursor(80, tft.getCursorY() + tft.getTextSizeY()-30);
    tft.print("GUARDAR LA COMIDA ACTUAL\x3F"""); 

    delay(500);

    // ----- TEXTO (COMEMTARIO) ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setCursor(100, 180);
    tft.println("LOS VALORES NUTRICIONALES PASAR\xC1""N AL ACUMULADO DE HOY");
    // -----------------------------------

    delay(1000);
    // ----------------------------------------------------------------------------------------------------


    
    // ------------ LINEA --------------------------------------------------------------------------------
    tft.fillRoundRect(252,250,764,258,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // ----- TEXTO (CONFIRMACIÓN) -------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setCursor(150, 300);
    tft.println("PARA CONFIRMAR, PULSE DE NUEVO");
    tft.setCursor(400, tft.getCursorY() + tft.getTextSizeY()+10);
    tft.print("EL BOT\xD3""N"); 
    // ----------------------------------------------------------------------------------------------------


    // ------------ BOTÓN ELIMINAR ------------------------------------------------------------------------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,450,172,130); 
    delay(800);
    // ----------------------------------------------------------------------------------------------------


     // ------------ CUADRADO REDONDEADO (PULSACION) -------------------------------------------------------
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados redondeados, separados por 1 píxel en cada dirección, para simular un grosor mayor.
    tft.drawRoundRect(425,453,590,575,20,YELLOW); // Alrededor de botón
    tft.drawRoundRect(424,452,591,576,20,YELLOW); 
    tft.drawRoundRect(423,451,592,577,20,YELLOW); 
    tft.drawRoundRect(422,450,593,578,20,YELLOW); 
    tft.drawRoundRect(421,449,594,579,20,YELLOW); 
    tft.drawRoundRect(420,448,595,580,20,YELLOW); 
    tft.drawRoundRect(419,447,596,581,20,YELLOW); 
    tft.drawRoundRect(418,446,597,582,20,YELLOW); 
    tft.drawRoundRect(417,445,598,583,20,YELLOW); 
    tft.drawRoundRect(416,444,599,584,20,YELLOW); 
    tft.drawRoundRect(415,443,600,585,20,YELLOW); 
    // ----------------------------------------------------------------------------------------------------


    // ------------ MANO -----------------------------------------------------------------------------------
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,WHITE); // handW 
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,RED); // manoR
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,GREEN_HAND); // manoG
    // ----------------------------------------------------------------------------------------------------


    delay(2000);

    // ----- TEXTO (PLATO AÑADIDO) -------------------------------------------------------------------------
    tft.clearScreen(RED);
    // ------ LINEA ---------
    tft.fillRoundRect(252,150,764,158,3,WHITE);
    // ------ TEXTO ---------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setCursor(100, 208);
    tft.println("COMIDA ACTUAL GUARDADA");
    // ------ LINEA ---------
    tft.fillRoundRect(252,330,764,338,3,WHITE);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setCursor(170, 388);
    tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO");
    tft.setCursor(350, tft.getCursorY() + tft.getTextSizeY()+40);
    tft.print("AL ACUMULADO DE HOY"); 
    // ----------------------------------------------------------------------------------------------------
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void crudo_cocinado(){ // Tb PAGE3, pero más a la derecha
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    // Fondo rojo en page1
    tft.clearScreen(RED);

    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(100, 30);
    tft.println("\xBF""EL ALIMENTO EST\xC1""");
    tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-30);
    tft.print("COCINADO O CRUDO\x3F"""); 
    delay(1000);
    // ----------------------------------------------------------------------------------------------------


    // ------------ LINEAS --------------------------------------------------------------------------------
    tft.fillRoundRect(0,250,256,258,3,WHITE);
    tft.fillRoundRect(768,517,1024,525,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // ------------ BOTONES -------------------------------------------------------------------------------
    // ------------ COCINADO 1 -------------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); 
    delay(1000);

    // ------------ CRUDO 1 ----------------
    // Borrar imagen cocinado 1 de la page1
    tft.clearArea(280,280,497,470,RED); 
    // Escribir imagen crudo en page1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160); 
    delay(1000);

    // ------------ COCINADO 2 -------------
    // Borrar imagen crudo 1 de la page1
    tft.clearArea(500,280,724,480,RED); 
    // Escribir imagen cocinado en page1    
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); 
    delay(1000);

    // ------------ CRUDO 2 ----------------
    // Borrar imagen cocinado 2 de la page1
    tft.clearArea(280,280,497,470,RED); 
    // Escribir imagen crudo en page1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160); 
    // ----------------------------------------------------------------------------------------------------





}



