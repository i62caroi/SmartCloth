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
      RA8876_FONT_SIZE_16 => 8x16
      RA8876_FONT_SIZE_24 => 12x24
      RA8876_FONT_SIZE_32 => 16x32
  ------------------------------------------------------------------------------------------------

  -------------- ROM DE FUENTES EXTERNAS ---------------------------------------------------------
  * Memoria ROM de fuentes externas GT30L16U2W de Genitop. El número de modelo 
    sigue el siguiente formato: GTppLhhcba
        ------------------------------------------------------------------
        | Parte |             Descripción               |  Valor en ROM  |
        ------------------------------------------------------------------
        | GT    | Marca                                 | Genitop        |
        | pp    | Tipo de empaquetado                   | 30 = SOP-8-B   |
        | L     | Bajo voltaje                          | 3.3 V          |
        | hh    | Altura de la fuente más alta incluida | 16 (height)    |
        | c     | Set de caracteres más prominente      | U = Unicode    |
        | b     | Número de bloques de librerías        | 2              | 
        | a     | Character arrangement                 | W              |
        ------------------------------------------------------------------

      
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
        |	   -	   |  ARIAL	|  varía   |	  16	 |	ASCII	    |  20-7F	  	|
        |	Latin	   |  ARIAL	|  varía   |    16	 |	UNICODE	  |  00A0-0217	|
        |	Greek	   |  ARIAL	|  varía   |    16	 |	UNICODE	  |  0370-03CF	|
        |	Cyrillic |  ARIAL	|  varía   |    16	 |	UNICODE	  |  0400-04F9	|
        |	Arabian	 |  ARIAL	|  varía   |    16	 |	UNICODE	  |  0600-06F9	|
        |	Arabian  |  ARIAL	|  varía   |    16	 |	  ?		    |  B000-B1F1	|
        |    -     |  TIMES	|  varía   |    16	 |	ASCII	    |  20-7F		  |
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

  tft.clearScreen(BLUE);

  //testInternalRA8876_Lite(); //YES
  //delay(5000);

  testInternalRA8876();
  delay(5000);

  tft.clearScreen(BLUE);
  
  externalFontTestGT30L24T3Y();
  delay(5000);

/*
  testDecimal();
  delay(5000);

  testFloat();
  delay(5000);

  testHex();
 */
}

/*
void testInternalRA8876_Lite(){

  Serial.println("Show internal font 8x16\n");
  //tft.setTextParameter1(RA8876_FONT_SOURCE_INTERNAL,RA8876_FONT_SIZE_16,RA8876_SELECT_8859_1);//cch
  tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_TRANS_OFF,RA8876_TEXT_WIDTH_ENLARGEMENT_X1,RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
  tft.setTextColor(WHITE,BLACK);
  tft.putString(10,0,"Show internal font 8x16   www.buydisplay.com");
  
  Serial.println("Show internal font 12x24\n");
  //tft.setTextParameter1(RA8876_FONT_SOURCE_INTERNAL,RA8876_FONT_SIZE_24,RA8876_SELECT_8859_1);//cch
  tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_TRANS_OFF,RA8876_TEXT_WIDTH_ENLARGEMENT_X1,RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
  tft.setTextColor(BLUE,MAGENTA);
  tft.putString(10,26,"Show internal font 12x24  www.buydisplay.com");
  
  Serial.println("Show internal font 16x32\n");
  //tft.setTextParameter1(RA8876_FONT_SOURCE_INTERNAL,RA8876_FONT_SIZE_32,RA8876_SELECT_8859_1);//cch
  tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_TRANS_OFF,RA8876_TEXT_WIDTH_ENLARGEMENT_X1,RA8876_TEXT_HEIGHT_ENLARGEMENT_X1);
  tft.setTextColor(RED,YELLOW);
  tft.putString(10,60,"Show internal font 16x32 www.buydisplay.com");
  
  Serial.println("Sfont enlarge x2\n");
  //tft.setTextParameter1(RA8876_FONT_SOURCE_INTERNAL,RA8876_FONT_SIZE_16,RA8876_SELECT_8859_1);//cch
  tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_TRANS_ON,RA8876_TEXT_WIDTH_ENLARGEMENT_X2,RA8876_TEXT_HEIGHT_ENLARGEMENT_X2);
  tft.setTextColor(WHITE,RED);
  tft.putString(10,102,"font enlarge x2");
  
  Serial.println("font enlarge x3\n");
  //tft.setTextParameter1(RA8876_FONT_SOURCE_INTERNAL,RA8876_FONT_SIZE_16,RA8876_SELECT_8859_1);//cch
  tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_TRANS_OFF,RA8876_TEXT_WIDTH_ENLARGEMENT_X3,RA8876_TEXT_HEIGHT_ENLARGEMENT_X3);
  tft.setTextColor(WHITE,RED);
  tft.putString(10,144,"font enlarge x3");
  
  Serial.println("font enlarge x4\n");
  tft.setTextParameter1(RA8876_FONT_SOURCE_INTERNAL,RA8876_FONT_SIZE_16,RA8876_SELECT_8859_1);//cch
  tft.setTextParameter2(RA8876_TEXT_FULL_ALIGN_DISABLE, RA8876_TEXT_TRANS_OFF,RA8876_TEXT_WIDTH_ENLARGEMENT_X4,RA8876_TEXT_HEIGHT_ENLARGEMENT_X4);
  tft.setTextColor(WHITE,LIGHTCYAN);
  tft.putString(10,202,"font enlarge x4");
}
*/

void testInternalRA8876()
{
  // Caso 1: Tamaño 8x16 con escala 1x1. Color de texto (BLACK) manteniendo color de background original del canvas (BLUE)
  tft.selectInternalFont(RA8876_FONT_SIZE_16); //RA8876_FONT_ENCODING_8859_1
  tft.setTextColor(BLACK,WHITE,RA8876_TEXT_TRANS_ON);
  tft.setCursor(10,0);
  tft.print("Show internal font 8x16");
  //tft.setTextForegroundColor(BLACK);
  //tft.putString(10,0,"Show internal font 8x16"); //YES


  // Caso 2: Tamaño 12x24 con escala 1x1. Color de texto (MAGENTA) manteniendo color de background original del canvas (BLUE)
  tft.selectInternalFont(RA8876_FONT_SIZE_24);
  tft.setTextColor(MAGENTA,BLUE,RA8876_TEXT_TRANS_ON);
  tft.setCursor(10,26);
  tft.print("Show internal font 12x24");
  //tft.setTextForegroundColor(MAGENTA);
  //tft.putString(10,26,"Show internal font 12x24  www.buydisplay.com");


  // Caso 3: Tamaño 16x32 con escala 1x1. Texto de color RED y resaltado YELLOW, mostrando el color de background especificado (YELLOW).
  tft.selectInternalFont(RA8876_FONT_SIZE_32);
  tft.setTextColor(RED,YELLOW,RA8876_TEXT_TRANS_OFF);
  tft.setCursor(10,60);
  tft.print("Show internal font 16x32");
  //tft.setTextColor(RED);
  //tft.putString(10,60,"Show internal font 16x32 www.buydisplay.com");
  

  // Caso 4: Tamaño 8x16 con escala 2x2. Texto de color WHITE y resaltado RED. Se ignora el color de background especificado (YELLOW) y se muestra el del canvas (BLUE).
  tft.selectInternalFont(RA8876_FONT_SIZE_16);
  tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); // tft.setTextScale(1,1);
  //tft.setCursor(10,102);
  //tft.print("font enlarge x2");
  //tft.setTextColor(WHITE);
  tft.putString(10,102,"font enlarge x2");
  

  // Caso 5: Tamaño 8x16 con escala 3x3. Color de texto (GRAYSCALE23) manteniendo color de background original del canvas (BLUE)
  tft.selectInternalFont(RA8876_FONT_SIZE_16);
  tft.setTextColor(GRAYSCALE23,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); // tft.setTextScale(2,2);
  //tft.setCursor(10, 144);
  //tft.println("font enlarge x3");
  //tft.setTextForegroundColor(GRAYSCALE23);
  tft.putString(10,144,"font enlarge x3");                     
  

  // Caso 6: Tamaño 8x16 con escala 4x4. Color de texto (CYAN), mostrando el color de background especificado (el último: RED)
  tft.selectInternalFont(RA8876_FONT_SIZE_16);
  tft.setTextForegroundColor(CYAN);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X4, RA8876_TEXT_H_SCALE_X4); // tft.setTextScale(3,3);
  //tft.setCursor(10, 202);
  //tft.println("font enlarge x4");
  tft.putString(10,202,"font enlarge x4");

}



void externalFontTestGT30L24T3Y()
{
  tft.initExternalFontRom(0, RA8876_FONT_ROM_GT30L16U2W);

  // HEIGHT MUST BE 16. ROM GT30L16U2W SOLO PERMITE ESA ALTURA DE CARACTERES.
  // No admite Bold (RA8876_FONT_FAMILY_FIXED_BOLD) => Datasheet 14.2.2

  // Fixed (normal) ==> UNICODE, ASCII, Latin
  // Arial          ==> ASCII, Latin
  // Times          ==> ASCII        
  // Bold           ==> No admite

  // ------- FIXED --------
  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(BLACK);
  tft.println("External Fixed 8x16.");
  showFont();

  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_UNICODE, RA8876_FONT_FLAG_XLAT_FULLWIDTH);
  tft.setTextForegroundColor(BLACK);
  tft.println("External Fixed 15x16.");
  showFont();

  delay(3000);
  tft.clearScreen(BLUE);

      // Enlargement
  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(BLACK,WHITE,RA8876_TEXT_TRANS_OFF);
  tft.setCursor(10,0);
  tft.print("External Fixed 8x16 with background");

  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); // tft.setTextScale(1,1);
  // Debe dar error porque no se soporta un height mayor de 16, aunque no sé si afecta tras escalar.
  tft.putString(10,102,"External Fixed font enlarge 2x2");

  tft.selectExternalFont(RA8876_FONT_FAMILY_FIXED, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(GRAYSCALE23,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X1); 
  tft.putString(10,144,"External Fixed font enlarge 3x1"); 

  delay(3000);
  tft.clearScreen(BLUE);




  // ----- ARIAL -----------------------
  tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(BLACK);
  tft.println("External Arial 16.");
  showFont();

  tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(RED,YELLOW,RA8876_TEXT_TRANS_OFF);
  tft.println("External Arial 16 with background");
  showFont();

  delay(3000);
  tft.clearScreen(BLUE);

        // Enlargement
  tft.selectExternalFont(RA8876_FONT_FAMILY_ARIAL, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(DARKPURPLE,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
  // Debe dar error porque no se soporta un height mayor de 16, aunque no sé si afecta tras escalar.
  tft.putString(10,102,"External Arial 16 enlarge 2x2");

  delay(3000);
  tft.clearScreen(BLUE);




  // ----- TIMES -----------------------
  tft.selectExternalFont(RA8876_FONT_FAMILY_TIMES, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextForegroundColor(BLACK);
  tft.println("External Times 16.");
  showFont();

  tft.selectExternalFont(RA8876_FONT_FAMILY_TIMES, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(RED,YELLOW,RA8876_TEXT_TRANS_OFF);
  tft.println("External Times 16 with background");
  showFont();

  delay(3000);
  tft.clearScreen(BLUE);

        // Enlargement
  tft.selectExternalFont(RA8876_FONT_FAMILY_TIMES, RA8876_FONT_SIZE_16, RA8876_FONT_ENCODING_ASCII);
  tft.setTextColor(DARKPURPLE,RED,RA8876_TEXT_TRANS_ON);
  tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
  // Debe dar error porque no se soporta un height mayor de 16, aunque no sé si afecta tras escalar.
  tft.putString(10,102,"External Times 16 enlarge 2x2");

  delay(3000);
  tft.clearScreen(BLUE);


}




void showFont()
{
  tft.println("ABCDEFGHIJKLMNOPQRSTUVWXYZ !@#$%^&*()");
  tft.println("abcdefghijklmnopqrstuvwxyz 0123456789");
  tft.println("_+-={}[];'\\|:\",./<>?`~");
  tft.println();
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

