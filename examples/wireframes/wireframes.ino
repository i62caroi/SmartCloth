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


    
    //loadPicturesRelojCompleto();  // 15 segundos solamente cargando imágenes de reloj (girando)
    //loadPicturesRelojCompletoMitad(); // 1/2 de tamaño para tardar menos
    loadPicturesRelojCompletoCuarto(); // 1/4 de tamaño


//    dashboard(); // PAGE3


  /*  arranque();  // PAGE2   (OK)
    delay(3000);
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(BLACK);
    delay(500);
   
    pantalla_inicial(); // PAGE2  (OK)
    delay(3000);
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(BLACK);
    delay(500);
*/
    select_Grupo();   // PAGE3 (~OK)
    delay(3000);
/*    tft.canvasImageStartAddress(PAGE1_START_ADDR);
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

    crudo_cocinado(); // PAGE4 (OK)*/
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



void loadPicturesRelojCompleto(){
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

      tft.sdCardDraw16bppBIN256bits(0,292,324,67,"bin/carga/full/logFull.bin"); // Cargar logFull (324x67) => x = 0   ->   y = <brain2(170) + brain2(108) + 1 = 279   ----- Escribiendo en page2
      tft.sdCardDraw16bppBIN256bits(529,131,260,410,"bin/carga/full/reloj1.bin"); // Cargar reloj1 (260x410) => x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = 131   ----- Escribiendo en page3
      
      tft.canvasImageStartAddress(PAGE1_START_ADDR); 
      tft.clearScreen(WHITE);
      
      tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,292,PAGE1_START_ADDR,SCREEN_WIDTH,690,523,324,67); // Mostrar logFull (324x67) -> x = 1024 - logFull(324) - 10 = 690  -> y = 600 - logFull(67) - 10 = 523
      tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // Mostrar reloj1 (260x410) -> x = 512 +/- 130 = 382     y = 300 +/- 205 = 95
      // ----------------------------------------

      // -------- RELOJ2, RELOJ3 Y RELOJ4 -------
      tft.canvasImageStartAddress(PAGE4_START_ADDR);
      tft.clearScreen(BLACK);
      // reloj2
      tft.sdCardDraw16bppBIN256bits(0,0,260,410,"bin/carga/full/reloj2.bin");   // Cargar reloj2 (260x410) =>  ----- Escribiendo en page4
      tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // Mostrar reloj2 --> x = 512 +/- 130 = 382     y = 300 +/- 205 = 95
      // reloj3
      tft.sdCardDraw16bppBIN256bits(261,0,260,410,"bin/carga/full/reloj3.bin"); // Cargar reloj3 (260x410) => x = <reloj2(0) + reloj2(260) + 1 = 261     ->   y = 0   ----- Escribiendo en page4
      tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,261,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // Mostrar reloj3 --> x = 512 +/- 130 = 382     y = 300 +/- 205 = 95
      // reloj4
      tft.sdCardDraw16bppBIN256bits(522,0,260,410,"bin/carga/full/reloj4.bin"); // Cargar reloj4 (260x410) => x = <reloj3(261) + reloj3(260) + 1 = 522   ->   y = 0   ----- Escribiendo en page4
      tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,522,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // Mostrar reloj4 --> x = 512 +/- 130 = 382     y = 300 +/- 205 = 95
      // ----------------------------------------

      // --------- RELOJES GIRADOS 1 Y 2 ---------
      tft.canvasImageStartAddress(PAGE5_START_ADDR);
      tft.clearScreen(BLACK);
      // relGir1
      tft.sdCardDraw16bppBIN256bits(0,0,380,458,"bin/carga/full/relGir1.bin");   // Cargar relGir1 (380x458) =>  ----- Escribiendo en page5
      // No hace falta borrar reloj4 para mostrar relGir1 porque relGir1 ocupa más espacio -> se coloca encima
      tft.bteMemoryCopy(PAGE5_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,322,71,380,458); // Mostrar relGir1 --> x = 512 +/- 190 = 322     y = 300 +/- 229 = 71
      // relGir2
      tft.sdCardDraw16bppBIN256bits(381,0,447,447,"bin/carga/full/relGir2.bin"); // Cargar relGir2 (380x458) => x = <relGir1(0) + relGir1(380) + 1 = 381     ->   y = 0   ----- Escribiendo en page5
      tft.canvasImageStartAddress(PAGE1_START_ADDR);
      tft.clearArea(322,71,702,529,WHITE); // Borrar relGir1 antes de mostrar relGir2 --> xDest = 322 + 380 = 702 -> yDest = 71 + 458 = 529
      tft.bteMemoryCopy(PAGE5_START_ADDR,SCREEN_WIDTH,381,0,PAGE1_START_ADDR,SCREEN_WIDTH,289,77,447,447); // Mostrar relGir2 --> x = 512 +/- 223 = 289     y = 300 +/- 223 = 77
      // ----------------------------------------

      // --------- RELOJES GIRADOS 3 Y 4 ---------
      tft.canvasImageStartAddress(PAGE6_START_ADDR);
      tft.clearScreen(BLACK);
      // relGir3
      tft.sdCardDraw16bppBIN256bits(0,0,473,392,"bin/carga/full/relGir3.bin");   // Cargar relGir3 (473x392) en PAGE6
      tft.canvasImageStartAddress(PAGE1_START_ADDR);
      tft.clearArea(289,77,736,524,WHITE); // Borrar relGir2 antes de mostrar relGir3 --> xDest = 289 + 447 = 736 -> yDest = 77 + 447 = 524
      tft.bteMemoryCopy(PAGE6_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,276,104,473,392); // Mostrar relGir3 --> x = 512 +/- 236 = 276     y = 300 +/- 196 = 104
      // relGir4
      tft.canvasImageStartAddress(PAGE6_START_ADDR); // Regresar a PAGE6
      tft.sdCardDraw16bppBIN256bits(474,0,402,261,"bin/carga/full/relGir4.bin"); // Cargar relGir4 (402x261) en PAGE6 => x = <relGir3(0) + relGir3(473) + 1 = 474     ->   y = 0   
      tft.canvasImageStartAddress(PAGE1_START_ADDR);
      tft.clearArea(276,104,749,496,WHITE); // Borrar relGir3 antes de mostrar relGir4 --> xDest = 276 + 473 = 749 -> yDest = 104 + 392 = 496
      tft.bteMemoryCopy(PAGE6_START_ADDR,SCREEN_WIDTH,474,0,PAGE1_START_ADDR,SCREEN_WIDTH,311,170,402,261); // Mostrar relGir4 en PAGE1 --> x = 512 +/- 201 = 311     y = 300 +/- 130 = 170
      // -----------------------------------------

      // --------- RELOJES GIRADOS 5 Y 6 ---------
      tft.canvasImageStartAddress(PAGE7_START_ADDR);
      tft.clearScreen(BLACK);
      // relGir5
      tft.sdCardDraw16bppBIN256bits(0,0,454,376,"bin/carga/full/relGir5.bin");   // Cargar relGir5 (454x376) en PAGE7 
      tft.canvasImageStartAddress(PAGE1_START_ADDR);
      tft.clearArea(311,170,713,431,WHITE); // Borrar relGir4 antes de mostrar relGir5 --> xDest = 311 + 402 = 713 -> yDest = 170 + 261 = 431
      tft.bteMemoryCopy(PAGE7_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,285,112,454,376); // Mostrar relGir5 en PAGE7 --> x = 512 +/- 227 = 285   ->  y = 300 +/- 188 = 112
      // relGir6
      tft.canvasImageStartAddress(PAGE7_START_ADDR); // Regresar a PAGE7
      tft.sdCardDraw16bppBIN256bits(455,0,398,454,"bin/carga/full/relGir6.bin"); // Cargar relGir6 (398x454) en PAGE7 => x = <relGir5(0) + relGir5(454) + 1 = 455     ->   y = 0  
      tft.canvasImageStartAddress(PAGE1_START_ADDR);
      tft.clearArea(285,112,739,488,WHITE); // Borrar relGir5 antes de mostrar relGir6 --> xDest = 285 + 454 = 739 -> yDest = 112 + 376 = 488
      tft.bteMemoryCopy(PAGE7_START_ADDR,SCREEN_WIDTH,455,0,PAGE1_START_ADDR,SCREEN_WIDTH,313,73,398,454); // Mostrar relGir6 en PAGE7 --> x = 512 +/- 199 = 313  ->  y = 300 +/- 227 = 73
      // ----------------------------------------
    // ------------- FIN CARGA INICIAL --------------------------------------------------------------




    // ----------------- ARRANQUE -------------------------------------------------------------------
      tft.canvasImageStartAddress(PAGE2_START_ADDR);
      // -------- LETRAS S-M-A-R-T -------
      tft.sdCardDraw16bppBIN256bits(0,0,95,159,"bin/arranque/S.bin");      // Cargar S  (95x159) en PAGE2  =>  x  =  <S  =  0                                ->    y = 0  
      tft.sdCardDraw16bppBIN256bits(96,0,104,159,"bin/arranque/M.bin");    // Cargar M  (104x154) en PAGE2 =>  x  =  <M  =  <S(0)    + S(95)   + 1  =  96    ->    y = 0   
      tft.sdCardDraw16bppBIN256bits(201,0,104,159,"bin/arranque/A.bin");   // Cargar A  (104x159) en PAGE2 =>  x  =  <A  =  <M(96)   + M(104)  + 1  =  201   ->    y = 0   
      tft.sdCardDraw16bppBIN256bits(306,0,85,159,"bin/arranque/R.bin");    // Cargar R  (85x159)  en PAGE2 =>  x  =  <R  =  <A(201)  + A(104)  + 1  =  306   ->    y = 0  
      tft.sdCardDraw16bppBIN256bits(392,0,104,159,"bin/arranque/T.bin");   // Cargar T1 (104x159) en PAGE2 =>  x  =  <T1 =  <R(306)  + R(85)   + 1  =  392   ->    y = 0   

    // -------- RELOJ1 (CARGA) -------
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearArea(313,73,711,527,WHITE); // Borrar relGir6 antes de mostrar reloj1 --> xDest = 313 + 398 = 711 -> yDest = 73 + 454 = 527
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // Mostrar reloj1 en PAGE1 --> x = 512 +/- 130 = 382     y = 300 +/- 205 = 95

      // -------- LETRAS C-L-O-H -------
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(497,0,85,159,"bin/arranque/C.bin");    // Cargar C (85x159) en PAGE2 =>  x  =  <C  =  <T1(392) + T1(104) + 1  =  497   ->    y = 0  
      tft.sdCardDraw16bppBIN256bits(583,0,85,159,"bin/arranque/L.bin");    // Cargar L (85x159) en PAGE2 =>  x  =  <L  =  <C(497)  + C(85)   + 1  =  583   ->    y = 0   
      tft.sdCardDraw16bppBIN256bits(669,0,85,159,"bin/arranque/O.bin");    // Cargar O (85x159) en PAGE2 =>  x  =  <O  =  <L(583)  + L(85)   + 1  =  669   ->    y = 0  
      tft.sdCardDraw16bppBIN256bits(755,0,85,159,"bin/arranque/H.bin");    // Cargar H (85x159) en PAGE2 =>  x  =  <H  =  <O(669)  + O(85)   + 1  =  755   ->    y = 0   

    // -------- RELOJ2 (CARGA) -------
    // No hace falta borrar reloj1 para mostrar reloj2 porque tienen el mismo tamaño -> sobreescribe el mismo espacio
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // Mostrar reloj2 en PAGE1 --> x = 512 +/- 130 = 382     y = 300 +/- 205 = 95

      // --------- LOGO ---------
      tft.sdCardDraw16bppBIN256bits(841,0,162,169,"bin/arranque/Log.bin"); // Cargar log (162x169) en PAGE2 => x  = <log =  <H(755)  + H(85)   + 1  =  841   ->    y = 0  
    // -------------- FIN ARRANQUE -----------------------------------------------------------------------



    // ----------------- PANTALLA INICIAL ----------------------------------------------------------------
      tft.sdCardDraw16bppBIN256bits(0,170,120,108,"bin/inicial/brain1.bin");    // Cargar brain1 (120x108) en PAGE2 => x = 0 ->   y = <Log(0) + Log(169) + 1 = 170  
      tft.sdCardDraw16bppBIN256bits(121,170,120,108,"bin/inicial/brain2.bin");  // Cargar brain2 (120x108) en PAGE2 => x = <brain2  = <brain1(0)  + brain1(120) + 1 = 121  ->  y = 170  
    
    // -------- RELOJ3 (CARGA) ------- 
    // No hace falta borrar reloj2 para mostrar reloj3 porque tienen el mismo tamaño -> sobreescribe el mismo espacio
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,261,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // Mostrar reloj3 en PAGE1 --> x = 512 +/- 130 = 382     y = 300 +/- 205 = 95
    // ------------- FIN PANTALLA INICIAL ----------------------------------------------------------------



    // ----------------- ESCOGER GRUPO -------------------------------------------------------------------
      tft.canvasImageStartAddress(PAGE3_START_ADDR);
      // -------- GRUPOS  -------
      tft.sdCardDraw16bppBIN256bits(0,0,130,125,"bin/grupo/assets/grupo1.bin");     // Cargar grupo1 (130x125) en PAGE3  =>  x  =  <grupo1  =  0                                      ->   y = 0 
      tft.sdCardDraw16bppBIN256bits(131,0,130,125,"bin/grupo/assets/grupo2.bin");   // Cargar grupo2 (130x125) en PAGE3  =>  x  =  <grupo2  =  <grupo1(0)   + grupo1(130) + 1 = 131   ->   y = 0 
      tft.sdCardDraw16bppBIN256bits(262,0,130,125,"bin/grupo/assets/grupo3.bin");   // Cargar grupo3 (130x125) en PAGE3  =>  x  =  <grupo3  =  <grupo2(131) + grupo2(130) + 1 = 262   ->   y = 0 
      tft.sdCardDraw16bppBIN256bits(393,0,130,125,"bin/grupo/assets/grupo4.bin");   // Cargar grupo4 (130x125) en PAGE3  =>  x  =  <grupo4  =  <grupo3(262) + grupo3(130) + 1 = 393   ->   y = 0 

    // -------- RELOJ4 (CARGA) ------- 
    // No hace falta borrar reloj3 para mostrar reloj4 porque tienen el mismo tamaño -> sobreescribe el mismo espacio
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,522,0,PAGE1_START_ADDR,SCREEN_WIDTH,382,95,260,410); // Mostrar reloj4 en PAGE1 --> x = 512 +/- 130 = 382     y = 300 +/- 205 = 95

      // -------- MANOS (tb para botones) ---------
      // Estas imágenes dejan residuos de rojo o verde al aplicar el chroma porque no todos los píxeles
      // que se quieren eliminar tienen el color que se quiere filtrar (no es un rojo o verde puro, sino
      // que está mezclado con blanco al encontrarse en el borde de la figura)
      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,"bin/grupo/assets/manoG.bin");    // Cargar manoG (120x129) en PAGE3 =>  x  =  <manoG  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,"bin/grupo/assets/manoR.bin");    // Cargar manoR (120x129) en PAGE3  =>  x  =  <manoR  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      // Esta otra imagen con la mano roja y el fondo blanco se filtra mejor. Aunque queden residuos de
      // blanco en el borde de la figura, no queda mal.
      tft.sdCardDraw16bppBIN256bits(524,0,120,129,"bin/grupo/assets/handW.bin");    // Cargar handW (120x129) en PAGE3   =>  x  =  <handW  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
    // --------------- FIN ESCOGER GRUPO -----------------------------------------------------------------



    // ----------------- AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO ----------------------------------------
    // -------- BOTONES  -------
      tft.sdCardDraw16bppBIN256bits(645,0,172,130,"bin/botones/assets/anadir.bin");       // Cargar anadir   (172x130) en PAGE3 =>  x  =  <anadir   =  <manoR(524) + manoR(120) + 1 = 645      ->   y = 0  
      tft.sdCardDraw16bppBIN256bits(818,0,172,130,"bin/botones/assets/borrar.bin");       // Cargar borrar   (172x130) en PAGE3 =>  x  =  <borrar   =  <anadir(645) + anadir(172) + 1 = 818    ->   y = 0  

    // -------- RELOJ GIRADO 1 (CARGA) ------- 
    // No hace falta borrar reloj4 para mostrar relGir1 porque relGir1 ocupa más espacio -> se coloca encima
    tft.bteMemoryCopy(PAGE5_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,322,71,380,458); // Mostrar relGir1 en PAGE1

      // -------- BOTONES  -------
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regreso a PAGE3
      tft.sdCardDraw16bppBIN256bits(0,131,172,130,"bin/botones/assets/guardar.bin");      // Cargar guardar  (172x130) en PAGE3 =>  x  =   0  ->   y = <borrar(0) + borrar(130) + 1 = 131 
      tft.sdCardDraw16bppBIN256bits(173,131,177,160,"bin/botones/assets/cociGra.bin");    // Cargar cociGra  (172x130) en PAGE3 =>  x  =  <cociGra  =  <guardar(0) + guardar(172) + 1 = 173    ->   y = 131  
      tft.sdCardDraw16bppBIN256bits(351,131,177,160,"bin/botones/assets/crudoGra.bin");   // Cargar crudoGra (172x130) en PAGE3 =>  x  =  <crudoGra =  <cociGra(131) + cociGra(177) + 1 = 351  ->   y = 131  
    
    // -------- RELOJ GIRADO 2 (CARGA) -------
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearArea(322,71,702,529,WHITE); // Borrar relGir1 antes de mostrar relGir2 --> xDest = 322 + 380 = 702 -> yDest = 71 + 458 = 529
    tft.bteMemoryCopy(PAGE5_START_ADDR,SCREEN_WIDTH,381,0,PAGE1_START_ADDR,SCREEN_WIDTH,289,77,447,447); // Mostrar relGir2 en PAGE1
    delay(500);

    // --------- FIN AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO --------------------------------------------



    // Regresamos la dirección de inicio del canvas a la PAGE1 --> ya se ha hecho al borrar relGir1 para mostrar relGir2
    //tft.canvasImageStartAddress(PAGE1_START_ADDR); 

}


void putReloj1(){ 
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE); 
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj1 (260x410) en PAGE1 -> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

void putReloj2(){ 
  // No necesita limpiar porque ocupa el mismo espacio que reloj1
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,66,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj2 en PAGE1 --> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

void putReloj3(){
  // No necesita limpiar porque ocupa el mismo espacio que reloj2
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,132,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj3 en PAGE1 --> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

void putReloj4(){
  // No necesita limpiar porque ocupa el mismo espacio que reloj3
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,198,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj4 en PAGE1 --> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

void putRelojGirado1(){
  // No necesita limpiar porque ocupa más espacio que reloj4
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,264,279,PAGE1_START_ADDR,SCREEN_WIDTH,465,243,95,115); // Mostrar relGir1 en PAGE1 --> x = 512 +/- 47 = 465     y = 300 +/- 57 = 243
}

void putRelojGirado2(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,360,279,PAGE1_START_ADDR,SCREEN_WIDTH,456,244,112,112); // Mostrar relGir2 --> x = 512 +/- 56 = 456     y = 300 +/- 56 = 244
}

void putRelojGirado3(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,473,279,PAGE1_START_ADDR,SCREEN_WIDTH,456,253,113,94); // Mostrar relGir3 en PAGE1 --> x = 512 +/- 56 = 456     y = 300 +/- 47 = 253
}

void putRelojGirado4(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,587,279,PAGE1_START_ADDR,SCREEN_WIDTH,462,268,100,65); // Mostrar relGir4 en PAGE1 --> x = 512 +/- 50 = 462     y = 300 +/- 32 = 268
}

void putRelojGirado5(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,688,279,PAGE1_START_ADDR,SCREEN_WIDTH,456,253,113,94); // Mostrar relGir5 en PAGE1 --> x = 512 +/- 56 = 456   ->  y = 300 +/- 47 = 253
}

void putRelojGirado6(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,802,279,PAGE1_START_ADDR,SCREEN_WIDTH,463,244,99,113); // Mostrar relGir6 en PAGE1 --> x = 512 +/- 49 = 463  ->  y = 300 +/- 56 = 244
}



void loadPicturesRelojCompletoCuarto(){
  /*
    ------------------ POSICIONES DE IMAGENES EN LAS PAGINAS ----------------------------------------------------------------------------------------------------
    PAGE 2:
        S -> (0,0)    M -> (96,0)   A -> (201,0)    R -> (306,0)    T -> (392,0)    C -> (497,0)    L -> (583,0)   O -> (669,0)   H  -> (775,0)   Log -> (841,0)
        brain1  ->  (0,170)   brain2  ->  (121,170)   [[NO[[logFull ->  (242,170)]]NO]]
        reloj1 -> (0,279)   reloj2 -> (66,279)   reloj3 -> (132,279)   reloj4 -> (198,279)   relGir1 -> (264,279)    relGir2 -> (360,279)   relGir3 -> (473,279)    relGir4 -> (587,279)    relGir5 -> (688,279)    relGir6 -> (802,279)
        
    PAGE 3:
        grupo1 -> (0,0)   grupo2 -> (131,0)   grupo3 -> (262,0)   grupo4 -> (393,0)   mano -> (524,0)    anadir -> (645,0)    borrar -> (818,0)
        guardar -> (0,131)   cociGra -> (173,131)   crudoGra -> (351,131)    cociPeq -> (529,131)   crudoPeq -> (577,131)
                                                                             kcal -> (529,174)
    --------------------------------------------------------------------------------------------------------------------------------------------------------------
  */

    // ----------------- CARGA INICIAL -------------------------------------------------------------------
      // -------- LOGFULL Y RELOJ1 -------
      tft.canvasImageStartAddress(PAGE2_START_ADDR);
      tft.clearScreen(BLACK);

      tft.sdCardDraw16bppBIN256bits(0,279,65,103,"bin/carga/cuarto/reloj1.bin"); // Cargar reloj1 (65x103) en PAGE2 => x = 0   ->   y = <brain1(170) + brain1(108) + 1 = 279  
      
      putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1
      // ----------------------------------------

      // -------- RELOJ2, RELOJ3 Y RELOJ4 -------
      // reloj2
      tft.canvasImageStartAddress(PAGE2_START_ADDR);
      tft.sdCardDraw16bppBIN256bits(66,279,65,103,"bin/carga/cuarto/reloj2.bin");   // Cargar reloj2 (65x103) en PAGE2 => <reloj1(0) + reloj1(65) + 1 = 66  -> y = 279
      putReloj2(); // Mostrar reloj2 en PAGE1

      // reloj3
      // No hace falta volver a PAGE2 porque no ha hecho falta borrar al escribir reloj2 en PAGE1
      tft.sdCardDraw16bppBIN256bits(132,279,65,103,"bin/carga/cuarto/reloj3.bin"); // Cargar reloj3 (65x103) en PAGE2 => x = <reloj2(66) + reloj2(65) + 1 = 132   ->   y = 279
      putReloj3(); // Mostrar reloj3 en PAGE1

      // reloj4
      // No hace falta volver a PAGE2 porque no ha hecho falta borrar al escribir reloj3 en PAGE1
      tft.sdCardDraw16bppBIN256bits(198,279,65,103,"bin/carga/cuarto/reloj4.bin"); // Cargar reloj4 (65x103) en PAGE2 => x = <reloj3(132) + reloj3(65) + 1 = 198   ->   y = 279   
      putReloj4(); // Mostrar reloj4 en PAGE1
      // ----------------------------------------

      // --------- RELOJES GIRADOS 1 Y 2 ---------
      // relGir1
      // No hace falta volver a PAGE2 porque no ha hecho falta borrar al escribir reloj4 en PAGE1
      tft.sdCardDraw16bppBIN256bits(264,279,95,115,"bin/carga/cuarto/relGir1.bin");   // Cargar relGir1 (95x115) en PAGE2 => x = <reloj4(198) + reloj4(65) + 1 = 264   ->   y = 279   
      putRelojGirado1(); // Mostrar relGir1 en PAGE1 => No hace falta borrar reloj4 para mostrar relGir1 porque relGir1 ocupa más espacio -> se coloca encima

      // relGir2
      tft.canvasImageStartAddress(PAGE2_START_ADDR);
      tft.sdCardDraw16bppBIN256bits(360,279,112,112,"bin/carga/cuarto/relGir2.bin"); // Cargar relGir2 (112x112) en PAGE2 => x = <relGir1(264) + relGir1(95) + 1 = 360  ->   y = 279   
      putRelojGirado2();  // Mostrar relGir2 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir2
      // ----------------------------------------

      // --------- RELOJES GIRADOS 3 Y 4 ---------
      // relGir3
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 
      tft.sdCardDraw16bppBIN256bits(473,279,113,94,"bin/carga/cuarto/relGir3.bin");   // Cargar relGir3 (113x94) en PAGE2 => x = <relGir2(360) + relGir2(112) + 1 = 473  ->   y = 279 
      putRelojGirado3(); // Mostrar relGir3 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir3

      // relGir4
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(587,279,100,65,"bin/carga/cuarto/relGir4.bin"); // Cargar relGir4 (100x65) en PAGE2 => x = <relGir3(473) + relGir3(113) + 1 = 587     ->   y = 279   
      putRelojGirado4(); // Mostrar relGir4 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir4
      // -----------------------------------------

      // --------- RELOJES GIRADOS 5 Y 6 ---------
      // relGir5
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(688,279,113,94,"bin/carga/cuarto/relGir5.bin");   // Cargar relGir5 (113x94) en PAGE2 => x = <relGir4(587) + relGir4(100) + 1 = 688    ->   y = 279 
      putRelojGirado5(); // Mostrar relGir5 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir5

      // relGir6
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(802,279,99,113,"bin/carga/cuarto/relGir6.bin"); // Cargar relGir6 (99x113) en PAGE2 => x = <relGir5(688) + relGir5(113) + 1 = 802     ->   y = 279  
      putRelojGirado6(); // Mostrar relGir6 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir6
      // ----------------------------------------
    // ------------- FIN CARGA INICIAL --------------------------------------------------------------




    // ----------------- ARRANQUE -------------------------------------------------------------------
      // -------- LETRAS S-M-A-R-T-C-L-O-H -------
      // Letra S
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(0,0,95,159,"bin/arranque/S.bin");      // Cargar S  (95x159) en PAGE2  =>  x  =  <S  =  0   ->    y = 0  
      
      putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1

      // Letra M
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(96,0,104,159,"bin/arranque/M.bin");    // Cargar M  (104x154) en PAGE2 =>  x  =  <M  =  <S(0)    + S(95)   + 1  =  96    ->    y = 0   
      
      putReloj2(); // Mostrar reloj2 en PAGE1 

      // Letra A
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(201,0,104,159,"bin/arranque/A.bin");   // Cargar A  (104x159) en PAGE2 =>  x  =  <A  =  <M(96)   + M(104)  + 1  =  201   ->    y = 0   
      
      putReloj3(); // Mostrar reloj3 en PAGE1 
      
      // Letra R
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(306,0,85,159,"bin/arranque/R.bin");    // Cargar R  (85x159)  en PAGE2 =>  x  =  <R  =  <A(201)  + A(104)  + 1  =  306   ->    y = 0  
     
      putReloj4(); // Mostrar reloj4 en PAGE1 
     
      // Letra T
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(392,0,104,159,"bin/arranque/T.bin");   // Cargar T1 (104x159) en PAGE2 =>  x  =  <T1 =  <R(306)  + R(85)   + 1  =  392   ->    y = 0   
      
      putRelojGirado1(); // Mostrar relGir1 en PAGE1 

      // Letra C
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(497,0,85,159,"bin/arranque/C.bin");    // Cargar C (85x159) en PAGE2 =>  x  =  <C  =  <T1(392) + T1(104) + 1  =  497   ->    y = 0  
      
      putRelojGirado2(); // Mostrar relGir2 en PAGE1 
      
      // Letra L
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(583,0,85,159,"bin/arranque/L.bin");    // Cargar L (85x159) en PAGE2 =>  x  =  <L  =  <C(497)  + C(85)   + 1  =  583   ->    y = 0   
      
      putRelojGirado3(); // Mostrar relGir3 en PAGE1 
      
      // Letra O
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(669,0,85,159,"bin/arranque/O.bin");    // Cargar O (85x159) en PAGE2 =>  x  =  <O  =  <L(583)  + L(85)   + 1  =  669   ->    y = 0  
      
      putRelojGirado4(); // Mostrar relGir4 en PAGE1 
      
      // Letra H
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(755,0,85,159,"bin/arranque/H.bin");    // Cargar H (85x159) en PAGE2 =>  x  =  <H  =  <O(669)  + O(85)   + 1  =  755   ->    y = 0  
      
      putRelojGirado5(); // Mostrar relGir5 en PAGE1 

      // --------- LOGO ---------
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(841,0,162,169,"bin/arranque/Log.bin"); // Cargar log (162x169) en PAGE2 => x  = <log =  <H(755)  + H(85)   + 1  =  841   ->    y = 0  
     
      putRelojGirado6(); // Mostrar relGir6 en PAGE1 
    // -------------- FIN ARRANQUE -----------------------------------------------------------------------



    // ----------------- PANTALLA INICIAL ----------------------------------------------------------------
      // brain1
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(0,170,120,108,"bin/inicial/brain1.bin");    // Cargar brain1 (120x108) en PAGE2 => x = 0 ->   y = <Log(0) + Log(169) + 1 = 170  
      
      putReloj1(); // Mostrar reloj1 en PAGE1 
      
      // brain2
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(121,170,120,108,"bin/inicial/brain2.bin");  // Cargar brain2 (120x108) en PAGE2 => x = <brain1(0)  + brain1(120) + 1 = 121  ->  y = 170  
      
      putReloj2(); // Mostrar reloj2 en PAGE1 
    // ------------- FIN PANTALLA INICIAL ----------------------------------------------------------------



    // ----------------- ESCOGER GRUPO -------------------------------------------------------------------
      // -------- GRUPOS  -------
      // grupo1
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Canvas inicia en PAGE3
      tft.sdCardDraw16bppBIN256bits(0,0,130,125,"bin/grupo/grupo1.bin");     // Cargar grupo1 (130x125) en PAGE3  =>  x = 0   ->   y = 0 
      
      putReloj3(); // Mostrar reloj3 en PAGE1 
      
      // grupo2
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(131,0,130,125,"bin/grupo/grupo2.bin");   // Cargar grupo2 (130x125) en PAGE3  =>  x = <grupo1(0)   + grupo1(130) + 1 = 131   ->   y = 0 
      
      putReloj4(); // Mostrar reloj4 en PAGE1 
      
      // grupo3
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(262,0,130,125,"bin/grupo/grupo3.bin");   // Cargar grupo3 (130x125) en PAGE3  =>  x = <grupo2(131) + grupo2(130) + 1 = 262   ->   y = 0 
      
      putRelojGirado1(); // Mostrar relGir1 en PAGE1 
      
      // grupo4
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(393,0,130,125,"bin/grupo/grupo4.bin");   // Cargar grupo4 (130x125) en PAGE3  =>  x = <grupo3(262) + grupo3(130) + 1 = 393   ->   y = 0 
      
      putRelojGirado2(); // Mostrar relGir2 en PAGE1 

      // -------- MANOS (tb para botones) ---------
      // Estas imágenes dejan residuos de rojo o verde al aplicar el chroma porque no todos los píxeles
      // que se quieren eliminar tienen el color que se quiere filtrar (no es un rojo o verde puro, sino
      // que está mezclado con blanco al encontrarse en el borde de la figura)

      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,"bin/grupo/assets/manoG.bin");  // Cargar manoG (120x129) en PAGE3 =>  x  =   <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,"bin/grupo/assets/manoR.bin");  // Cargar manoR (120x129) en PAGE3  =>  x  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      
      // Esta otra imagen con la mano roja y el fondo blanco se filtra mejor. Aunque queden residuos de
      // blanco en el borde de la figura, no queda mal.
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(524,0,120,129,"bin/grupo/handW.bin");    // Cargar handW (120x129) en PAGE3   =>  x  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      
      putRelojGirado3(); // Mostrar relGir3 en PAGE1 
    // --------------- FIN ESCOGER GRUPO -----------------------------------------------------------------



    // ----------------- AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO ----------------------------------------
    // -------- BOTONES  -------
      // añadir
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(645,0,172,130,"bin/botones/anadir.bin");  // Cargar anadir (172x130) en PAGE3 =>  x  =  <manoR(524) + manoR(120) + 1 = 645      ->   y = 0  
      
      putRelojGirado4(); // Mostrar relGir4 en PAGE1
      
      // borrar
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(818,0,172,130,"bin/botones/borrar.bin");  // Cargar borrar (172x130) en PAGE3 =>  x  = <anadir(645) + anadir(172) + 1 = 818    ->   y = 0  
      
      putRelojGirado5(); // Mostrar relGir5 en PAGE1

      // guardar
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(0,131,172,130,"bin/botones/guardar.bin"); // Cargar guardar (172x130) en PAGE3 =>  x  =   0  ->   y = <borrar(0) + borrar(130) + 1 = 131 
     
      putRelojGirado6(); // Mostrar relGir6 en PAGE1
     
      // cociGra
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(173,131,177,160,"bin/botones/cociGra.bin"); // Cargar cociGra (172x130) en PAGE3 =>  x  =  <guardar(0) + guardar(172) + 1 = 173    ->   y = 131  
     
      putReloj1(); // Mostrar reloj1 en PAGE1
     
      // crudoGra
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(351,131,177,160,"bin/botones/crudoGra.bin"); // Cargar crudoGra (172x130) en PAGE3 =>  x  =  <cociGra(131) + cociGra(177) + 1 = 351  ->   y = 131  
     
      putReloj2(); // Mostrar reloj2 en PAGE1

    // --------- FIN AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO --------------------------------------------


    // --------- DASHBOARD -------------------------------------------------------------------------------
      // cociPeq
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(529,131,47,42,"bin/dash/cociPeq.bin"); // Cargar cociPeq (47x42) en PAGE3 =>  x  =  <crudoGra(351) + crudoGra(177) + 1 = 529  ->   y = 131  

      //putReloj3(); // Mostrar reloj3 en PAGE1

      // crudoPeq
      tft.sdCardDraw16bppBIN256bits(577,131,47,42,"bin/dash/crudoPeq.bin"); // Cargar crudoPeq (47x42) en PAGE3 =>  x  =  <cociPeq(529) + crudoGra(47) + 1 = 577  ->   y = 131  

      //putReloj4(); // Mostrar reloj4 en PAGE1

      // kcal
      tft.sdCardDraw16bppBIN256bits(529,174,80,87,"bin/dash/kcal.bin"); // Cargar kcal (80x87) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174
    // --------- FIN DASHBOARD ---------------------------------------------------------------------------

    //delay(200);

    // Regresamos la dirección de inicio del canvas a la PAGE1 
    tft.canvasImageStartAddress(PAGE1_START_ADDR); 

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------




void dashboard(){ // OK ==> HECHO
    tft.clearScreen(AZUL_FONDO); // Fondo azul oscuro en PAGE1

    // ------------ ZONA SUPERIOR --------------------------------------------
    // ---------- GRÁFICOS -------------------
    // Recuadro grupo y ejemplos
    tft.fillRoundRect(30,20,932,135,20,GRIS_CUADROS); // 902 x 115

    // Recuadro cocinado pequeño
    tft.fillRoundRect(937,20,994,74,10,GRIS_CUADROS); // 57 x 52
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE1_START_ADDR,SCREEN_WIDTH,942,26,47,42);  // Mostrar cociPeq (47x42) en PAGE1

    // Recuadro crudo pequeño
    tft.fillRoundRect(937,79,994,133,10,GRIS_CUADROS); // 57 x 52
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,577,131,PAGE1_START_ADDR,SCREEN_WIDTH,942,85,47,42);  // Mostrar crudoPeq (47x42) en PAGE1
    // ---------- FIN GRÁFICOS ---------------


    // -------- TEXTO ------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextForegroundColor(WHITE); 

    // Título
    tft.setCursor(40,30);
    tft.print("Grupo Actual: ");  // 32 escale x1

    // Nombre grupo
    tft.setTextForegroundColor(COLOR_G1); // COLOR como atributo de struct 'grupo'
    tft.setCursor(tft.getCursorX(),tft.getCursorY());
    tft.print("L\xE1""cteos enteros"); // Nombre grupo // 32 escale x1

    // Ejemplos grupo
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setCursor(40,70);
    tft.println("Leche entera de vaca (pasteurizada o UHT), de oveja, de cabra, yogurt\n   natural entero, cuajada, etc."); // 24 escale x1
    // -------- FIN TEXTO --------------------
    // ------------- FIN ZONA SUPERIOR ---------------------------------------


    // ------------- PLATO ACTUAL  -------------------------------------------
    // ---------- GRÁFICOS -------------------
    // Recuadro "Plato actual"
    tft.fillRoundRect(30,145,504,580,20,GRIS_CUADROS); // 474 x 425
    tft.drawRoundRect(30,145,504,580,20,AZUL_BORDE_CUADRO); // Borde => 474 x 425
    tft.drawRoundRect(31,146,503,579,20,AZUL_BORDE_CUADRO); // Borde => 473 x 424

    // Recuadro Raciones Carbohidratos
    tft.fillRoundRect(401,288,479,345,10,GRIS_CUADROS_VALORES); // 78 x 57

    // Recuadro Raciones Proteinas
    tft.fillRoundRect(401,365,479,422,10,GRIS_CUADROS_VALORES); // 78 x 57

    // Recuadro Raciones Grasas
    tft.fillRoundRect(401,442,479,499,10,GRIS_CUADROS_VALORES); // 78 x 57

     // Dibujo kcal
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,174,PAGE1_START_ADDR,SCREEN_WIDTH,117,483,80,87);  // Mostrar kcal (80x87) en PAGE1
    // ---------- FIN GRÁFICOS ---------------


    // -------- TEXTO ------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setTextForegroundColor(WHITE); 

    // Título
    tft.setCursor(120,155);
    tft.print("Plato actual");  // 24 escale x2

    // Peso
    tft.setCursor(50,220);
    tft.setTextForegroundColor(ROJO_PESO); 
    tft.print("Peso: 145g"); // 24 escale x2

    // ---- RACIONES ------
    // Antes que valores para no estar continuamente cambiando los tamaños y escalas
    // Raciones de Carbohidratos
    tft.setCursor(416,293);
    tft.setTextForegroundColor(WHITE); 
    tft.print("10"); // 24 escale x2

    // Raciones de Proteinas
    tft.setCursor(416,370);
    tft.setTextForegroundColor(WHITE); 
    tft.print("7"); // 24 escale x2

    // Raciones de Grasas
    tft.setCursor(416,447);
    tft.setTextForegroundColor(WHITE); 
    tft.print("5"); // 24 escale x2
    // ---- FIN RACIONES ----

    // ---- VALORES ---------
    // Texto "Raciones"
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(370,243);
    tft.print("Raciones"); // 32 escale x1

    // Carbohidratos
    tft.setCursor(50,293);
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.print("Carbohidratos: 100 g"); // 32 escale x1
    
    // Proteinas
    tft.setCursor(50,370);
    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.print("Prote\xED""nas: 70 g"); // 32 escale x1
    
    // Grasas
    tft.setCursor(50,447);
    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.print("Grasas: 49 g");  // 32 escale x1
    
    // Kcal
    tft.setCursor(207,516);
    tft.setTextForegroundColor(ROJO_KCAL); 
    tft.print("1121 Kcal."); // 32 escale X1
    // ----- FIN VALORES ----
    // -------- FIN TEXTO --------------------
    // ------------- FIN PLATO ACTUAL ----------------------------------------




    // ------------- ACUMULADO HOY  -------------------------------------------
    // ---------- GRÁFICOS -------------------
    // Recuadro "Acumulado hoy"
    tft.fillRoundRect(520,145,994,580,20,GRIS_CUADROS); // 474 x 425

    // Recuadro Carbohidratos
    tft.fillRoundRect(891,288,969,345,10,GRIS_CUADROS_VALORES); // 78 x 57

    // Recuadro Proteinas
    tft.fillRoundRect(891,365,969,422,10,GRIS_CUADROS_VALORES); // 78 x 57

    // Recuadro Grasas
    tft.fillRoundRect(891,442,969,499,10,GRIS_CUADROS_VALORES); // 78 x 57

    // kcal
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,174,PAGE1_START_ADDR,SCREEN_WIDTH,607,483,80,87);  // Mostrar kcal (80x87) en PAGE1
    // ---------- FIN GRÁFICOS ---------------


    // -------- TEXTO ------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setTextForegroundColor(WHITE); 

    // Título
    tft.setCursor(590, 155);
    tft.print("Acumulado hoy");  // 24 escale x2

    // Peso
    tft.setCursor(540,220);
    tft.setTextForegroundColor(ROJO_PESO); 
    tft.print("Peso: 145g"); // 24 escale x2

    // ---- RACIONES ------
    // Antes que valores para no estar continuamente cambiando los tamaños y escalas
    // Raciones de Carbohidratos
    tft.setCursor(906,293);
    tft.setTextForegroundColor(WHITE); 
    tft.print("10"); // 24 escale x2

    // Raciones de Proteinas
    tft.setCursor(906,370);
    tft.setTextForegroundColor(WHITE); 
    tft.print("7"); // 24 escale x2

    // Raciones de Grasas
    tft.setCursor(906,447);
    tft.setTextForegroundColor(WHITE); 
    tft.print("5"); // 24 escale x2
    // ---- FIN RACIONES ----

    // ---- VALORES ---------
    // Texto "Raciones"
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(860,243);
    tft.print("Raciones"); // 32 escale x1

    // Carbohidratos
    tft.setCursor(540,293);
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.print("Carbohidratos: 100 g"); // 32 escale x1
    
    // Proteinas
    tft.setCursor(540,370);
    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.print("Prote\xED""nas: 70 g"); // 32 escale x1
    
    // Grasas
    tft.setCursor(540,447);
    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.print("Grasas: 49 g");  // 32 escale x1
    
    // Kcal
    tft.setCursor(697,516);
    tft.setTextForegroundColor(ROJO_KCAL); 
    tft.print("1121 Kcal."); // 32 escale X1
    // ----- FIN VALORES ----
    // -------- FIN TEXTO --------------------
    // -------------- FIN ACUMULADO HOY ---------------------------------------



}



void arranque(){ // OK ==> HECHO
    tft.clearScreen(WHITE); // Fondo blanco en PAGE1
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

void pantalla_inicial(){ // PAGE2 (OK) ==> HECHO

    // ----------- TEXTO ------------------------------------------------------------------------------------
    tft.clearScreen(RED); // Fondo rojo en PAGE1

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
    tft.fillCircle(512,380,65,WHITE); // 65 pixeles de diametro
    delay(1000);

    // ------- CUADRADO REDONDEADO ----
    tft.fillRoundRect(447,315,577,445,10,WHITE); // x = 512 +/- 65 = 447   ->   y = 380 +/- 65 = 315
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


    // ----------- BRAINS (120x108)  ------------------------------------------------------------------------
    // BRAIN1 (120x108) --> centrar en el cuadrado blanco
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); // Mostrar brain1 en PAGE1
    delay(1000);

    // BRAIN2 (120x108) (rojo)
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,121,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); // Mostrar brain2 en PAGE1
    // ------------------------------------------------------------------------------------------------------

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void select_Grupo(){ // PAGE3 (OK) ==> HECHO

    // ----- TEXTO (INTERNAL FIXED 12x24 X3) --------------------------------------------------------------
    tft.clearScreen(RED); // Fondo rojo en PAGE1

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
    // 30 pixeles entre cuadro y cuadro
    // Mostrar en PAGE1 (copiar de PAGE3 a PAGE1)

    // ------ Grupo 1 (130x125) ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125); // x = 236  ->  y = 288
    delay(800);

    // ------ Grupo 2 (130x125) ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125); // x = <grupo1(236) + grupo1(130) + 30 = 396  ->  y = 288
    delay(800);

    // ------ Grupo 3 (130x125) ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); // x = <grupo2(396) + grupo2(130) + 30 = 556  ->  y = 288
    delay(800);

    // ------ Grupo 4 (130x125) ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125); // x = <grupo3(556) + grupo3(130) + 30 = 716  ->  y = 288
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


    // ------------ MANO (120x129) -----------------------------------------------------------------------------------
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,556,370,120,129,WHITE); // Mostrar handW (120x129) en PAGE1
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,556,370,120,129,RED); // manoR (120x129)
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,556,370,120,129,GREEN_HAND); // manoG (120x129)
    // ----------------------------------------------------------------------------------------------------

}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void add_Plato(){ // Tb PAGE3, pero más abajo ==> HECHO
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    tft.clearScreen(RED); // Fondo rojo en PAGE1

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
    // Copiar de PAGE3 a PAGE1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); // Mostrar añadir (172x130) en PAGE1
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
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,WHITE); // Mostrar handW (120x129) en PAGE1
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,RED); // manoR (120x129)
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,GREEN_HAND); // manoG (120x129)
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

void delete_Plato(){ // Tb PAGE3, pero más a la derecha ==> HECHO
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    tft.clearScreen(RED); // Fondo rojo en PAGE1

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


    // ------------ BOTÓN ELIMINAR ------------------------------------------------------------------------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); // Mostrar borrar (172x130) en PAGE1
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
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,WHITE); // handW (120x129)
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
    tft.clearScreen(RED); // Fondo rojo en PAGE1

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
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,450,172,130); // Mostrar guardar (172x130) en PAGE1
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
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,WHITE); // handW (120x129)
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,RED); // manoR
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,GREEN_HAND); // manoG
    // ----------------------------------------------------------------------------------------------------


    delay(2000);

    // ----- TEXTO (COMIDA GUARDADA) -------------------------------------------------------------------------
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
    tft.clearScreen(RED); // Fondo rojo en PAGE1

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
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); // Mostrar cociGra (177x160) en PAGE1
    delay(1000);

    // ------------ CRUDO 1 ----------------
    // Borrar imagen cocinado 1 de la page1
    tft.clearArea(280,280,497,470,RED); 
    // Escribir imagen crudo en page1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160); // Mostrar crudoGra (177x160) en PAGE1
    delay(1000);

    // ------------ COCINADO 2 -------------
    // Borrar imagen crudo 1 de la page1
    tft.clearArea(500,280,724,480,RED); 
    // Escribir imagen cocinado en page1    
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); // Mostrar cociGra (177x160) en PAGE1
    delay(1000);

    // ------------ CRUDO 2 ----------------
    // Borrar imagen cocinado 2 de la page1
    tft.clearArea(280,280,497,470,RED); 
    // Escribir imagen crudo en page1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160); // Mostrar crudoGra (177x160) en PAGE1
    // ----------------------------------------------------------------------------------------------------



}







