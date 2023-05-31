
/* 
 
  Modelo pantalla: ER-TFTM070-6 de BuyDisplay [1] (SPI 7"TFT LCD Dislay 1024x600 OPTL Capacitive Touch Screen)

	------ ESPECIFICACIONES BASE --------------
	* 1024x600 (7") TFT LCD
	* Pantalla táctil capacitiva
	* Comunicación SPI
	-------------------------------------------

	------ FLASH ------------------------------
	* No tiene memoria Flash externa!!!
	-------------------------------------------

  ------ CHIP GRÁFICO ---------------------------------------------------------------------------
  * Chip gráfico RA8876L4N. Este es una verión más reciente y mejorada del controlador RA8876 [2].  
    Algunas de las mejoras incluyen:

  		1 - Mejor rendimiento: El RA8876L4N puede ofrecer una mayor velocidad de actualización de 
  			  la pantalla y un rendimiento mejorado en términos de respuesta y procesamiento de gráficos.

		  2 - Mayor resolución: El RA8876L4N es compatible con una resolución de pantalla más alta, 
			    lo que permite una mayor densidad de píxeles y una calidad de imagen mejorada.

		  3 - Funcionalidad adicional: El RA8876L4N puede ofrecer nuevas características y funcionalidades, 
			    como la capacidad de soportar más colores, una mayor cantidad de capas gráficas superpuestas y 
			    mejoras en la administración de energía.
  -----------------------------------------------------------------------------------------------

  -------- MEMORIA SDRAM  -----------------------------------------------------------------------
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
      
      En la ra8876SdramInitial() de RA8876_Lite no aparece esta SDRAM como opción, pero sí la W9812G6JH. La principal 
      diferencia entre la W9812G6JH y la W9812G6KH-6 es la velocidad de acceso y el tiempo de ciclo de reloj, siendo 
      la W9812G6JH más rápida en esos términos.
  ------------------------------------------------------------------------------------------------

  -------------- FUENTES INTERNAS -----------------------------------------------------------------
  * El chip RA8876 tiene una fuente interna de ancho fijo disponible en tres tamaños:
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

  -------- MEMORIA ROM FUENTES EXTERNAS  ----------------------------------------------------------------
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

      --- FUENTES QUE EL RA8876 PUEDE USAR DESDE ESTE CHIP [4] (Datasheet RA8876 14.2.2 GT30L16U2W) -----

      Nota: la columna "Range" solo proporciona el carácter mínimo y máximo admitido. 
      No todos los caracteres dentro del rango indicado estarán necesariamente disponibles.

        -----------------------------------------------------------------------
        |   Note    |  Family	|   Width  |  Height |	Encoding  | Range (hex) |
        -----------------------------------------------------------------------
        |	    -     |  FIXED	|    8	   |	  16	 |	 ASCII	  |  20-7F  	  |
        |	    -	    |  FIXED	|   15	   |    16	 |	UNICODE	  |     ?			  |
        |	Latin	    |  FIXED	|    8	   |    16	 |	UNICODE	  |  00A0-0217  |
        |	Greek	    |  FIXED	|    8	   |    16	 |	UNICODE	  |  0370-03CF	|
        |	Cyrillic  |  FIXED	|    8	   |    16	 |	UNICODE	  |  0400-04F9	|
        |	    -	    |  ARIAL	|  varies  |	  16	 |	ASCII	    |  20-7F	  	|
        |	Latin	    |  ARIAL	|  varies  |    16	 |	UNICODE	  |  00A0-0217	|
        |	Greek	    |  ARIAL	|  varies  |    16	 |	UNICODE	  |  0370-03CF	|
        |	Cyrillic  |  ARIAL	|  varies  |    16	 |	UNICODE	  |  0400-04F9	|
        |	Arabian	  |  ARIAL	|  varies  |    16	 |	UNICODE	  |  0600-06F9	|
        |	Arabian   |  ARIAL	|  varies  |    16	 |	  ?		    |  B000-B1F1	|
        |     -     |  TIMES	|  varies  |    16	 |	ASCII	    |  20-7F		  |
        |	Symbol    |    ?	  |    8	   |	  16	 |	GB2312	  |  ACA1-ACDF	|
        -----------------------------------------------------------------------

  ------------------------------------------------------------------------------------------------------

  ------------- CONCLUSIÓN USO DE FUENTES INTERNAS/EXTERNAS --------------------------------------
    	>> Interna -> Texto de 8x16/12x24/16x32, escalándolo hasta 4 veces.
    	>> Externa -> Texto de 8x16/15x16. Habrá que ver si se escala también.
  ------------------------------------------------------------------------------------------------


  [1] Pantalla ER-TFTM070-6: https://www.buydisplay.com/spi-7-inch-tft-lcd-dislay-module-1024x600-ra8876-optl-touch-screen-panel

  [2] Chip RA8876: https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf
  
  [3] SDRAM W9812G6KH-6: https://www.mouser.es/datasheet/2/949/w9812g6kh_a05_20170222-1489666.pdf

  [4] Fuentes ROM GT30L16U2W: https://github.com/xlatb/ra8876/wiki/Font-ROM-GT30L16U2W

  



  ------------------------------------------------------------------------------------------------------------
  --------- CONEXIÓN TFT/LCD CON ARDUINO DUE -----------------------------------------------------------------

  Alimentación y referencia a tierra de la TFT: 
    (pines de conexión en el JP1 de la TFT)
                      ---------------------------------
                      |           TFT (JP1)           |
      -------------------------------------------------
      |  VDD (3.3 V)  |  pines 3, 4, 37 y 38          |   (rojo)
      |  GND          |  pines 1, 2, 13, 31, 39 y 40  |   (negro)
      -------------------------------------------------
  

  TFT conectada al bus SPI del Arduino de la siguiente forma:
    (pines de conexión en el JP1 de la TFT)
      ---------------------------------------------------------
      |  SPI  |       TFT (JP1)     |       Arduino Due       |
      ---------------------------------------------------------
      |  MOSI |    pin 7 (marron)   |      pin 4 (SPI)        |    
      |  MISO |    pin 6 (naranja)  |      pin 1 (SPI)        |
      |  CLK  |    pin 8 (blanco)   |      pin 3 (SPI)        |
      |  CS   |    pin 5 (morado)   |   pin 12 (digital PWM)  | 
      ---------------------------------------------------------

  Otras conexiones de la TFT al Arduino:
    (pines de conexión en el JP1 de la TFT)
      ----------------------------------------------------------------------
      |              TFT (JP1)                    |      Arduino Due       |
      ----------------------------------------------------------------------
      |  pin 11 (Reset) (2º naranja)              |  pin 11 (digital PWM)  |
      |  pin 14 (Backlight) (2º blanco moraito)   |  pin 10 (digital PWM)  |
      ----------------------------------------------------------------------

----------------------------------------------------------------------------------------------------------------------
*/


#ifndef SCREEN_H
#define SCREEN_H


#include "RA8876_v2.h"
#include "State_Machine.h"
#include "Variables.h"
#include "icons.h"  // iconos de 'crudo', 'cocinado' y 'smartcloth_icono' como bitmap


/* Screen circuit wiring */
#define RA8876_CS        12 
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10 


#define   MARGEN_IZQ       25
#define   MARGEN_IZQ_ACC   550 //Margen izquierdo del bloque de valores del acumulado de hoy


// Screen object 
RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);

int ANCHO; // X (1024)
int ALTO; // Y (600)

String cad;




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void setupScreen();
void Welcome();
void printEjemplosyGrupo();
void printCentral();
void printValoresComida();
void printValoresTemporales();
void printValoresDiario();
void printStateEmpty();
void printStatePlato();
void printStateGroups();
void printStateRaw();
void printStateCooked();
void printStateWeighted();
void printStateAdded();
void printStateDeleted();
void printStateSaved();
void printEventError(String msg);
void printEmptyObjectError(String msg);
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------
   setupScreen(): Inicializar pantalla
----------------------------------------------------------------------------------------------------------*/
void setupScreen(){
    // initialize the bus for a device on pin 10 ==> Extended SPI Library Usage with the Arduino Due
                                                //   (https://docs.arduino.cc/tutorials/due/due-extended-spi)
    //SPI.begin(RA8876_CS);

    pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
    digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight
    if (!tft.init()){
      Serial.println(F("Could not initialize RA8876"));
      while(1);
    }
    ANCHO = tft.getWidth(); // X
    ALTO = tft.getHeight(); // Y
    Serial.println(F("\nScreen startup complete..."));
    delay(200);
}




/***************************************************************************************************/
/*---------------------------- BIENVENIDA A SMARTCLOTH   ------------------------------------------*/
/***************************************************************************************************/
void Welcome(){
    tft.clearScreen(0); //0x0000 --> Negro
    
    // ---- Word en icons.h desde flash Arduino ------
    // MUCHISIMO MÁS RÁPIDO QUE BMP DESDE SD, PERO OCUPA MUCHA MEMORIA
    //tft.putPicture_16bpp(250,150,500,350,smartcloth_icono); 

    // ---- Fichero BMP desde SD -----
    // TERRIBLEMENTE LENTO
    //tft.sdCardDraw24bppBMP(fileBMP, 250, 150);

    // ---- Fichero BIN desde SD -----
    //tft.sdCardDraw16bppBIN(250,150,482,350,fileBIN);        // Lo más rápido desde SD => Se ha mejorado aún más:
    //tft.sdCardDraw16bppBIN64bits(250,150,482,350,fileBIN);  // 2.65 veces más rápido que sdCardDraw16bppBIN()
    tft.sdCardDraw16bppBIN256bits(250,150,482,350,fileBIN);   // 2.96 veces más rápido que sdCardDraw16bppBIN(). 
                                                              // A partir de 256 bits no hay mayor incremento de la velocidad
    
    delay(2000);
}



/***************************************************************************************************/
/*------------------------- EJEMPLOS Y GRUPO DE ALIMENTOS   ---------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printEjemplosyGrupo(): Muestra ejemplos del grupo de alimentos seleccionado
----------------------------------------------------------------------------------------------------------*/
void printEjemplosyGrupo(){    
    //tft.clearScreen(0);           // Limpiar
    
    /* ----- EJEMPLOS ----- */
    tft.setTextScale(1);
    tft.selectInternalFont(RA8876_FONT_SIZE_16);       // Tamaño texto
    tft.setCursor(MARGEN_IZQ, 40);                     // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto                  
    tft.println(grupoEscogido.Ejemplos_grupo);         // Imprimir texto

    /* ----- GRUPO ----- */
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(MARGEN_IZQ, 120);                    // Posicion inicio texto => tft.getCursorY()
    tft.setTextColor(WHITE);                          // Color texto      
    tft.setTextScale(1);        
    tft.println(grupoEscogido.Nombre_grupo);           // Imprimir texto

    /* ----- CRUDO/COCINADO ----- */
    /*tft.setCursor(850, 130); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);                           
    tft.setTextColor(CIAN);      
    tft.print(procesamiento);                          // Predeterminadamente "CRUDO"
    */
    
    
}



/***************************************************************************************************/
/*------------------------------- ESTRUCTURA CENTRAL   --------------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printCentral(): Muestra el peso del alimento, del plato y del total de la comida
----------------------------------------------------------------------------------------------------------*/
void printCentral(){
    // ------------ ALIMENTO  -------------------------
    tft.setCursor(MARGEN_IZQ, 210); //tft.getCursorY()
    tft.selectInternalFont(RA8876_FONT_SIZE_24);  
    tft.setTextScale(1);
    tft.setTextColor(GREEN);
    tft.print("Alimento: "); 
    tft.setCursor(tft.getCursorX(), 217); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(pesoBascula); tft.println(" g\n");

    // ------------ PLATO  -------------------------
    tft.setCursor(410, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Plato: "); 
    tft.setCursor(tft.getCursorX(), 217); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(platoActual.getPesoPlato()); tft.println(" g\n");

    // ------------ TOTAL  -------------------------
    /* El peso total se actualiza al guardar la comida final */
    tft.setCursor(700, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Total: "); 
    tft.setCursor(tft.getCursorX(), 217); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(diaActual.getPesoDiario()); tft.println(" g\n");
}



/***************************************************************************************************/
/*------------------------------- VALORES NUTRICIONALES   -----------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printValoresComida(): Muestra los valores nutricionales de la comida actual
----------------------------------------------------------------------------------------------------------*/
void printValoresComida(){
    float valor;
    int raciones;
    
    tft.setCursor(MARGEN_IZQ, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextScale(1);
    tft.setTextColor(WHITE);
    tft.println("Comida Actual\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    // ----- CARBOHIDRATOS -----
    tft.setCursor(MARGEN_IZQ, 375);
    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
    valor = comidaActual.getValoresComida().getCarbValores();
    raciones = comidaActual.getValoresComida().getCarbRaciones();
    //raciones = round(2.0*(carb/10));
    //raciones = raciones/2;
    tft.print("Carb: "); tft.print(valor); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)\n");
    
    // ----- LIPIDOS -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
    valor = comidaActual.getValoresComida().getLipValores();
    raciones = comidaActual.getValoresComida().getLipRaciones();
    //raciones = round(2.0*(lip/10));
    //raciones = raciones/2;
    tft.print("Lip:  "); tft.print(valor); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)\n");
    
    // ----- PROTEÍNAS -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
    valor = comidaActual.getValoresComida().getProtValores();
    raciones = comidaActual.getValoresComida().getProtRaciones();
    //raciones = round(2.0*(prot/10));
    //raciones = raciones/2;
    tft.print("Prot: "); tft.print(valor); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)\n");
    
    // ----- KCAL -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(comidaActual.getValoresComida().getKcalValores()); tft.println(" Kcal");
}


/*---------------------------------------------------------------------------------------------------------
   printValoresTemporales(): Muestra los valores nutricionales temporales causados por el cambio de peso
----------------------------------------------------------------------------------------------------------*/
void printValoresTemporales(){
    Alimento AlimentoAux(grupoEscogido, pesoBascula);         // Alimento auxiliar usado para mostrar información variable de lo pesado
    
    float carb = AlimentoAux.getValoresAlimento().getCarbValores() + comidaActual.getValoresComida().getCarbValores();
    float lip = AlimentoAux.getValoresAlimento().getLipValores() + comidaActual.getValoresComida().getLipValores();
    float prot = AlimentoAux.getValoresAlimento().getProtValores() + comidaActual.getValoresComida().getProtValores();
    float kcal = AlimentoAux.getValoresAlimento().getKcalValores() + comidaActual.getValoresComida().getKcalValores();

    int raciones;
    
    tft.setCursor(MARGEN_IZQ, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextColor(WHITE);
    tft.println("Comida Actual\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    // ----- CARBOHIDRATOS -----
    tft.setCursor(MARGEN_IZQ, 375);
    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
    raciones = round(2.0*(carb/10));
    raciones = raciones/2;
    tft.print("Carb: "); tft.print(carb); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)\n");
    
    // ----- LIPIDOS -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
    raciones = round(2.0*(lip/10));
    raciones = raciones/2;
    tft.print("Lip:  "); tft.print(lip); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)\n");
    
    // ----- PROTEINAS -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
    raciones = round(2.0*(prot/10)); 
    raciones = raciones/2;
    tft.print("Prot: "); tft.print(prot); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)\n");
    
    // ----- KCAL -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(kcal); tft.println(" Kcal");
}


/*---------------------------------------------------------------------------------------------------------
   printValoresDiario(): Muestra los valores nutricionales del acumulado del día
----------------------------------------------------------------------------------------------------------*/
void printValoresDiario(){
    float valor;
    int raciones;
    
    tft.setCursor(MARGEN_IZQ_ACC, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextScale(1);
    tft.setTextColor(WHITE);
    tft.println("Acumulado Hoy\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    // ----- CARBOHIDRATOS -----
    tft.setCursor(MARGEN_IZQ_ACC, 375);
    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
    valor = diaActual.getValoresDiario().getCarbValores();
    raciones = diaActual.getValoresDiario().getCarbRaciones();
    //raciones = round(2.0*(carb/10));
    //raciones = raciones/2;
    tft.print("Carb: "); tft.print(valor); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)\n");
    
    // ----- LIPIDOS -----
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
    valor = diaActual.getValoresDiario().getLipValores();
    raciones = diaActual.getValoresDiario().getLipRaciones();
    //raciones = round(2.0*(lip/10));
    //raciones = raciones/2;
    tft.print("Lip:  "); tft.print(valor); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)\n");
    
    // ----- PROTEINAS -----
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
    valor = diaActual.getValoresDiario().getProtValores();
    raciones = diaActual.getValoresDiario().getProtRaciones();
    //raciones = round(2.0*(prot/10));
    //raciones = raciones/2;
    tft.print("Prot: "); tft.print(valor); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)\n");
    
    // ----- KCAL -----
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(diaActual.getValoresDiario().getKcalValores()); tft.println(" Kcal");
}


/*********************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------------- INFORMACIÓN POR ESTADO ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*********************************************************************************************************/



/*---------------------------------------------------------------------------------------------------------
   printStateEmpty(): Información del STATE_Empty
----------------------------------------------------------------------------------------------------------*/
void printStateEmpty(){
    //simplePrint("Escoja grupo de alimentos");
    
    tft.clearScreen(0);                                // Limpiar

    // Ya no se necesita el 'BIENVENIDO' porque no es la primera pantalla
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto
    tft.setTextScale(1);        
    cad = "B\xC1""SCULA LIBRE";                        // BÁSCULA LIBRE
    tft.println(cad);                                  // Imprimir texto
    
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CYAN);                                
    tft.setTextScale(1);        
    if(comidaActual.isComidaEmpty()) cad = "Coloque un recipiente sobre la b\xE1""scula";
    else cad = "Coloque un recipiente sobre la b\xE1""scula o guarde la comida";
    tft.println(cad);           
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
}


/*---------------------------------------------------------------------------------------------------------
   printStatePlato(): Información del STATE_Plato
----------------------------------------------------------------------------------------------------------*/
void printStatePlato(){
    
   // tft.clearScreen(0);                                // Limpiar
    tft.clearArea(0,0,tft.getWidth(),180,0); //(xOrig, yOrig, xDest, yDest, color) ==> Limpiar zona superior de Ejemplos y Grupo
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto
    tft.setTextScale(1);        
    cad = "\xA1""RECIPIENTE COLOCADO\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CYAN);                                
    tft.setTextScale(1);        
    /*
    // Esto solo sirve si se permite guardar comida desde STATE_Plato,
    // pero no tendría mucho sentido que alguien pusiera un recipiente
    // y luego guardara la comida. Tendría que quitar el recipiente y luego,
    // estando de nuevo en STATE_Empty, guardar la comida.
    if(comidaActual.isComidaEmpty()) cad = "Escoja grupo de alimentos";
    else{
        tft.selectInternalFont(RA8876_FONT_SIZE_16);      
        cad = "Escoja grupo de alimentos o guarde la comida";
    }*/
    cad = "Escoja un grupo de alimentos";
    tft.println(cad);           
    
    // Esto no hace falta si no se borra toda la pantalla, solo la zona superior
    //printCentral();                                    // 2 - Estructura central
    //printValoresComida();                              // 3 - Valores comida actual
    //printValoresDiario();                              // 4 - Valores acumulado hoy
}




/*---------------------------------------------------------------------------------------------------------
   printStateGroups(): Incluye grupo de alimentos, ejemplos y valores nutricionales de la comida actual
----------------------------------------------------------------------------------------------------------*/
void printStateGroups(){
    tft.clearScreen(0);           // Limpiar ==> TODO => Ver qué ha cambiado (grupo y valores) y solo borrar esa zona
    printEjemplosyGrupo();        // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    printCentral();               // 2 - Estructura central (peso 0.0)
    printValoresComida();         // 3 - Valores comida actual
    printValoresDiario();         // 4 - Valores acumulado hoy
}

/*---------------------------------------------------------------------------------------------------------
   printStateRaw():
----------------------------------------------------------------------------------------------------------*/
void printStateRaw()
{
    //tft.clearArea(800,100,895,891,0); //(xOrig, yOrig, xDest, yDest,color)
    tft.clearArea(0,0,tft.getWidth(),195,0); //(xOrig, yOrig, xDest, yDest,color) ==> Limpiar zona superior de Ejemplos y Grupo
    printEjemplosyGrupo();        // Grupo (crudo) => Esto solo es para cambiar el nombre del grupo, pues hay algunos que tienen "(crudo)"

    //tft.sdCardDraw24bppBMP(fileRaw, 800, 100);
    tft.putPicture_16bpp(800,95,95,91,crudo);
}

/*---------------------------------------------------------------------------------------------------------
   printStateCooked():
----------------------------------------------------------------------------------------------------------*/
void printStateCooked()
{
    //tft.clearArea(800,100,895,891,0); //(xOrig, yOrig, xDest, yDest,color)
    tft.clearArea(0,0,tft.getWidth(),195,0); //(xOrig, yOrig, xDest, yDest,color) ==> Limpiar zona superior de Ejemplos y Grupo
    printEjemplosyGrupo();        // Grupo (cocinado) => Esto solo es para cambiar el nombre del grupo, pues hay algunos que tienen "(cocinado)"

    //tft.sdCardDraw24bppBMP(fileCooked, 800, 100);
    tft.putPicture_16bpp(800,95,95,91,cocinado); 
}




/*---------------------------------------------------------------------------------------------------------
   printStateWeighted():  Incluye grupo de alimentos, ejemplos, procesamiento (crudo/cocinado)
                          y valores nutricionales de la comida actual junto con lo pesado temporalmente
----------------------------------------------------------------------------------------------------------*/
void printStateWeighted(){
    tft.clearScreen(0);           // Limpiar
    printEjemplosyGrupo();         // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    printCentral();                // 2 - Estructura central
    printValoresTemporales();      // 3 - Valores temporales de comida actual 
    printValoresDiario();          // 4 - Valores acumulado hoy
}




/*---------------------------------------------------------------------------------------------------------
   printStateAdded(): Información del STATE_Added
----------------------------------------------------------------------------------------------------------*/
void printStateAdded(){
    //simplePrint("Plato a\xF1""adido\n\n Retire el plato");
    tft.clearScreen(0);
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto
    tft.setTextScale(1);        
    cad = "\xA1""PLATO GUARDADO\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CYAN);                                
    tft.setTextScale(1);        
    tft.selectInternalFont(RA8876_FONT_SIZE_16);       
    cad = "Retire el plato para comenzar otro o guarde la comida";
    tft.println(cad);  
        
    printCentral();                                   // 2 - Estructura central
    printValoresComida();                             // 3 - Valores comida actual
    printValoresDiario();                             // 4 - Valores acumulado hoy
}





/*---------------------------------------------------------------------------------------------------------
   printStateDeleted(): Información del STATE_Deleted
----------------------------------------------------------------------------------------------------------*/
void printStateDeleted(){
    //simplePrint("Plato eliminado\n\n Retire el plato");

    tft.clearScreen(0);
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto
    tft.setTextScale(1);        
    cad = "\xA1""PLATO ELIMINADO\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    if(pesoARetirar != 0.0){            // Solo se pone a 0.0 al volver a INI. Mientras no se quite el plato
                                        // y no se marque LIBERAR, no se resetea
        tft.setCursor(MARGEN_IZQ, 120);                             
        tft.setTextColor(CYAN);                                
        tft.setTextScale(1);        
        tft.selectInternalFont(RA8876_FONT_SIZE_16);       
        cad = "Retire el plato eliminado para empezarlo de nuevo o guarde la comida";
        tft.println(cad); 
    }

    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
}




/*---------------------------------------------------------------------------------------------------------
   printStateSaved(): Información del STATE_Saved
----------------------------------------------------------------------------------------------------------*/
void printStateSaved(){
    //simplePrint("Comida guardada\n\n Retire el plato");

    tft.clearScreen(0);
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto
    tft.setTextScale(1);        
    cad = "\xA1""COMIDA GUARDADA\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    if(pesoARetirar != 0.0){            // Solo se pone a 0.0 al volver a INI. Mientras no se quite el plato
                                        // y no se marque LIBERAR, no se resetea
        tft.setCursor(MARGEN_IZQ, 120);                             
        tft.setTextColor(CYAN);                                
        //tft.setTextScale(1);  
        tft.selectInternalFont(RA8876_FONT_SIZE_16);       
        cad = "Retire el plato";
        tft.println(cad); 
    }
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
}














/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- ERRORES POR ESTADO ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   printEventError(): Información de error según estado
          Parámetros:
                msg - String => mensaje mostrado según el estado
----------------------------------------------------------------------------------------------------------*/
void printEventError(String msg){    
    cad = "\xA1""ACCI\xD3N INCORRECTA\x21""";
    
    //tft.clearScreen(0);                                // Limpiar
    tft.clearArea(0,0,tft.getWidth(),195,0); //(xOrig, yOrig, xDest, yDest,color) ==> Limpiar zona superior de Ejemplos y Grupo
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(RED);                        // Color texto
    tft.setTextScale(1);        
    tft.println(cad);                                  // Imprimir texto

    //tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    //tft.setTextScale(0);        
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CYAN);                                
    tft.println(msg);           

    //printCentral();                                    // 2 - Estructura central
    //printValoresComida();                              // 3 - Valores comida actual
    //printValoresDiario();                              // 4 - Valores acumulado hoy
}



/*---------------------------------------------------------------------------------------------------------
   printEmptyObjectError(): Información de aviso por intentar guardar/borrar plato o comida estando vacíos
          Parámetros:
                msg - String => mensaje mostrado según el estado
----------------------------------------------------------------------------------------------------------*/
void printEmptyObjectError(String msg){    
    cad = "\xA1""\xA1""AVISO\x21""\x21""";
    
    //tft.clearScreen(0);                                // Limpiar
    tft.clearArea(0,0,tft.getWidth(),195,0); //(xOrig, yOrig, xDest, yDest,color) ==> Limpiar zona superior de Ejemplos y Grupo
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(350, 50);                            // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto
    tft.setTextScale(1);        
    tft.println(cad);                                  // Imprimir texto

    //tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    //tft.setTextScale(0);        
    tft.setCursor(MARGEN_IZQ, 120);                             
    //tft.setTextColor(CIAN);                                
    tft.println(msg);           

    //printCentral();                                    // 2 - Estructura central
    //printValoresComida();                              // 3 - Valores comida actual
    //printValoresDiario();                              // 4 - Valores acumulado hoy
}





#endif
