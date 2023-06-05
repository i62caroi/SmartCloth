
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
    	>> Externa -> Texto de 8x16/15x16, escalándolo hasta 4 veces.
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
//#include "icons.h"  // iconos de 'crudo', 'cocinado' y 'smartcloth_icono' como bitmap


/* Screen circuit wiring */
#define RA8876_CS        12 
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10 


#define   MARGEN_IZQ       25
#define   MARGEN_IZQ_ACC   550 //Margen izquierdo del bloque de valores del acumulado de hoy


// Screen object 
RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);

int SCREEN_WIDTH; // X (1024)
int SCREEN_HEIGHT; // Y (600)

String cad;




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    setupScreen();
void    Welcome();                     // Cargar imágenes (loadPicturesShowHourglass()) y mostrar wireframe de Arranque (logo SmartCloth)
void    printEjemplosyGrupo();
void    printCentral();
void    printValoresComida();
void    printValoresTemporales();
void    printValoresDiario();
void    printStateEmpty();
void    printStatePlato();
void    printStateGroups();
void    printStateRaw();
void    printStateCooked();
void    printStateWeighted();
void    printStateAdded();
void    printStateDeleted();
void    printStateSaved();
void    printEventError(String msg);
void    printEmptyObjectError(String msg);
// --- Carga de imágenes ---
void    loadPicturesShowHourglass();   // Cargar imágenes en la SDRAM de la pantalla mientras se muestra un reloje de arena (hourglass)
void    putReloj1();
void    putReloj2();
void    putReloj3();
void    putReloj4();
void    putReloj5();
void    putRelojGirado1();
void    putRelojGirado2();
void    putRelojGirado3();
void    putRelojGirado4();
void    putRelojGirado5();
void    putRelojGirado6();
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
    SCREEN_WIDTH = tft.getWidth(); // X
    SCREEN_HEIGHT = tft.getHeight(); // Y
    Serial.println(F("Screen initialized"));
    delay(200);
}




/***************************************************************************************************/
/*---------------------------- BIENVENIDA A SMARTCLOTH   ------------------------------------------*/
/***************************************************************************************************/
void Welcome(){
    tft.canvasImageStartAddress(PAGE1_START_ADDR); 
    tft.clearScreen(BLACK); 

    // 1 -  Cargar imágenes mientras muestra reloj de arena
    loadPicturesShowHourglass();  


    // 2 - Mostrar Logo SmartCloth
    //tft.canvasImageStartAddress(PAGE1_START_ADDR);  // YA ESTÁ
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
    tft.print(grupoEscogido.Ejemplos_grupo);         // Imprimir texto

    /* ----- GRUPO ----- */
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(MARGEN_IZQ, 120);                    // Posicion inicio texto => tft.getCursorY()
    tft.setTextColor(WHITE);                          // Color texto      
    tft.setTextScale(1);        
    tft.print(grupoEscogido.Nombre_grupo);           // Imprimir texto

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
    tft.print(pesoBascula); tft.print(" g\n");

    // ------------ PLATO  -------------------------
    tft.setCursor(410, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Plato: "); 
    tft.setCursor(tft.getCursorX(), 217); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(platoActual.getPesoPlato()); tft.print(" g\n");

    // ------------ TOTAL  -------------------------
    /* El peso total se actualiza al guardar la comida final */
    tft.setCursor(700, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Total: "); 
    tft.setCursor(tft.getCursorX(), 217); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(diaActual.getPesoDiario()); tft.print(" g\n");
}



/***************************************************************************************************/
/*------------------------------- VALORES NUTRICIONALES   -----------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printValoresComida(): Muestra los valores nutricionales de la comida actual
----------------------------------------------------------------------------------------------------------*/
void printValoresComida(){
    tft.setCursor(MARGEN_IZQ, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextScale(1);
    tft.setTextColor(WHITE);
    tft.print("Comida Actual\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    // ----- CARBOHIDRATOS -----
    tft.setCursor(MARGEN_IZQ, 375);
    tft.print("Carb: "); tft.print(comidaActual.getValoresComida().getCarbValores()); tft.print(" g ("); 
                         tft.print(comidaActual.getValoresComida().getCarbRaciones()); tft.print(" raciones)\n");
    
    // ----- LIPIDOS -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Lip:  "); tft.print(comidaActual.getValoresComida().getLipValores()); tft.print(" g ("); 
                         tft.print(comidaActual.getValoresComida().getLipRaciones()); tft.print(" raciones)\n");
    
    // ----- PROTEÍNAS -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Prot: "); tft.print(comidaActual.getValoresComida().getProtValores()); tft.print(" g ("); 
                         tft.print(comidaActual.getValoresComida().getProtRaciones()); tft.print(" raciones)\n");
    
    // ----- KCAL -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(comidaActual.getValoresComida().getKcalValores()); tft.print(" Kcal");
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

    ValoresNutricionales valAux(carb, lip, prot, kcal);       // Calcula raciones automáticamente a partir de los valores
    
    tft.setCursor(MARGEN_IZQ, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextColor(WHITE);
    tft.print("Comida Actual\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    // ----- CARBOHIDRATOS -----
    tft.setCursor(MARGEN_IZQ, 375);
    tft.print("Carb: "); tft.print(valAux.getCarbValores()); tft.print(" g ("); 
                         tft.print(valAux.getCarbRaciones()); tft.print(" raciones)\n");
    
    // ----- LIPIDOS -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Lip:  "); tft.print(valAux.getLipValores()); tft.print(" g ("); 
                         tft.print(valAux.getLipRaciones()); tft.print(" raciones)\n");
    
    // ----- PROTEINAS -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Prot: "); tft.print(valAux.getProtValores()); tft.print(" g ("); 
                         tft.print(valAux.getProtRaciones()); tft.print(" raciones)\n");
    
    // ----- KCAL -----
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(kcal); tft.print(" Kcal");
}


/*---------------------------------------------------------------------------------------------------------
   printValoresDiario(): Muestra los valores nutricionales del acumulado del día
----------------------------------------------------------------------------------------------------------*/
void printValoresDiario(){    
    tft.setCursor(MARGEN_IZQ_ACC, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextScale(1);
    tft.setTextColor(WHITE);
    tft.print("Acumulado Hoy\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    // ----- CARBOHIDRATOS -----
    tft.setCursor(MARGEN_IZQ_ACC, 375);
    tft.print("Carb: "); tft.print(diaActual.getValoresDiario().getCarbValores()); tft.print(" g ("); 
                         tft.print(diaActual.getValoresDiario().getCarbRaciones()); tft.print(" raciones)\n");
    
    // ----- LIPIDOS -----
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    tft.print("Lip:  "); tft.print(diaActual.getValoresDiario().getLipValores()); tft.print(" g ("); 
                         tft.print(diaActual.getValoresDiario().getLipRaciones()); tft.print(" raciones)\n");
    
    // ----- PROTEINAS -----
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    tft.print("Prot: "); tft.print(diaActual.getValoresDiario().getProtValores()); tft.print(" g ("); 
                         tft.print(diaActual.getValoresDiario().getProtRaciones()); tft.print(" raciones)\n");
    
    // ----- KCAL -----
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(diaActual.getValoresDiario().getKcalValores()); tft.print(" Kcal");
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
  /*
    // ----------- VERSION SIMPLE -------------------------------------------------------------
    tft.clearScreen(0);                                // Limpiar

    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto
    tft.setTextScale(1);        
    cad = "B\xC1""SCULA LIBRE";                        // BÁSCULA LIBRE
    tft.print(cad);                                  // Imprimir texto
    
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CYAN);                                
    tft.setTextScale(1);        
    if(comidaActual.isComidaEmpty()) cad = "Coloque un recipiente sobre la b\xE1""scula";
    else cad = "Coloque un recipiente sobre la b\xE1""scula o guarde la comida";
    tft.print(cad);           
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
    // ------------ FIN VERSION SIMPLE ---------------------------------------------------------
   */

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

    // -------- CIRCULO ---------------
    tft.fillCircle(512,380,65,WHITE); // 65 pixeles de diametro
    delay(300);

    // ------- CUADRADO REDONDEADO ----
    tft.fillRoundRect(447,315,577,445,10,WHITE); // x = 512 +/- 65 = 447   ->   y = 380 +/- 65 = 315
    delay(300);

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
    delay(300);
    // ------------------------------------------------------------------------------------------------------

    // ----------- BRAINS (120x108)  ------------------------------------------------------------------------
    // BRAIN1 (120x108) --> centrar en el cuadrado blanco
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); // Mostrar brain1 en PAGE1
    delay(300);

    // BRAIN2 (120x108) (rojo)
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,121,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); // Mostrar brain2 en PAGE1
    // ------------------------------------------------------------------------------------------------------
}


/*---------------------------------------------------------------------------------------------------------
   printStatePlato(): Información del STATE_Plato
----------------------------------------------------------------------------------------------------------*/
void printStatePlato(){
  /*
    // ----------- VERSION SIMPLE -------------------------------------------------------------                               // Limpiar
    tft.clearArea(0,0,tft.getWidth(),180,0); //(xOrig, yOrig, xDest, yDest, color) ==> Limpiar zona superior de Ejemplos y Grupo
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto
    tft.setTextScale(1);        
    cad = "\xA1""RECIPIENTE COLOCADO\x21"""; // ¡BIENVENIDO/A!
    tft.print(cad);                                  // Imprimir texto
    
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CYAN);                                
    tft.setTextScale(1);        

    cad = "Escoja un grupo de alimentos";
    tft.print(cad);           
    
    // Esto no hace falta si no se borra toda la pantalla, solo la zona superior
    //printCentral();                                    // 2 - Estructura central
    //printValoresComida();                              // 3 - Valores comida actual
    //printValoresDiario();                              // 4 - Valores acumulado hoy
    // ------------ FIN VERSION SIMPLE ---------------------------------------------------------
   */

   // ----- TEXTO (PLATO AÑADIDO) -------------------------------------------------------------------------
    tft.clearScreen(RED);
    // ------ LINEA ---------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------ TEXTO ---------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setCursor(170, 258);
    tft.println("RECIPIENTE COLOCADO");
    // ------ LINEA ---------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // ----------------------------------------------------------------------------------------------------
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
    //tft.putPicture_16bpp(800,95,95,91,crudo);
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
    //tft.putPicture_16bpp(800,95,95,91,cocinado); 
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
    tft.print(cad);                                  // Imprimir texto
    
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CYAN);                                
    tft.setTextScale(1);        
    tft.selectInternalFont(RA8876_FONT_SIZE_16);       
    cad = "Retire el plato para comenzar otro o guarde la comida";
    tft.print(cad);  
        
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
    tft.print(cad);                                  // Imprimir texto
    
    if(pesoARetirar != 0.0){            // Solo se pone a 0.0 al volver a INI. Mientras no se quite el plato
                                        // y no se marque LIBERAR, no se resetea
        tft.setCursor(MARGEN_IZQ, 120);                             
        tft.setTextColor(CYAN);                                
        tft.setTextScale(1);        
        tft.selectInternalFont(RA8876_FONT_SIZE_16);       
        cad = "Retire el plato eliminado para empezarlo de nuevo o guarde la comida";
        tft.print(cad); 
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
    tft.print(cad);                                  // Imprimir texto
    
    if(pesoARetirar != 0.0){            // Solo se pone a 0.0 al volver a INI. Mientras no se quite el plato
                                        // y no se marque LIBERAR, no se resetea
        tft.setCursor(MARGEN_IZQ, 120);                             
        tft.setTextColor(CYAN);                                
        //tft.setTextScale(1);  
        tft.selectInternalFont(RA8876_FONT_SIZE_16);       
        cad = "Retire el plato";
        tft.print(cad); 
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
    tft.print(cad);                                  // Imprimir texto

    //tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    //tft.setTextScale(0);        
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CYAN);                                
    tft.print(msg);           

    //printCentral();                                    // 2 - Estructura central
    //printValoresComida();                              // 3 - Valores comida actual
    //printValoresDiario();                              // 4 - Valores acumulado hoy
}



/*---------------------------------------------------------------------------------------------------------
   printEmptyObjectWarning(): Información de aviso por intentar guardar/borrar plato o comida estando vacíos
          Parámetros:
                msg - String => mensaje mostrado según el estado
----------------------------------------------------------------------------------------------------------*/
void printEmptyObjectWarning(String msg){    
    cad = "\xA1""\xA1""AVISO\x21""\x21""";
    
    //tft.clearScreen(0);                                // Limpiar
    tft.clearArea(0,0,tft.getWidth(),195,0); //(xOrig, yOrig, xDest, yDest,color) ==> Limpiar zona superior de Ejemplos y Grupo
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(350, 50);                            // Posicion inicio texto
    tft.setTextColor(YELLOW);                        // Color texto
    tft.setTextScale(1);        
    tft.print(cad);                                  // Imprimir texto

    //tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    //tft.setTextScale(0);        
    tft.setCursor(MARGEN_IZQ, 120);                             
    //tft.setTextColor(CIAN);                                
    tft.print(msg);           

    //printCentral();                                    // 2 - Estructura central
    //printValoresComida();                              // 3 - Valores comida actual
    //printValoresDiario();                              // 4 - Valores acumulado hoy
}









/***************************************************************************************************/
/*---------------------------- CARGAR IMÁGENES EN SDRAM  ------------------------------------------*/
/***************************************************************************************************/

/*---------------------------------------------------------------------------------------------------------
   loadPicturesShowHourglass(): Cargar imágenes en memoria SDRAM de la pantalla mientras se muestra
                                un reloj de arena indicando la carga.
----------------------------------------------------------------------------------------------------------*/
void loadPicturesShowHourglass(){
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
      // -------- RELOJ1, RELOJ2, RELOJ3 Y RELOJ4 -------
      tft.canvasImageStartAddress(PAGE2_START_ADDR);
      tft.clearScreen(BLACK);

      // reloj1
      tft.sdCardDraw16bppBIN256bits(0,279,65,103,"bin/carga/cuarto/reloj1.bin"); // Cargar reloj1 (65x103) en PAGE2 => x = 0   ->   y = <brain1(170) + brain1(108) + 1 = 279  
      putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1

      // reloj2
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar reloj1 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(66,279,65,103,"bin/carga/cuarto/reloj2.bin");   // Cargar reloj2 (65x103) en PAGE2 => <reloj1(0) + reloj1(65) + 1 = 66  -> y = 279
      putReloj2(); // Mostrar reloj2 en PAGE1

      // reloj3
      // No hace falta volver a PAGE2 porque no ha hecho falta activar PAGE1 para borrar reloj1 al escribir reloj2 en PAGE1
      tft.sdCardDraw16bppBIN256bits(132,279,65,103,"bin/carga/cuarto/reloj3.bin"); // Cargar reloj3 (65x103) en PAGE2 => x = <reloj2(66) + reloj2(65) + 1 = 132   ->   y = 279
      putReloj3(); // Mostrar reloj3 en PAGE1

      // reloj4
      // No hace falta volver a PAGE2 porque no ha hecho falta activar PAGE1 para borrar reloj2 al escribir reloj3 en PAGE1
      tft.sdCardDraw16bppBIN256bits(198,279,65,103,"bin/carga/cuarto/reloj4.bin"); // Cargar reloj4 (65x103) en PAGE2 => x = <reloj3(132) + reloj3(65) + 1 = 198   ->   y = 279   
      putReloj4(); // Mostrar reloj4 en PAGE1
      // -------------------------------------------------

      // --------- RELOJES GIRADOS 1, 2, 3, 4, 5 y 6 -----------------
      // relGir1
      // No hace falta volver a PAGE2 porque no ha hecho falta activar PAGE1 para borrar reloj3 al escribir reloj4 en PAGE1
      tft.sdCardDraw16bppBIN256bits(264,279,95,115,"bin/carga/cuarto/relGir1.bin");   // Cargar relGir1 (95x115) en PAGE2 => x = <reloj4(198) + reloj4(65) + 1 = 264   ->   y = 279   
      putRelojGirado1(); // Mostrar relGir1 en PAGE1 => No hace falta borrar reloj4 para mostrar relGir1 porque relGir1 ocupa más espacio -> se coloca encima

      // relGir2
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir1 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(360,279,112,112,"bin/carga/cuarto/relGir2.bin"); // Cargar relGir2 (112x112) en PAGE2 => x = <relGir1(264) + relGir1(95) + 1 = 360  ->   y = 279   
      putRelojGirado2();  // Mostrar relGir2 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir2

      // relGir3
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir2 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(473,279,113,94,"bin/carga/cuarto/relGir3.bin");   // Cargar relGir3 (113x94) en PAGE2 => x = <relGir2(360) + relGir2(112) + 1 = 473  ->   y = 279 
      putRelojGirado3(); // Mostrar relGir3 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir3

      // relGir4
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir3 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(587,279,100,65,"bin/carga/cuarto/relGir4.bin"); // Cargar relGir4 (100x65) en PAGE2 => x = <relGir3(473) + relGir3(113) + 1 = 587     ->   y = 279   
      putRelojGirado4(); // Mostrar relGir4 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir4

      // relGir5
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir4 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(688,279,113,94,"bin/carga/cuarto/relGir5.bin");   // Cargar relGir5 (113x94) en PAGE2 => x = <relGir4(587) + relGir4(100) + 1 = 688    ->   y = 279 
      putRelojGirado5(); // Mostrar relGir5 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir5

      // relGir6
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2 porque al mostrar relGir5 se activa PAGE1 para borrarla
      tft.sdCardDraw16bppBIN256bits(802,279,99,113,"bin/carga/cuarto/relGir6.bin"); // Cargar relGir6 (99x113) en PAGE2 => x = <relGir5(688) + relGir5(113) + 1 = 802     ->   y = 279  
      putRelojGirado6(); // Mostrar relGir6 en PAGE1 => Se borra la PAGE1 entera (más fácil que solo un área) antes de escribir relGir6
      // -------------------------------------------------------------
    // ------------- FIN CARGA INICIAL --------------------------------------------------------------------


    // ----------------- ARRANQUE -------------------------------------------------------------------------
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

      putReloj3(); // Mostrar reloj3 en PAGE1

      // crudoPeq
      tft.sdCardDraw16bppBIN256bits(577,131,47,42,"bin/dash/crudoPeq.bin"); // Cargar crudoPeq (47x42) en PAGE3 =>  x  =  <cociPeq(529) + crudoGra(47) + 1 = 577  ->   y = 131  

      putReloj4(); // Mostrar reloj4 en PAGE1

      // kcal
      tft.sdCardDraw16bppBIN256bits(529,174,80,87,"bin/dash/kcal.bin"); // Cargar kcal (80x87) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174
    // --------- FIN DASHBOARD ---------------------------------------------------------------------------

    //delay(200);

    // Regresamos la dirección de inicio del canvas a la PAGE1 
    tft.canvasImageStartAddress(PAGE1_START_ADDR); 

}



/*---------------------------------------------------------------------------------------------------------
   putReloj1(): Borrar PAGE1 en blanco y mostrar reloj1
----------------------------------------------------------------------------------------------------------*/
void putReloj1(){ 
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE); 
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj1 (260x410) en PAGE1 -> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

/*---------------------------------------------------------------------------------------------------------
   putReloj2(): Mostrar reloj2 en PAGE1. No hace falta borrar porque se coloca encima de reloj1
----------------------------------------------------------------------------------------------------------*/
void putReloj2(){ 
  // No necesita limpiar porque ocupa el mismo espacio que reloj1
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,66,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj2 en PAGE1 --> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

/*---------------------------------------------------------------------------------------------------------
   putReloj3(): Mostrar reloj3 en PAGE1. No hace falta borrar porque se coloca encima de reloj2
----------------------------------------------------------------------------------------------------------*/
void putReloj3(){
  // No necesita limpiar porque ocupa el mismo espacio que reloj2
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,132,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj3 en PAGE1 --> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

/*---------------------------------------------------------------------------------------------------------
   putReloj4(): Mostrar reloj4 en PAGE1. No hace falta borrar porque se coloca encima de reloj3
----------------------------------------------------------------------------------------------------------*/
void putReloj4(){
  // No necesita limpiar porque ocupa el mismo espacio que reloj3
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,198,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj4 en PAGE1 --> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

/*---------------------------------------------------------------------------------------------------------
   putRelojGirado1(): Mostrar relGir1 en PAGE1. No hace falta borrar porque se coloca encima de reloj4
----------------------------------------------------------------------------------------------------------*/
void putRelojGirado1(){
  // No necesita limpiar porque ocupa más espacio que reloj4
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,264,279,PAGE1_START_ADDR,SCREEN_WIDTH,465,243,95,115); // Mostrar relGir1 en PAGE1 --> x = 512 +/- 47 = 465     y = 300 +/- 57 = 243
}

/*---------------------------------------------------------------------------------------------------------
   putRelojGirado2(): Borrar PAGE1 y mostrar relGir2
----------------------------------------------------------------------------------------------------------*/
void putRelojGirado2(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,360,279,PAGE1_START_ADDR,SCREEN_WIDTH,456,244,112,112); // Mostrar relGir2 --> x = 512 +/- 56 = 456     y = 300 +/- 56 = 244
}

/*---------------------------------------------------------------------------------------------------------
   putRelojGirado3(): Borrar PAGE1 y mostrar relGir3
----------------------------------------------------------------------------------------------------------*/
void putRelojGirado3(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,473,279,PAGE1_START_ADDR,SCREEN_WIDTH,456,253,113,94); // Mostrar relGir3 en PAGE1 --> x = 512 +/- 56 = 456     y = 300 +/- 47 = 253
}

/*---------------------------------------------------------------------------------------------------------
   putRelojGirado4(): Borrar PAGE1 y mostrar relGir4
----------------------------------------------------------------------------------------------------------*/
void putRelojGirado4(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,587,279,PAGE1_START_ADDR,SCREEN_WIDTH,462,268,100,65); // Mostrar relGir4 en PAGE1 --> x = 512 +/- 50 = 462     y = 300 +/- 32 = 268
}

/*---------------------------------------------------------------------------------------------------------
   putRelojGirado5(): Borrar PAGE1 y mostrar relGir5
----------------------------------------------------------------------------------------------------------*/
void putRelojGirado5(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,688,279,PAGE1_START_ADDR,SCREEN_WIDTH,456,253,113,94); // Mostrar relGir5 en PAGE1 --> x = 512 +/- 56 = 456   ->  y = 300 +/- 47 = 253
}

/*---------------------------------------------------------------------------------------------------------
   putRelojGirado6(): Borrar PAGE1 y mostrar relGir6
----------------------------------------------------------------------------------------------------------*/
void putRelojGirado6(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,802,279,PAGE1_START_ADDR,SCREEN_WIDTH,463,244,99,113); // Mostrar relGir6 en PAGE1 --> x = 512 +/- 49 = 463  ->  y = 300 +/- 56 = 244
}




#endif
