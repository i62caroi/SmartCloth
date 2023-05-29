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


  -------------- FUENTES INTERNAS -----------------------------------------------------------------
  * El chip RA8876 tiene una fuente interna de ancho fijo disponible 
    en tres tamaños:
      RA8876_FONT_SIZE_16    =>  8x16
      RA8876_FONT_SIZE_24    =>  12x24
      RA8876_FONT_SIZE_32    =>  16x32
    
    Estos tamaños se pueden variar escalando hasta 4 veces la anchura (W) y la altura (H),
    independientemente (p.ej. W x2 y H x3):
      Escala x1 (sin escala) =>  RA8876_TEXT_W_SCALE_X1 | RA8876_TEXT_H_SCALE_X1
      Escala x2              =>  RA8876_TEXT_W_SCALE_X2 | RA8876_TEXT_H_SCALE_X2
      Escala x3              =>  RA8876_TEXT_W_SCALE_X3 | RA8876_TEXT_H_SCALE_X3
      Escala x4              =>  RA8876_TEXT_W_SCALE_X4 | RA8876_TEXT_H_SCALE_X4
  ------------------------------------------------------------------------------------------------

  -------------- ROM DE FUENTES EXTERNAS ---------------------------------------------------------
  * Memoria ROM de fuentes externas GT30L16U2W de Genitop. El número de modelo 
    sigue el siguiente formato: GT-pp-L-hh-c-b-a

    	· pp => tipo de empaquetado (20 = SOT23-6, 21 = SOP-8, 30 = SOP-8-B)
		  · L  => bajo voltaje (3.3V)
		  · hh => altura de la fuente más alta incluida 
		  · c  => set de caracteres más prominente (S = GB2312, M = GB18030, U = Unicode, 
					T = BIG5, F = Foreign Unicode and ISO-8859)
		  · b  => número de bloques de librerías 
		  · a  => character arrangement

	  Por tanto, la GT30L16U2W trabaja a 3.3V, permite una altura máxima de 16 pixeles y 
	  el set de caracteres más prominente es UNICODE, entre otras características.

      
    --- FUENTES QUE EL RA8876 PUEDE USAR DESDE ESTE CHIP -----

      Nota: la columna "Range" solo proporciona el carácter mínimo y máximo admitido. 
      No todos los caracteres dentro del rango indicado estarán necesariamente disponibles.

        ---------------------------------------------------------------------
        |   Note    |  Family	|  Width |  Height |	Encoding  | Range (hex) |
        ---------------------------------------------------------------------
        |	   -     |  FIXED	|    8	   |	  16	 |	 ASCII	  |  20-7F  		|
        |	   -	   |  FIXED	|   15	   |    16	 |	UNICODE	  |     ?			  |
        |	Latin	   |  FIXED	|    8	   |    16	 |	UNICODE	  |  00A0-0217	|
        |	Greek	   |  FIXED	|    8	   |    16	 |	UNICODE	  |  0370-03CF	|
        |	Cyrillic |  FIXED	|    8	   |    16	 |	UNICODE	  |  0400-04F9	|
        |	   -	   |  ARIAL	|  varies  |	  16	 |	ASCII	    |  20-7F	  	|
        |	Latin	   |  ARIAL	|  varies  |    16	 |	UNICODE	  |  00A0-0217	|
        |	Greek	   |  ARIAL	|  varies  |    16	 |	UNICODE	  |  0370-03CF	|
        |	Cyrillic |  ARIAL	|  varies  |    16	 |	UNICODE	  |  0400-04F9	|
        |	Arabian	 |  ARIAL	|  varies  |    16	 |	UNICODE	  |  0600-06F9	|
        |	Arabian  |  ARIAL	|  varies  |    16	 |	  ?		    |  B000-B1F1	|
        |    -     |  TIMES	|  varies  |    16	 |	ASCII	    |  20-7F		  |
        |	Symbol   |    ?	  |    8	   |	  16	 |	GB2312	  |  ACA1-ACDF	|
        ---------------------------------------------------------------------

	  Info obtenida de:
	  - https://github.com/xlatb/ra8876/wiki/Font-ROM-GT30L16U2W
	  - [Datasheet RA8876 14.2.2 GT30L16U2W] https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf
    --------------------------------------------------------------
  ------------------------------------------------------------------------------------------------

  ------------- CONCLUSIÓN USO DE FUENTES INTERNAS/EXTERNAS --------------------------------------
    >> Interna -> Texto de 8x16/12x24/16x32, escalándolo hasta 4 veces.
    >> Externa -> Texto de 8x16/15x16. Habrá que ver si se escala también.
  ------------------------------------------------------------------------------------------------

*/


#include "RA8876_v2.h"

// Screen circuit wiring
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10


RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);
   



void setup() {
  Serial.begin(115200);
  delay(1000);
  while (!Serial);

  Serial.println("\nStarting up...");

  //setting up the screen
  pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
  digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight
   
  if (!tft.init())
  {
    Serial.println("Could not initialize RA8876");
  }
  
  Serial.println("Startup complete...");

  Serial.println("Starting test.");

  //tft.clearScreen(BLUE);

  //testInternalRA8876(); // OK

  //tft.clearScreen(BLUE);
  
   //externalFontTestGT30L24T3Y(); // Fixed, Arial y Times => OK

   //externalFontFixed(); // OK

   //externalFontArial(); // OK

   //externalFontTimes(); // OK

   testMixedFonts();

}

void testMixedFonts()
{
  tft.clearScreen(RED);

  // ----- INTERNAL FIXED 12x24 X3 --------------
  tft.selectInternalFont(RA8876_FONT_SIZE_24);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
  tft.setTextForegroundColor(WHITE); // Si solo estuviera esta línea (no estuviera ignoreTextBackground()), se mostraría el background 
                                    // del caso anterior (Caso 5): RED
  //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
  tft.setCursor(130, 90);
  tft.println("COLOQUE UN RECIPIENTE");
  tft.setCursor(150, tft.getCursorY() + tft.getTextSizeY()-10);
  tft.print("EN LA ZONA DE PESADA"); 
  

/*
  // ------ EXTERNAL ----------------------------
  tft.initExternalFontRom(0, RA8876_FONT_ROM_GT30L16U2W);

  // ------ ARIAL 8x16 X4 -----------------------
  tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(WHITE);
  tft.ignoreTextBackground();
  tft.setTextScale(RA8876_TEXT_W_SCALE_X4, RA8876_TEXT_H_SCALE_X4); 
  tft.setCursor(100, 90);
  tft.println("COLOQUE UN RECIPIENTE");
  tft.setCursor(130, tft.getCursorY() + tft.getTextSizeY());
  tft.print("EN LA ZONA DE PESADA"); 
*/

}


void testInternalRA8876()
{
  // Caso 1 (OK): Tamaño 8x16 sin escala. Color de texto (BLACK). Se indica color background (WHITE) pero se ignora activando
  //              la transparencia (RA8876_TEXT_TRANS_ON) => se muestra color de background original del canvas (BLUE)
  tft.selectInternalFont(RA8876_FONT_SIZE_16); //RA8876_FONT_ENCODING_8859_1
  tft.setTextColor(BLACK,WHITE,RA8876_TEXT_TRANS_ON);
  tft.setCursor(10,0);
  tft.print("Show internal font 8x16"); 
  // Otra forma indicando posición y texto directamente:
  // tft.putString(10,0,"Show internal font 8x16"); 


  // Caso 2 (OK): Tamaño 12x24 sin escala. Color de texto (MAGENTA). Se indica color background (GREEN) pero se ignora activando
  //              la transparencia (RA8876_TEXT_TRANS_ON) => se muestra color de background original del canvas (BLUE)
  tft.selectInternalFont(RA8876_FONT_SIZE_24);
  tft.setTextColor(MAGENTA,GREEN,RA8876_TEXT_TRANS_ON);
  tft.setCursor(10,26);
  tft.print("Show internal font 12x24"); 
  // Otra forma indicando posición y texto directamente:
  // tft.putString(10,26,"Show internal font 12x24  www.buydisplay.com");


  // Caso 3 (OK): Tamaño 16x32 sin escala. Color de texto (RED). Se muestra color background (YELLOW) desactivando la 
  //              transparencia (RA8876_TEXT_TRANS_OFF) 
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  tft.setTextColor(RED,YELLOW,RA8876_TEXT_TRANS_OFF);
  tft.setCursor(10,60);
  tft.print("Show internal font 16x32"); 
  // Otra forma indicando posición y texto directamente:
  // tft.putString(10,60,"Show internal font 16x32 www.buydisplay.com");
  

  // Caso 4 (OK): Tamaño 8x16 con escala 2x2 (16x32). Color de texto (WHITE). Se indica color background (RED) pero se ignora activando
  //              la transparencia (RA8876_TEXT_TRANS_ON) => se muestra color de background original del canvas (BLUE)
  tft.selectInternalFont(RA8876_FONT_SIZE_16);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); // tft.setTextScale(1,1);
  tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_ON);
  tft.putString(10,102,"font enlarge x2"); 
  // Otra forma indicando posición y luego texto:
  // tft.setCursor(10,102);
  // tft.print("font enlarge x2");
  

  // Caso 5 (OK): Tamaño 8x16 con escala 3x3 (24x48). Color de texto (GRAYSCALE23). Se muestra color background (RED) desactivando la 
  //              transparencia (RA8876_TEXT_TRANS_OFF) 
  tft.selectInternalFont(RA8876_FONT_SIZE_16);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); // tft.setTextScale(2,2);
  tft.setTextColor(GRAYSCALE23,RED,RA8876_TEXT_TRANS_OFF);
  tft.putString(10,144,"font enlarge x3"); 
  // Otra forma indicando posición y luego texto:   
  // tft.setCursor(10, 144);
  // tft.println("font enlarge x3");                
  

  // Caso 6 (OK): Tamaño 8x16 con escala 4x4 (32x64). Color de texto (CYAN). Si no se indicara nada más, mostraría el color background
  //              especificado anteriormente en el Caso 5 (RED). Sin embargo, con ignoreTextBackground() se activa la transparencia 
  //              y se muestra el color del canvas (BLUE). 
  tft.selectInternalFont(RA8876_FONT_SIZE_16);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X4, RA8876_TEXT_H_SCALE_X4); // tft.setTextScale(3,3);
  tft.setTextForegroundColor(CYAN); // Si solo estuviera esta línea (no estuviera ignoreTextBackground()), se mostraría el background 
                                    // del caso anterior (Caso 5): RED
  tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
  tft.putString(10,202,"font enlarge x4"); 
  // Otra forma indicando posición y luego texto: 
  // tft.setCursor(10, 202);
  // tft.println("font enlarge x4");


  // Caso 7 (OK): Tamaño 12x24 restaurando la escala a 'sin escala' (restoreScale()). Se mantiene el color de texto indicado previamente 
  //              en el Caso 6 (CYAN). Se muestra el color background especificado (BROWN) desactivando la transparencia (RA8876_TEXT_TRANS_OFF).
  tft.selectInternalFont(RA8876_FONT_SIZE_24);
  tft.setTextBackgroundColor(BROWN);                  // Manteniendo foreground anterior (CYAN), se indica background (BROWN) 
  tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_OFF);  // Desactiva transparencia => mostrar background especificado (BROWN)
  tft.restoreScale();                                 // Restaurar escala a 'sin escala' (1x1 -> 12x24)
  tft.putString(10,300,"font size 24 no enlargement"); 
  // Otra forma indicando posición y luego texto: 
  // tft.setCursor(10, 300);
  // tft.println("font size 24 no enlargement");

}



void externalFontTestGT30L24T3Y()
{
  tft.initExternalFontRom(0, RA8876_FONT_ROM_GT30L16U2W);

  // HEIGHT DEBE SER 16 (se puede escalar). ROM GT30L16U2W SOLO PERMITE ESA ALTURA DE CARACTERES.

  // No admite Bold (RA8876_FONT_FAMILY_FIXED_BOLD) => Datasheet 14.2.2

  // Fixed (normal) ==> UNICODE, ASCII, Latin
  // Arial          ==> ASCII, Latin
  // Times          ==> ASCII        
  // Bold           ==> No admite

  // ------- FIXED --------
  // External Fixed: Tamaño 8x16 sin escala. Color de texto (BLACK). Se ignora el background (BROWN), indicando antes en el Caso 7
  //                 de testInternalRA8876(), Activando la transparencia (ignoreTextBackground()).
  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(BLACK);
  tft.ignoreTextBackground();     // Activa la transparencia para ignorar el color background (BROWN) 
  tft.setCursor(10,0);
  tft.println("External Fixed 8x16.");

  
  // -- ENLARGEMENT ---

  // External Fixed Enlarged: Tamaño 8x16 con escala 2x2 (16x32). Color de texto (WHITE). Se indica color background (RED) pero se ignora 
  //                          activando la transparencia (RA8876_TEXT_TRANS_ON) => se muestra color de background original del canvas (BLUE).
  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); // tft.setTextScale(1,1);
  tft.putString(10,50,"External Fixed 16 enlarge 2x2"); // OKKKKKK




  // ----- ARIAL -----------------------
  // External Arial: ARIAL tamaño 8x16 sin escala. Color de texto (BLACK). Se muestra el color background (RED) indicando en el caso anterior.
  tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(BLACK);
  tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_OFF);
  tft.restoreScale();
  tft.setCursor(10,100);
  tft.println("External Arial 16.");

  // -- ENLARGEMENT ---

  // External Arial Enlarged: ARIAL tamaño 8x16 con escala 2x2 (16x32). Color de texto (DARKPURPLE). Se indica color background (RED) 
  //                          pero se ignora activando la transparencia (RA8876_TEXT_TRANS_ON) => se muestra color de background original 
  //                          del canvas (BLUE).
  tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(GREEN,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
  tft.putString(10,200,"External Arial 16 enlarge 2x2"); // OK




  // ----- TIMES -----------------------
  // External Times: TIMES tamaño 8x16 sin escala. Color de texto (BLACK). Se muestra el color background (RED) indicando en el caso anterior.
  tft.selectExternalFont(RA8876_FONT_FAMILY_TIMES, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(BLACK);
  tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_OFF);
  tft.restoreScale();
  tft.setCursor(10,280);
  tft.println("External Times 16.");

  // -- ENLARGEMENT ---

  // External Times Enlarged: TIMES tamaño 8x16 con escala 2x2 (16x32). Color de texto (DARKPURPLE). Se indica color background (RED) 
  //                          pero se ignora activando la transparencia (RA8876_TEXT_TRANS_ON) => se muestra color de background
  //                          original del canvas (BLUE)
  tft.selectExternalFont(RA8876_FONT_FAMILY_TIMES, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
  tft.putString(10,400,"External Times 16 enlarge 2x2"); // OK


}



void externalFontFixed()
{
  tft.initExternalFontRom(0, RA8876_FONT_ROM_GT30L16U2W);

  // HEIGHT DEBE SER 16 (se puede escalar). ROM GT30L16U2W SOLO PERMITE ESA ALTURA DE CARACTERES.

  // No admite Bold (RA8876_FONT_FAMILY_FIXED_BOLD) => Datasheet 14.2.2

  // Fixed (normal) ==> UNICODE, ASCII, Latin
  // Arial          ==> ASCII, Latin
  // Times          ==> ASCII        
  // Bold           ==> No admite

  // ------- FIXED --------
  // Caso 1 : Tamaño 8x16 sin escala. Color de texto (BLACK). Se ignora el background (BROWN), indicando antes en el Caso 7
  //          de testInternalRA8876(), Activando la transparencia (ignoreTextBackground()).
  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(BLACK);
  tft.ignoreTextBackground();     // Activa la transparencia para ignorar el color background (BROWN) 
  tft.setCursor(10,0);
  tft.println("External Fixed 8x16."); // OK

  // Caso 2 : Tamaño 8x16 aumentando anchura hasta 15x16. Color de texto (BLACK). El color background se ignora (transparencia) 
  //          desde el caso anterior.
  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_UNICODE, RA8876_FONT_FLAG_XLAT_FULLWIDTH);
  tft.setTextForegroundColor(BLACK);
  tft.setCursor(10,26);
  tft.println("External Fixed 15x16."); // OK

  
  // -- ENLARGEMENT ---

  // Caso 3 : Tamaño 8x16 sin escala. Color de texto (BLACK). Se muestra color background (WHITE) desactivando la 
  //              transparencia (RA8876_TEXT_TRANS_OFF) 
  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(BLACK,WHITE,RA8876_TEXT_TRANS_OFF);
  tft.setCursor(10,60);
  tft.print("External Fixed 8x16 with background"); // OK


  // Caso 4 : Tamaño 8x16 con escala 2x2 (16x32). Color de texto (WHITE). Se indica color background (RED) pero se ignora activando
  //              la transparencia (RA8876_TEXT_TRANS_ON) => se muestra color de background original del canvas (BLUE)
  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); // tft.setTextScale(1,1);
  tft.setCursor(10,102);
  tft.putString(10,102,"External Fixed font enlarge 2x2"); // OKKKKKK


  // Caso 5 : Tamaño 8x16 con escala 3x1 (24x16). Color de texto (GRAYSCALE23). Se muestra color background (RED) desactivando la 
  //              transparencia (RA8876_TEXT_TRANS_OFF) 
  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(GRAYSCALE23,RED,RA8876_TEXT_TRANS_OFF);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X1); 
  tft.putString(10,300,"External Fixed font enlarge 3x1"); // OK


}


void externalFontArial()
{
  tft.initExternalFontRom(0, RA8876_FONT_ROM_GT30L16U2W);

  // HEIGHT DEBE SER 16 (se puede escalar). ROM GT30L16U2W SOLO PERMITE ESA ALTURA DE CARACTERES.

  // No admite Bold (RA8876_FONT_FAMILY_FIXED_BOLD) => Datasheet 14.2.2

  // Fixed (normal) ==> UNICODE, ASCII, Latin
  // Arial          ==> ASCII, Latin
  // Times          ==> ASCII        
  // Bold           ==> No admite

  // ----- ARIAL -----------------------
  // Caso 1 : ARIAL tamaño 8x16 sin escala. Color de texto (BLACK). Se muestra el color background (RED) indicando en el caso anterior.
  tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(BLACK);
  tft.setCursor(10,0);
  tft.println("External Arial 16."); // OK

  // Caso 2 : Tamaño 8x16 sin escala. Color de texto (RED). Se muestra color background (YELLOW) desactivando la 
  //              transparencia (RA8876_TEXT_TRANS_OFF) 
  tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(RED,YELLOW,RA8876_TEXT_TRANS_OFF);
  tft.setCursor(10,26);
  tft.println("External Arial 16 with background"); // OK

  // -- ENLARGEMENT ---

  // Caso 3 : ARIAL tamaño 8x16 con escala 2x2 (16x32). Color de texto (DARKPURPLE). Se indica color background (RED) pero se ignora activando
  //              la transparencia (RA8876_TEXT_TRANS_ON) => se muestra color de background original del canvas (BLUE)
  tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(PINK,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
  tft.putString(10,102,"External Arial 16 enlarge 2x2"); // OK


}


void externalFontTimes()
{
  tft.initExternalFontRom(0, RA8876_FONT_ROM_GT30L16U2W);

  // HEIGHT DEBE SER 16 (se puede escalar). ROM GT30L16U2W SOLO PERMITE ESA ALTURA DE CARACTERES.

  // No admite Bold (RA8876_FONT_FAMILY_FIXED_BOLD) => Datasheet 14.2.2

  // Fixed (normal) ==> UNICODE, ASCII, Latin
  // Arial          ==> ASCII, Latin
  // Times          ==> ASCII        
  // Bold           ==> No admite

  // ----- TIMES -----------------------
  // Caso 1 : TIMES tamaño 8x16 sin escala. Color de texto (BLACK). Se muestra el color background (RED) indicando en el caso anterior.
  tft.selectExternalFont(RA8876_FONT_FAMILY_TIMES, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(BLACK);
  tft.setCursor(10,0);
  tft.println("External Times 16."); // OK

  // Caso 2 : TIMES tamaño 8x16 sin escala. Color de texto (RED). Se muestra color background (YELLOW) desactivando la 
  //              transparencia (RA8876_TEXT_TRANS_OFF) 
  tft.selectExternalFont(RA8876_FONT_FAMILY_TIMES, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(RED,YELLOW,RA8876_TEXT_TRANS_OFF);
  tft.setCursor(10,26);
  tft.println("External Times 16 with background"); // OK

  // -- ENLARGEMENT ---

  // Caso 3 : TIMES tamaño 8x16 con escala 2x2 (16x32). Color de texto (DARKPURPLE). Se indica color background (RED) pero se ignora activando
  //              la transparencia (RA8876_TEXT_TRANS_ON) => se muestra color de background original del canvas (BLUE)
  tft.selectExternalFont(RA8876_FONT_FAMILY_TIMES, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(PINK,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
  tft.putString(10,102,"External Times 16 enlarge 2x2"); // OK


}




void loop(){
  

}



/*





void testDecimal(){
   Serial.println("demo display decimals\n");
  tft.canvasImageStartAddress(PAGE1_START_ADDR);
  tft.canvasImageWidth(tft.getWidth());
  tft.setCanvasWindow(0,0,tft.getWidth(),tft.getHeight());
  tft.fillRect(0, 0, 1023, 599, BLUE);

  tft.setTextParameter1(RA8876_FONT_SOURCE_INTERNAL,RA8876_FONT_SIZE_32,RA8876_SELECT_8859_1);//cch
  tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE,RA8876_TEXT_WIDTH_ENLARGEMENT_X1,RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
  tft.setTextColor(WHITE,BLACK);
  
  tft.putDec(10,10,1,2,"n");
  tft.putDec(10,44,2147483647,11,"n");
  tft.putDec(10,78,-12345,10,"n");
  tft.putDec(10,112,-2147483648,11,"n");
  
  tft.putDec(10,146,1,2,"-");
  tft.putDec(10,180,2147483647,11,"-");
  tft.putDec(10,214,-12345,10,"-");
  tft.putDec(10,248,-2147483648,11,"-");
  
  tft.putDec(10,282,1,2,"+");
  tft.putDec(10,316,2147483647,11,"+");
  tft.putDec(10,350,-12345,10,"+");
  tft.putDec(10,384,-2147483648,11,"+");
  
  tft.putDec(10,418,1,2,"0");
  tft.putDec(10,452,2147483647,11,"0");
  tft.putDec(10,486,-12345,10,"0");
  tft.putDec(10,520,-2147483648,11,"0");
  
  delay(3000);
}



void testFloat(){
    //demo display float
  Serial.println("demo display float\n");
  tft.canvasImageStartAddress(PAGE1_START_ADDR);
  tft.canvasImageWidth(tft.getWidth());
  tft.setCanvasWindow(0,0,tft.getWidth(),tft.getHeight());
  tft.fillRect(0, 0, 1023, 599, BLUE);

  tft.setTextParameter1(RA8876_FONT_SOURCE_INTERNAL,RA8876_FONT_SIZE_32,RA8876_SELECT_8859_1);//cch
  tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE,RA8876_TEXT_WIDTH_ENLARGEMENT_X1,RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
  tft.setTextColor(WHITE,BLACK);
 
  tft.putFloat(10,10,1.1,7,1,"n");
  tft.putFloat(10,44,483647.12,11,2,"n");
  tft.putFloat(10,78,-12345.123,11,3,"n");
  tft.putFloat(10,112,-123456.1234,11,4,"n");
  
  tft.putFloat(10,146,1.1234,7,1,"-");
  tft.putFloat(10,180,483647.12,11,2,"-");
  tft.putFloat(10,214,-12345.123,11,3,"-");
  tft.putFloat(10,248,-123456.1234,11,4,"-");
  
  tft.putFloat(10,282,1.1,7,1,"+");
  tft.putFloat(10,316,483647.12,11,2,"+");
  tft.putFloat(10,350,-12345.123,11,3,"+");
  tft.putFloat(10,384,-123456.1234,11,4,"+");
  
  tft.putFloat(10,418,1.1,7,1,"0");
  tft.putFloat(10,452,483647.12,11,2,"0");
  tft.putFloat(10,486,-12345.123,11,3,"0");
  tft.putFloat(10,520,-123456.1234,11,4,"0");
  delay(3000);
  //while(1);
}



void testHex(){
  Serial.println("demo display Hex\n");
  tft.canvasImageStartAddress(PAGE1_START_ADDR);
  tft.canvasImageWidth(tft.getWidth());
  tft.setCanvasWindow(0,0,tft.getWidth(),tft.getHeight());
  tft.fillRect(0, 0, 1023, 599, BLUE);

  tft.setTextParameter1(RA8876_FONT_SOURCE_INTERNAL,RA8876_FONT_SIZE_32,RA8876_SELECT_8859_1);//cch
  tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_CHROMA_KEY_DISABLE,RA8876_TEXT_WIDTH_ENLARGEMENT_X1,RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
  tft.setTextColor(WHITE,BLACK);
  
  tft.putHex(10,10,1,4,"n");
  tft.putHex(10,44,255,6,"n");
  tft.putHex(10,78,0xa7c8,6,"n");
  tft.putHex(10,112,0xdd11ff55,10,"n");
  
  tft.putHex(10,146,1,4,"0");
  tft.putHex(10,180,255,6,"0");
  tft.putHex(10,214,0xa7c8,6,"0");
  tft.putHex(10,248,0xdd11ff55,10,"0");
  
  tft.putHex(10,282,1,4,"#");
  tft.putHex(10,316,255,6,"#");
  tft.putHex(10,350,0xa7c8,6,"#");
  tft.putHex(10,384,0xdd11ff55,10,"#");
  
  tft.putHex(10,418,1,4,"x");
  tft.putHex(10,452,255,6,"x");
  tft.putHex(10,486,0xa7c8,6,"x");
  tft.putHex(10,520,0xdd11ff55,10,"x");
  delay(3000);
  //while(1);
}
*/

