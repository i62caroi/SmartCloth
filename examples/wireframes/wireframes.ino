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

    //prueba();

    //arranque();  // PAGE2   (OK)
    
    //pantalla_inicial(); // PAGE3  (OK)

    //select_Grupo();   // PAGE4 (~OK)

    //add_Plato();      // PAGE4 (~OK)

    //delete_Plato();   // PAGE4 (~OK)

    //save_Comida();    // PAGE4 (~OK)

    crudo_cocinado();
}





void loop() {
     
}

void prueba(){
  // --------- bteMemoryCopyDemo() --------------------------------
  // clear page1
  tft.canvasImageStartAddress(PAGE1_START_ADDR);
  tft.canvasImageWidth(SCREEN_WIDTH);
  tft.setCanvasWindow(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
  tft.clearScreen(BLACK);

  // clear page2
  tft.canvasImageStartAddress(PAGE2_START_ADDR);
  tft.clearScreen(BLACK);
  // write picture to page2
  tft.sdCardDraw256bitsBIN(0,0,378,362,"bin/prueba/crudo.bin");

  // clear page3
  tft.canvasImageStartAddress(PAGE3_START_ADDR);
  tft.clearScreen(BLACK);
  // write picture to page3
  tft.sdCardDraw256bitsBIN(0,0,378,362,"bin/prueba/cocinado.bin");

  //copy page2 picture to page1 
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH, 50,50,378,362); 
  //copy page3 picture to page1 
  //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH, 450,50,378,362); 

  // ------------------------------------------------------------




  // ---- CHROMA ------
  // ------------- bteMemoryCopyChromaDemo() --------------------
  //copy page2 picture to page1 with Chroma
  tft.bteMemoryCopyWithChromaKey(PAGE2_START_ADDR,SCREEN_WIDTH,0,0, PAGE1_START_ADDR,SCREEN_WIDTH,450,50,378,362,WHITE);
  // ------------------------------------------------------------

 
}



void arranque(){ // OK

    //PAG 1 ==> fondo blanco
    tft.clearScreen(WHITE);
    delay(300);
/*
    // Al colocar el logo (symbolo) abajo, ya no hace falta esta parte?
    // Porque ahora es lo último que sale

    //PAG 2 ==> círculo gris
    tft.fillCircle(100,300,50,COLOR65K_GRAYSCALE20);
    delay(300);

    //PAG 3 ==> cuadrado esquinas redondeadas girado
    tft.clearArea(30, 200, 200, 400, WHITE);
    //tft.clearScreen(WHITE);
    tft.fillRoundRect(30,300,100,300,10,COLOR65K_GRAYSCALE26);
    delay(300);

    //PAG 4 ==> cuadrado girado
    tft.clearArea(30, 200, 200, 400, WHITE);
    //tft.clearScreen(WHITE);
    tft.fillRect(30,300,100,300,COLOR65K_GRAYSCALE16);
    delay(300);

    //PAG 5 ==> cuadrado normal
    tft.clearArea(30, 200, 200, 400, WHITE);
    //tft.clearScreen(WHITE);
    tft.fillRect(50,250,100,350,COLOR65K_GRAYSCALE26);
    delay(300);

    //PAG 6 => symbol muy tenue ==> DIFICIL
*/
    

    // -------- LETRAS ------- 

    /*
      S (95x159)    =>  x  =  <S  =  40                          -->   tft.sdCardDraw256bitsBIN(40,150,95,159,"bin/arranque/S.bin");
        
      M (104x154)   =>  x  =  <M  =  <S(40) + S(95)     =  135   -->   tft.sdCardDraw256bitsBIN(135,150,104,159,"bin/arranque/M.bin");

      A (104x159)   =>  x  =  <A  =  <M(135) + M(104)   =  239   -->   tft.sdCardDraw256bitsBIN(239,150,104,159,"bin/arranque/A.bin");

      R (85x159)    =>  x  =  <R  =  <A(239) + A(104)   =  343   -->   tft.sdCardDraw256bitsBIN(343,150,85,159,"bin/arranque/R.bin");

      T1 (104x159)  =>  x  =  <T1 =  <R(343) + R(85)    =  428   -->   tft.sdCardDraw256bitsBIN(428,150,104,159,"bin/arranque/T.bin");

      C (85x159)    =>  x  =  <C  =  <T1(428) + T1(104) =  532   -->   tft.sdCardDraw256bitsBIN(532,150,85,159,"bin/arranque/C.bin");

      L (85x159)    =>  x  =  <L  =  <C(532) + C(85)    =  617   -->   tft.sdCardDraw256bitsBIN(617,150,85,159,"bin/arranque/L.bin");

      O (85x159)    =>  x  =  <O  =  <L(617) + L(85)    =  702   -->   tft.sdCardDraw256bitsBIN(702,150,85,159,"bin/arranque/O.bin");

      T2 (104x159)  =>  x  =  <T2 =  <O(702) + O(85)    =  787   -->   tft.sdCardDraw256bitsBIN(787,150,104,159,"bin/arranque/T.bin");

      H (85x159)    =>  x  =  <H  =  <T2(787) + T2(104) =  891   -->   tft.sdCardDraw256bitsBIN(891,150,85,159,"bin/arranque/H.bin");

    */

    //clear page1
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearScreen(WHITE);

    //clear page2
    tft.canvasImageStartAddress(PAGE2_START_ADDR);
    tft.clearScreen(WHITE);


    // S M A R T C L O T H ==> S T O T M L R A C H 

    // ------------ S (95x159) -------------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(40,150,95,159,"bin/arranque/S.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,40,150,PAGE1_START_ADDR,SCREEN_WIDTH,40,150,95,159); 
    //delay(300);
    // -------------------------------------------------------------------------------------------------------

    // ------------ T1 (104x159) -----------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(428,150,104,159,"bin/arranque/T.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,428,150,PAGE1_START_ADDR,SCREEN_WIDTH,428,150,104,159); 
    //delay(300);
    // -------------------------------------------------------------------------------------------------------

    // ------------ O (85x159) -------------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(702,150,85,159,"bin/arranque/O.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,702,150,PAGE1_START_ADDR,SCREEN_WIDTH,702,150,85,159); 
    //delay(300);
    // -------------------------------------------------------------------------------------------------------

    // ------------ T2 (104x159) -----------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(787,150,104,159,"bin/arranque/T.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,787,150,PAGE1_START_ADDR,SCREEN_WIDTH,787,150,104,159); 
    //delay(300);
    // -------------------------------------------------------------------------------------------------------

    // ------------ M (104x154) ------------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(135,150,104,159,"bin/arranque/M.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,135,150,PAGE1_START_ADDR,SCREEN_WIDTH,135,150,104,159); 
    //delay(300);
    // -------------------------------------------------------------------------------------------------------

    // ------------ L (85x159) -------------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(617,150,85,159,"bin/arranque/L.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,617,150,PAGE1_START_ADDR,SCREEN_WIDTH,617,150,85,159); 
    //delay(300);
    // -------------------------------------------------------------------------------------------------------

    // ------------ R (85x159) -------------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(343,150,85,159,"bin/arranque/R.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,343,150,PAGE1_START_ADDR,SCREEN_WIDTH,343,150,85,159); 
    //delay(300);
    // -------------------------------------------------------------------------------------------------------

    // ------------ A (104x159) ------------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(239,150,104,159,"bin/arranque/A.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,239,150,PAGE1_START_ADDR,SCREEN_WIDTH,239,150,104,159); 
    //delay(300);
    // -------------------------------------------------------------------------------------------------------

    // ------------ C (85x159) -------------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(532,150,85,159,"bin/arranque/C.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,532,150,PAGE1_START_ADDR,SCREEN_WIDTH,532,150,85,159); 
    //delay(300);
    // -------------------------------------------------------------------------------------------------------

    // ------------ H (85x159) -------------------------------------------------------------------------------
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(891,150,85,159,"bin/arranque/H.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,891,150,PAGE1_START_ADDR,SCREEN_WIDTH,891,150,85,159); 
    //delay(300);   
    // -------------------------------------------------------------------------------------------------------


    // ---------------- LOGO (SYMBOL) ------------------------------------------------------------------------

    // Logo (162x169) ==> debajo
    // Se coloca en la misma posición en la page2 que en la page1, solamente por organización. Podría ponerse
    // en cualquier posición de la page2 y luego colocarla correctamente al copiarla a la page1.
    tft.sdCardDraw256bitsBIN(417,350,162,169,"bin/arranque/Log.bin"); // Escribiendo en page2
    //copy page2 picture to page1 
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,417,350,PAGE1_START_ADDR,SCREEN_WIDTH,417,350,162,169); 
    delay(300);
    // -------------------------------------------------------------------------------------------------------

    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    
}



//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void pantalla_inicial(){ // PAGE3 (OK)

    // ----------- TEXTO ------------------------------------------------------------------------------------
    //PAG 1 (OK) ==> fondo rojo y texto
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


    // ----------- CIRCULO ----------------------------------------------------------------------------------
    //PAG 2 (OK) ==> Círculo (65 pixeles de diametro)
    tft.fillCircle(512,380,65,WHITE);
    delay(1000);
    // ------------------------------------------------------------------------------------------------------


    // ----------- CUADRADO REDONDEADO ----------------------------------------------------------------------
    //PAG 3 ==> cuadrado esquinas redondeadas girado ==> NO PUEDE SER GIRADO

    //PAG 4 ==> cuadrado esquinas redondeadas normal
    //tft.clearArea(400, 300, 600, 500, ROJO);
    //delay(100);
    tft.fillRoundRect(447,315,577,445,10,WHITE); // x = 512 +/- 65   y = 380 +/- 65
    delay(1000);
    // ------------------------------------------------------------------------------------------------------


    // ----------- PALITOS ----------------------------------------------------------------------------------
    //PAG 5 ==> palitos alrededor cuadrado (fillRect o drawLine??). 4 pixeles entre barra y barra
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
    //PAG 6 => brain1 --> centrar en el cuadrado blanco
    //clear page3
    tft.canvasImageStartAddress(PAGE3_START_ADDR);
    tft.clearScreen(WHITE);
    // Se coloca en posición (50,50) en page3, pero luego se copia en la page1 en la posición correcta (450,325)
    tft.sdCardDraw256bitsBIN(50,50,120,108,"bin/inicial/brain1.bin");  // Escribiendo en page3
    //copy page3 picture to page1 
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,50,50,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); 
    delay(1000);

    //PAG 7 => brain2 (rojo)
    tft.sdCardDraw256bitsBIN(170,50,120,108,"bin/inicial/brain2.bin"); // Escribiendo en page3
    // Se coloca en posición (170,50) en page3, pero luego se copia en la page1 en la posición correcta (450,325)
    //copy page3 picture to page1 
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,170,50,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); 
    //delay(300);
    // ------------------------------------------------------------------------------------------------------

    tft.canvasImageStartAddress(PAGE1_START_ADDR);
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------


void select_Grupo(){ // PAGE4 (~OK)

    // ----- TEXTO (INTERNAL FIXED 12x24 X3) --------------------------------------------------------------
    //PAG 1 ==> fondo rojo y texto
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
    //tft.fillRect(0, 250, 220, 258, WHITE); 
    tft.fillRoundRect(0,250,220,258,3,WHITE);
    //tft.fillRect(0, 450, 512, 458, WHITE); 
    tft.fillRoundRect(0,450,512,458,3,WHITE);
    delay(1000);
    // ----------------------------------------------------------------------------------------------------

    
    // ------------ CUADROS -------------------------------------------------------------------------------
    //clear page4 para escribir imágenes de grupos y luego aplicar Memory Copy de la page4 a la page1
    // 50 pixeles entre cuadro y cuadro
    tft.canvasImageStartAddress(PAGE4_START_ADDR);
    tft.clearScreen(RED);

    // ------ Cuadro 1 ---------
    // Se coloca en posición (0,0) en page4, pero luego se copia en la page1 en la posición correcta (130,125)
    tft.sdCardDraw256bitsBIN(0,0,130,125,"bin/grupo/grupo1.bin"); // Escribiendo en page4
    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125); 
    //copy page2 picture to page1 with Chroma ==> No coincide el rojo con RED !!!!!!
    //tft.bteMemoryCopyWithChromaKey(PAGE4_START_ADDR,SCREEN_WIDTH,0,0, PAGE1_START_ADDR,SCREEN_WIDTH,256,288,130,125,RED); 
    delay(800);
    // -------------------------

    // ------ Cuadro 2 ---------
    // Se coloca en posición (131,0) en page4, pero luego se copia en la page1 en la posición correcta (436,288)
    tft.sdCardDraw256bitsBIN(131,0,130,125,"bin/grupo/grupo2.bin"); // Escribiendo en page4
    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125); // 396 = 236(X inicio de g1) + 130(width g1) + 30(espacio)
    delay(800);
    // -------------------------

    // ------ Cuadro 3 ---------
    // Se coloca en posición (262,0) en page4, pero luego se copia en la page1 en la posición correcta (616,288)
    tft.sdCardDraw256bitsBIN(262,0,130,125,"bin/grupo/grupo3.bin"); // Escribiendo en page4
    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,262,50,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); // 556 = 396(X inicio de g1) + 130(width g2) + 30(espacio)
    delay(800);
    // -------------------------

    // ------ Cuadro 4 ---------
    // Se coloca en posición (393,0) en page4, pero luego se copia en la page1 en la posición correcta (796,288)
    tft.sdCardDraw256bitsBIN(393,0,130,125,"bin/grupo/grupo4.bin"); // Escribiendo en page4
    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125); // 716 = 556(X inicio de g1) + 130(width g2) + 50(espacio)
    delay(800);
    // -------------------------
    // ----------------------------------------------------------------------------------------------------



    // ------------ MANO -----------------------------------------------------------------------------------
    // Se coloca en posición (525,0) en page4, pero luego se copia en la page1 en la posición correcta (556,370) => sobre grupo3
    //tft.sdCardDraw256bitsBIN(525,0,120,129,"bin/grupo/mano.bin"); // Escribiendo en page4
    //copy page4 picture to page1 // No cuadra el rojo de fondo para aplicar Chroma!!!!
    //tft.bteMemoryCopyWithChromaKey(PAGE4_START_ADDR,SCREEN_WIDTH,525,0, PAGE1_START_ADDR,SCREEN_WIDTH,556,370,120,129,RED); 
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
}

//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void add_Plato(){ // Tb PAGE4, pero más abajo
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    //PAG 1 ==> fondo rojo y texto
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
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
    //tft.fillRect(252, 250, 764, 258, WHITE); // fillRect o fillRoundRect??
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
    tft.canvasImageStartAddress(PAGE4_START_ADDR);

    // Se coloca en posición (646,0) en page4, compartida con cuadros de grupos  y otros botones, pero luego se copia en la page1 
    // en la posición correcta (420,410)
    tft.sdCardDraw256bitsBIN(646,0,172,130,"bin/botones/anadir.bin"); // Escribiendo en page4
    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,646,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); 
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

    delay(2000);

    // ----- TEXTO (PLATO AÑADIDO) -------------------------------------------------------------------------
    tft.clearScreen(RED);
    // ------------ LINEA -----------------------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setCursor(170, 258);
    tft.println("NUEVO PLATO A\xD1""ADIDO");
    // ------------ LINEA -----------------------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // ------------------------------------------
    // ----------------------------------------------------------------------------------------------------
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void delete_Plato(){ // Tb PAGE4, pero más a la derecha
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    //PAG 1 ==> fondo rojo y texto
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
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
    //tft.fillRect(252, 250, 764, 258, WHITE); // fillRect o fillRoundRect??
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
    tft.canvasImageStartAddress(PAGE4_START_ADDR);

    // Se coloca en posición (819,0) en page4, compartida con cuadros de grupos y otros botones, pero luego se copia en la page1 
    // en la posición correcta (420,410)
    tft.sdCardDraw256bitsBIN(819,0,172,130,"bin/botones/borrar.bin"); // Escribiendo en page4
    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,819,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); 
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

    delay(2000);

    // ----- TEXTO (PLATO BORRADO) -------------------------------------------------------------------------
    tft.clearScreen(RED);
    // ------------ LINEA -----------------------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setCursor(100, 258);
    tft.println("PLATO ACTUAL ELIMINADO");
    // ------------ LINEA -----------------------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // ------------------------------------------
    // ----------------------------------------------------------------------------------------------------
}


//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void save_Comida(){ // Tb PAGE4, pero más a la derecha
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    //PAG 1 ==> fondo rojo y texto
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
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
    //tft.fillRect(252, 250, 764, 258, WHITE); // fillRect o fillRoundRect??
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


    // ------------ BOTÓN ELIMINAR -------------------------------------------------------------------------------
    tft.canvasImageStartAddress(PAGE4_START_ADDR);

    // Se coloca en posición (0,131) en page4, compartida con cuadros de grupos y otros botones, pero luego se copia en la page1 
    // en la posición correcta (420,450)
    tft.sdCardDraw256bitsBIN(0,131,172,130,"bin/botones/guardar.bin"); // Escribiendo en page4
    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,450,172,130); 
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

    delay(2000);

    // ----- TEXTO (PLATO AÑADIDO) -------------------------------------------------------------------------
    tft.clearScreen(RED);
    // ------------ LINEA -----------------------
    tft.fillRoundRect(252,150,764,158,3,WHITE);
    // ------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setCursor(100, 208);
    tft.println("COMIDA ACTUAL GUARDADA");
    // ------------ LINEA -----------------------
    tft.fillRoundRect(252,330,764,338,3,WHITE);
    // ------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setCursor(170, 388);
    tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO");
    tft.setCursor(350, tft.getCursorY() + tft.getTextSizeY()+40);
    tft.print("AL ACUMULADO DE HOY"); 
    // ----------------------------------------------------------------------------------------------------
}



//---------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------

void crudo_cocinado(){ // Tb PAGE4, pero más a la derecha
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    //PAG 1 ==> fondo rojo y texto
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
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
    // EN LUGAR DE DELAY, APROVECHAMOS PARA ESCRIBIR LAS IMÁGENES EN PAGE4 
    tft.canvasImageStartAddress(PAGE4_START_ADDR);
    tft.sdCardDraw256bitsBIN(173,131,177,160,"bin/botones/cocinado.bin"); // Escribiendo en page4
    tft.sdCardDraw256bitsBIN(351,131,177,160,"bin/botones/crudo.bin"); // Escribiendo en page4

    tft.canvasImageStartAddress(PAGE1_START_ADDR); // Para los borrados

    // ------------ COCINADO 1 -------------
    // La imagen ya está escrita en la page4, solo hay que copiarla en la page1
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); 
    delay(1000);
    // -------------------------------------

    // ------------ CRUDO 1 ----------------
    // La imagen ya está escrita en la page4, solo hay que borrar la de cocinado y volver a copiar la de crudo.
    // Borrar imagen cocinado 1 de la page1
    //tft.canvasImageStartAddress(PAGE1_START_ADDR);
    tft.clearArea(280,280,497,470,RED); 

    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160); 
    delay(1000);
    // -------------------------------------

    // ------------ COCINADO 2 -------------
    // La imagen ya está escrita en la page4, solo hay que borrar la de crudo y volver a copiar la de cocinado.
    // Borrar imagen crudo 1 de la page1
    tft.clearArea(500,280,724,480,RED); 
    
    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); 
    delay(1000);
    // -------------------------------------

    // ------------ CRUDO 2 ----------------
    // La imagen ya está escrita en la page4, solo hay que borrar la de cocinado y volver a copiar la de crudo.
    // Borrar imagen cocinado 2 de la page1
    tft.clearArea(280,280,497,470,RED); 
    
    //copy page4 picture to page1 
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160); 
    // -------------------------------------
    // ----------------------------------------------------------------------------------------------------





}


