/** 
 * @file Screen.h.h
 * @brief Módulo Tarjeta SD
 * 
 * @author Irene Casares Rodríguez
 * @date 16/06/23
 * @version 1.0
 *
 * Modelo pantalla: ER-TFTM070-6 de BuyDisplay [1] (SPI 7"TFT LCD Dislay 1024x600 OPTL Capacitive Touch Screen)
 * @see https://www.buydisplay.com/spi-7-inch-tft-lcd-dislay-module-1024x600-ra8876-optl-touch-screen-panel 
 */

 /*
 
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


#include "RA8876_v2.h" // COLORS.h
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
void    setupScreen();                 // Inicializar pantalla
void    Welcome();                     // Cargar imágenes (loadPicturesShowHourglass()) y mostrar wireframe de Arranque (logo SmartCloth)
// --- DASHBOARD ---
void    printGrupoyEjemplos();               // Zona 1 => Mostrar nombre de grupo escogido y ejemplos
void    printProcesamiento();                // Zona 2 => Mostrar imagen de 'crudo' o 'cocinado' según el procesamiento activo (comienza en crudo) => STATE_raw y STATE_cooked
void    printPlatoActual(bool definitivo);   // Zona 3 => Mostrar valores del "Plato actual"   =>   true: valores reales  ->  false: valores temporales
void    printAcumuladoHoy();                 // Zona 4 => Mostrar valores del "Acumulado hoy"
void    showFullDashboard(bool definitivo);  // Mostrar dashboard completo (zonas 1-4)  =>  STATE_GroupA, STATE_GroupB y STATE_weighted. 'definitivo' para pasarlo a printPlatoActual()
// --- pantallas transitorias ---
void    pedirRecipiente();                // Pedir colocar recipiente          =>  STATE_Empty
void    pedirGrupoAlimentos();            // Pedir escoger grupo de alimentos  =>  STATE_Plato
void    pedirConfirmacion(int option);    // Pregunta de confirmación general  =>  STATE_add_check (option: 1), STATE_delete_check (option: 2) y STATE_save_check (option: 3)
void    showAccionConfirmada(int option); // Mensaje general de confirmación   =>  STATE_added (option: 1), STATE_deleted (option: 2) y STATE_saved (option: 3)
void    showAccionCancelada();            // Mensaje general de acción cancelada  => STATE_add_check, STATE_delete_check y STATE_save_check 
// --- Errores o avisos ---
void    showError(int option);            // Pantalla de error con mensaje según estado (del 1 al 13)
void    showWarning(int option);          // Warning de acción inncesaria => STATE_added (option: 1), STATE_deleted (option: 2) y STATE_saved (option: 3)
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

/*---------------------------------------------------------------------------------------------------------
   Welcome(): Carga las imágenes que se van a usar (loadPicturesShowHourglass) mientras muestra un reloj
              de arena y después muestra el logo de SmartCloth (wireframe de arranque).
----------------------------------------------------------------------------------------------------------*/
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



/*********************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------- ZONAS DEL DASHBOARD --------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*********************************************************************************************************/

/*---------------------------------------------------------------------------------------------------------
   printGrupoyEjemplos(): Zona 1 => Muestra ejemplos del grupo de alimentos seleccionado, así como si está
                                    crudo o cocinado.
----------------------------------------------------------------------------------------------------------*/
void printGrupoyEjemplos(){    
    // ------------ ZONA SUPERIOR --------------------------------------------
    // ---------- GRÁFICOS -------------------
    // Recuadro grupo y ejemplos
    tft.fillRoundRect(30,20,932,135,20,GRIS_CUADROS); // 902 x 115

    // Recuadros procesamiento
    //printProcesamiento(); // imagen de 'crudo' o 'cocinado' según procesamiento activo (comienza en crudo)
    // ---------- FIN GRÁFICOS ---------------


    // -------- TEXTO ------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextForegroundColor(WHITE); 

    // Título
    tft.setCursor(40,30);
    tft.print("Grupo Actual: ");  // 16x32 escale x1

    // Nombre grupo
    tft.setTextForegroundColor(grupoEscogido.color_grupo); // 'color_grupo'' como atributo de struct 'Grupo'
    tft.setCursor(tft.getCursorX(),tft.getCursorY());
    tft.print(grupoEscogido.Nombre_grupo); // Nombre grupo // 16x32 escale x1

    // Ejemplos grupo
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setCursor(40,68);
    tft.print(grupoEscogido.Ejemplos_grupo); // 12x24 escale x1
    // -------- FIN TEXTO --------------------
    // ------------- FIN ZONA SUPERIOR ---------------------------------------
    
}


/*---------------------------------------------------------------------------------------------------------
   printProcesamiento(): Zona 2 => Muestra el símbolo de 'crudo' o 'cocinado' según el procesamiento activo
----------------------------------------------------------------------------------------------------------*/
void printProcesamiento(){ 
    // Recuadro cocinado pequeño 
    tft.fillRoundRect(937,20,994,74,10,GRIS_CUADROS); // 57 x 52

    // Recuadro crudo pequeño
    tft.fillRoundRect(937,79,994,133,10,GRIS_CUADROS); // 57 x 52


    if(procesado){ // COCINADO activo
        // Mostrar cociPeq normal
        tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE1_START_ADDR,SCREEN_WIDTH,942,26,47,42);  // Mostrar cociPeq (47x42) en PAGE1

        // Mostrar crudoPeq con opacidad a nivel 24/32. Utiliza un recuadro de color GRIS_CUADROS escrito en page3 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,577,131,PAGE3_START_ADDR,SCREEN_WIDTH,610,174,PAGE1_START_ADDR,SCREEN_WIDTH,942,85,47,42,RA8876_ALPHA_OPACITY_24);
    }
    else{ // CRUDO activo
        // Mostrar cociPeq con opacidad a nivel 24/32. Utiliza un recuadro de color GRIS_CUADROS escrito en page3 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE3_START_ADDR,SCREEN_WIDTH,610,174,PAGE1_START_ADDR,SCREEN_WIDTH,942,26,47,42,RA8876_ALPHA_OPACITY_24);

        // Mostrar crudoPeq normal
        tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,577,131,PAGE1_START_ADDR,SCREEN_WIDTH,942,85,47,42);  // Mostrar crudoPeq (47x42) en PAGE1
    }
}


/*---------------------------------------------------------------------------------------------------------
   printPlatoActual(): Zona 3 => Muestra los valores nutricionales del "Plato actual"
----------------------------------------------------------------------------------------------------------*/
void printPlatoActual(bool definitivo){ // true --> valores reales    false --> valores temporales
    float raciones;

    // ---------- GRÁFICOS ---------------------------------------------------------------------------------------- 
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
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,175,PAGE1_START_ADDR,SCREEN_WIDTH,127,507,60,64);  // Mostrar kcal_20 (60x65) en PAGE1
    // ---------- FIN GRÁFICOS ------------------------------------------------------------------------------------ 



    // -------- TEXTO --------------------------------------------------------------------------------------------- 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);  // 12x24 escale x2
    tft.setTextForegroundColor(WHITE); 

    // Título
    tft.setCursor(120,155);
    tft.print("Plato actual");  // 12x24 escale x2

    // Peso
    tft.setCursor(50,220); 
    tft.setTextForegroundColor(ROJO_PESO); 
    tft.print("PESO: "); tft.print(pesoBascula,1); tft.print("g"); // 12x24 escale x2 



    // ------ VALORES A MOSTRAR -----------------------------------------------------------------
    // Se mostrarán los valores de la comida actual real o de la temporal con los valores no definitivos del alimento pesado
    ValoresNutricionales valores; // Valores a mostrar
    if(definitivo){ // Valores reales y definitivos de la comida actual
        valores.setValores(comidaActual.getValoresComida());
    }
    else{ // Valores temporales calculados a partir del peso, que puede variar
        Alimento AlimentoAux(grupoEscogido, pesoBascula);         // Alimento auxiliar usado para mostrar información variable de lo pesado
        float carb = AlimentoAux.getValoresAlimento().getCarbValores() + comidaActual.getValoresComida().getCarbValores();
        float lip = AlimentoAux.getValoresAlimento().getLipValores() + comidaActual.getValoresComida().getLipValores();
        float prot = AlimentoAux.getValoresAlimento().getProtValores() + comidaActual.getValoresComida().getProtValores();
        float kcal = AlimentoAux.getValoresAlimento().getKcalValores() + comidaActual.getValoresComida().getKcalValores();
        valores.setValores(carb, lip, prot, kcal); // Calcula raciones automáticamente a partir de los valores
    }
    // -------------------------------------------------------------------------------------------



    // ---- VALORES ------------------------------------------------------------------------------ 
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 

    // ------------ Carbohidratos ------------
    tft.setCursor(50,303);
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.print("CARBOHIDRATOS: "); tft.print(valores.getCarbValores(),1); tft.print("g");  // 16x32 escale x1
    
    // ------------ Proteinas ------------
    tft.setCursor(50,380);
    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.print("PROTE\xCD""NAS: "); tft.print(valores.getProtValores(),1); tft.print("g"); // 16x32 escale x1
    
    // ------------ Grasas ------------
    tft.setCursor(50,457);
    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.print("GRASAS: "); tft.print(valores.getLipValores(),1); tft.print("g"); // 16x32 escale x1
    
    // ------------ Kcal ------------
    tft.setCursor(197,516);
    tft.setTextForegroundColor(ROJO_KCAL); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.print(valores.getKcalValores(),0); tft.print(" Kcal"); // 12x24 escale X2
    // ----- FIN VALORES ------------------------------------------------------------------------- 



    // ---- RACIONES ----------------------------------------------------------------------------- 
    // Texto "Raciones"
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextForegroundColor(WHITE);
    tft.setCursor(370,243);
    tft.print("Raciones"); // 16x32 escale x1


    // Cambiamos el tamaño de texto para los valores de Raciones. A continuación se cambia la escala según el caso.
    tft.selectInternalFont(RA8876_FONT_SIZE_24);


    // ------------ Raciones de Carbohidratos ------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    raciones = valores.getCarbRaciones();
    if (fmod(raciones, 1.0) == 0){ // Termina en .0
        // Cursor según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)) tft.setCursor(426,293); // Una cifra entera
        else tft.setCursor(416,293); // 2 cifras enteras
        tft.print(raciones,0); // Si termina en .0 no lo mostramos. 12x24 escale x2
    }  
    else{ // Termina en .5
        // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){   // Una cifra entera
            tft.setCursor(406,293);
        } 
        else{ // 2 cifras enteras
            tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); // Estrechar
            tft.setCursor(416,293); 
        }          
        tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura

    } 


    // ------------ Raciones de Proteinas ------------ 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    raciones = valores.getProtRaciones();
    if (fmod(raciones, 1.0) == 0){ // Termina en .0
        // Cursor según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)) tft.setCursor(426,370); // Una cifra entera
        else tft.setCursor(416,370); // 2 cifras enteras
        tft.print(raciones,0); // Si termina en .0 no lo mostramos. 12x24 escale x2
    } 
    else{ // Termina en .5
        // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){   // Una cifra entera
            tft.setCursor(406,370);
        } 
        else{ // 2 cifras enteras
            tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); // Estrechar
            tft.setCursor(416,370); 
        } 
        tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura
    } 


    // ---------- Raciones de Grasas ------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    raciones = valores.getLipRaciones();
    if (fmod(raciones, 1.0) == 0){ // Termina en .0
        // Cursor según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)) tft.setCursor(426,447); // Una cifra entera
        else tft.setCursor(416,447); // 2 cifras enteras
        tft.print(raciones,0); // Si termina en .0 no lo mostramos. 12x24 escale x2

    } 
    else{ // Termina en .5
        // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){   // Una cifra entera
            tft.setCursor(406,447);
        } 
        else{ // 2 cifras enteras
            tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); // Estrechar
            tft.setCursor(416,447); 
        }   
        tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura
    } 
    // ---- FIN RACIONES ------------------------------------------------------------------------- 
    // -------- FIN TEXTO ------------------------------------------------------------------------

}


/*---------------------------------------------------------------------------------------------------------
   printAcumuladoHoy(): Zona 4 => Muestra los valores nutricionales del "Acumulado hoy"
----------------------------------------------------------------------------------------------------------*/
void printAcumuladoHoy(){   
    float raciones;

    // ---------- GRÁFICOS --------------------------------------------------------------------------------------
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
    // ---------- FIN GRÁFICOS ----------------------------------------------------------------------------------


    // -------- TEXTO -------------------------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setTextForegroundColor(WHITE); 

    // Título
    tft.setCursor(590, 155);
    tft.print("Acumulado hoy");  // 12x24 escale x2

    // Peso
    tft.setCursor(540,220);
    tft.setTextForegroundColor(ROJO_PESO); 
    tft.print("PESO: "); tft.print(diaActual.getPesoDiario(),1); tft.print("g"); // 12x24 escale x2



    // ---- VALORES ----------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 

    // ------------ Carbohidratos ------------
    tft.setCursor(540,303);
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.print("CARBOHIDRATOS: "); tft.print(diaActual.getValoresDiario().getCarbValores(),1); tft.print("g"); // 16x32 escale x1
    
    // ------------ Proteinas ------------
    tft.setCursor(540,380);
    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.print("PROTE\xCD""NAS: "); tft.print(diaActual.getValoresDiario().getProtValores(),1); tft.print("g"); // 16x32 escale x1
    
    // ------------ Grasas ------------
    tft.setCursor(540,457);
    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.print("GRASAS: "); tft.print(diaActual.getValoresDiario().getLipValores(),1); tft.print("g"); // 16x32 escale x1
    
    // ------------ Kcal ------------
    tft.setCursor(697,516);
    tft.setTextForegroundColor(ROJO_KCAL); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);
    tft.print(diaActual.getValoresDiario().getKcalValores(),0); tft.print(" Kcal"); // 12x24 escale X2
    // ----- FIN VALORES -----------------------------------------------------------------------



    // ---- RACIONES ---------------------------------------------------------------------------
    // Texto "Raciones"
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(860,243);
    tft.print("Raciones"); // 16x32 escale x1


    // Cambiamos el tamaño de texto para los valores de Raciones. A continuación se cambia la escala según el caso.
    tft.selectInternalFont(RA8876_FONT_SIZE_24);


    // ------------ Raciones de Carbohidratos ------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    raciones = diaActual.getValoresDiario().getCarbRaciones();
    if (fmod(raciones, 1.0) == 0){ // Termina en .0
        // Cursor según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)) tft.setCursor(916,293); // Una cifra entera
        else tft.setCursor(906,293); // 2 cifras enteras
        tft.print(raciones,0); // Si termina en .0 no lo mostramos. 12x24 escale x2
    }
    else{ // Termina en .5
        // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){   // Una cifra entera
            tft.setCursor(896,293);
        } 
        else{ // 2 cifras enteras
            tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); // Estrechar
            tft.setCursor(906,293); 
        }          
        tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura
    } 


    // ------------ Raciones de Proteinas ------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    raciones = diaActual.getValoresDiario().getProtRaciones();
    if (fmod(raciones, 1.0) == 0){ // Termina en .0
        // Cursor según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)) tft.setCursor(916,370); // Una cifra entera
        else tft.setCursor(906,370); // 2 cifras enteras
        tft.print(raciones,0); // Si termina en .0 no lo mostramos. 12x24 escale x2
    } 
    else{ // Termina en .5
        // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){   // Una cifra entera
            tft.setCursor(896,370);
        } 
        else{ // 2 cifras enteras
            tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); // Estrechar
            tft.setCursor(906,370); 
        } 
        tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura
    } 


    // ------------ Raciones de Grasas ------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    raciones = diaActual.getValoresDiario().getLipRaciones();
    if (fmod(raciones, 1.0) == 0){ // Termina en .0
        // Cursor según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)) tft.setCursor(916,447); // Una cifra entera
        else tft.setCursor(906,447); // 2 cifras enteras
        tft.print(raciones,0); // Si termina en .0 no lo mostramos. 12x24 escale x2
    } 
    else{ // Termina en .5
        // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){   // Una cifra entera
            tft.setCursor(896,447);
        } 
        else{ // 2 cifras enteras
            tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); // Estrechar
            tft.setCursor(906,447); 
        }   
        tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura
    
    }
    // ---- FIN RACIONES -----------------------------------------------------------------------
    // -------- FIN TEXTO ---------------------------------------------------------------------------------------
}


/*---------------------------------------------------------------------------------------------------------
   showFullDashboard(): Muestra grupo de alimentos y sus ejemplos, valores nutricionales y raciones de la 
                        comida actual ("Plato actual") y del día ("Acumulado hoy")
----------------------------------------------------------------------------------------------------------*/
void showFullDashboard(bool definitivo){
    tft.clearScreen(AZUL_FONDO); // Fondo azul oscuro en PAGE1

    printGrupoyEjemplos();        // Zona 1 - Grupo y ejemplos
    printProcesamiento();         // Zona 2 - Procesamiento crudo o cocinado
    printPlatoActual(definitivo); // Zona 3 - Valores Plato actual ('true' para mostrar valores reales del plato, 'false' para temporales según peso actual)
    printAcumuladoHoy();          // Zona 4 - Valores Acumulado hoy
}









/*********************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------- PANTALLAS DE INFORMACIÓN ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*********************************************************************************************************/

/*---------------------------------------------------------------------------------------------------------
   pedirRecipiente(): Pide colocar un recipiente (STATE_Empty)
----------------------------------------------------------------------------------------------------------*/
void pedirRecipiente(){
    // ----------- TEXTO ------------------------------------------------------------------------------------
    tft.clearScreen(RED); // Fondo rojo en PAGE1

    // ----- INTERNAL FIXED 12x24 X3 --------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(130, 90);
    tft.println("COLOQUE UN RECIPIENTE"); // 12x24 escalado x3
    tft.setCursor(150, tft.getCursorY() + tft.getTextSizeY()-10);
    tft.print("EN LA ZONA DE PESADA"); // 12x24 escalado x3

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
   pedirGrupoAlimentos(): Pide escoger grupo de alimentos, tras haber colocado recipiente (STATE_Plato)
----------------------------------------------------------------------------------------------------------*/
void pedirGrupoAlimentos(){
    // ----- TEXTO (RECIPIENTE COLOCADO) -------------------------------------------------------------------
    tft.clearScreen(RED);
    // ------ LINEA ---------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(170, 258);
    tft.println("RECIPIENTE COLOCADO"); // 12x24 escalado x3. Color texto foreground a WHITE en STATE_Empty
    // ------ LINEA ---------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    delay(1000);

    // ----- TEXTO (INTERNAL FIXED 12x24 X3) --------------------------------------------------------------
    tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(130, 50);
    tft.println("SELECCIONE UN GRUPO"); // 12x24 escalado x3. Color texto foreground a WHITE en STATE_Empty
    tft.setCursor(255, tft.getCursorY() + tft.getTextSizeY()-10);
    tft.print("DE ALIMENTOS"); 
    delay(500);
    // ----------------------------------------------------------------------------------------------------

    // ------------ LINEAS --------------------------------------------------------------------------------
    tft.fillRoundRect(0,250,220,258,3,WHITE);
    tft.fillRoundRect(0,450,512,458,3,WHITE);
    delay(500);
    // ----------------------------------------------------------------------------------------------------

    // ------------ GRUPOS --------------------------------------------------------------------------------
    // 30 pixeles entre cuadro y cuadro
    // Mostrar en PAGE1 (copiar de PAGE3 a PAGE1)

    // ------ Grupo 1 (130x125) ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125); // x = 236  ->  y = 288
    delay(400);

    // ------ Grupo 2 (130x125) ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125); // x = <grupo1(236) + grupo1(130) + 30 = 396  ->  y = 288
    delay(400);

    // ------ Grupo 3 (130x125) ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); // x = <grupo2(396) + grupo2(130) + 30 = 556  ->  y = 288
    delay(400);

    // ------ Grupo 4 (130x125) ---------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125); // x = <grupo3(556) + grupo3(130) + 30 = 716  ->  y = 288
    delay(400);
    // ----------------------------------------------------------------------------------------------------
/*
    // ------------ CUADRADO REDONDEADO (PULSACION) -------------------------------------------------------
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados redondeados, separados por 1 píxel en cada dirección, para simular un grosor mayor.
    tft.drawRoundRect(556,288,680,413,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(555,287,681,414,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(554,286,682,415,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(553,285,683,416,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(552,284,684,417,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(551,283,685,418,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(550,282,686,419,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(549,281,687,420,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(548,280,688,421,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(547,279,689,422,20,RED_BUTTON); // Alrededor de grupo3
    tft.drawRoundRect(546,278,690,423,20,RED_BUTTON); // Alrededor de grupo3
    // ----------------------------------------------------------------------------------------------------
    */

    // ------------ CUADRADO ESQUINADO (PULSACION) --------------------------------------------------------
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados normales, separados por 1 píxel en cada dirección, para simular un grosor mayor.
    tft.drawRect(556,288,680,413,RED_BUTTON); // Alrededor de grupo3
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
    // ----------------------------------------------------------------------------------------------------


    // ------------ MANO (120x129) -----------------------------------------------------------------------------------
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,556,370,120,129,WHITE); // Mostrar handW (120x129) en PAGE1
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,1,PAGE1_START_ADDR,SCREEN_WIDTH,556,370,120,127,RED); // manoR (120x129)
    // ----------------------------------------------------------------------------------------------------
}


/*---------------------------------------------------------------------------------------------------------
   pedirConfirmacion(): Pregunta genérica para confirmar acción (añadir, eliminar o guardar) 
        Parámetros: 
            - option -> 1: botón añadir   2: botón eliminar   3: botón guardar
----------------------------------------------------------------------------------------------------------*/
void pedirConfirmacion(int option){
    // 
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    tft.clearScreen(RED); // Fondo rojo en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 

    if(option == 3) tft.setCursor(30, 20); // Guardar
    else tft.setCursor(30, 30); // Añadir y eliminar
    tft.println("\xBF""EST\xC1"" SEGURO DE QUE QUIERE");

    switch (option){
      case 1: tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-30); tft.print("A\xD1""ADIR UN NUEVO PLATO\x3F"""); break; // BOTÓN AÑADIR
      case 2: tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-30); tft.print("BORRAR EL PLATO ACTUAL\x3F""");     break; // BOTÓN ELIMINAR
      case 3: // BOTÓN GUARDAR
              tft.setCursor(80, tft.getCursorY() + tft.getTextSizeY()-30); 
              tft.print("GUARDAR LA COMIDA ACTUAL\x3F""");
              delay(500);
              // ----- TEXTO (COMEMTARIO) ---------
              tft.selectInternalFont(RA8876_FONT_SIZE_32);
              tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
              tft.setCursor(100, 200);
              tft.println("LOS VALORES NUTRICIONALES PASAR\xC1""N AL ACUMULADO DE HOY");
              // -----------------------------------
              break;
    }

    delay(1000);
    // ----------------------------------------------------------------------------------------------------

    // ------------ LINEA --------------------------------------------------------------------------------
    if(option == 3) tft.fillRoundRect(252,250,764,258,3,WHITE);
    else tft.fillRoundRect(252,220,764,228,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // ----- TEXTO (CONFIRMACIÓN) -------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    if(option == 3) tft.setCursor(150, 300); // Guardar
    else tft.setCursor(150, 270); // Añadir y eliminar
    tft.println("PARA CONFIRMAR, PULSE DE NUEVO");
    tft.setCursor(400, tft.getCursorY() + tft.getTextSizeY()+10);
    tft.print("EL BOT\xD3""N"); 
    // ----------------------------------------------------------------------------------------------------


    // ------------ BOTÓN A PULSAR ------------------------------------------------------------------------
    // Copiar de PAGE3 a PAGE1
    switch (option){
      case 1: tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); break; // Mostrar BOTÓN AÑADIR (172x130) en PAGE1      
      case 2: tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); break; // Mostrar BOTÓN ELIMINAR (172x130) en PAGE1   
      case 3: tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,450,172,130); break; // Mostrar BOTÓN GUARDAR (172x130) en PAGE1  
              
    }
    delay(800);
    // ----------------------------------------------------------------------------------------------------
/*
    // ------------ CUADRADO REDONDEADO (PULSACION) -------------------------------------------------------
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados redondeados, separados por 1 píxel en cada dirección, para simular un grosor mayor.
    if(option == 3){ // Guardar
        tft.drawRoundRect(425,453,590,575,20,RED_BUTTON); // Alrededor de botón guardar
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
    }
    else{ // Añadir y eliminar
        tft.drawRoundRect(425,413,590,535,20,RED_BUTTON); // Alrededor de botón añadir o eliminar
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
    }
    // ----------------------------------------------------------------------------------------------------
    */

    // ------------ CUADRADO ESQUINADO (PULSACION) -------------------------------------------------------
    tft.canvasImageStartAddress(PAGE1_START_ADDR);
    // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
    // cuadrados normales, separados por 1 píxel en cada dirección, para simular un grosor mayor.
    if(option == 3){ // Guardar
        tft.drawRect(425,453,590,575,RED_BUTTON); // Alrededor de botón guardar
        tft.drawRect(424,452,591,576,RED_BUTTON); 
        tft.drawRect(423,451,592,577,RED_BUTTON); 
        tft.drawRect(422,450,593,578,RED_BUTTON); 
        tft.drawRect(421,449,594,579,RED_BUTTON); 
        tft.drawRect(420,448,595,580,RED_BUTTON); 
        tft.drawRect(419,447,596,581,RED_BUTTON); 
        tft.drawRect(418,446,597,582,RED_BUTTON); 
        tft.drawRect(417,445,598,583,RED_BUTTON); 
        tft.drawRect(416,444,599,584,RED_BUTTON); 
        tft.drawRect(415,443,600,585,RED_BUTTON); 
    }
    else{ // Añadir y eliminar
        tft.drawRect(425,413,590,535,RED_BUTTON); // Alrededor de botón añadir o eliminar
        tft.drawRect(424,412,591,536,RED_BUTTON); 
        tft.drawRect(423,411,592,537,RED_BUTTON); 
        tft.drawRect(422,410,593,538,RED_BUTTON); 
        tft.drawRect(421,409,594,539,RED_BUTTON); 
        tft.drawRect(420,408,595,540,RED_BUTTON); 
        tft.drawRect(419,407,596,541,RED_BUTTON); 
        tft.drawRect(418,406,597,542,RED_BUTTON); 
        tft.drawRect(417,405,598,543,RED_BUTTON); 
        tft.drawRect(416,404,599,544,RED_BUTTON); 
        tft.drawRect(415,403,600,545,RED_BUTTON); 
    }
    // ----------------------------------------------------------------------------------------------------


    // ------------ MANO -----------------------------------------------------------------------------------
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0, PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,WHITE); // Mostrar handW (120x129) en PAGE1
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,1,PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,127,RED); // manoR (120x129)
    // ----------------------------------------------------------------------------------------------------

}


/*---------------------------------------------------------------------------------------------------------
   showAccionConfirmada(): Confirmación general de acción (añadir, eliminar o guardar) 
        Parámetros: 
            - option -> 1: botón añadir   2: botón eliminar   3: botón guardar
----------------------------------------------------------------------------------------------------------*/
void showAccionConfirmada(int option){
    tft.clearScreen(RED);

    // ------------ LINEA ---------------------------------------------------------------------------------
    tft.fillRoundRect(252,150,764,158,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (ACCION REALIZADA) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 

    switch (option){
      case 1: tft.setCursor(170, 208); tft.println("NUEVO PLATO A\xD1""ADIDO"); break; // PLATO AÑADIDO
      case 2: tft.setCursor(100, 208); tft.println("PLATO ACTUAL ELIMINADO");   break; // PLATO ELIMINADO
      case 3: tft.setCursor(120, 208); tft.println("COMIDA ACTUAL GUARDADA");   break; // COMIDA GUARDADA
    }
    // ----------------------------------------------------------------------------------------------------
    
    // ------------ LINEA ---------------------------------------------------------------------------------
    tft.fillRoundRect(252,330,764,338,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 

     switch (option){
      case 1: // AÑADIDO
              tft.setCursor(240, 388); tft.println("RETIRE EL PLATO PARA COMENZAR OTRO");
              tft.setCursor(350, tft.getCursorY() + tft.getTextSizeY()+40); tft.print("O GUARDE LA COMIDA");                                 
              break; 

      case 2: // ELIMINADO
              tft.setCursor(160, 388); tft.println("RETIRE EL PLATO ELIMINADO PARA COMENZAR DE NUEVO");            
              tft.setCursor(350, tft.getCursorY() + tft.getTextSizeY()+40); tft.print("O GUARDE LA COMIDA");       
              break; 
              
      case 3: // GUARDADA
              // No se pone if(pesoARetirar ...) porque aún no ha dado tiempo a actualizar 'pesoARetirar' y puede ser incorrecto
              if((pesoRecipiente + pesoPlato) == 0.0){
                  tft.setCursor(190, 388); tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO");
                  tft.setCursor(350, tft.getCursorY() + tft.getTextSizeY()+40); tft.print("AL ACUMULADO DE HOY"); 
              }
              else{ 
                  tft.setCursor(30, 388); tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO AL ACUMULADO DE HOY");  
                  // Puede ser que se quiera guardar desde el STATE_Empty, tras añadir o borrar. Si es así,
                  // la báscula estará vacía (pesoARetirar = 0), por eso el siguiente mensaje solo se mostrará
                  // si se ha querido guardar tras conformar el plato, estando aún en la báscula.
                  tft.setCursor(200,450); tft.println("RETIRE EL PLATO PARA COMENZAR DE NUEVO"); 
              }
              break; 
    }
    // ----------------------------------------------------------------------------------------------------

}


/*---------------------------------------------------------------------------------------------------------
   showAccionCancelada(): Indica que se ha cancelado la acción de añadir, eliminar o guardar.
                          Puede ser indicado por el usuario o por time-out de 10 segundos.
----------------------------------------------------------------------------------------------------------*/
void showAccionCancelada(){
    // ----- TEXTO (ACCION CANCELADA) -----------------------------------
    tft.clearScreen(RED);
    // ------ LINEA ---------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(220, 258);
    tft.println("ACCI\xD3""N CANCELADA"); // 12x24 escalado x3
    // ------ LINEA ---------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // -------------------------------------------------------------------
}





/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- ERRORES POR ESTADO ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------
   showError(): Pantalla genérica de error que indica una acción incorrecta. Según la opción indicada como
                parámetro, dependiente del estado actual, se muestra un mensaje diferente de acción posible.
          Parámetros:
                option - int -> 1: Empty        2: Plato          3: grupoA       4: grupoB   5: Crudo    
                                6: Cocinado     7: Pesado         8: add_check    9: Added    10: delete_check     
                                11: Deleted     12: save_check    13: Saved
----------------------------------------------------------------------------------------------------------*/
void showError(int option){ 
    // cruz
    tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
    tft.sdCardDraw16bppBIN256bits(0,292,114,127,fileCruz); // Cargar cruz (114x127) en PAGE3 =>  x  =  0  ->   y = <crudoGra(131) + crudoGra(160) + 1 = 292

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
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    switch (option){
      case 1: // Empty
              tft.setCursor(160, 420);                                     tft.println("COLOQUE UN RECIPIENTE ANTES DE"); 
              tft.setCursor(180, tft.getCursorY() + tft.getTextSizeY());   tft.print("ESCOGER UN GRUPO DE ALIMENTOS"); 
              break;

      case 2: // Plato
              tft.setCursor(160, 420);                                     tft.println("SELECCIONE GRUPO DE ALIMENTOS"); 
              tft.setCursor(120, tft.getCursorY() + tft.getTextSizeY());   tft.print("ANTES DE ESCOGER COCINADO O CRUDO"); 
              break;

      case 3: // grupoA
              tft.setCursor(190, 420);                                     tft.println("SELECCIONE COCINADO O CRUDO"); 
              tft.setCursor(200, tft.getCursorY() + tft.getTextSizeY());   tft.print("ANTES DE PESAR EL ALIMENTO"); 
              break;

      case 4: // grupoB
              tft.setCursor(190, 420);                                     tft.println("SELECCIONE COCINADO O CRUDO"); 
              tft.setCursor(200, tft.getCursorY() + tft.getTextSizeY());   tft.print("ANTES DE PESAR EL ALIMENTO"); 
              break;

      case 5: // Crudo
              tft.setCursor(100, 450);                                     tft.println("COLOQUE UN ALIMENTO SOBRE LA B\xC1""SCULA");  
              break;

      case 6: // Cocinado
              tft.setCursor(100, 450);                                     tft.println("COLOQUE UN ALIMENTO SOBRE LA B\xC1""SCULA"); 
              break;

      case 7: // Pesado
              tft.setCursor(140, 420);                                     tft.println("ESCOJA GRUPO PARA OTRO ALIMENTO,"); 
              tft.setCursor(100, tft.getCursorY() + tft.getTextSizeY());   tft.print("A\xD1""ADA OTRO PLATO O GUARDE LA COMIDA"); 
              break;

      case 8: // add_check
              tft.setCursor(70, 420);                                     tft.println("PULSE \"A\xD1""ADIR\" DE NUEVO PARA CONFIRMAR"); 
              tft.setCursor(100, tft.getCursorY() + tft.getTextSizeY());   tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              break;
      
      case 9: // Added
              tft.setCursor(50, 450);                                        tft.println("RETIRE EL PLATO PARA COMENZAR UNO NUEVO");  
              break;

      case 10: // delete_check
              tft.setCursor(60, 420);                                     tft.println("PULSE \"BORRAR\" DE NUEVO PARA CONFIRMAR"); 
              tft.setCursor(90, tft.getCursorY() + tft.getTextSizeY());   tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              break;

      case 11: // Deleted
              tft.setCursor(130, 450);                                    tft.println("RETIRE EL PLATO QUE HA ELIMINADO"); 
              break;

      case 12: // save_check
              tft.setCursor(50, 420);                                     tft.println("PULSE \"GUARDAR\" DE NUEVO PARA CONFIRMAR"); 
              tft.setCursor(90, tft.getCursorY() + tft.getTextSizeY());   tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              break;

      case 13: // Saved
              tft.setCursor(155, 450);                                        tft.println("RETIRE EL PLATO PARA CONTINUAR");  
              break;
    }
    // ----------------------------------------------------------------------------------------------------  

}




/*---------------------------------------------------------------------------------------------------------
   showWarning(): Pantalla genérica para avisar de que no se ha realizado la acción porque 
                  no era necesaria. Solo para casos de añadir, eliminar o guardar. 
        Parámetros: 
            option  - int -> 1: añadir   2: eliminar   3: guardar
----------------------------------------------------------------------------------------------------------*/
void showWarning(int option){
    // ----- TEXTO (AVISO) -------------------------------------------------------------------------------
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
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


    // ----- TEXTO (ACCION SIN EFECTO SEGÚN EL CASO) ------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    switch (option){
      case 1: // AÑADIR
              tft.setCursor(150, 410);                                      tft.println("NO SE HA CREADO UN NUEVO PLATO"); 
              tft.setCursor(180, tft.getCursorY() + tft.getTextSizeY());    tft.print("PORQUE EL ACTUAL EST\xC1"" VAC\xCD""O");  
              break;

      case 2: // ELIMINAR
              tft.setCursor(180, 410);                                      tft.println("NO SE HA ELIMINADO EL PLATO"); 
              tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY());    tft.print("PORQUE EST\xC1"" VAC\xCD""O"); 
              break;

      case 3: // BOTÓN GUARDAR
              tft.setCursor(190, 410);                                      tft.println("NO SE HA GUARDADO LA COMIDA"); 
              tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY());    tft.print("PORQUE EST\xC1"" VAC\xCD""A"); 
              break;
    }
    // ----------------------------------------------------------------------------------------------------  


    
}





/*********************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------- CARGA DE IMÁGENES EN SDRAM -------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*********************************************************************************************************/

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


    // ----------------- PANTALLA INICIAL ----------------------------------------------------------------
      // brain1
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(0,170,120,108,fileBrain1);    // Cargar brain1 (120x108) en PAGE2 => x = 0 ->   y = <Log(0) + Log(169) + 1 = 170  
      
      putReloj1(); // Mostrar reloj1 en PAGE1 
      
      // brain2
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(121,170,120,108,fileBrain2);  // Cargar brain2 (120x108) en PAGE2 => x = <brain1(0)  + brain1(120) + 1 = 121  ->  y = 170  
      
      putReloj2(); // Mostrar reloj2 en PAGE1 
    // ------------- FIN PANTALLA INICIAL ----------------------------------------------------------------


    // ----------------- ESCOGER GRUPO -------------------------------------------------------------------
      // -------- GRUPOS  -------
      // grupo1
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Canvas inicia en PAGE3
      tft.sdCardDraw16bppBIN256bits(0,0,130,125,fileGrupo1);     // Cargar grupo1 (130x125) en PAGE3  =>  x = 0   ->   y = 0 
      
      putReloj3(); // Mostrar reloj3 en PAGE1 
      
      // grupo2
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(131,0,130,125,fileGrupo2);   // Cargar grupo2 (130x125) en PAGE3  =>  x = <grupo1(0)   + grupo1(130) + 1 = 131   ->   y = 0 
      
      putReloj4(); // Mostrar reloj4 en PAGE1 
      
      // grupo3
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(262,0,130,125,fileGrupo3);   // Cargar grupo3 (130x125) en PAGE3  =>  x = <grupo2(131) + grupo2(130) + 1 = 262   ->   y = 0 
      
      putRelojGirado1(); // Mostrar relGir1 en PAGE1 
      
      // grupo4
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(393,0,130,125,fileGrupo4);   // Cargar grupo4 (130x125) en PAGE3  =>  x = <grupo3(262) + grupo3(130) + 1 = 393   ->   y = 0 
      
      putRelojGirado2(); // Mostrar relGir2 en PAGE1 

      // -------- MANOS (tb para botones) ---------
      // Esta otra imagen con la mano roja y el fondo blanco se filtra mejor. Aunque queden residuos de
      // blanco en el borde de la figura, no queda mal.
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3

      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,fileHandW);    // Cargar handW (120x129) en PAGE3   =>  x  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,fileManoW);    // Cargar manoW (120x129) en PAGE3   =>  x  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      
      tft.sdCardDraw16bppBIN256bits(524,0,120,129,fileManoR);    // Cargar manoR (120x129) en PAGE3  =>  x  =  <manoR  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      
      putRelojGirado3(); // Mostrar relGir3 en PAGE1 
    // --------------- FIN ESCOGER GRUPO -----------------------------------------------------------------


    // ----------------- AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO ----------------------------------------
      // añadir
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(645,0,172,130,fileBotonAnadir);  // Cargar anadir (172x130) en PAGE3 =>  x  =  <manoR(524) + manoR(120) + 1 = 645      ->   y = 0  
      
      putRelojGirado4(); // Mostrar relGir4 en PAGE1
      
      // borrar
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(818,0,172,130,fileBotonEliminar);  // Cargar borrar (172x130) en PAGE3 =>  x  = <anadir(645) + anadir(172) + 1 = 818    ->   y = 0  
      
      putRelojGirado5(); // Mostrar relGir5 en PAGE1

      // guardar
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(0,131,172,130,fileBotonGuardar); // Cargar guardar (172x130) en PAGE3 =>  x  =   0  ->   y = <borrar(0) + borrar(130) + 1 = 131 
     
      putRelojGirado6(); // Mostrar relGir6 en PAGE1
     
      // cociGra
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(173,131,177,160,fileCocinadoGrande); // Cargar cociGra (172x130) en PAGE3 =>  x  =  <guardar(0) + guardar(172) + 1 = 173    ->   y = 131  
     
      putReloj1(); // Mostrar reloj1 en PAGE1
     
      // crudoGra
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(351,131,177,160,fileCrudoGrande); // Cargar crudoGra (172x130) en PAGE3 =>  x  =  <cociGra(131) + cociGra(177) + 1 = 351  ->   y = 131  
     
      putReloj2(); // Mostrar reloj2 en PAGE1

    // --------- FIN AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO --------------------------------------------


    
    // --------- ERROR / AVISO ---------------------------------------------------------------------------
      // error
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(0,292,114,127,fileCruz); // Cargar cruz (114x127) en PAGE3 =>  x  =  0  ->   y = <crudoGra(131) + crudoGra(160) + 1 = 292

      putReloj3(); // Mostrar reloj3 en PAGE1

      // aviso
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      // Imagen aviso2 con fondo rojo
      //tft.sdCardDraw16bppBIN256bits(115,292,135,113,fileAvisoRed); // Cargar aviso2 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292
      // Imagen aviso2 con fondo naranja oscuro
      tft.sdCardDraw16bppBIN256bits(115,292,135,113,fileAvisoDarkOrange); // Cargar aviso2 (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292

      putReloj4(); // Mostrar reloj4 en PAGE1
    // --------- FIN ERROR / AVISO -----------------------------------------------------------------------


    // --------- DASHBOARD -------------------------------------------------------------------------------
      // cociPeq
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(529,131,47,42,fileCocinadoPeq); // Cargar cociPeq (47x42) en PAGE3 =>  x  =  <crudoGra(351) + crudoGra(177) + 1 = 529  ->   y = 131  

      putRelojGirado1(); // Mostrar relGir1 en PAGE1 

      // crudoPeq
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(577,131,47,42,fileCrudoPeq); // Cargar crudoPeq (47x42) en PAGE3 =>  x  =  <cociPeq(529) + crudoGra(47) + 1 = 577  ->   y = 131  

      putRelojGirado2(); // Mostrar relGir2 en PAGE1 

      // kcal
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      //tft.sdCardDraw16bppBIN256bits(529,174,80,87,fileKcal); // Cargar kcal (80x87) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174
      tft.sdCardDraw16bppBIN256bits(529,174,60,65,fileKcal20); // Cargar kcal_20 (60x65) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174

      putRelojGirado3(); // Mostrar relGir3 en PAGE1 

      // Recuadro azul utilizado para la transparencia de crudo/cocinado
      // Tiene el mismo tamaño que las imágenes para evitar posibles problemas
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.fillRect(610,174,657,216,GRIS_CUADROS); // Cargar cuadro (47x42) en PAGE3 => x = <kcal(529) + kcal(80) + 1 = 610    ->  y = <kcal = 174
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
