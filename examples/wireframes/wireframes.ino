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
#include "Files.h"

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


    loadPicturesShowHourglass();
    
    //pantalla_inicial(); // OK
    //select_Grupo(); // OK con movimiento de mano y 2º pulsación. Se ha cambiado la mano por un icono nuevo con borde rojo y fondo verde
    //crudo_cocinado(); // OK con 3º alternancia y aparición/desaparición paulatinas
    //colocar_alimento(); // OK con aparición paulatina

    add_Plato(); // Ok con mano (icono) borde rojo y fondo blanco


    
    //loadPicturesRelojCompleto();  // 15 segundos solamente cargando imágenes de reloj (girando)
    //loadPicturesRelojCompletoMitad(); // 1/2 de tamaño para tardar menos
    //loadPicturesRelojCompletoCuarto(); // 1/4 de tamaño


    //error();
    //showError(1);
    //delay(2000);

    //aviso_v1(); // Primero texto de ¡AVISO! entre líneas, luego imagen y luego comentario
    //aviso_v2(); // Primero texto, luego imagen, luego una línea y luego comentario
    
    //showWarning(3);
 
 /* showWarning(1); // Igual que aviso_v2() pero con comentario específico según opción (1: añadir, 2: eliminar o 3: guardar)
    delay(3000);
    showWarning(2); 
    delay(3000);
    showWarning(3); 
*/

    //dashboard(); // PAGE3


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

    crudo_cocinado(); // PAGE4 (OK)*/
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void loop() {
     
}











//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- PANTALLA INICIAL ----------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void pantalla_inicial(){ // PAGE2 (OK) ==> HECHO

    // ----------- TEXTO ------------------------------------------------------------------------------------
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1

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
    //tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,121,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); // Mostrar brain2 en PAGE1

    // BRAIN2G (99x83) (verde)
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,121,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); // Mostrar brain2G en PAGE1
    // ------------------------------------------------------------------------------------------------------

}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- ESCOGER GRUPO -------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void select_Grupo(){ // PAGE3 (OK) ==> HECHO

    // ----- TEXTO (INTERNAL FIXED 12x24 X3) --------------------------------------------------------------
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1

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


    // ------------ DESPLAZAR MANO PARA SIMULAR MOVIMIENTO ------------------------------------------------
    // MANO por el camino
    desplazar_mano_Grupos();
    // ----------------------------------------------------------------------------------------------------


    // ------------ ALTERNAR 2 PULSACIONES ------------------------------------------------------------------
    // Ya se está en no pulsación, tras desplazar la mano.

    // Dividir 1 seg de delay en partes para chequear interrupciones mientras
    delay(500);
    // checkinterrupt
    delay(500);

    bool pulsacion = true;

    for(int i = 0; i < 4; i++){
        if(pulsacion){
            Serial.println("Pulsacion");
            pulsacion_Grupos();
        }
        else{
          Serial.println("No pulsacion");
          sin_pulsacion_Grupos(); 
        }
        if(i < 3){ // No hacer el delay si es la última pulsación/no pulsación (debería ser no pulsación)
            // Dividir 1 seg de delay en partes para chequear interrupciones mientras
            delay(500);
            // checkinterrupt
            delay(500);
        }
        pulsacion = !pulsacion;
    }
    // ----------------------------------------------------------------------------------------------------


/*
    // -------- SIN PULSACIÓN -----------------------------------------------------------------------------
    sin_pulsacion_Grupos(false); // false = no borrar
    // ----------------------------------------------------------------------------------------------------

    // Dividir 1 seg de delay en partes para chequear interrupciones mientras
    delay(500);
    // checkinterrupt
    delay(500);


    // -------- 1º PULSACIÓN ------------------------------------------------------------------------------
    pulsacion_Grupos();
    // ----------------------------------------------------------------------------------------------------


    // Dividir 1 seg de delay en partes para chequear interrupciones mientras
    delay(500);
    // checkinterrupt
    delay(500);

    // -------- SIN PULSACIÓN -----------------------------------------------------------------------------
    sin_pulsacion_Grupos(true); // true = borrar
    // ----------------------------------------------------------------------------------------------------


    // Dividir 1 seg de delay en partes para chequear interrupciones mientras
    delay(500);
    // checkinterrupt
    delay(500);

    // -------- 2º PULSACIÓN ------------------------------------------------------------------------------
    pulsacion_Grupos();
    // ----------------------------------------------------------------------------------------------------


    // Dividir 1 seg de delay en partes para chequear interrupciones mientras
    delay(500);
    // checkinterrupt
    delay(500);

    // -------- SIN PULSACIÓN -----------------------------------------------------------------------------
    sin_pulsacion_Grupos(true); // true = borrar
    // ----------------------------------------------------------------------------------------------------
    */
}

void desplazar_mano_Grupos(){
    int alto = 127;
    int posY = 480;

    // MANO por el camino
    while(posY >= 410){
        // manoGppt
        tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,posY,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)
        // manoWppt
        //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,252,293,PAGE1_START_ADDR,SCREEN_WIDTH,556,posY,118,126,WHITE); // Transparencia manoWppt (120x128)
        // manoG
        //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,556,posY,118,127,VERDE_PEDIR); // Mostrar manoG (120x129) en PAGE1
        delay(50);
        if(posY < 413) posY = 413; // Solo afecta al penúltimo movimiento de la mano, para evitar que se borre parte del grupo3 que está debajo
        tft.clearArea(556,posY,690,posY + alto+5,VERDE_PEDIR); // Desaparece de esa zona para aparecer en otra --> se mueve
        //tft.clearArea(556,posY,674,posY + alto,VERDE_PEDIR); // Desaparece de esa zona para aparecer en otra --> se mueve
        posY -= 10; // Subimos verticalmente la imagen 10 píxeles
    }
    
    posY = 400;
    while(posY >= 380){
        // Mostrar grupo3 (130x125). Para superponerse a la mano, que aún se está "moviendo"
        tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 
        // Mostrar mano (manoGppt)
        tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,posY,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)
        delay(50);
        if(posY == 400) tft.clearArea(556,413,690,528,VERDE_PEDIR); // Se borra desde y = 413 para no borrar parte del botón. Solo se borra en la primera iteración del bucle
        posY -= 20;
    }
/*
    // Mostrar grupo3 (130x125). Para superponerse a la mano, que aún se está "moviendo"
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 

    // Penúltimo movimiento de la mano (manoGppt)
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,400,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)
    // manoWppt
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,252,293,PAGE1_START_ADDR,SCREEN_WIDTH,566,400,118,126,WHITE); // Transparencia manoWppt (120x128)
    // manoG
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,556,400,118,127,VERDE_PEDIR); // Mostrar manoG (120x129) en PAGE1
    delay(50);
    tft.clearArea(556,413,690,528,VERDE_PEDIR); // Se borra desde y = 413 para no borrar parte del grupo3

    // Mostrar grupo3 (130x125). Para superponerse a la mano, que aún se está "moviendo"
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125);

    // Movimiento final de la mano (manoGppt)
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,119,127,VERDE_PEDIR);
*/
}

void sin_pulsacion_Grupos(){
    // Borrar todo (grupo, mano y pulsación)
    tft.clearArea(546,278,690,528,VERDE_PEDIR); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano

    // Mostrar grupo3 (130x125)
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 

    // Mostrar mano
    // manoGppt
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)
    // manoWppt
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,252,293,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,118,126,WHITE); // Transparencia manoWppt (120x128)
    // manoG
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,556,380,118,127,VERDE_PEDIR); // Mostrar manoG (120x129) en PAGE1
}

void pulsacion_Grupos(){
    int x1, y1, x2, y2;
    // ------------- 1º PULSACIÓN ---------------------------------------------------------------------------------------------------------------------
    // 1 - Borrar todo (grupo, mano y pulsación)
    tft.clearArea(546,278,690,528,VERDE_PEDIR); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
    
    // 2 - Volver a mostrar grupo3 (130x125)
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 
    
    // 3 - Pulsación
    // ------------ CUADRADO ESQUINADO (PULSACION) --------------------------------------------------------   
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados normales, separados por 1 píxel en cada dirección, para simular un grosor mayor.
   /* tft.drawRect(556,288,680,413,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(555,287,681,414,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(554,286,682,415,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(553,285,683,416,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(552,284,684,417,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(551,283,685,418,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(550,282,686,419,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(549,281,687,420,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(548,280,688,421,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(547,279,689,422,RED_BUTTON); // Alrededor de grupo3
    tft.drawRect(546,278,690,423,RED_BUTTON); // Alrededor de grupo3
*/

    for (int i = 0; i <= 10; i++) {
        x1 = 556 - i;   y1 = 288 - i;   
        x2 = 680 + i;   y2 = 413 + i;
        tft.drawRect(x1,y1,x2,y2,RED_BUTTON); // Alrededor de grupo3
    }
    // ----------------------------------------------------------------------------------------------------

    // 4 - Mano
    // ------------ MANO (120x129) ------------------------------------------------------------------------
    // Mano final pulsando
    // manoGppt
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)
    // manoWppt
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,252,293,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,118,126,WHITE); // Transparencia manoWppt (120x128)
    // manoG
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,556,380,118,127,VERDE_PEDIR); // Mostrar manoG (120x129) en PAGE1
    // ----------------------------------------------------------------------------------------------------

    // 5 - Rayitas pulsación
    // ------------ RAYITAS (PULSACION) ------------------
    // Línea izquierda
    /*tft.drawLine(584,355,594,375,RED_BUTTON);
    tft.drawLine(585,355,595,375,RED_BUTTON);
    tft.drawLine(586,355,596,375,RED_BUTTON);
    tft.drawLine(587,355,597,375,RED_BUTTON);
    tft.drawLine(588,355,598,375,RED_BUTTON);
    */
    for (int i = 0; i <= 4; i++) {
        x1 = 584 + i;   y1 = 355;   
        x2 = 594 + i;   y2 = 375;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }

    // Línea central
    /*tft.drawLine(604,350,604,370,RED_BUTTON);
    tft.drawLine(605,350,605,370,RED_BUTTON);
    tft.drawLine(606,350,606,370,RED_BUTTON);
    tft.drawLine(607,350,607,370,RED_BUTTON);
    tft.drawLine(608,350,608,370,RED_BUTTON);
    */
    for (int i = 0; i <= 4; i++) {
        x1 = 604 + i;   y1 = 350;   
        x2 = 604 + i;   y2 = 370;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }

    // Línea derecha
    /*tft.drawLine(614,375,624,355,RED_BUTTON);
    tft.drawLine(615,375,625,355,RED_BUTTON);
    tft.drawLine(616,375,626,355,RED_BUTTON);
    tft.drawLine(617,375,627,355,RED_BUTTON);
    tft.drawLine(618,375,628,355,RED_BUTTON);
    */
    for (int i = 0; i <= 4; i++) {
        x1 = 614 + i;   y1 = 375;   
        x2 = 624 + i;   y2 = 355;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }
    // ---------------------------------------------------
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- FIN ESCOGER GRUPO ---------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- CRUDO/COCINADO ------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void crudo_cocinado(){ // Tb PAGE3, pero más a la derecha
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(100, 30);
    tft.println("\xBF""EL ALIMENTO EST\xC1""");
    tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-50);
    tft.print("COCINADO O CRUDO\x3F"""); 
    delay(1000);
    // ----------------------------------------------------------------------------------------------------


    // ------------ LINEAS --------------------------------------------------------------------------------
    tft.fillRoundRect(0,250,256,258,3,WHITE);
    tft.fillRoundRect(768,517,1024,525,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    delay(1000);

    // ------------ BOTONES -------------------------------------------------------------------------------
    // ------------ COCINADO 1 -------------
    //mostrarOpcionProcesamiento(1); // 1 = Cocinado 
    // Mostrar cociGra
    slowAppearanceImage(1); // option = 1 --> imagen cocinado con 16/32 a 7/32 de opacidad
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); // Mostrar cociGra (177x160) en PAGE1
    delay(1000);

    // ------------ CRUDO 1 ----------------
    //mostrarOpcionProcesamiento(2); // 2 = Crudo
    slowAppearanceAndDisappareanceProcesamiento(2); // Desaparecer COCINADO y aparecer CRUDO
    delay(1000);

    // Otras 2 alternancias más
    for(uint8_t i = 0; i < 2; i++){
        for(uint8_t j = 1; j <= 2; j++){
            slowAppearanceAndDisappareanceProcesamiento(j); // j = 1 --> Desaparecer CRUDO y aparecer COCINADO  |  j = 2 -->  Desaparecer COCINADO y aparecer CRUDO
            delay(1000);
        }
    }
   /* // ------------ COCINADO 2 -------------
    //mostrarOpcionProcesamiento(1); // 1 = Cocinado 
    slowAppearanceAndDisappareance(1); // Desaparecer CRUDO y aparecer COCINADO 
    delay(1000);

    // ------------ CRUDO 2 ----------------
    //mostrarOpcionProcesamiento(2); // 2 = Crudo
    slowAppearanceAndDisappareance(2); // Desaparecer COCINADO y aparecer CRUDO
    delay(1000);

    // ------------ COCINADO 3 -------------
    //mostrarOpcionProcesamiento(1); // 1 = Cocinado 
    slowAppearanceAndDisappareance(1); // Desaparecer CRUDO y aparecer COCINADO 
    delay(1000);

    // ------------ CRUDO 3 ----------------
    //mostrarOpcionProcesamiento(2); // 2 = Crudo
    slowAppearanceAndDisappareance(2); // Desaparecer COCINADO y aparecer CRUDO
    // ----------------------------------------------------------------------------------------------------
*/
}



//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- FIN CRUDO/COCINADO --------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void slowDisappearance(int option){
    uint8_t i;
    switch(option){
        case 1: // Cocinado
            for(i = 4; i > 30; i++){
                // Mostrar cociGra desapareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160,i);
                delay(50);
            }
            break;

        case 2: // Crudo
            for(i = 4; i > 30; i++){
                // Mostrar crudoGra desapareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160,i);
                delay(50);
            }
            break;

        default: break;
    }
    
}

void slowAppearanceAndDisappareanceProcesamiento(int option){
    switch(option){
        case 1: // Desaparecer CRUDO y aparecer COCINADO 
            for (int i = 4, j = 30; i <= 30 && j >= 4; i++, j--) {  // i|j = 16  --> RA8876_ALPHA_OPACITY_16
                // Mostrar crudoGra desapareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160,i);
                // Mostrar cociGra apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160,j);
                delay(10);
            }
            tft.clearArea(500,280,724,480,VERDE_PEDIR); // Borrar crudoGra
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); // Mostrar cociGra (177x160) en PAGE1
            break;

        case 2: // Desaparecer COCINADO y aparecer CRUDO
            for (int i = 4, j = 30; i <= 30 && j >= 4; i++, j--) {  // i|j = 16  --> RA8876_ALPHA_OPACITY_16
                // Mostrar cociGra desapareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160,i);
                // Mostrar crudoGra apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160,j);
                delay(10);
            }
            tft.clearArea(280,280,497,470,VERDE_PEDIR); // Borrar cociGra
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160); // Mostrar crudoGra (177x160) en PAGE1
            break;

        default: break;
    }
}

void slowAppearanceImage(int option){
    uint8_t i;
    switch(option){
        case 1: // Cocinado
            for(i = 30; i >= 4; i--){
                // Mostrar cociGra apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160,i);
                delay(10);
            }
            break;

        case 2: // Scale
            for(i = 30; i >= 4; i--){
                // Mostrar scale apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,372,293,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,437,320,150,149,i);
                delay(10);
            }
            break;

        default: break;
    }
    
}

//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- COLOCAR ALIMENTO ----------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void colocar_alimento(){ // PAGE3 (OK)

    // ----------- TEXTO ------------------------------------------------------------------------------------
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1

    // ----- INTERNAL FIXED 12x24 X3 --------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(160, 50);
    tft.println("COLOQUE UN ALIMENTO");
    tft.setCursor(150, tft.getCursorY() + tft.getTextSizeY()-20);
    tft.print("EN LA ZONA DE PESADA"); 

    delay(1000);

    // ------------ LINEAS --------------------------------------------------------------------------------
    tft.fillRoundRect(0,270,276,278,3,WHITE);
    tft.fillRoundRect(748,517,1024,525,3,WHITE);
    delay(1000);
    // ----------------------------------------------------------------------------------------------------
      
    // ----------- SCALE (150x150)  ------------------------------------------------------------------------

    slowAppearanceImage(2); // option = 2 --> imagen scale con 16/32 a 7/32 de opacidad

    // Scale 
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,372,293,PAGE1_START_ADDR,SCREEN_WIDTH,437,320,150,149); // Mostrar scale (150x150) en PAGE1
    delay(1000);

}





//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//----------------------------- AÑADIR PLATO --------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
void desplazar_mano_Botones(){
    int alto = 128;
    int posY = 580;

    // MANO por el camino
    while(posY >= 510){
        // manoWppt
        tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,posY,120,128,WHITE); // Transparencia manoWppt (120x128)
        delay(50);
        //if(posY < 413) posY = 413; // Solo afecta al penúltimo movimiento de la mano, para evitar que se borre parte del botón que está debajo
        tft.clearArea(430,posY,567,posY + alto,AMARILLO_CONFIRM_Y_AVISO); // Desaparece de esa zona para aparecer en otra --> se mueve
        posY -= 10; // Subimos verticalmente la imagen 10 píxeles
    }

    // ------ Botón añadir (172x130) ---------
    // Para superponerse a la última mano y que desaparezca para simular el movimiento
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) 

    // Movimiento final de la mano (manoWppt)
    // manoWppt
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,472,120,128,WHITE); // Transparencia manoWppt (120x128)
    delay(50);
    //tft.clearArea(420,510,690,528,AMARILLO_CONFIRM_Y_AVISO); // Se borra desde y = 413 para no borrar parte del botón añadir
    
}
/*
void sin_pulsacion_Botones(bool borrar){
    if(borrar){
        // Borrar todo (botón, mano y pulsación)
        tft.clearArea(410,370,266,620,AMARILLO_CONFIRM_Y_AVISO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
    }
    // Mostrar botón añadir (172x130)
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) 

    // Mostrar mano (manoWppt)
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,472,120,128,WHITE); // Transparencia manoWppt (120x128)
}

void pulsacion_Botones(){
    int x1, y1, x2, y2;
    // ------------- 1º PULSACIÓN ---------------------------------------------------------------------------------------------------------------------
    // 1 - Borrar todo (grupo, mano y pulsación)
    tft.clearArea(410,370,266,620,AMARILLO_CONFIRM_Y_AVISO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano

    // 2 - Volver a mostrar botón añadir (172x130)
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) en PAGE1
    
    // 3 - Pulsación
    // ------------ CUADRADO ESQUINADO (PULSACION) --------------------------------------------------------   
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados normales, separados por 1 píxel en cada dirección, para simular un grosor mayor.
    for (int i = 0; i <= 10; i++) {
        x1 = 425 - i;   y1 = 383 - i;   
        x2 = 590 + i;   y2 = 505 + i;
        tft.drawRect(x1,y1,x2,y2,RED_BUTTON); // Alrededor de añadir
    }
    // ----------------------------------------------------------------------------------------------------

    // 4 - Mano
    // ------------ MANO (120x129) ------------------------------------------------------------------------
    // Mano (manoWppt) final pulsando
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,472,120,128,WHITE); // Transparencia manoWppt (120x128)
    // ----------------------------------------------------------------------------------------------------

    // 5 - Rayitas pulsación
    // ------------ RAYITAS (PULSACION) ------------------
    // Línea izquierda
    for (int i = 0; i <= 4; i++) {
        x1 = 448 + i;   y1 = 447;   
        x2 = 458 + i;   y2 = 467;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }

    // Línea central
    for (int i = 0; i <= 4; i++) {
        x1 = 468 + i;   y1 = 442;   
        x2 = 468 + i;   y2 = 462;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }

    // Línea derecha
    for (int i = 0; i <= 4; i++) {
        x1 = 478 + i;   y1 = 467;   
        x2 = 488 + i;   y2 = 447;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }
    // ---------------------------------------------------
}
*/


void add_Plato(){ // Tb PAGE3, pero más abajo ==> HECHO
    int x1, y1, x2, y2;

    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO); // Fondo amarillo en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(30, 30);
    tft.println("\xBF""EST\xC1"" SEGURO DE QUE QUIERE");
    tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-40);
    //tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-30);
    tft.print("A\xD1""ADIR UN NUEVO PLATO\x3F"""); 
    delay(1000);
    // ----------------------------------------------------------------------------------------------------


    // ------------ LINEA --------------------------------------------------------------------------------
    tft.fillRoundRect(252,205,764,213,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    //tft.fillRoundRect(252,220,764,228,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    // ----------------------------------------------------------------------------------------------------
    delay(500);


    // ----- TEXTO (CONFIRMACIÓN) -------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setCursor(150, 245);
    //tft.setCursor(150, 270);
    tft.println("PARA CONFIRMAR, PULSE DE NUEVO");
    tft.setCursor(400, tft.getCursorY() + tft.getTextSizeY()-10); // + 10
    tft.print("EL BOT\xD3""N"); 
    // ----------------------------------------------------------------------------------------------------
    delay(500);

    // ------------ BOTÓN AÑADIR --------------------------------------------------------------------------
    // Copiar de PAGE3 a PAGE1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) en PAGE1
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); // Mostrar añadir (172x130) en PAGE1
    delay(800);
    // ----------------------------------------------------------------------------------------------------

    // ------------ DESPLAZAR MANO PARA SIMULAR MOVIMIENTO ------------------------------------------------
    // MANO por el camino
    desplazar_mano_Botones();
    // ----------------------------------------------------------------------------------------------------
    
     // ------------ CUADRADO REDONDEADO (PULSACION) -------------------------------------------------------
    
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados redondeados, separados por 1 píxel en cada dirección, para simular un grosor mayor.
   /* tft.drawRoundRect(425,413,590,535,20,RED_BUTTON); // Alrededor de botón
    tft.drawRoundRect(424,412,591,536,20,RED_BUTTON); 
    tft.drawRoundRect(423,411,592,537,20,RED_BUTTON); 
    tft.drawRoundRect(422,410,593,538,20,RED_BUTTON); 
    tft.drawRoundRect(421,409,594,539,20,RED_BUTTON); 
    tft.drawRoundRect(420,408,595,540,20,RED_BUTTON); 
    tft.drawRoundRect(419,407,596,541,20,RED_BUTTON); 
    tft.drawRoundRect(418,406,597,542,20,RED_BUTTON); 
    tft.drawRoundRect(417,405,598,543,20,RED_BUTTON); 
    tft.drawRoundRect(416,404,599,544,20,RED_BUTTON); 
    tft.drawRoundRect(415,403,600,545,20,RED_BUTTON); 
   */ 
   for (int i = 0; i <= 10; i++) { // Subido
        x1 = 425 - i;   y1 = 383 - i;   
        x2 = 590 + i;   y2 = 505 + i;
        tft.drawRect(x1,y1,x2,y2,RED_BUTTON); // Alrededor de grupo3
    }
    /*for (int i = 0; i <= 10; i++) {
        x1 = 425 - i;   y1 = 413 - i;   
        x2 = 590 + i;   y2 = 535 + i;
        tft.drawRect(x1,y1,x2,y2,RED_BUTTON); // Alrededor de grupo3
    }*/
    // ----------------------------------------------------------------------------------------------------


    // ------------ MANO (120x128) ------------------------------------------------------------------------
    // Mano con fondo blanco:
    //      La imagen de la mano no es blanco puro, por eso se puede filtrar el fondo blanco y que se siga viendo
    //      la mano. Así no aparecen los píxeles de color que se veían en los casos de tener el fondo con color (rojo, verde, amarillo, etc.).
    
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,472,120,128,WHITE); // Transparencia manoWppt
    
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,420,472,120,128,WHITE); // Transparencia manoW
    // ----------------------------------------------------------------------------------------------------
    

    // ------------ RAYITAS (PULSACION) --------------------------------------------------------
    // Línea izquierda
    /*tft.drawLine(448,467,458,487,RED_BUTTON);
    tft.drawLine(449,467,459,487,RED_BUTTON);
    tft.drawLine(450,467,460,487,RED_BUTTON);
    tft.drawLine(451,467,461,487,RED_BUTTON);
    tft.drawLine(452,467,462,487,RED_BUTTON);
    */
    for (int i = 0; i <= 4; i++) { // subido
        x1 = 448 + i;   y1 = 447;   
        x2 = 458 + i;   y2 = 467;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }
    /*for (int i = 0; i <= 4; i++) {
        x1 = 448 + i;   y1 = 467;   
        x2 = 458 + i;   y2 = 487;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }*/

    // Línea central
    /*tft.drawLine(468,462,468,482,RED_BUTTON);
    tft.drawLine(469,462,469,482,RED_BUTTON);
    tft.drawLine(470,462,470,482,RED_BUTTON);   
    tft.drawLine(471,462,471,482,RED_BUTTON);
    tft.drawLine(472,462,472,482,RED_BUTTON);
    */
    for (int i = 0; i <= 4; i++) { // subido
        x1 = 468 + i;   y1 = 442;   
        x2 = 468 + i;   y2 = 462;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }
    /*for (int i = 0; i <= 4; i++) {
        x1 = 468 + i;   y1 = 462;   
        x2 = 468 + i;   y2 = 482;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }*/

    // Línea derecha
    /*tft.drawLine(478,487,488,467,RED_BUTTON);
    tft.drawLine(479,487,489,467,RED_BUTTON);
    tft.drawLine(480,487,490,467,RED_BUTTON);   
    tft.drawLine(481,487,491,467,RED_BUTTON);
    tft.drawLine(482,487,492,467,RED_BUTTON);
    */
    for (int i = 0; i <= 4; i++) { // subido
        x1 = 478 + i;   y1 = 467;   
        x2 = 488 + i;   y2 = 447;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }
    /*for (int i = 0; i <= 4; i++) {
        x1 = 478 + i;   y1 = 487;   
        x2 = 488 + i;   y2 = 467;
        tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
    }*/
    // ----------------------------------------------------------------------------------------------------
/*
    delay(2000);

    // ----- TEXTO (PLATO AÑADIDO) -------------------------------------------------------------------------
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO);
    // ------ LINEA ---------
    tft.fillRoundRect(252,200,764,208,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    // ------ TEXTO ---------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setCursor(170, 258);
    tft.println("NUEVO PLATO A\xD1""ADIDO");
    // ------ LINEA ---------
    tft.fillRoundRect(252,380,764,388,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    // ----------------------------------------------------------------------------------------------------

    */
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void error(){ // Tb PAGE3, pero más abajo
     // cruz
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(0,292,114,127,"bin/error/cruz.bin"); // Cargar cruz (114x127) en PAGE3 =>  x  =  0  ->   y = <crudoGra(131) + crudoGra(160) + 1 = 292

    tft.canvasImageStartAddress(PAGE1_START_ADDR); 

    // ----- TEXTO (ERROR) --------------------------------------------------------------------------------
    tft.clearScreen(RED); // Fondo rojo en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(170, 100);
    tft.println("\xA1""ACCI\xD3""N INCORRECTA\x21""");
    // ----------------------------------------------------------------------------------------------------


    // ------------ CRUZ --------------------------------------------------------------------------------
    // Copiar de PAGE3 a PAGE1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,292,PAGE1_START_ADDR,SCREEN_WIDTH,451,231,114,127); // Mostrar cruz (114x127) en PAGE1
    // ----------------------------------------------------------------------------------------------------


    // ------------ LINEA --------------------------------------------------------------------------------
    // Por encima de la imagen de cruz para no tener que cuadrarla con la línea de la imagen
    tft.fillRoundRect(252,290,764,298,3,WHITE);
    // ----------------------------------------------------------------------------------------------------



    // ----- TEXTO (SUGERENCIA SEGÚN ESTADO ACTUAL) -------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setCursor(270, 420);
    tft.println("COLOQUE UN RECIPIENTE ANTES DE"); 
    tft.setCursor(240, tft.getCursorY() + tft.getTextSizeY()+30);
    tft.print("SELECCIONAR UN GRUPO DE ALIMENTOS"); 
    // ----------------------------------------------------------------------------------------------------
}


void showError(int option){ 
    // cruz
    tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
    tft.sdCardDraw16bppBIN256bits(0,292,114,127,"bin/error/cruz.bin"); // Cargar cruz (114x127) en PAGE3 =>  x  =  0  ->   y = <crudoGra(131) + crudoGra(160) + 1 = 292

    tft.canvasImageStartAddress(PAGE1_START_ADDR); 

    // ----- TEXTO (ERROR) --------------------------------------------------------------------------------
    tft.clearScreen(RED); // Fondo rojo en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(170, 100);
    tft.println("\xA1""ACCI\xD3""N INCORRECTA\x21""");
    // ----------------------------------------------------------------------------------------------------


    // ------------ CRUZ --------------------------------------------------------------------------------
    // Copiar de PAGE3 a PAGE1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,292,PAGE1_START_ADDR,SCREEN_WIDTH,451,231,114,127); // Mostrar cruz (114x127) en PAGE1
    // ----------------------------------------------------------------------------------------------------


    // ------------ LINEA --------------------------------------------------------------------------------
    // Por encima de la imagen de cruz para no tener que cuadrarla con la línea de la imagen
    tft.fillRoundRect(252,290,764,298,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // ----- TEXTO (SUGERENCIA SEGÚN ESTADO ACTUAL) ------------------------------------------------------
    // OPCION 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
    //tft.selectInternalFont(RA8876_FONT_SIZE_32);
    //tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    
    // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    switch (option){
      case 1: // Empty
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(270, 420);                                        tft.println("COLOQUE UN RECIPIENTE ANTES DE"); 
              //tft.setCursor(280, tft.getCursorY() + tft.getTextSizeY()+30);   tft.print("ESCOGER UN GRUPO DE ALIMENTOS"); 
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(160, 420);                                     tft.println("COLOQUE UN RECIPIENTE ANTES DE"); 
              tft.setCursor(180, tft.getCursorY() + tft.getTextSizeY());   tft.print("ESCOGER UN GRUPO DE ALIMENTOS"); 
              break;

      case 2: // Plato
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(280, 420);                                        tft.println("SELECCIONE GRUPO DE ALIMENTOS"); 
              //tft.setCursor(250, tft.getCursorY() + tft.getTextSizeY()+30);   tft.print("ANTES DE ESCOGER COCINADO O CRUDO"); 
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(160, 420);                                     tft.println("SELECCIONE GRUPO DE ALIMENTOS"); 
              tft.setCursor(120, tft.getCursorY() + tft.getTextSizeY());   tft.print("ANTES DE ESCOGER COCINADO O CRUDO"); 
              break;

      case 3: // grupoA
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(330, 420);                                        tft.println("ESCOJA COCINADO O CRUDO"); 
              //tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY()+30);   tft.print("ANTES DE PESAR EL ALIMENTO"); 
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(190, 420);                                     tft.println("SELECCIONE COCINADO O CRUDO"); 
              tft.setCursor(200, tft.getCursorY() + tft.getTextSizeY());   tft.print("ANTES DE PESAR EL ALIMENTO"); 
              break;

      case 4: // grupoB
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(330, 420);                                        tft.println("ESCOJA COCINADO O CRUDO"); 
              //tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY()+30);   tft.print("ANTES DE PESAR EL ALIMENTO"); 
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(190, 420);                                     tft.println("SELECCIONE COCINADO O CRUDO"); 
              tft.setCursor(200, tft.getCursorY() + tft.getTextSizeY());   tft.print("ANTES DE PESAR EL ALIMENTO"); 
              break;

      case 5: // Crudo
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(220, 450);                                        tft.println("COLOQUE UN ALIMENTO SOBRE LA B\xC1""SCULA");  
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(100, 450);                                     tft.println("COLOQUE UN ALIMENTO SOBRE LA B\xC1""SCULA");  
              break;

      case 6: // Cocinado
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(220, 450);                                        tft.println("COLOQUE UN ALIMENTO SOBRE LA B\xC1""SCULA");  
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(100, 450);                                     tft.println("COLOQUE UN ALIMENTO SOBRE LA B\xC1""SCULA"); 
              break;

      case 7: // Pesado
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(270, 420);                                        tft.println("ESCOJA GRUPO PARA OTRO ALIMENTO,"); 
              //tft.setCursor(240, tft.getCursorY() + tft.getTextSizeY()+30);   tft.print("A\xD1""ADA OTRO PLATO O GUARDE LA COMIDA"); 
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(140, 420);                                     tft.println("ESCOJA GRUPO PARA OTRO ALIMENTO,"); 
              tft.setCursor(100, tft.getCursorY() + tft.getTextSizeY());   tft.print("A\xD1""ADA OTRO PLATO O GUARDE LA COMIDA"); 
              break;

      case 8: // add_check
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(210, 420);                                        tft.println("PULSE \"A\xD1""ADIR\" DE NUEVO PARA CONFIRMAR"); 
              //tft.setCursor(220, tft.getCursorY() + tft.getTextSizeY()+30);   tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(70, 420);                                     tft.println("PULSE \"A\xD1""ADIR\" DE NUEVO PARA CONFIRMAR"); 
              tft.setCursor(100, tft.getCursorY() + tft.getTextSizeY());   tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              break;
      
      case 9: // Added
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(200, 450);                                        tft.println("RETIRE EL PLATO PARA COMENZAR UNO NUEVO");  
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(50, 450);                                        tft.println("RETIRE EL PLATO PARA COMENZAR UNO NUEVO");  
              break;

      case 10: // delete_check
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(210, 420);                                        tft.println("PULSE \"BORRAR\" DE NUEVO PARA CONFIRMAR"); 
              //tft.setCursor(220, tft.getCursorY() + tft.getTextSizeY()+30);   tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(60, 420);                                     tft.println("PULSE \"BORRAR\" DE NUEVO PARA CONFIRMAR"); 
              tft.setCursor(90, tft.getCursorY() + tft.getTextSizeY());   tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              break;

      case 11: // Deleted
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(250, 450);                                        tft.println("RETIRE EL PLATO QUE HA ELIMINADO"); 
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(130, 450);                                    tft.println("RETIRE EL PLATO QUE HA ELIMINADO"); 
              break;

      case 12: // save_check
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(200, 420);                                        tft.println("PULSE \"GUARDAR\" DE NUEVO PARA CONFIRMAR"); 
              //tft.setCursor(220, tft.getCursorY() + tft.getTextSizeY()+30);   tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              
              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(50, 420);                                     tft.println("PULSE \"GUARDAR\" DE NUEVO PARA CONFIRMAR"); 
              tft.setCursor(90, tft.getCursorY() + tft.getTextSizeY());   tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              break;


      case 13: // Saved
              // OPCIÓN 1 DE TAMAÑO 16X32 SIN ESCALA: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(270, 450);                                        tft.println("RETIRE EL PLATO PARA CONTINUAR");  

              // OPCION 2 DE TAMAÑO 12X24 ESCALA X2: MEJOR
              tft.setCursor(155, 450);                                        tft.println("RETIRE EL PLATO PARA CONTINUAR");  
              break;
    }
    // ----------------------------------------------------------------------------------------------------  

}




void aviso_v1(){ // Tb PAGE3, pero más abajo
    // Aumentar un poco el tamaño de la imagen de aviso no haría daño.
    // Imagen de "aviso"
    tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
    //tft.sdCardDraw16bppBIN256bits(115,292,135,113,"bin/aviso/aviso1.bin"); // Cargar aviso1 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    // Imagen aviso2 con fondo rojo
    //tft.sdCardDraw16bppBIN256bits(115,292,135,113,"bin/aviso/aviso2R.bin"); // Cargar aviso2 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    // Imagen aviso2 con fondo naranja oscuro
    tft.sdCardDraw16bppBIN256bits(115,292,135,113,"bin/aviso/aviso2O.bin"); // Cargar aviso2 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    //tft.sdCardDraw16bppBIN256bits(115,292,135,119,"bin/aviso/aviso3.bin"); // Cargar aviso3 (135x119) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    tft.canvasImageStartAddress(PAGE1_START_ADDR); 
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(DARKORANGE); // Fondo amarillo oscuro en PAGE1

    // ----- TEXTO (AVISO) ----------------------------------------------------------------------------
    // ------ LINEA ---------
    tft.fillRoundRect(252,50,764,57,3,WHITE);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(380, 100);
    tft.println("\xA1""AVISO\x21""");
    // ------ LINEA ---------
    tft.fillRoundRect(252,180,764,187,3,WHITE);
    // ----------------------------------------------------------------------------------------------------



    // ------------ ADVERTENCIA ---------------------------------------------------------------------------
    // Copiar de PAGE3 a PAGE1

    // Aumentar un poco el tamaño de la imagen de aviso no haría daño.

    // Decimos que está en el (115,293) en lugar de (115,292) para eliminar la línea de arriba, que no sé por qué aparece.
    // Eso hace que si se sigue indicando un tamaño de 135x113, ahora aparece justo debajo una línea de basura de la PAGE3.
    // Por eso se dice que mide 135x112, para evitar que saque esa línea de basura.
    // Ocurre lo mismo con la imagen de aviso3: se dice que mide 135x118 en lugar de 135x119 para eliminar esa línea que aparece
    // al modificar el punto de inicio de la imagen en PAGE3. 

    // aviso1 => Apenas de se ve el borde rojo de la figura
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,115,293,PAGE1_START_ADDR,SCREEN_WIDTH,445,240,135,112); // Mostrar aviso1 (135x113) en PAGE1
    
    // aviso2 => Queda guay. Mejor opción en este estilo de pantalla
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,115,293,PAGE1_START_ADDR,SCREEN_WIDTH,445,240,135,112); // Mostrar aviso2 (135x113) en PAGE1
    
    // aviso3 => No se ve el borde pero queda bien porque el interior es negro, aunque el negro puede desentonar con el resto de la pantalla.
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,115,293,PAGE1_START_ADDR,SCREEN_WIDTH,445,237,135,118); // Mostrar aviso3 (135x119) en PAGE1
    // ----------------------------------------------------------------------------------------------------


    // ----- TEXTO (ACCION SIN EFECTO SEGÚN EL CASO) ------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setCursor(270, 430);
    tft.println("NO SE HA CREADO UN NUEVO PLATO"); 
    tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY()+30);
    tft.print("PORQUE EL ACTUAL EST\xC1"" VAC\xCD""O"); 
    // ----------------------------------------------------------------------------------------------------
}


void aviso_v2(){ // Tb PAGE3, pero más abajo
    // Aumentar un poco el tamaño de la imagen de aviso no haría daño.
    // Imagen de "aviso"
    tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
    //tft.sdCardDraw16bppBIN256bits(115,292,135,113,"bin/aviso/aviso1.bin"); // Cargar aviso1 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    // Imagen aviso2 con fondo rojo
    //tft.sdCardDraw16bppBIN256bits(115,292,135,113,"bin/aviso/aviso2R.bin"); // Cargar aviso2 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    // Imagen aviso2 con fondo naranja oscuro
    tft.sdCardDraw16bppBIN256bits(115,292,135,113,"bin/aviso/aviso2O.bin"); // Cargar aviso2 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    //tft.sdCardDraw16bppBIN256bits(115,292,135,119,"bin/aviso/aviso3.bin"); // Cargar aviso3 (135x119) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    tft.canvasImageStartAddress(PAGE1_START_ADDR); 
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(DARKORANGE); // Fondo amarillo oscuro en PAGE1


    // ----- TEXTO (AVISO) -------------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(384, 100);
    tft.println("\xA1""AVISO\x21""");
    // ---------------------------------------------------------------------------------------------------



    // ------------ ADVERTENCIA ---------------------------------------------------------------------------
    // Copiar de PAGE3 a PAGE1

    // Aumentar un poco el tamaño de la imagen de aviso no haría daño.

    // Decimos que está en el (115,293) en lugar de (115,292) para eliminar la línea de arriba, que no sé por qué aparece.
    // Eso hace que si se sigue indicando un tamaño de 135x113, ahora aparece justo debajo una línea de basura de la PAGE3.
    // Por eso se dice que mide 135x112, para evitar que saque esa línea de basura.
    // Ocurre lo mismo con la imagen de aviso3: se dice que mide 135x118 en lugar de 135x119 para eliminar esa línea que aparece
    // al modificar el punto de inicio de la imagen en PAGE3. 

    // aviso1 => Apenas de se ve el borde rojo de la figura
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,115,293,PAGE1_START_ADDR,SCREEN_WIDTH,445,50,135,112); // Mostrar aviso1 (135x113) en PAGE1
    
    // aviso2 => Queda guay. Mejor opción en este estilo de pantalla
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,115,293,PAGE1_START_ADDR,SCREEN_WIDTH,445,180,135,112); // Mostrar aviso2 (135x113) en PAGE1
    
    // aviso3 => No se ve el borde pero queda bien porque el interior es negro, aunque el negro puede desentonar con el resto de la pantalla.
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,115,293,PAGE1_START_ADDR,SCREEN_WIDTH,445,47,135,118); // Mostrar aviso3 (135x119) en PAGE1
    // ----------------------------------------------------------------------------------------------------



    // ------------ LINEA --------------------------------------------------------------------------------
    tft.fillRoundRect(252,350,764,358,3,WHITE);
    // ---------------------------------------------------------------------------------------------------




    // ----- TEXTO (ACCION SIN EFECTO SEGÚN EL CASO) ------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setCursor(270, 430);
    tft.println("NO SE HA CREADO UN NUEVO PLATO"); 
    tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY()+30);
    tft.print("PORQUE EL ACTUAL EST\xC1"" VAC\xCD""O"); 
    // ----------------------------------------------------------------------------------------------------
}



void showWarning(int option){

    // Aumentar un poco el tamaño de la imagen de aviso no haría daño.
    // Imagen de "aviso"
    tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
    //tft.sdCardDraw16bppBIN256bits(115,292,135,113,"bin/aviso/aviso1.bin"); // Cargar aviso1 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    // Imagen aviso2 con fondo rojo
    //tft.sdCardDraw16bppBIN256bits(115,292,135,113,"bin/aviso/aviso2R.bin"); // Cargar aviso2 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    // Imagen aviso2 con fondo naranja oscuro
    tft.sdCardDraw16bppBIN256bits(115,292,135,113,"bin/aviso/aviso2O.bin"); // Cargar aviso2 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

    //tft.sdCardDraw16bppBIN256bits(115,292,135,119,"bin/aviso/aviso3.bin"); // Cargar aviso3 (135x119) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292


    // ----- TEXTO (AVISO) -------------------------------------------------------------------------------
    tft.canvasImageStartAddress(PAGE1_START_ADDR); 
    tft.clearScreen(DARKORANGE); // Fondo amarillo oscuro en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 

    tft.setCursor(384, 100);
    tft.println("\xA1""AVISO\x21""");
    // ---------------------------------------------------------------------------------------------------


    // ------------ LINEA --------------------------------------------------------------------------------
    tft.fillRoundRect(252,286,764,294,3,WHITE); // Cruza la imagen de aviso por detrás
    // ---------------------------------------------------------------------------------------------------


    // ------------ ADVERTENCIA ---------------------------------------------------------------------------
    // Copiar de PAGE3 a PAGE1

    // Aumentar un poco el tamaño de la imagen de aviso no haría daño.

    // Decimos que está en el (115,293) en lugar de (115,292) para eliminar la línea de arriba, que no sé por qué aparece.
    // Eso hace que si se sigue indicando un tamaño de 135x113, ahora aparece justo debajo una línea de basura de la PAGE3.
    // Por eso se dice que mide 135x112, para evitar que saque esa línea de basura.
    // Ocurre lo mismo con la imagen de aviso3: se dice que mide 135x118 en lugar de 135x119 para eliminar esa línea que aparece
    // al modificar el punto de inicio de la imagen en PAGE3. 

    // aviso2 
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,115,293,PAGE1_START_ADDR,SCREEN_WIDTH,445,230,135,112); // Mostrar aviso2 (135x113) en PAGE1
    // ----------------------------------------------------------------------------------------------------



    // ------------ LINEA --------------------------------------------------------------------------------
   // tft.fillRoundRect(252,350,764,358,3,WHITE); // Por debajo de la imagen de aviso
    // ---------------------------------------------------------------------------------------------------


    // ----- TEXTO (ACCION SIN EFECTO SEGÚN EL CASO) ------------------------------------------------------
    
    // OPCION 1 DE TAMAÑO: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
    /*tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); */
    
    // OPCION 2 DE TAMAÑO:
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    switch (option){
      case 1: // AÑADIR
              
              // OPCIÓN 1 DE TAMAÑO: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(270, 430);                                        tft.println("NO SE HA CREADO UN NUEVO PLATO"); 
              //tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY()+30);   tft.print("PORQUE EL ACTUAL EST\xC1"" VAC\xCD""O"); 
              
              // OPCION 2: MEJOR
              tft.setCursor(150, 410);                                      tft.println("NO SE HA CREADO UN NUEVO PLATO"); 
              tft.setCursor(180, tft.getCursorY() + tft.getTextSizeY());    tft.print("PORQUE EL ACTUAL EST\xC1"" VAC\xCD""O");  
              
              break;

      case 2: // ELIMINAR
              
              // OPCIÓN 1 DE TAMAÑO: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(150, 430); tft.println("NO SE HA ELIMINADO EL PLATO PORQUE EST\xC1"" VAC\xCD""O"); 
              
              // OPCION 2: MEJOR
              tft.setCursor(180, 410);                                      tft.println("NO SE HA ELIMINADO EL PLATO"); 
              tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY());    tft.print("PORQUE EST\xC1"" VAC\xCD""O"); 
              
              break;

      case 3: // BOTÓN GUARDAR
              
              // OPCIÓN 1 DE TAMAÑO: QUEDA OK PERO PUEDE QUE EL COMENTARIO SE QUEDE PEQUEÑO
              //tft.setCursor(150, 430); tft.println("NO SE HA GUARDADO LA COMIDA PORQUE EST\xC1"" VAC\xCD""A"); 
              
              // OPCION 2: MEJOR
              tft.setCursor(190, 410);                                      tft.println("NO SE HA GUARDADO LA COMIDA"); 
              tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY());    tft.print("PORQUE EST\xC1"" VAC\xCD""A"); 
     
              break;
    }
    // ----------------------------------------------------------------------------------------------------  


    
}










void dashboard(){ // OK ==> HECHO
    float raciones;
    int integerPart;

    // --------- DASHBOARD -------------------------------------------------------------------------------
      // cociPeq
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(529,131,47,42,"bin/dash/cociPeq.bin"); // Cargar cociPeq (47x42) en PAGE3 =>  x  =  <crudoGra(351) + crudoGra(177) + 1 = 529  ->   y = 131  

      // crudoPeq
      tft.sdCardDraw16bppBIN256bits(577,131,47,42,"bin/dash/crudoPeq.bin"); // Cargar crudoPeq (47x42) en PAGE3 =>  x  =  <cociPeq(529) + crudoGra(47) + 1 = 577  ->   y = 131  

      // kcal
      //tft.sdCardDraw16bppBIN256bits(529,174,80,87,"bin/dash/kcal.bin"); // Cargar kcal (80x87) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174
      //tft.sdCardDraw16bppBIN256bits(529,174,70,76,"bin/dash/kcal_10.bin"); // Cargar kcal_10 (70x76) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174
      tft.sdCardDraw16bppBIN256bits(529,174,60,65,"bin/dash/kcal_20.bin"); // Cargar kcal_20 (60x65) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174
      //tft.sdCardDraw16bppBIN256bits(529,174,50,54,"bin/dash/kcal_30.bin"); // Cargar kcal_30 (50x54) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174

    // --------- FIN DASHBOARD ---------------------------------------------------------------------------

    tft.canvasImageStartAddress(PAGE1_START_ADDR); // Regresar a PAGE1
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
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,174,PAGE1_START_ADDR,SCREEN_WIDTH,117,483,80,87);  // Mostrar kcal (80x87) en PAGE1
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,175,PAGE1_START_ADDR,SCREEN_WIDTH,117,497,70,75);  // Mostrar kcal_10 (70x76) en PAGE1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,175,PAGE1_START_ADDR,SCREEN_WIDTH,127,507,60,64);  // Mostrar kcal_20 (60x65) en PAGE1
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,174,PAGE1_START_ADDR,SCREEN_WIDTH,117,496,50,54);  // Mostrar kcal_30 (50x54) en PAGE1
   
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
    tft.print("PESO: 145.2g"); // 24 escale x2


    // ---- VALORES ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1);

    // Carbohidratos
    tft.setCursor(50,303);
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.print("CARBOHIDRATOS: 100.1g"); // 32 escale x1
    
    // Proteinas
    tft.setCursor(50,380);
    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.print("PROTE\xCD""NAS: 70.1g"); // 32 escale x1
    
    // Grasas
    tft.setCursor(50,457);
    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.print("GRASAS: 49.1g");  // 32 escale x1
    
    // Kcal
    tft.setCursor(197,516);
    tft.setTextForegroundColor(ROJO_KCAL); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.print("121 Kcal."); // 24 escale x2
    // ----- FIN VALORES ----


    // ---- RACIONES ------
    // Texto "Raciones"
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(370,243);
    tft.print("Raciones"); // 32 escale x1

    // Cambiamos el tamaño de texto para los valores de Raciones. 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    // Raciones de Carbohidratos
    raciones = 1.0;
    integerPart = abs((int)raciones);
    if((integerPart == 0) or ((integerPart/10) == 0)){ // Una cifra entera
      tft.setCursor(426,293);
    }
    else{ // 2 cifras enteras
      tft.setCursor(416,293);
    } 
    tft.setTextForegroundColor(WHITE); 
    tft.print(integerPart); // 24 escale x2

    // Raciones de Proteinas
    raciones = 10.0;
    integerPart = abs((int)raciones);
    if((integerPart == 0) or ((integerPart/10) == 0)){ // Una cifra entera
      tft.setCursor(426,370);
    }
    else{ // 2 cifras enteras
      tft.setCursor(416,370);
    } 
    tft.setTextForegroundColor(WHITE); 
    tft.print(integerPart); // 24 escale x2

    // Raciones de Grasas
    tft.setCursor(416,447);
    tft.setTextForegroundColor(WHITE); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); 
    tft.print("0.5"); // 24 escale x2
    // ---- FIN RACIONES ----

    // ---- RACIONES ------ 

    
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
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,174,PAGE1_START_ADDR,SCREEN_WIDTH,607,483,80,87);  // Mostrar kcal (80x87) en PAGE1
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,175,PAGE1_START_ADDR,SCREEN_WIDTH,617,507,60,64);  // Mostrar kcal_20 (60x65) en PAGE1

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
    tft.print("PESO: 2145.2g"); // 24 escale x2


    // ---- VALORES ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1);
    
    // Carbohidratos
    tft.setCursor(540,303);
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.print("CARBOHIDRATOS: 1000.1g"); // 32 escale x1
    
    // Proteinas
    tft.setCursor(540,380);
    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.print("PROTE\xCD""NAS: 700.1g"); // 32 escale x1
    
    // Grasas
    tft.setCursor(540,457);
    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.print("GRASAS: 490.1g");  // 32 escale x1
    
    // Kcal
    tft.setCursor(687,516);
    tft.setTextForegroundColor(ROJO_KCAL); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.print("1121 Kcal."); // 24 escale x2
    // ----- FIN VALORES ----


    // ---- RACIONES ------
    // Texto "Raciones"
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(860,243);
    tft.print("Raciones"); // 32 escale x1

    // Cambiamos el tamaño de texto para los valores de Raciones. 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    // Raciones de Carbohidratos
    tft.setCursor(906,293);
    tft.setTextForegroundColor(WHITE); 
    tft.print("10"); // 24 escale x2

    // Raciones de Proteinas
    tft.setCursor(916,370);
    tft.setTextForegroundColor(WHITE); 
    tft.print("7"); // 24 escale x2

    // Raciones de Grasas
    tft.setCursor(916,447);
    tft.setTextForegroundColor(WHITE); 
    tft.print("5"); // 24 escale x2
    // ---- FIN RACIONES ----
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
    tft.drawRoundRect(425,413,590,535,20,RED_BUTTON); // Alrededor de botón
    tft.drawRoundRect(424,412,591,536,20,RED_BUTTON); 
    tft.drawRoundRect(423,411,592,537,20,RED_BUTTON); 
    tft.drawRoundRect(422,410,593,538,20,RED_BUTTON); 
    tft.drawRoundRect(421,409,594,539,20,RED_BUTTON); 
    tft.drawRoundRect(420,408,595,540,20,RED_BUTTON); 
    tft.drawRoundRect(419,407,596,541,20,RED_BUTTON); 
    tft.drawRoundRect(418,406,597,542,20,RED_BUTTON); 
    tft.drawRoundRect(417,405,598,543,20,RED_BUTTON); 
    tft.drawRoundRect(416,404,599,544,20,RED_BUTTON); 
    tft.drawRoundRect(415,403,600,545,20,RED_BUTTON); 
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
    tft.drawRoundRect(425,453,590,575,20,RED_BUTTON); // Alrededor de botón
    tft.drawRoundRect(424,452,591,576,20,RED_BUTTON); 
    tft.drawRoundRect(423,451,592,577,20,RED_BUTTON); 
    tft.drawRoundRect(422,450,593,578,20,RED_BUTTON); 
    tft.drawRoundRect(421,449,594,579,20,RED_BUTTON); 
    tft.drawRoundRect(420,448,595,580,20,RED_BUTTON); 
    tft.drawRoundRect(419,447,596,581,20,RED_BUTTON); 
    tft.drawRoundRect(418,446,597,582,20,RED_BUTTON); 
    tft.drawRoundRect(417,445,598,583,20,RED_BUTTON); 
    tft.drawRoundRect(416,444,599,584,20,RED_BUTTON); 
    tft.drawRoundRect(415,443,600,585,20,RED_BUTTON); 
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








//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------- CARGA DE IMÁGENES EN MEMORIA --------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------



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



/*---------------------------------------------------------------------------------------------------------
   loadPicturesShowHourglass(): Cargar imágenes en memoria SDRAM de la pantalla mientras se muestra
                                un reloj de arena indicando la carga.
----------------------------------------------------------------------------------------------------------*/
void loadPicturesShowHourglass(){
  /*
    ------------------ POSICIONES DE IMAGENES EN LAS PAGINAS ----------------------------------------------------------------------------------------------------
    PAGE 2:
        S -> (0,0)    M -> (96,0)   A -> (201,0)    R -> (306,0)    T -> (392,0)    C -> (497,0)    L -> (583,0)   O -> (669,0)   H  -> (775,0)   Log -> (841,0)
        brain1  ->  (0,170)   brain2G  ->  (121,170)   [[NO[[logFull ->  (242,170)]]NO]]
        reloj1 -> (0,279)   reloj2 -> (66,279)   reloj3 -> (132,279)   reloj4 -> (198,279)   relGir1 -> (264,279)    relGir2 -> (360,279)   relGir3 -> (473,279)    relGir4 -> (587,279)    relGir5 -> (688,279)    relGir6 -> (802,279)
        
    PAGE 3:
        grupo1 -> (0,0)   grupo2 -> (131,0)   grupo3 -> (262,0)   grupo4 -> (393,0)   manoG -> (524,0)    anadir -> (645,0)    borrar -> (818,0)
        guardar -> (0,131)   cociGra -> (173,131)   crudoGra -> (351,131)    cociPeq -> (529,131)   crudoPeq -> (577,131)
                                                                             kcal -> (529,174)   cuadroGris -> (610,174)
        cruz -> (0,292)   aviso -> (115,292)   manoY -> (251,292)   scale -> (372,292)
    --------------------------------------------------------------------------------------------------------------------------------------------------------------
  */

  // EN PRIMER LUGAR SE CARGAN LAS IMÁGENES DEL RELOJ, SEGUIDAS DE LAS LETRAS DEL LOGO DE ARRANQUE. A PARTIR DE AHÍ, LA CARGA DE HA ORDENADO SEGÚN
  // EL PESO, DE MÁS PESADAS A MENOS, PARA QUE DÉ LA SENSACIÓN DE QUE CADA VEZ GIRA MÁS RÁPIDO EL RELOJ.

    // ----------------- CARGA INICIAL -------------------------------------------------------------------
      // -------- RELOJ1, RELOJ2, RELOJ3 Y RELOJ4 -------
      tft.canvasImageStartAddress(PAGE2_START_ADDR);
      tft.clearScreen(BLACK);

      // reloj1
      tft.sdCardDraw16bppBIN256bits(0,279,65,103,fileReloj1); // Cargar reloj1 (65x103) en PAGE2 => x = 0   ->   y = <brain1(170) + brain1(108) + 1 = 279  
      putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1

      // reloj2
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar reloj1 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(66,279,65,103,fileReloj2);   // Cargar reloj2 (65x103) en PAGE2 => <reloj1(0) + reloj1(65) + 1 = 66  -> y = 279
      putReloj2(); // Mostrar reloj2 en PAGE1

      // reloj3
      // No hace falta volver a PAGE2 porque no ha hecho falta activar PAGE1 para borrar reloj1 al escribir reloj2 en PAGE1
      tft.sdCardDraw16bppBIN256bits(132,279,65,103,fileReloj3); // Cargar reloj3 (65x103) en PAGE2 => x = <reloj2(66) + reloj2(65) + 1 = 132   ->   y = 279
      putReloj3(); // Mostrar reloj3 en PAGE1

      // reloj4
      // No hace falta volver a PAGE2 porque no ha hecho falta activar PAGE1 para borrar reloj2 al escribir reloj3 en PAGE1
      tft.sdCardDraw16bppBIN256bits(198,279,65,103,fileReloj4); // Cargar reloj4 (65x103) en PAGE2 => x = <reloj3(132) + reloj3(65) + 1 = 198   ->   y = 279   
      putReloj4(); // Mostrar reloj4 en PAGE1
      // -------------------------------------------------

      // --------- RELOJES GIRADOS 1, 2, 3, 4, 5 y 6 -----------------
      // relGir1
      // No hace falta volver a PAGE2 porque no ha hecho falta activar PAGE1 para borrar reloj3 al escribir reloj4 en PAGE1
      tft.sdCardDraw16bppBIN256bits(264,279,95,115,fileRelGir1);   // Cargar relGir1 (95x115) en PAGE2 => x = <reloj4(198) + reloj4(65) + 1 = 264   ->   y = 279   
      putRelojGirado1(); // Mostrar relGir1 en PAGE1 => No hace falta borrar reloj4 para mostrar relGir1 porque relGir1 ocupa más espacio -> se coloca encima

      // relGir2
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir1 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(360,279,112,112,fileRelGir2); // Cargar relGir2 (112x112) en PAGE2 => x = <relGir1(264) + relGir1(95) + 1 = 360  ->   y = 279   
      putRelojGirado2();  // Mostrar relGir2 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir2

      // relGir3
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir2 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(473,279,113,94,fileRelGir3);   // Cargar relGir3 (113x94) en PAGE2 => x = <relGir2(360) + relGir2(112) + 1 = 473  ->   y = 279 
      putRelojGirado3(); // Mostrar relGir3 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir3

      // relGir4
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir3 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(587,279,100,65,fileRelGir4); // Cargar relGir4 (100x65) en PAGE2 => x = <relGir3(473) + relGir3(113) + 1 = 587     ->   y = 279   
      putRelojGirado4(); // Mostrar relGir4 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir4

      // relGir5
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir4 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(688,279,113,94,fileRelGir5);   // Cargar relGir5 (113x94) en PAGE2 => x = <relGir4(587) + relGir4(100) + 1 = 688    ->   y = 279 
      putRelojGirado5(); // Mostrar relGir5 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir5

      // relGir6
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir5 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(802,279,99,113,fileRelGir6); // Cargar relGir6 (99x113) en PAGE2 => x = <relGir5(688) + relGir5(113) + 1 = 802     ->   y = 279  
      putRelojGirado6(); // Mostrar relGir6 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir6
      // -------------------------------------------------------------
    // ------------- FIN CARGA INICIAL --------------------------------------------------------------------


    // ----------------- ARRANQUE -------------------------------------------------------------------------
      // -------- LETRAS S-M-A-R-T-C-L-O-H -------
      // Letra S
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(0,0,95,159,fileS);      // Cargar S  (95x159) en PAGE2  =>  x  =  <S  =  0   ->    y = 0  
      
      putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1

      // Letra M
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(96,0,104,159,fileM);    // Cargar M  (104x154) en PAGE2 =>  x  =  <M  =  <S(0)    + S(95)   + 1  =  96    ->    y = 0   
      
      putReloj2(); // Mostrar reloj2 en PAGE1 

      // Letra A
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(201,0,104,159,fileA);   // Cargar A  (104x159) en PAGE2 =>  x  =  <A  =  <M(96)   + M(104)  + 1  =  201   ->    y = 0   
      
      putReloj3(); // Mostrar reloj3 en PAGE1 
      
      // Letra R
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(306,0,85,159,fileR);    // Cargar R  (85x159)  en PAGE2 =>  x  =  <R  =  <A(201)  + A(104)  + 1  =  306   ->    y = 0  
     
      putReloj4(); // Mostrar reloj4 en PAGE1 
     
      // Letra T
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(392,0,104,159,fileT);   // Cargar T1 (104x159) en PAGE2 =>  x  =  <T1 =  <R(306)  + R(85)   + 1  =  392   ->    y = 0   
      
      putRelojGirado1(); // Mostrar relGir1 en PAGE1 

      // Letra C
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(497,0,85,159,fileC);    // Cargar C (85x159) en PAGE2 =>  x  =  <C  =  <T1(392) + T1(104) + 1  =  497   ->    y = 0  
      
      putRelojGirado2(); // Mostrar relGir2 en PAGE1 
      
      // Letra L
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(583,0,85,159,fileL);    // Cargar L (85x159) en PAGE2 =>  x  =  <L  =  <C(497)  + C(85)   + 1  =  583   ->    y = 0   
      
      putRelojGirado3(); // Mostrar relGir3 en PAGE1 
      
      // Letra O
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(669,0,85,159,fileO);    // Cargar O (85x159) en PAGE2 =>  x  =  <O  =  <L(583)  + L(85)   + 1  =  669   ->    y = 0  
      
      putRelojGirado4(); // Mostrar relGir4 en PAGE1 
      
      // Letra H
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(755,0,85,159,fileH);    // Cargar H (85x159) en PAGE2 =>  x  =  <H  =  <O(669)  + O(85)   + 1  =  755   ->    y = 0  
      
      putRelojGirado5(); // Mostrar relGir5 en PAGE1 

      // --------- LOGO ---------
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(841,0,162,169,fileLogo); // Cargar log (162x169) en PAGE2 => x  = <log =  <H(755)  + H(85)   + 1  =  841   ->    y = 0  
     
      putRelojGirado6(); // Mostrar relGir6 en PAGE1 
    // -------------- FIN ARRANQUE -----------------------------------------------------------------------


    // ----------------- AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO ----------------------------------------
      // añadir
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.clearScreen(BLACK);
      tft.sdCardDraw16bppBIN256bits(645,0,172,130,fileBotonAnadir);  // Cargar anadir (172x130) en PAGE3 =>  x  =  <manoR(524) + manoR(120) + 1 = 645      ->   y = 0  
      
      putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1
      
      // borrar
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(818,0,172,130,fileBotonEliminar);  // Cargar borrar (172x130) en PAGE3 =>  x  = <anadir(645) + anadir(172) + 1 = 818    ->   y = 0  
      
      putReloj2(); // Mostrar reloj2 en PAGE1 

      // guardar
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(0,131,172,130,fileBotonGuardar); // Cargar guardar (172x130) en PAGE3 =>  x  =   0  ->   y = <borrar(0) + borrar(130) + 1 = 131 
     
      putReloj3(); // Mostrar reloj3 en PAGE1 
     
      // cociGra
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(173,131,177,160,fileCocinadoGrande); // Cargar cociGra (172x130) en PAGE3 =>  x  =  <guardar(0) + guardar(172) + 1 = 173    ->   y = 131  
     
      putReloj4(); // Mostrar reloj4 en PAGE1 
     
      // crudoGra
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(351,131,177,160,fileCrudoGrande); // Cargar crudoGra (172x130) en PAGE3 =>  x  =  <cociGra(131) + cociGra(177) + 1 = 351  ->   y = 131  
     
      putRelojGirado1(); // Mostrar relGir1 en PAGE1 

    // --------- FIN AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO --------------------------------------------


    // -------------- COLOCAR ALIMENTO -------------------------------------------------------------------
      // scale
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(372,292,150,150,fileScale); // Cargar scale (150x150) en PAGE3 => x =  <manoY(251) + manoY(120) + 1 = 372   ->  y = 292

      putRelojGirado2(); // Mostrar relGir2 en PAGE1 
    // ----------- FIN COLOCAR ALIMENTO ------------------------------------------------------------------


    // ----------------- ESCOGER GRUPO -------------------------------------------------------------------
      // -------- GRUPOS  -------
      // grupo1
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Canvas inicia en PAGE3
      tft.sdCardDraw16bppBIN256bits(0,0,130,125,fileGrupo1);     // Cargar grupo1 (130x125) en PAGE3  =>  x = 0   ->   y = 0 
      
      putRelojGirado3(); // Mostrar relGir3 en PAGE1  
      
      // grupo2
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(131,0,130,125,fileGrupo2);   // Cargar grupo2 (130x125) en PAGE3  =>  x = <grupo1(0)   + grupo1(130) + 1 = 131   ->   y = 0 
      
      putRelojGirado4(); // Mostrar relGir4 en PAGE1 
      
      // grupo3
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(262,0,130,125,fileGrupo3);   // Cargar grupo3 (130x125) en PAGE3  =>  x = <grupo2(131) + grupo2(130) + 1 = 262   ->   y = 0 
      
      putRelojGirado5(); // Mostrar relGir5 en PAGE1 
      
      // grupo4
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(393,0,130,125,fileGrupo4);   // Cargar grupo4 (130x125) en PAGE3  =>  x = <grupo3(262) + grupo3(130) + 1 = 393   ->   y = 0 
      
      putRelojGirado6(); // Mostrar relGir6 en PAGE1 

      // mano con fondo verde ==> usada en grupos
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(524,0,120,128,fileManoGreenIcon);    // Cargar manoGppt (120x128) en PAGE3  =>  x  =  <manoG  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,fileManoGreen);    // Cargar manoG (120x129) en PAGE3  =>  x  =  <manoG  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  

      putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1
    // --------------- FIN ESCOGER GRUPO -----------------------------------------------------------------

    
    // --------- ERROR / AVISO ---------------------------------------------------------------------------
      // error
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(0,292,114,127,fileCruz); // Cargar cruz (114x127) en PAGE3 =>  x  =  0  ->   y = <crudoGra(131) + crudoGra(160) + 1 = 292

      putReloj2(); // Mostrar reloj2 en PAGE1

      // aviso
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      // Imagen aviso con fondo naranja oscuro
      //tft.sdCardDraw16bppBIN256bits(115,292,135,113,fileAvisoDarkOrange); // Cargar avisoO (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292
      // Imagen aviso con fondo amarillo
      tft.sdCardDraw16bppBIN256bits(115,292,135,113,fileAvisoYellow); // Cargar avisoY (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292
      
      putReloj3(); // Mostrar reloj3 en PAGE1

      // Mano con fondo blanco: usada en botones add/delete/save
      //      La imagen de la mano no es blanco puro, por eso se puede filtrar el fondo blanco y que se siga viendo
      //      la mano. Así no aparecen los píxeles de color, como sí ocurre con los píxeles amarillos de manoY.
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(251,292,120,128,fileManoWhiteIcon);    // Cargar manoWppt (120x128) en PAGE3  =>  x  =  <avisoY(115) + avisoY(135) + 1 = 251   ->   y = 292
      //tft.sdCardDraw16bppBIN256bits(251,292,120,128,fileManoWhite);    // Cargar manoW (120x128) en PAGE3  =>  x  =  <avisoY(115) + avisoY(135) + 1 = 251   ->   y = 292
      //tft.sdCardDraw16bppBIN256bits(251,292,120,129,fileManoYellow);    // Cargar manoY (120x129) en PAGE3  =>  x  =  <avisoY(115) + avisoY(135) + 1 = 251   ->   y = 292

      putReloj4(); // Mostrar reloj4 en PAGE1

    // --------- FIN ERROR / AVISO -----------------------------------------------------------------------


    // ----------------- PANTALLA INICIAL ----------------------------------------------------------------
      // brain1
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(0,170,120,108,fileBrain1);    // Cargar brain1 (120x108) en PAGE2 => x = 0 ->   y = <Log(0) + Log(169) + 1 = 170  
      
      putRelojGirado1(); // Mostrar relGir1 en PAGE1 
      
      // brain2G
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(121,170,120,108,fileBrain2Green);  // Cargar brain2G (120x108) en PAGE2 => x = <brain1(0)  + brain1(120) + 1 = 121  ->  y = 170  

      putRelojGirado2(); // Mostrar relGir2 en PAGE1 
    // ------------- FIN PANTALLA INICIAL ----------------------------------------------------------------


    // --------- DASHBOARD -------------------------------------------------------------------------------
      // cociPeq
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(529,131,47,42,fileCocinadoPeq); // Cargar cociPeq (47x42) en PAGE3 =>  x  =  <crudoGra(351) + crudoGra(177) + 1 = 529  ->   y = 131  

      putRelojGirado3(); // Mostrar relGir3 en PAGE1 

      // crudoPeq
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(577,131,47,42,fileCrudoPeq); // Cargar crudoPeq (47x42) en PAGE3 =>  x  =  <cociPeq(529) + crudoGra(47) + 1 = 577  ->   y = 131  

      putRelojGirado4(); // Mostrar relGir4 en PAGE1 

      // kcal
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(529,174,60,65,fileKcal); // Cargar kcal (60x65) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174

      putRelojGirado5(); // Mostrar relGir5 en PAGE1 

      // Recuadro azul utilizado para la transparencia de crudo/cocinado en dashboard. Tiene el mismo tamaño que esas imágenes.
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.fillRect(610,174,657,216,GRIS_CUADROS); // Cargar cuadro (47x42) en PAGE3 => x = <kcal(529) + kcal(80) + 1 = 610    ->  y = <kcal = 174
    // --------- FIN DASHBOARD ---------------------------------------------------------------------------


    // Regresamos la dirección de inicio del canvas a la PAGE1 
    tft.canvasImageStartAddress(PAGE1_START_ADDR); 

}

