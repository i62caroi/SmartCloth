/** 
 * @file Screen.h
 * @brief Módulo Tarjeta SD
 * 
 * @author Irene Casares Rodríguez
 * @date 17/07/23
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
    	>> Interna -> Texto de 8x16/12x24/16x32, escalándolo hasta 4 veces. ==> MEJOR OPCIÓN
    	>> Externa -> Texto de 8x16/15x16, escalándolo hasta 4 veces. ==> SE PIXELA DEMASIADO
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


void    checkBascula();           // Está en Scale.h, pero hay que declararla aquí también para que esté en este ámbito
bool    interruptionOccurred();   // Está en ISR.h, pero hay que declararla aquí también para que esté en este ámbito

#include "ISR.h" 
#include "RA8876_v2.h" // COLORS.h
#include "State_Machine.h"  


/* Screen circuit wiring */
#define RA8876_CS        12 
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10 


#define   MARGEN_IZQ       25
#define   MARGEN_IZQ_ACC   550 //Margen izquierdo del bloque de valores del acumulado de hoy

// ZONA 3 --> copia de la comida o alimento actual
#define   SHOW_COMIDA_ACTUAL_ZONA3    0   // Mostrar la comida actual (copia) en la zona 3
#define   SHOW_ALIMENTO_ACTUAL_ZONA3  1   // Mostrar el alimento actual en la zona 3

// ZONA 4 --> comida actual o acumulado
#define   SHOW_COMIDA_ACTUAL_ZONA4    0   // Mostrar la comida actual (real) en la zona 4
#define   SHOW_ACUMULADO_HOY_ZONA4    1   // Mostrar el acumulado del día en la zona 4

// Ubicación valores según zona
#define   SHOW_VALORES_ZONA3         0   // Ubicacion en zona 3 de los valores en showValores()
#define   SHOW_VALORES_ZONA4         1   // Ubicacion en zona 4 de las valores en showValores()

// Ubicación raciones según zona
#define   SHOW_RACIONES_ZONA3         0   // Ubicacion en zona 3 de las raciones en showRaciones()
#define   SHOW_RACIONES_ZONA4         1   // Ubicacion en zona 4 de las raciones en showRaciones()

// Lenta aparición de imágenes: 
#define   SLOW_APPEAR_COCINADO            1
#define   SLOW_APPEAR_SCALE               2
#define   SLOW_APPEAR_GRUPO1              3
#define   SLOW_APPEAR_GRUPO2              4
#define   SLOW_APPEAR_GRUPO3              5
#define   SLOW_APPEAR_GRUPO4              6
#define   SLOW_APPEAR_SCALE_SUGERENCIA    7
#define   SLOW_APPEAR_GRUPO1_SUGERENCIA   8
#define   SLOW_APPEAR_ANADIR_SUGERENCIA   9
#define   SLOW_APPEAR_BORRAR_SUGERENCIA   10
#define   SLOW_APPEAR_GUARDAR_SUGERENCIA  11

// Lenta aparición/desaparición de imágenes:
#define   SLOW_DISAPPEAR_CRUDO_APPEAR_COCINADO  1   // Desaparecer crudoGra y aparecer cociGra 
#define   SLOW_DISAPPEAR_COCINADO_APPEAR_CRUDO  2   // Desaparecer cociGra y aparecer crudoGra

// Movimiento de mano y pulsación botones:
#define   MANO_Y_PULSACION_GRUPOS     1  
#define   MANO_Y_PULSACION_ANADIR     2
#define   MANO_Y_PULSACION_ELIMINAR   3
#define   MANO_Y_PULSACION_GUARDAR    4




// Screen object 
RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);

int SCREEN_WIDTH; // X (1024)
int SCREEN_HEIGHT; // Y (600)




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    setupScreen();                    // Inicializar pantalla
void    Welcome();                        // Cargar imágenes (loadPicturesShowHourglass()) y mostrar wireframe de Arranque (logo SmartCloth)
// --- DASHBOARD ---
void    blinkGrupoyProcesamiento(int msg_option); // Mostrar zonas 1 y 2 parpadeando (msg de sin grupo o recipiente) o solo zona 2 (sin msg)
void    printGrupoyEjemplos();            // Zona 1 => Mostrar nombre de grupo escogido y ejemplos
void    printProcesamiento();             // Zona 2 => Mostrar imagen de 'crudo' o 'cocinado' según el procesamiento activo => STATE_raw y STATE_cooked
void    printZona3(int show_objeto);      // Zona 3 => Mostrar comida actual copiada (SHOW_COMIDA_ACTUAL_ZONA3) o alimento actual (SHOW_ALIMENTO_ACTUAL_ZONA3)
void    printZona4(int show_objeto);      // Zona 4 => Mostrar comida actual real (SHOW_COMIDA_ACTUAL_ZONA4) o acumulado hoy (SHOW_ACUMULADO_HOY_ZONA4)
void    showValores(ValoresNutricionales &valores, int zona);  // Mostrar valores en la 'zona' correspondiente (SHOW_VALORES_ZONA3 O SHOW_VALORES_ZONA4).
void    showRaciones(ValoresNutricionales &valores, int zona); // Mostrar raciones con decimales mínimos y centradas según la 'zona' (SHOW_RACIONES_ZONA3 o SHOW_RACIONES_ZONA4).
void    showDashboardStyle1(int msg_option);  // Mostrar dashboard estilo 1 (zonas 1-2 vacías y con mensaje, Comida copiada en zona 3 y Acumulado en zona 4) => STATE_Init y STATE_Plato
void    showDashboardStyle2();                // Mostrar dashboard estilo 2 (zonas 1-2 rellenas, Alimento en zona 3 y Comida en zona 4) => STATE_groupA/B, STATE_raw/cooked y STATE_weighted
bool    showSemiDashboard_PedirProcesamiento(); // Mostrar medio dashboard (zonas 1 y 2). Las zonas 3 y 4 se tapan con pantalla de pedir procesamiento => STATE_groupA/B
// --- PANTALLAS TRANSITORIAS ---
// -- Recipiente ---------
void    pedirRecipiente();                // Pedir colocar recipiente          =>  STATE_Init
void    recipienteColocado();             // Mostrar "Recipiente colocado"     =>  solo una vez en STATE_Plato
void    recipienteRetirado();             // Mostrar "Recipiente retirado"     =>  solo si se ha retirado (LIBERAR --> STATE_Init)
// -- Grupo --------------
void    pedirGrupoAlimentos();            // Pedir escoger grupo de alimentos  =>  STATE_Plato
// -- Procesamiento ------
//void    pedirProcesamiento();             // Pedir escoger crudo o cocinado    =>  STATE_groupA y STATE_groupB
bool    pedirProcesamientoZonas3y4();       // Pedir procesamiento sobre zonas 3 y 4 del dashboard  =>  STATE_groupA y STATE_groupB
bool    alternateButtonsProcesamiento();    // Pedir escoger crudo o cocinado (sobre Dashboard) => STATE_groupA y STATE_groupB
// -- Colocar alimento ---
void    pedirAlimento();                  // Pedir colocar alimento
// -- Sugerir acción ----
void    sugerirAccion();                  // Sugerir acción: añadir más alimento, escoger otro grupo, añadir otro plato, borrar plato actual o guardar comida.
// -- Confirmar acción ---
void    pedirConfirmacion(int option);        // Pregunta de confirmación general  =>  STATE_add_check (option: 1), STATE_delete_check (option: 2) y STATE_save_check (option: 3)
// -- Acción realizada ---
void    showAccionRealizada(int option);  // Mensaje general de confirmación   =>  STATE_added (option: 1), STATE_deleted (option: 2) y STATE_saved (option: 3)
// -- Acción cancelada ---
void    showAccionCancelada();            // Mensaje general de acción cancelada  => STATE_add_check, STATE_delete_check y STATE_save_check 
// --- Errores o avisos ---
void    showError(int option);            // Pantalla de error con mensaje según estado 
void    showWarning(int option);          // Warning de acción innecesaria => STATE_added (option: 1), STATE_deleted (option: 2) y STATE_saved (option: 3)
// -- Aparición/Desaparición imágenes --
bool    slowAppearanceImage(int option);                          // Mostrar imagen de cociGra (option = 1) o scale (option = 2)
bool    slowAppearanceAndDisappareanceProcesamiento(int option);  // Mostrar crudoGra desapareciendo y cociGra apareciendo (option = 1) o viceversa (option = 2)
// --- CARGA DE IMÁGENES ---
void    loadPicturesShowHourglass();      // Cargar imágenes en la SDRAM de la pantalla mientras se muestra un reloj de arena (hourglass)
void    putReloj1();
void    putReloj2();
void    putReloj3();
void    putReloj4();
void    putRelojGirado1();
void    putRelojGirado2();
void    putRelojGirado3();
void    putRelojGirado4();
void    putRelojGirado5();
void    putRelojGirado6();
/*-----------------------------------------------------------------------------*/
// --- INTERRUPCIONES/EVENTOS ---
bool    eventOccurred();                  // Comprobar si ha habido interrupciones de botoneras o eventos de báscula
/*-----------------------------------------------------------------------------*/
// --- ESPERA E INTERRUPCION ---
bool    doubleDelayAndCheckInterrupt(unsigned long period);  // El tiempo especificado se divide a la mitad y se comprueba interrupción tras cada espera
/*-----------------------------------------------------------------------------*/
// --- MOVIMIENTO PANTALLAS GRUPOS Y CONFIRMACIÓN ---
bool    desplazar_mano(int option);   // Desplazar imagen de "mano" por la pantalla hasta el botón correspondiente. Devuelve bool para poder salir de pedirConfirmacion() si hay interrupción.
void    sin_pulsacion(int option);    // Mano sobre botón sin pulsación 
void    con_pulsacion(int option);    // Pulsación de mano en botón


// --- PANTALLAS CHECK Y CONFIRMADO DEL BORRADO CSV ---
void    pedirConfirmacion_DELETE_CSV();   // Pantalla de confirmación de borrado del csv
void    showAcumuladoBorrado(bool exito); // Pantalla con mensaje de "Fichero borrado" (exito = true) o "Error en el borrado" (exito = false)




/*---------------------------------------------------------------------------------------------------------
   setupScreen(): Inicializar pantalla
----------------------------------------------------------------------------------------------------------*/
void setupScreen(){
    pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
    digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight

    if (!tft.init()){
      Serial.println(F("Could not initialize RA8876"));
      while(1);
    }

    tft.canvasImageStartAddress(PAGE1_START_ADDR); 
    tft.clearScreen(BLACK); 

    SCREEN_WIDTH = tft.getWidth(); // X
    SCREEN_HEIGHT = tft.getHeight(); // Y
    Serial.println(F("Screen initialized"));
    delay(200);
}


/*---------------------------------------------------------------------------------------------------------
   eventOccurred(): Comprobar si ha habido eventos en botoneras o báscula
----------------------------------------------------------------------------------------------------------*/
bool eventOccurred(){
    checkBascula();     // Comprueba interrupción de báscula. Lo necesito para ver si hace falta marcar evento
    if(interruptionOccurred()) return true; // Si ha habido interrupción en botoneras (pulsación) o evento en báscula (cambio de peso, no solo interrupción)
    return false;
}


/*---------------------------------------------------------------------------------------------------------
   doubleDelayAndCheckInterrupt(): Divide a la mitad el tiempo especificado y hace dos (double) delays según
                                   esos valores. Tras cada espera, comprueba si ha habido interrupciones.
          Parámetros:
                - period --> período de tiempo que se quiere esperar pero "escuchando" a las interrupciones.
          Return:
                - true --> ha habido interrupcion durante algún delay
                - false --> no ha habido interrupción durante ningún delay
----------------------------------------------------------------------------------------------------------*/
bool doubleDelayAndCheckInterrupt(unsigned long period){
    unsigned long timeForDelay = period/2;

    // ----- INT -------------------
    if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
    // ----- ESPERA ----
    delay(timeForDelay);
    // ----- INT -------------------
    if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula) 
    // ----- ESPERA ----
    delay(timeForDelay);
    // -------- INT -------------------
    if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)

    return false;
}


/***************************************************************************************************/
/*---------------------------- BIENVENIDA A SMARTCLOTH   ------------------------------------------*/
/***************************************************************************************************/

/*---------------------------------------------------------------------------------------------------------
   Welcome(): Carga las imágenes que se van a usar (loadPicturesShowHourglass) mientras muestra un reloj
              de arena y después muestra el logo de SmartCloth (wireframe de arranque).
----------------------------------------------------------------------------------------------------------*/
void Welcome(){

    uint8_t i;

    showingTemporalScreen = false; // Desactivar flag de estar mostrando pantalla temporal/transitoria

    //tft.canvasImageStartAddress(PAGE1_START_ADDR); 
    //tft.clearScreen(BLACK); 

    // 1 -  Cargar imágenes mientras muestra reloj de arena
    loadPicturesShowHourglass();  


    // 2 - Mostrar Logo SmartCloth
    //tft.canvasImageStartAddress(PAGE1_START_ADDR);  // YA ESTÁ
    tft.clearScreen(WHITE); // Fondo blanco en PAGE1
    delay(300);

    // S M A R T C L O T H ==> S T O T M L R A C H (orden de aparición)

    // ----------- LETRA S ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar S apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,40,150,95,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,40,150,95,159);       // S (95x159) 
    delay(200);

    // ----------- LETRA T1 ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar T1 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,428,150,104,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,428,150,104,159);   // T1 (104x159)
    delay(200);

    // ----------- LETRA O ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar  apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,669,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,702,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,669,0,PAGE1_START_ADDR,SCREEN_WIDTH,702,150,85,159);    // O (85x159)
    delay(200);
    
    // ----------- LETRA T2 ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar T2 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,787,150,104,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,787,150,104,159);   // T2 (104x159) 
    delay(200);

    // ----------- LETRA M ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar M apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,96,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,135,150,104,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,96,0,PAGE1_START_ADDR,SCREEN_WIDTH,135,150,104,159);    // M (104x154) 
    delay(200);
    
    // ----------- LETRA L ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar L apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,583,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,617,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,583,0,PAGE1_START_ADDR,SCREEN_WIDTH,617,150,85,159);    // L (85x159) 
    delay(200);

    // ----------- LETRA R ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar R apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,306,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,343,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,306,0,PAGE1_START_ADDR,SCREEN_WIDTH,343,150,85,159);    // R (85x159) 
    delay(200);

    // ----------- LETRA A ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar A apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,201,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,239,150,104,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,201,0,PAGE1_START_ADDR,SCREEN_WIDTH,239,150,104,159);   // A (104x159) 
    delay(200);

    // ----------- LETRA C ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar C apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,497,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,532,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,497,0,PAGE1_START_ADDR,SCREEN_WIDTH,532,150,85,159);    // C (85x159) 
    delay(200);

    // ----------- LETRA H ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar H apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,755,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,891,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,755,0,PAGE1_START_ADDR,SCREEN_WIDTH,891,150,85,159);    // H (85x159) 
    delay(200);   

    
    // ----------- LOGO ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar LOGO apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,841,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,417,350,162,169,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,841,0,PAGE1_START_ADDR,SCREEN_WIDTH,417,350,162,169);   // Logo (162x169) ==> debajo
    
    delay(2000);
}



/*********************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------- ZONAS DEL DASHBOARD --------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*********************************************************************************************************/



/*---------------------------------------------------------------------------------------------------------
   blinkGrupoyProcesamiento(): Muestra un mensaje en la zona 1 (recipiente no colocado o grupo no escogido)
                               y la zona 2 vacía. El borde de ambas zonas parpadea si hay mensaje, indicando 
                               que no se ha escogido aún grupo ni procesamiento.
                               Si no hay mensaje porque ya se ha colocado recipiente y escogido grupo, solo 
                               parpadea la zona 2 indicando que no se ha ecogido aún procesamiento.

                               Haya mensaje o no, la zona 2 siempre parpadea mientras no se escoja crudo o
                               cocinado.

            Parámetros:
                    msg_option - int   -->   0: sin mensaje     1: mensaje "no hay recipiente"    2: mensaje "no hay grupo"
----------------------------------------------------------------------------------------------------------*/
void blinkGrupoyProcesamiento(int msg_option){
    // Tiempos utilizados para alternar el resaltado de los recuadros:
    static unsigned long previousTime = 0;      // Variable estática para almacenar el tiempo anterior
    const unsigned long interval = 500;        // Intervalo de tiempo para alternar entre resaltar el recuadro o no (0.5 seg)

    unsigned long currentTime = millis();

    static bool resaltar_cuadros = true;


    // ----- ALTERNANCIA RESALTADO -------------------------
    // --- RESALTAR --------------------
    if(resaltar_cuadros){
        if (currentTime - previousTime >= interval) {
            previousTime = currentTime;

            // ----- ZONA 1 -----------------------------------------
            if(msg_option != NO_MSG){ // Hay mensaje porque no se ha colocado recipiente o escogido grupo
                // Recuadro grupo y ejemplos
                tft.fillRoundRect(30,20,932,135,20,GRIS_CUADROS); // 902 x 115
                // Resaltar zona de grupos para recordar que no se han escogido
                tft.drawRoundRect(30,20,932,135,20,RED); // Resaltado x1
                tft.drawRoundRect(31,21,931,134,20,RED); // Resaltado x2
                tft.drawRoundRect(32,22,930,133,20,RED); // Resaltado x3

                // Mensaje
                tft.selectInternalFont(RA8876_FONT_SIZE_32);
                tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
                tft.setCursor(50,50);
                tft.setTextForegroundColor(WHITE);
                if(msg_option == MSG_SIN_RECIPIENTE) tft.print("NO SE HA COLOCADO NING\xDA""N RECIPIENTE");  // 16x32 escale x1
                else if(msg_option == MSG_SIN_GRUPO) tft.print("NO SE HA SELECCIONADO NING\xDA""N GRUPO DE ALIMENTOS");  // 16x32 escale x1
            }
            // ----- FIN ZONA 1 -------------------------------------


            // ----- ZONA 2 -----------------------------------------
            // Haya mensaje o no, esta zona siempre parpadea
            // mientras no se escoja procesamiento

            // ----- COCINADO ---------
            // Dibujar cuadro cocinado
            tft.fillRoundRect(937,20,994,74,10,GRIS_CUADROS); 
            // Mostrar cociPeq con opacidad a nivel 24/32. Utiliza un recuadro de color GRIS_CUADROS escrito en page3 como S1.
            tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE3_START_ADDR,SCREEN_WIDTH,610,174,PAGE1_START_ADDR,SCREEN_WIDTH,942,26,47,42,RA8876_ALPHA_OPACITY_24);
            // Resaltado en cuadro cocinado
            tft.drawRoundRect(937,20,994,74,10,RED); // Resaltado x1 en cuadro cocinado
            tft.drawRoundRect(938,21,993,73,10,RED); // Resaltado x2 en cuadro cocinado
            tft.drawRoundRect(939,22,992,72,10,RED); // Resaltado x3 en cuadro cocinado
            // ----- FIN COCINADO -----

            // ----- CRUDO ------------
            // Dibujar cuadro crudo
            tft.fillRoundRect(937,79,994,133,10,GRIS_CUADROS); 
            // Mostrar crudoPeq con opacidad a nivel 24/32. Utiliza un recuadro de color GRIS_CUADROS escrito en page3 como S1.
            tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,577,131,PAGE3_START_ADDR,SCREEN_WIDTH,610,174,PAGE1_START_ADDR,SCREEN_WIDTH,942,85,47,42,RA8876_ALPHA_OPACITY_24);
            // Resaltado en cuadro crudo
            tft.drawRoundRect(937,79,994,133,10,RED); // Resaltado x1 en cuadro crudo
            tft.drawRoundRect(938,80,993,132,10,RED); // Resaltado x2 en cuadro crudo
            tft.drawRoundRect(939,81,992,131,10,RED); // Resaltado x3 en cuadro crudo
            // ----- FIN CRUDO --------
            // ----- FIN ZONA 2 -------------------------------------


            resaltar_cuadros = false;
        }
    }
    // --- FIN RESALTAR --------------------

    else{   // --- NO RESALTAR --------------------
        if (currentTime - previousTime >= interval) {
            previousTime = currentTime;

            // ----- ZONA 1 -----------------------------------------
            if(msg_option != NO_MSG){ // Hay mensaje porque no se ha colocado recipiente o escogido grupo
                // Recuadro grupo y ejemplos
                tft.fillRoundRect(30,20,932,135,20,GRIS_CUADROS); // 902 x 115

                // Mensaje
                tft.selectInternalFont(RA8876_FONT_SIZE_32);
                tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
                tft.setCursor(50,50);
                tft.setTextForegroundColor(WHITE);
                if(msg_option == MSG_SIN_RECIPIENTE) tft.print("NO SE HA COLOCADO NING\xDA""N RECIPIENTE");  // 16x32 escale x1
                else if(msg_option == MSG_SIN_GRUPO) tft.print("NO SE HA SELECCIONADO NING\xDA""N GRUPO DE ALIMENTOS");  // 16x32 escale x1
            }
            // ----- FIN ZONA 1 -------------------------------------


            // ----- ZONA 2 -----------------------------------------
            // Haya mensaje o no, esta zona siempre parpadea 
            // mientras no se escoja procesamiento

            // ----- COCINADO ---------
            // Dibujar cuadro cocinado
            tft.fillRoundRect(937,20,994,74,10,GRIS_CUADROS); 
            // Mostrar cociPeq con opacidad a nivel 24/32. Utiliza un recuadro de color GRIS_CUADROS escrito en page3 como S1.
            tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE3_START_ADDR,SCREEN_WIDTH,610,174,PAGE1_START_ADDR,SCREEN_WIDTH,942,26,47,42,RA8876_ALPHA_OPACITY_24);
            // ----- FIN COCINADO -----

            // ----- CRUDO ------------
            // Dibujar cuadro crudo
            tft.fillRoundRect(937,79,994,133,10,GRIS_CUADROS); 
            // Mostrar crudoPeq con opacidad a nivel 24/32. Utiliza un recuadro de color GRIS_CUADROS escrito en page3 como S1.
            tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,577,131,PAGE3_START_ADDR,SCREEN_WIDTH,610,174,PAGE1_START_ADDR,SCREEN_WIDTH,942,85,47,42,RA8876_ALPHA_OPACITY_24);
            // ----- FIN CRUDO --------
            // ----- FIN ZONA 2 -------------------------------------


            resaltar_cuadros = true;
        }
    }
    // --- FIN NO RESALTAR --------------------
    // ----- FIN ALTERNANCIA RESALTADO ---------------------

}


/*---------------------------------------------------------------------------------------------------------
   printGrupoyEjemplos(): Zona 1 => Muestra ejemplos del grupo de alimentos seleccionado.
----------------------------------------------------------------------------------------------------------*/
void printGrupoyEjemplos(){    
    // -------- TEXTO ------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 

    // Recuadro grupo y ejemplos
    tft.fillRoundRect(30,20,932,135,20,GRIS_CUADROS); // 902 x 115

    // Título
    tft.setCursor(40,30);
    tft.setTextForegroundColor(WHITE); 
    tft.print("Grupo Actual: ");  // 16x32 escale x1

    // Color grupo
    tft.setTextForegroundColor(grupoEscogido.color_grupo); // 'color_grupo' como atributo de struct 'Grupo'
    
    // Nombre grupo
    tft.setCursor(tft.getCursorX(),tft.getCursorY());
    tft.print(grupoEscogido.Nombre_grupo); // Nombre grupo -> 16x32 escale x1

    // Ejemplos grupo
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setCursor(40,68);
    tft.print(grupoEscogido.Ejemplos_grupo); // Ejemplos grupo -> 12x24 escale x1
    // -------- FIN TEXTO --------------------
    
}


/*---------------------------------------------------------------------------------------------------------
   printProcesamiento(): Zona 2 => Muestra el símbolo de 'crudo' o 'cocinado' según el procesamiento activo
                                  Si no hay ningún procesamiento activo (SIN_PROCESAMIENTO), se muestran los
                                  recuadros parpadeando.
----------------------------------------------------------------------------------------------------------*/
void printProcesamiento(){ 
    if(procesamiento == SIN_PROCESAMIENTO){
        blinkGrupoyProcesamiento(NO_MSG); // Solo parpadea zona 2 (procesamiento sin escoger)
    }
    else{
        // Recuadro cocinado pequeño 
        tft.fillRoundRect(937,20,994,74,10,GRIS_CUADROS); // 57 x 52

        // Recuadro crudo pequeño
        tft.fillRoundRect(937,79,994,133,10,GRIS_CUADROS); // 57 x 52

        switch(procesamiento){
            case ALIMENTO_CRUDO:  // CRUDO activo
                // Mostrar crudoPeq normal
                tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,577,131,PAGE1_START_ADDR,SCREEN_WIDTH,942,85,47,42);  // Mostrar crudoPeq (47x42) en PAGE1
                // Mostrar cociPeq con opacidad a nivel 24/32. Utiliza un recuadro de color GRIS_CUADROS escrito en page3 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE3_START_ADDR,SCREEN_WIDTH,610,174,PAGE1_START_ADDR,SCREEN_WIDTH,942,26,47,42,RA8876_ALPHA_OPACITY_24);
                break;

            case ALIMENTO_COCINADO: // COCINADO activo
                // Mostrar cociPeq normal
                tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,131,PAGE1_START_ADDR,SCREEN_WIDTH,942,26,47,42);  // Mostrar cociPeq (47x42) en PAGE1
                // Mostrar crudoPeq con opacidad a nivel 24/32. Utiliza un recuadro de color GRIS_CUADROS escrito en page3 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,577,131,PAGE3_START_ADDR,SCREEN_WIDTH,610,174,PAGE1_START_ADDR,SCREEN_WIDTH,942,85,47,42,RA8876_ALPHA_OPACITY_24);
                break;

            default: break;
        }
    }
    
}


/*---------------------------------------------------------------------------------------------------------
   printZona3(): Zona 3 => Muestra los valores nutricionales según el caso.
          Parámetros:
                        show_objeto - int -> 0: Comida actual copiada tras guardar     1: alimento actual
----------------------------------------------------------------------------------------------------------*/
void printZona3(int show_objeto){ 
    float pesoMostrado = 0.0;

    // ---------- GRÁFICOS ---------------------------------------------------------------------------------------- 
    // Recuadro "Comida actual" o "Alimento actual" 
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
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,175,PAGE1_START_ADDR,SCREEN_WIDTH,127,507,60,64);  // Mostrar kcal_20 (60x65) en PAGE1
    // ---------- FIN GRÁFICOS ------------------------------------------------------------------------------------ 



    // ------ VALORES Y PESO A MOSTRAR ----------------------------------------------------------------------------
    ValoresNutricionales valores; // Valores a mostrar

    if(show_objeto == SHOW_COMIDA_ACTUAL_ZONA3){ // Valores de la comida copiada tras guardar. Al inicio están a 0.
        valores.setValores(comidaActualCopia.getValoresComida());
        pesoMostrado = comidaActualCopia.getPesoComida();
    }
    else if(show_objeto == SHOW_ALIMENTO_ACTUAL_ZONA3){ // Valores temporales calculados a partir del peso del alimento, que puede variar
        Alimento AlimentoAux(grupoEscogido, pesoBascula);         // Alimento auxiliar usado para mostrar información variable de lo pesado
        float carb = AlimentoAux.getValoresAlimento().getCarbValores();
        float lip = AlimentoAux.getValoresAlimento().getLipValores();
        float prot = AlimentoAux.getValoresAlimento().getProtValores();
        float kcal = AlimentoAux.getValoresAlimento().getKcalValores();
        valores.setValores(carb, lip, prot, kcal); // Calcula raciones automáticamente a partir de los valores
        pesoMostrado = AlimentoAux.getPesoAlimento();
    }
    // ------- FIN VALORES Y PESO A MOSTRAR -----------------------------------------------------------------------



    // -------- TEXTO --------------------------------------------------------------------------------------------- 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);  // 12x24 escale x2
    tft.setTextForegroundColor(WHITE); 

    // Título
    tft.setCursor(120,155);
    if(show_objeto == SHOW_COMIDA_ACTUAL_ZONA3){
        if(flagComidaSaved){ 
            tft.setTextForegroundColor(YELLOW); 
            tft.setCursor(90,155);
            tft.print("Comida guardada");  // 12x24 escale x2
        }
        else tft.print("Comida actual");  // 12x24 escale x2
    } 
    else if(show_objeto == SHOW_ALIMENTO_ACTUAL_ZONA3) tft.print("Alimento actual"); // 12x24 escale x2 

    // Peso
    tft.setCursor(50,220); 
    tft.setTextForegroundColor(ROJO_PESO); 
    tft.print("PESO: "); tft.print(pesoMostrado,1); tft.print("g"); // 12x24 escale x2 

    // --- VALORES EN ZONA 3 ---
    showValores(valores, SHOW_VALORES_ZONA3);

    // --- RACIONES EN ZONA 3 ---
    showRaciones(valores, SHOW_RACIONES_ZONA3);

    // -------- FIN TEXTO ------------------------------------------------------------------------

}


/*---------------------------------------------------------------------------------------------------------
   printZona4(): Zona 4 => Muestra los valores nutricionales según el caso.
          Parámetros:
                        show_objeto - int -> 0: Comida actual real     1: acumulado hoy
----------------------------------------------------------------------------------------------------------*/
void printZona4(int show_objeto){ 
    float pesoMostrado = 0.0;

    // ---------- GRÁFICOS --------------------------------------------------------------------------------------
    // Recuadro "Comida actual" o "Acumulado hoy"
    tft.fillRoundRect(520,145,994,580,20,GRIS_CUADROS); // 474 x 425

    // Recuadro Carbohidratos
    tft.fillRoundRect(891,288,969,345,10,GRIS_CUADROS_VALORES); // 78 x 57

    // Recuadro Proteinas
    tft.fillRoundRect(891,365,969,422,10,GRIS_CUADROS_VALORES); // 78 x 57

    // Recuadro Grasas
    tft.fillRoundRect(891,442,969,499,10,GRIS_CUADROS_VALORES); // 78 x 57

    // kcal
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,529,175,PAGE1_START_ADDR,SCREEN_WIDTH,617,507,60,64);  // Mostrar kcal_20 (60x65) en PAGE1
    // ---------- FIN GRÁFICOS ----------------------------------------------------------------------------------



    // ------ VALORES A MOSTRAR -----------------------------------------------------------------
    ValoresNutricionales valores; // Valores a mostrar
                                  // En el caso de SHOW_COMIDA_ACTUAL_ZONA4, se mostrarán los valores temporales con los valores no definitivos del alimento pesado

    if(show_objeto == SHOW_COMIDA_ACTUAL_ZONA4){ // Valores temporales de la comida actual
        Alimento AlimentoAux(grupoEscogido, pesoBascula);         // Alimento auxiliar usado para mostrar información variable de lo pesado
        float carb = AlimentoAux.getValoresAlimento().getCarbValores() + comidaActual.getValoresComida().getCarbValores();
        float lip = AlimentoAux.getValoresAlimento().getLipValores() + comidaActual.getValoresComida().getLipValores();
        float prot = AlimentoAux.getValoresAlimento().getProtValores() + comidaActual.getValoresComida().getProtValores();
        float kcal = AlimentoAux.getValoresAlimento().getKcalValores() + comidaActual.getValoresComida().getKcalValores();
        valores.setValores(carb, lip, prot, kcal); // Calcula raciones automáticamente a partir de los valores

        pesoMostrado = AlimentoAux.getPesoAlimento() + comidaActual.getPesoComida();
    }
    else if(show_objeto == SHOW_ACUMULADO_HOY_ZONA4){ // Valores del acumulado hoy
        valores.setValores(diaActual.getValoresDiario());
        pesoMostrado = diaActual.getPesoDiario();
    }
    // -------------------------------------------------------------------------------------------



    // -------- TEXTO --------------------------------------------------------------------------------------------- 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);  // 12x24 escale x2
    tft.setTextForegroundColor(WHITE); 

    // Título
    tft.setCursor(590, 155);
    if(show_objeto == SHOW_COMIDA_ACTUAL_ZONA4) tft.print("Comida actual");  // 12x24 escale x2
    else if(show_objeto == SHOW_ACUMULADO_HOY_ZONA4) tft.print("Acumulado hoy"); // 12x24 escale x2 

    // Peso
    tft.setCursor(540,220);
    tft.setTextForegroundColor(ROJO_PESO); 
    tft.print("PESO: "); tft.print(pesoMostrado,1); tft.print("g"); // 12x24 escale x2 

    // --- VALORES EN ZONA 4 ---
    showValores(valores, SHOW_VALORES_ZONA4);

    // --- RACIONES EN ZONA 4 ---
    showRaciones(valores, SHOW_RACIONES_ZONA4);

    // -------- FIN TEXTO ------------------------------------------------------------------------

}



/*---------------------------------------------------------------------------------------------------------
   showValores(): Mostrar los valores pasados en la ubicación correspondiente de la pantalla según la zona.
      Parámetros:
                  valores - ValoresNutricionales  --> objeto con los valores a mostrar
                  zona - int    --> zona en la que se están mostrando, necesaria para saber la ubicación en pantalla.
----------------------------------------------------------------------------------------------------------*/
void showValores(ValoresNutricionales &valores, int zona){

    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 

    // ------------ Carbohidratos ------------
    if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(50,303);
    else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(540,303);
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.print("CARBOHIDRATOS: "); tft.print(valores.getCarbValores(),1); tft.print("g");  // 16x32 escale x1
    
    // ------------ Proteinas ------------
    if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(50,380);
    else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(540,380);
    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.print("PROTE\xCD""NAS: "); tft.print(valores.getProtValores(),1); tft.print("g"); // 16x32 escale x1
    
    // ------------ Grasas ------------
    if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(50,457);
    else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(540,457);
    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.print("GRASAS: "); tft.print(valores.getLipValores(),1); tft.print("g"); // 16x32 escale x1
    
    // ------------ Kcal ------------
    if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(197,516);
    else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(697,516);
    tft.setTextForegroundColor(ROJO_KCAL); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.print(valores.getKcalValores(),0); tft.print(" Kcal"); // 12x24 escale X2

}


/*---------------------------------------------------------------------------------------------------------
   showRaciones(): Mostrar raciones de los valores pasados como atributo, mostrando los decimales solo en 
                   determinados casos y centrando el valor en el recuadro correspondiente.
      Parámetros:
                  valores - ValoresNutricionales  --> objeto con las raciones a mostrar
                  zona - int    --> zona en la que se están mostrando, necesaria para saber la ubicación en pantalla.
----------------------------------------------------------------------------------------------------------*/
void showRaciones(ValoresNutricionales &valores, int zona){
    float raciones;

    // Texto "Raciones"
    tft.selectInternalFont(RA8876_FONT_SIZE_32); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setTextForegroundColor(WHITE);
    if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(370,243);
    else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(860,243);
    tft.print("Raciones"); // 16x32 escale x1

    // Cambiar tamaño para las raciones
    tft.selectInternalFont(RA8876_FONT_SIZE_24);


    // ------------ Raciones de Carbohidratos ------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    raciones = valores.getCarbRaciones();
    if (fmod(raciones, 1.0) == 0){ // Termina en .0
        // Cursor según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){  // Una cifra entera
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(426,293);
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(916,293); 
        } 
        else{ // 2 cifras enteras
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(416,293);
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(906,293); 
        } 
        tft.print(raciones,0); // Si termina en .0 no lo mostramos. 12x24 escale x2
    }  
    else{ // Termina en .5
        // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){   // Una cifra entera
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(406,293);
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(896,293);
        } 
        else{ // 2 cifras enteras
            tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); // Estrechar
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(416,293); 
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(906,293); 
        }          
        tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura
    } 



    // ------------ Raciones de Proteinas ------------ 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    raciones = valores.getProtRaciones();
    if (fmod(raciones, 1.0) == 0){ // Termina en .0
        // Cursor según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){  // Una cifra entera
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(426,370); 
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(916,370); 
        } 
        else{ // 2 cifras enteras
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(416,370);
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(906,370); 
        } 
        tft.print(raciones,0); // Si termina en .0 no lo mostramos. 12x24 escale x2
    } 
    else{ // Termina en .5
        // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){   // Una cifra entera
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(406,370);
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(896,370);
        } 
        else{ // 2 cifras enteras
            tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); // Estrechar
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(416,370); 
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(906,370);
        } 
        tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura
    } 



    // ---------- Raciones de Grasas ------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    raciones = valores.getLipRaciones();
    if (fmod(raciones, 1.0) == 0){ // Termina en .0
        // Cursor según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){  // Una cifra entera
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(426,447); 
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(916,447);
        } 
        else{ // 2 cifras enteras
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(416,447);
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(906,447); 
        } 
        tft.print(raciones,0); // Si termina en .0 no lo mostramos. 12x24 escale x2

    } 
    else{ // Termina en .5
        // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
        if((abs((int)raciones) == 0) or ((abs((int)raciones)/10) == 0)){   // Una cifra entera
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(406,447);
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(896,447);
        } 
        else{ // 2 cifras enteras
            tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X2); // Estrechar
            if(zona == SHOW_RACIONES_ZONA3) tft.setCursor(416,447); 
            else if(zona == SHOW_RACIONES_ZONA4) tft.setCursor(906,447);
        }   
        tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura
    } 
}



/*---------------------------------------------------------------------------------------------------------
   showDashboardStyle1(): Muestra el dashboard de estilo 1 con Zona 1 (grupo), Zona 2 (procesamiento), 
                          Zona 3 (Comida Actual copiada) y Zona 4 (Acumulado Hoy).

                          Este dashboard solo se muestra en STATE_Init y STATE_Plato.
          Parámetros:
                    msg_option - int   -->   0: sin mensaje   1: "no hay recipiente"    2: "no hay grupo"
----------------------------------------------------------------------------------------------------------*/
void showDashboardStyle1(int msg_option){
    showingTemporalScreen = false; // Desactivar flag de estar mostrando pantalla temporal/transitoria

    tft.clearScreen(AZUL_FONDO); // Fondo azul oscuro en PAGE1

    blinkGrupoyProcesamiento(msg_option);     // Zonas 1 y 2 - Parpadeando y mensaje de falta recipiente o grupo
    printZona3(SHOW_COMIDA_ACTUAL_ZONA3);     // Zona 3 - Valores comida copiada tras guardar. Al inicio están a 0.
    printZona4(SHOW_ACUMULADO_HOY_ZONA4);     // Zona 4 - Valores Acumulado hoy
}



/*---------------------------------------------------------------------------------------------------------
   showDashboardStyle2(): Muestra el dashboard de estilo 2 con Zona 1 (grupo), Zona 2 (procesamiento), 
                          Zona 3 (Alimento Actual) y Zona 4 (Comida Actual).

                          Este dashboard se muestra en STATE_groupA, STATE_groupB, STATE_raw, STATE_cooked y STATE_weighted.
----------------------------------------------------------------------------------------------------------*/
void showDashboardStyle2(){
    showingTemporalScreen = false; // Desactivar flag de estar mostrando pantalla temporal/transitoria

    tft.clearScreen(AZUL_FONDO); // Fondo azul oscuro en PAGE1

    printGrupoyEjemplos();                  // Zona 1 - Grupo y ejemplos 
    printProcesamiento();                   // Zona 2 - Procesamiento crudo o cocinado (según modo: ALIMENTO_CRUDO o ALIMENTO_COCINADO)
    printZona3(SHOW_ALIMENTO_ACTUAL_ZONA3); // Zona 3 - Valores alimento actual pesado
    printZona4(SHOW_COMIDA_ACTUAL_ZONA4);   // Zona 4 - Valores Comida actual actualizada en tiempo real según el peso del alimento
}



/*---------------------------------------------------------------------------------------------------------
   showSemiDashboard_PedirProcesamiento(): 
                        Muestra la Zona 1 (ejemplos) y la Zona 2 (sin escoger, parpadeando). Las zonas 3 y 4
                        se tapan con la pantalla de escoger procesamiento (crudo o cocinado)

                        Este dashboard se muestra en STATE_groupA y STATE_groupB, donde aún no se ha escogido
                        crudo o cocinado.
----------------------------------------------------------------------------------------------------------*/
bool showSemiDashboard_PedirProcesamiento(){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria.
                                  // En STATE_raw y STATE_cooked se muestra todo el dashboard 2 si esta flag
                                  // está activa. Si no lo está, solo se modifica la zona 2.

    tft.clearScreen(AZUL_FONDO); // Fondo azul oscuro en PAGE1

    printGrupoyEjemplos();                    // Zona 1 - Grupo y ejemplos 
    blinkGrupoyProcesamiento(NO_MSG);         // Zona 2 - Procesamiento sin escoger (parpadeo)
    if(pedirProcesamientoZonas3y4()) return true;   // Zonas 3 y 4 - Pedir procesamiento
                                              // Si ha ocurrido una interrupción mientras se formaba la pantalla de pedir recipiente, se sale de la función
    return false;
}











/*********************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------- PANTALLAS DE AYUDA ---------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*********************************************************************************************************/


/*-------------------------------------------------------------------------------------------------------*/
/*--------------------------------- PEDIR RECIPIENTE ----------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   pedirRecipiente(): Pide colocar un recipiente (STATE_Init)
----------------------------------------------------------------------------------------------------------*/
void pedirRecipiente(){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    // ************ TEXTO ********************************************************************************
    // ----- FONDO VERDE -----------
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1

    // ----- INTERNAL FIXED 12x24 X3 --------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 

    if(comidaActual.isComidaEmpty()){ // Si no hay nada que se pueda guardar (comida vacía), solo se pide recipiente
        tft.setCursor(130, 90);                                         tft.println("COLOQUE UN RECIPIENTE"); // 12x24 escalado x3
        tft.setCursor(150, tft.getCursorY() + tft.getTextSizeY()-10);   tft.print("EN LA ZONA DE PESADA"); // 12x24 escalado x3
    }
    else{ // Si se puede guardar comida, también se da esa opción
        tft.setCursor(130, 90);                                         tft.println("COLOQUE UN RECIPIENTE"); 
        tft.setCursor(190, tft.getCursorY() + tft.getTextSizeY()-10);   tft.print("O GUARDE LA COMIDA"); 
    }     
    // ****************************************************************************************************

    // ************ CÍRCULO *******************************************************************************
    tft.fillCircle(512,380,65,WHITE); // 65 pixeles de diametro
    // ****************************************************************************************************

    // -------- INT -------------------
    if(eventOccurred()) return; // Evento de interrupción (botonera o báscula)

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(600)) return;

    // ************ CUADRADO REDONDEADO *******************************************************************
    tft.fillRoundRect(447,315,577,445,10,WHITE); // x = 512 +/- 65 = 447   ->   y = 380 +/- 65 = 315
    // ****************************************************************************************************
    
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(600)) return;

    // ************ PALITOS *******************************************************************************
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
    // ****************************************************************************************************
    
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(800)) return;
    // ------------------------------------------------------------------------------------------------------

    // *********** BRAINS (120X108) *************************************************************************
    // **** BRAIN 1 ********************************************
    // BRAIN1 (120x108) --> centrar en el cuadrado blanco
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); // Mostrar brain1 en PAGE1
    // *********************************************************

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(800)) return;

    // **** BRAIN 2 ********************************************
    // BRAIN2G (99x83) (verde)
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,121,170,PAGE1_START_ADDR,SCREEN_WIDTH,450,325,120,108); // Mostrar brain2G en PAGE1
    // *********************************************************

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(800)) return;
    // ****************************************************************************************************
    
}

/*---------------------------------------------------------------------------------------------------------
   recipienteColocado(): Muestra mensaje de recipiente colocado
----------------------------------------------------------------------------------------------------------*/
void recipienteColocado(){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    // ----- TEXTO (RECIPIENTE COLOCADO) -------------------------------------------------------------------
    //tft.clearScreen(RED);
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1
    // ------ LINEA ---------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(170, 258);  tft.println("RECIPIENTE COLOCADO"); // 12x24 escalado x3. Color texto foreground a WHITE en STATE_Init
    // ------ LINEA ---------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // ----------------------------------------------------------------------------------------------------
}

/*---------------------------------------------------------------------------------------------------------
   recipienteRetirado(): Muestra mensaje de recipiente retirado
----------------------------------------------------------------------------------------------------------*/
void recipienteRetirado(){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    // ----- TEXTO (PLATO RETIRADO) -----------------------------------------------------------------------
    //tft.clearScreen(RED);
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1
    // ------ LINEA ---------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(170, 258);  tft.println("RECIPIENTE RETIRADO"); // 12x24 escalado x3. Color texto foreground a WHITE en STATE_Init
    // ------ LINEA ---------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // Puede ser que se haya retirado el único plato de la comida sin haberla guardado antes,
    // entonces se indica que se ha borrado la info no guardada. 
    // Si aún quedaran platos en la comida, aparecería su info.
  /*  if(comidaSinGuardar_recipienteRetirado){
        tft.selectInternalFont(RA8876_FONT_SIZE_32);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
        tft.setCursor(190, 418); tft.println("LA INFORMACIÓN NO GUARDADA HA SIDO ELIMINADA");
    }*/
}
/*------------------------------ FIN PEDIR RECIPIENTE ---------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------------*/
/*--------------------------------- ESCOGER GRUPO -------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   pedirGrupoAlimentos(): Pide escoger grupo de alimentos, tras haber colocado recipiente (STATE_Plato)

   TODO: DIBUJAR BORDE DE RECTANGULO REDONDEADO (VARIAS VECES POR GROSOR) DEL COLOR DEL FONDO Y PONERLO 
          ENCIMA DE LAS IMÁGENES DE GRUPOS PARA TAPAR LAS SOMBRAS EN LAS ESQUINAS.
----------------------------------------------------------------------------------------------------------*/
void pedirGrupoAlimentos(){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria
    bool pulsacion = true;

    // ***** FONDO VERDE ***********
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1


    // ******* TEXTO (INTERNAL FIXED 12x24 X3) ***********************************************************
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(130, 50);                                           tft.println("SELECCIONE UN GRUPO"); // 12x24 escalado x3. Color texto foreground a WHITE en STATE_Init
    tft.setCursor(255, tft.getCursorY() + tft.getTextSizeY()-10);     tft.print("DE ALIMENTOS"); 

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;
    // ****************************************************************************************************


    // ************ LINEAS ********************************************************************************
    tft.fillRoundRect(0,250,220,258,3,WHITE);
    tft.fillRoundRect(0,450,512,458,3,WHITE);

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return; // 800
    // ****************************************************************************************************


    // ************ GRUPOS ********************************************************************************
    // 30 pixeles entre cuadro y cuadro
    // Mostrar en PAGE1 (copiar de PAGE3 a PAGE1)

    // ------ Grupo 1 (130x125) ---------
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125); // x = 236  ->  y = 288
    slowAppearanceImage(SLOW_APPEAR_GRUPO1);

    // DIBUJAR BORDE DE RECTANGULO REDONDEADO (VARIAS VECES POR GROSOR) DEL COLOR DEL FONDO Y PONERLO ENCIMA 
    // DE LAS IMÁGENES DE GRUPOS PARA TAPAR LAS SOMBRAS EN LAS ESQUINAS.

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(800)) return;

    // ------ Grupo 2 (130x125) ---------
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125); // x = <grupo1(236) + grupo1(130) + 30 = 396  ->  y = 288
    slowAppearanceImage(SLOW_APPEAR_GRUPO2);

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(800)) return;

    // ------ Grupo 3 (130x125) ---------
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); // x = <grupo2(396) + grupo2(130) + 30 = 556  ->  y = 288
    slowAppearanceImage(SLOW_APPEAR_GRUPO3);

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(800)) return;

    // ------ Grupo 4 (130x125) ---------
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125); // x = <grupo3(556) + grupo3(130) + 30 = 716  ->  y = 288
    slowAppearanceImage(SLOW_APPEAR_GRUPO4);

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(800)) return;
    // ****************************************************************************************************


    // ************ DESPLAZAR MANO PARA SIMULAR MOVIMIENTO ************************************************
    // MANO por el camino
    if(desplazar_mano(MANO_Y_PULSACION_GRUPOS)) return; // Si ha ocurrido interrupción mientras se desplazaba la mano, se sale de la función.
    // ****************************************************************************************************


    // ******** ALTERNANCIA 2 PULSACIONES *****************************************************************
    // Tras trasladar la mano, no hay pulsación. Se van a hacer dos alternancias de pulsación,
    // es decir: pulsacion - no pulsacion - pulsacion - no pulsacion
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;

    for(int i = 0; i < 4; i++){
        if(pulsacion) con_pulsacion(MANO_Y_PULSACION_GRUPOS);
        else sin_pulsacion(MANO_Y_PULSACION_GRUPOS);
        if(doubleDelayAndCheckInterrupt(1000)) return;
        pulsacion = !pulsacion;
    }
    // ****************************************************************************************************

}

/*---------------------------------------------------------------------------------------------------------
   desplazar_mano(): Desplazar imagen de "mano" por la pantalla simulando movimiento hasta botón correspondiente.
        Return:   true: ha habido interrupción    false: no ha habido interrupción
            Devuelve bool para salir de la función que llamó de desplazar_mano() si hubo interrupción.  
----------------------------------------------------------------------------------------------------------*/

bool desplazar_mano(int option){
    int alto, posY;

    switch(option){
        case MANO_Y_PULSACION_GRUPOS: // Grupos
              // 1 - MANO por el camino
              alto = 127; posY = 480;
              while(posY >= 410){
                  // manoGppt
                  tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,posY,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)
                  delay(50);
                  if(posY < 413) posY = 413; // Solo afecta al penúltimo movimiento de la mano, para evitar que se borre parte del grupo3 que está debajo
                  tft.clearArea(556,posY,690,posY + alto+5,VERDE_PEDIR); // Desaparece de esa zona para aparecer en otra --> se mueve
                  posY -= 10; // Subimos verticalmente la imagen 10 píxeles
                  // -------- INT -------------------
                  if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula) 
              }
              posY = 400;
              while(posY >= 380){
                  // Mostrar grupo3 (130x125). Para superponerse a la mano, que aún se está "moviendo"
                  tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 
                  
                  // Mostrar mano (manoGppt)
                  tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,posY,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)
                  
                  delay(50);
                  // -------- INT -------------------
                  if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula) 

                  if(posY == 400) tft.clearArea(556,413,690,528,VERDE_PEDIR); // Se borra desde y = 413 para no borrar parte del botón. Solo se borra en la primera iteración del bucle
                  posY -= 20;
              }

              break;

        case MANO_Y_PULSACION_ANADIR: // Añadir
        case MANO_Y_PULSACION_ELIMINAR: // Eliminar
              // 1 - MANO por el camino
              alto = 128; posY = 580;
              while(posY >= 510){
                  // manoWppt
                  tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,posY,120,128,WHITE); // Mostrar manoWppt (120x128)
                  delay(50);
                  tft.clearArea(430,posY,567,posY + alto,AMARILLO_CONFIRM_Y_AVISO); // Desaparece de esa zona para aparecer en otra --> se mueve
                  posY -= 10; // Subimos verticalmente la imagen 10 píxeles
                  // -------- INT -------------------
                  if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula) 
              }
              
              // 2 - Botón correspondiente --> para superponerse a la última mano y que desaparezca para simular el movimiento
              if(option == MANO_Y_PULSACION_ANADIR) tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) 
              else tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar borrar (172x130)
              
              // 3 - Movimiento final de la mano (manoWppt)
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,472,120,128,WHITE); // Mostrar manoWppt (120x128)
              delay(50);

              // -------- INT -------------------
              if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula) 
              
              break;


        case MANO_Y_PULSACION_GUARDAR: // Guardar
              // 1 - MANO por el camino
              alto = 128; posY = 590;
              while(posY >= 530){
                  // manoWppt
                  tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,420,posY,120,128,WHITE); // Mostrar manoWppt (120x128)
                  delay(50);
                  tft.clearArea(420,posY,557,posY + alto,AMARILLO_CONFIRM_Y_AVISO); // Desaparece de esa zona para aparecer en otra --> se mueve
                  posY -= 10; // Subimos verticalmente la imagen 10 píxeles
                  // -------- INT -------------------
                  if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
              }
              
              // 2 - Botón guardar --> para superponerse a la última mano y que desaparezca para simular el movimiento
              tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,400,172,130); // Mostrar guardar (172x130)
              
              // 3 - Movimiento final de la mano (manoWppt)
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,128,WHITE); // Mostrar manoWppt (120x128)    
              delay(50);

              // -------- INT -------------------
              if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula) 
              
              break;

        default: break;
    }

    return false; // Si no ha ocurrido ninguna interrupción mientras se desplazaba la mano,
                  // se continúa con la función que llamó a esta.
}

/*---------------------------------------------------------------------------------------------------------
   sin_pulsacion(): Mostrar la "mano" sobre el botón sin pulsación
----------------------------------------------------------------------------------------------------------*/

void sin_pulsacion(int option){
    switch(option){
        case MANO_Y_PULSACION_GRUPOS: // Grupos
              // 1 - Borrar todo (grupo, mano y pulsación)
              tft.clearArea(546,278,690,528,VERDE_PEDIR); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano

              // 2 - Mostrar grupo3 (130x125)
              tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 

              // 3 - Mostrar mano (manoGppt)
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)

              break;


        case MANO_Y_PULSACION_ANADIR: // Añadir
        case MANO_Y_PULSACION_ELIMINAR: // Eliminar
              // 1 - Borrar todo (botón, mano y pulsación)
              tft.clearArea(400,370,612,620,AMARILLO_CONFIRM_Y_AVISO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
              
              // 2 - Botón correspondiente --> para superponerse a la última mano y que desaparezca para simular el movimiento
              if(option == MANO_Y_PULSACION_ANADIR) tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) 
              else tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar borrar (172x130)
              
              // 3 - Movimiento final de la mano (manoWppt)
              // manoWppt
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,472,120,128,WHITE); // Transparencia manoWppt (120x128)
              
              break;


        case MANO_Y_PULSACION_GUARDAR: // Guardar
              // 1 - Borrar todo (botón, mano y pulsación)
              tft.clearArea(400,390,612,660,AMARILLO_CONFIRM_Y_AVISO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
              
              // 2 - Botón guardar --> para superponerse a la última mano y que desaparezca para simular el movimiento
              tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,400,172,130); // Mostrar guardar (172x130)

              // 3 - Movimiento final de la mano (manoWppt)
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,128,WHITE); // Transparencia manoWppt (120x128)
              
              break;

        default: break;
    }

}

/*---------------------------------------------------------------------------------------------------------
   con_pulsacion(): Simular pulsación causada por la "mano" sobre botón.
----------------------------------------------------------------------------------------------------------*/
void con_pulsacion(int option){
    int x1, y1, x2, y2;

    switch(option){
        case MANO_Y_PULSACION_GRUPOS: // Grupos
              // 1 - Borrar todo (grupo, mano y pulsación)
              tft.clearArea(546,278,690,528,VERDE_PEDIR); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
              
              // 2 - Volver a mostrar grupo3 (130x125)
              tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 
              
              // 3 - Pulsación
              // ------------ CUADRADO ESQUINADO (PULSACION) --------------------------------------------------------   
              // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
              // cuadrados normales, separados por 1 píxel en cada dirección, para simular un grosor mayor.
              for (int i = 0; i <= 10; i++) {
                  x1 = 556 - i;   y1 = 288 - i;   
                  x2 = 680 + i;   y2 = 413 + i;
                  tft.drawRect(x1,y1,x2,y2,RED_BUTTON); // Alrededor de grupo3
              }
              // ----------------------------------------------------------------------------------------------------

              // 4 - Mano
              // ------------ MANO (120x129) ------------------------------------------------------------------------
              // Mano final pulsando (manoGppt)
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)
              // ----------------------------------------------------------------------------------------------------

              // 5 - Rayitas pulsación
              // ------------ RAYITAS (PULSACION) ------------------
              // Línea izquierda
              for (int i = 0; i <= 4; i++) {
                  x1 = 584 + i;   y1 = 355;   
                  x2 = 594 + i;   y2 = 375;
                  tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
              }

              // Línea central
              for (int i = 0; i <= 4; i++) {
                  x1 = 604 + i;   y1 = 350;   
                  x2 = 604 + i;   y2 = 370;
                  tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
              }

              // Línea derecha
              for (int i = 0; i <= 4; i++) {
                  x1 = 614 + i;   y1 = 375;   
                  x2 = 624 + i;   y2 = 355;
                  tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
              }
              // ---------------------------------------------------

              break;


        case MANO_Y_PULSACION_ANADIR: // Añadir
        case MANO_Y_PULSACION_ELIMINAR: // Eliminar
              // 1 - Borrar todo (botón, mano y pulsación)
              tft.clearArea(400,370,612,620,AMARILLO_CONFIRM_Y_AVISO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
              
              // 2 - Botón correspondiente --> para superponerse a la última mano y que desaparezca para simular el movimiento
              if(option == MANO_Y_PULSACION_ANADIR) tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) 
              else tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar borrar (172x130)
              
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

              break;


        case MANO_Y_PULSACION_GUARDAR: // Guardar
              // 1 - Borrar todo (botón, mano y pulsación)
              tft.clearArea(400,410,612,660,AMARILLO_CONFIRM_Y_AVISO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
              
              // 2 - Botón guardar --> para superponerse a la última mano y que desaparezca para simular el movimiento
              tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,400,172,130); // Mostrar guardar (172x130)

              // 3 - Pulsación
              // ------------ CUADRADO ESQUINADO (PULSACION) --------------------------------------------------------   
              // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
              // cuadrados normales, separados por 1 píxel en cada dirección, para simular un grosor mayor.
              for (int i = 0; i <= 10; i++) {
                  x1 = 425 - i;   y1 = 403 - i;   
                  x2 = 590 + i;   y2 = 525 + i;
                  tft.drawRect(x1,y1,x2,y2,RED_BUTTON); // Alrededor de añadir
              }
              // ----------------------------------------------------------------------------------------------------

              // 4 - Mano
              // ------------ MANO (120x129) ------------------------------------------------------------------------
              // Mano (manoWppt) final pulsando
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,128,WHITE); // Transparencia manoWppt (120x128)
              // ----------------------------------------------------------------------------------------------------

              // 5 - Rayitas pulsación
              // ------------ RAYITAS (PULSACION) ------------------
              // Línea izquierda
              for (int i = 0; i <= 4; i++) {
                  x1 = 438 + i;   y1 = 467;   
                  x2 = 448 + i;   y2 = 487;
                  tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
              }

              // Línea central
              for (int i = 0; i <= 4; i++) {
                  x1 = 458 + i;   y1 = 462;   
                  x2 = 458 + i;   y2 = 482;
                  tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
              }

              // Línea derecha
              for (int i = 0; i <= 4; i++) {
                  x1 = 468 + i;   y1 = 487;   
                  x2 = 478 + i;   y2 = 467;
                  tft.drawLine(x1, y1, x2, y2, RED_BUTTON);
              }
              // ---------------------------------------------------
              break;

        default: break;
    }
    
}

/*-------------------------------- FIN ESCOGER GRUPO ----------------------------------------------------*/



/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------- ESCOGER CRUDO/COCINADO ---------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   formGraphicsPedirProcesamiento(): Compone la pantalla (forma, colores, texto y 1º botón de cocinado) de
                                     pedir procesamiento sobre las zonas 3 y 4 del dashboard.
----------------------------------------------------------------------------------------------------------*/
bool formGraphicsPedirProcesamiento(){
    // ******* ZONAS 3 Y 4 **************************************************************************
    // ------- GRÁFICOS -------------------------------------
        // Recuadro tapando zonas 3 y 4 del dashboard
        tft.fillRoundRect(30,145,994,580,20,VERDE_PEDIR); // 964 x 435
        tft.drawRoundRect(30,145,994,580,20,VERDE_BORDE_PEDIR_COCCION); // Borde => 964 x 435
        // ------ LINEAS -----
        tft.fillRoundRect(30,330,226,338,3,WHITE); // 256 -> 226
        tft.fillRoundRect(798,527,994,535,3,WHITE); // 768 -> 798
    // ------- FIN GRÁFICOS ---------------------------------

    // ------ TEXTO (PREGUNTA) ---------------------------------------------------
        tft.selectInternalFont(RA8876_FONT_SIZE_32);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setTextForegroundColor(WHITE); 
        tft.setCursor(230, 185);                                        tft.println("\xBF""EL ALIMENTO EST\xC1""");
        tft.setCursor(250, tft.getCursorY() + tft.getTextSizeY()-20);   tft.print("COCINADO O CRUDO\x3F"""); 
    // ---------------------------------------------------------------------------

    // ------- 1º BOTÓN -------------------------------------
    if(slowAppearanceImage(SLOW_APPEAR_COCINADO)) return true; // Si ha ocurrido interrupción mientras aparecía la imagen de cocinado

    // ----- ESPERA E INTERRUPCION ----------------
    //if(doubleDelayAndCheckInterrupt(1000)) return true; // Si ha ocurrido interrupción mientras se hacía el "delay"
    // ***********************************************************************************************

    return false;
}

/*---------------------------------------------------------------------------------------------------------
   alternateButtonsProcesamiento(): Alternar botones de crudo cocinado en la pantalla de pedir procesamiento.
                                    Zonas 3 y 4 tapadas con mensaje de petición en formGraphicsPedirProcesamiento().
----------------------------------------------------------------------------------------------------------*/
bool alternateButtonsProcesamiento(){ 
    // Tiempos utilizados para alternar entre mostrar botón COCINADO o CRUDO:
    static unsigned long previousTime = 0;      // Variable estática para almacenar el tiempo anterior
    const unsigned long interval = 1000;        // Intervalo de tiempo para alternar entre botones (1 seg)

    unsigned long currentTime = millis();

    static bool showingCocinado = true;
    static bool showingCrudo = false;

    // ------- ALTERNANCIAS CRUDO/COCINADO ----------------------------------------
    if(currentTime - previousTime >= interval) {
        previousTime = currentTime;
        if(showingCocinado){
            // Mostrar CRUDO
            if(slowAppearanceAndDisappareanceProcesamiento(SLOW_DISAPPEAR_COCINADO_APPEAR_CRUDO)) return true; 
                // Si ha ocurrido interrupción mientras aparecía la imagen de crudo y desaparecía la de cocinado, 
                // se sale de la función.
            showingCrudo = true;
            showingCocinado = false;
        }
        else if(showingCrudo){
            // Mostrar COCINADO
            if(slowAppearanceAndDisappareanceProcesamiento(SLOW_DISAPPEAR_CRUDO_APPEAR_COCINADO)) return true; 
                // Si ha ocurrido interrupción mientras aparecía la imagen de cocinado y desaparecía la de crudo, 
                // se sale de la función.
            showingCocinado = true;
            showingCrudo = false;
        }
    }

    return false;
    // ----------------------------------------------------------------------------

}

/*---------------------------------------------------------------------------------------------------------
   pedirProcesamientoZonas3y4(): Muestra la pantalla de pedir procesamiento (crudo o cocinado) sobre las 
                                 zonas 3 y 4 del dashboard. 

                                  Esto se hace en STATE_groupA y STATE_groupB, donde aún no se ha escogido
                                  crudo o cocinado.
----------------------------------------------------------------------------------------------------------*/
bool pedirProcesamientoZonas3y4(){
    if(formGraphicsPedirProcesamiento()) return true;    // Zonas 3 y 4 - Base (formas, colores, texto y 1º botón cocinado) de la pantalla de pedir procesamiento
    if(alternateButtonsProcesamiento()) return true;     // Zonas 3 y 4 - Alternar botones de crudo o cocinado en pantalla de pedir procesamiento

    return false;
}

/*---------------------------------------------------------------------------------------------------------
   pedirProcesamiento(): Pide escoger crudo cocinado, tras haber escogido grupo (STATE_groupA/B)
----------------------------------------------------------------------------------------------------------*/
/*
void pedirProcesamiento(){ 
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    // ***** FONDO VERDE ***********
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1

    // ******* TEXTO (PREGUNTA) **************************************************************************
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(100, 30);                                           tft.println("\xBF""EL ALIMENTO EST\xC1""");
    tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-50);     tft.print("COCINADO O CRUDO\x3F"""); 
    
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return; 
    // ****************************************************************************************************

    // ******** LINEAS ************************************************************************************
    tft.fillRoundRect(0,250,256,258,3,WHITE);
    tft.fillRoundRect(768,517,1024,525,3,WHITE);
    // ****************************************************************************************************

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;

    // ******** BOTONES ***********************************************************************************
    // ******** 1º ALTERNANCIA ***********************************
    // Apareciendo imagen cocinado
    if(slowAppearanceImage(SLOW_APPEAR_COCINADO)) return; // Si ha ocurrido interrupción mientras aparecía la imagen de cocinado, 
                                                          // se sale de la función.

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;

    // Mostrar CRUDO
    if(slowAppearanceAndDisappareanceProcesamiento(SLOW_DISAPPEAR_COCINADO_APPEAR_CRUDO)) return; 
            // Si ha ocurrido interrupción mientras aparecía la imagen de crudo y desaparecía la de cocinado, 
            // se sale de la función.
    
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return; 
    // ***********************************************************


    // ******** 2º Y 3º ALTERNANCIAS ***********************************
    for(uint8_t i = 0; i < 2; i++){ // 2 alternancias más
        for(uint8_t j = 1; j <= 2; j++){
            if(slowAppearanceAndDisappareanceProcesamiento(j)) return;  // j = 1 --> Desaparecer CRUDO y aparecer COCINADO 
                                                                        // j = 2 --> Desaparecer COCINADO y aparecer CRUDO
            // ----- ESPERA E INTERRUPCION ----------------
            if(doubleDelayAndCheckInterrupt(1000)) return;
        }
    }
    // ***********************************************************

}

*/

/*
void showButtonCocinado(){

        // ******* ZONAS 3 Y 4 **************************************************************************
        // ------- GRÁFICOS -------------------------------------
            // Recuadro tapando zonas 3 y 4 del dashboard
            tft.fillRoundRect(30,145,994,580,20,VERDE_PEDIR); // 964 x 435
            tft.drawRoundRect(30,145,994,580,20,VERDE_BORDE_PEDIR_COCCION); // Borde => 964 x 435
            // ------ LINEAS -----
            tft.fillRoundRect(30,330,256,338,3,WHITE);
            tft.fillRoundRect(768,527,994,535,3,WHITE);
        // ------- FIN GRÁFICOS ---------------------------------

        // ------ TEXTO (PREGUNTA) ---------------------------------------------------
            tft.selectInternalFont(RA8876_FONT_SIZE_32);
            tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
            tft.setTextForegroundColor(WHITE); 
            tft.setCursor(230, 185);                                        tft.println("\xBF""EL ALIMENTO EST\xC1""");
            tft.setCursor(250, tft.getCursorY() + tft.getTextSizeY()-20);   tft.print("COCINADO O CRUDO\x3F"""); 
        // ---------------------------------------------------------------------------
        // ***********************************************************************************************

        // ******* ZONA 2 ********************************************************************************
        //blinkGrupoyProcesamiento(NO_MSG);     // Zona 2 - Parpadea (procesamiento sin escoger)
        // ***********************************************************************************************

        // -------- INT -------------------
        if(eventOccurred()) return; // Evento de interrupción (botonera o báscula)

        // -------- BOTONES ----------------------------------------------------------
            // Apareciendo imagen cocinado
            if(slowAppearanceImage(SLOW_APPEAR_COCINADO)) return; // Si ha ocurrido interrupción mientras aparecía la imagen de cocinado, 
                                                                  // se sale de la función.
            showingCocinado = true;
    }
}*/

/*---------------------------------------------------------------------------------------------------------
   alternateButtonsProcesamiento(): Alternar botones de crudo cocinado en la pantalla de pedir procesamiento.
                                    Zonas 3 y 4 tapadas con mensaje de petición en formGraphicsPedirProcesamiento().
----------------------------------------------------------------------------------------------------------*/
/*bool alternateButtonsProcesamiento(){ 
    // Tiempos utilizados para alternar entre mostrar botón COCINADO o CRUDO:
    static unsigned long previousTime = 0;      // Variable estática para almacenar el tiempo anterior
    const unsigned long interval = 1000;        // Intervalo de tiempo para alternar entre botones (1 seg)

    unsigned long currentTime = millis();

    //static bool firstAppearance = true;
    static bool showingCocinado = false;
    static bool showingCrudo = false;

    showingTemporalScreen = true; // Desactivar flag de estar mostrando pantalla temporal/transitoria

    if(firstAppearance){    
        //tft.clearScreen(AZUL_FONDO); // Fondo azul oscuro en PAGE1

        // ******* ZONA 1 ********************************************************************************
        //printGrupoyEjemplos();                // Zona 1 - Grupo y ejemplos 
        // ***********************************************************************************************

        // ******* ZONAS 3 Y 4 **************************************************************************
        // ------- GRÁFICOS -------------------------------------
            // Recuadro tapando zonas 3 y 4 del dashboard
            tft.fillRoundRect(30,145,994,580,20,VERDE_PEDIR); // 964 x 435
            tft.drawRoundRect(30,145,994,580,20,VERDE_BORDE_PEDIR_COCCION); // Borde => 964 x 435
            // ------ LINEAS -----
            tft.fillRoundRect(30,330,226,338,3,WHITE); // 256 -> 226
            tft.fillRoundRect(798,527,994,535,3,WHITE); // 768 -> 798
        // ------- FIN GRÁFICOS ---------------------------------

        // ------ TEXTO (PREGUNTA) ---------------------------------------------------
            tft.selectInternalFont(RA8876_FONT_SIZE_32);
            tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
            tft.setTextForegroundColor(WHITE); 
            tft.setCursor(230, 185);                                        tft.println("\xBF""EL ALIMENTO EST\xC1""");
            tft.setCursor(250, tft.getCursorY() + tft.getTextSizeY()-20);   tft.print("COCINADO O CRUDO\x3F"""); 
        // ---------------------------------------------------------------------------
        // ***********************************************************************************************


        // ******* ZONA 2 ********************************************************************************
        //blinkGrupoyProcesamiento(NO_MSG);     // Zona 2 - Parpadea (procesamiento sin escoger)
        // ***********************************************************************************************

        // -------- INT -------------------
        //if(eventOccurred()) return; // Evento de interrupción (botonera o báscula)

        // -------- BOTONES ----------------------------------------------------------
            // Apareciendo imagen cocinado
            if(slowAppearanceImage(SLOW_APPEAR_COCINADO)) return; // Si ha ocurrido interrupción mientras aparecía la imagen de cocinado, 
                                                                  // se sale de la función.
            showingCocinado = true;
    //}
    //else{  // -------- BOTONES ----------------------------------------------------------
        if(currentTime - previousTime >= interval) {
            previousTime = currentTime;
            if(showingCocinado){
                // Mostrar CRUDO
                if(slowAppearanceAndDisappareanceProcesamiento(SLOW_DISAPPEAR_COCINADO_APPEAR_CRUDO)) return; 
                    // Si ha ocurrido interrupción mientras aparecía la imagen de crudo y desaparecía la de cocinado, 
                    // se sale de la función.
                showingCrudo = true;
                showingCocinado = false;
            }
            else if(showingCrudo){
                // Mostrar COCINADO
                if(slowAppearanceAndDisappareanceProcesamiento(SLOW_DISAPPEAR_CRUDO_APPEAR_COCINADO)) return; 
                    // Si ha ocurrido interrupción mientras aparecía la imagen de cocinado y desaparecía la de crudo, 
                    // se sale de la función.
                showingCocinado = true;
                showingCrudo = false;
            }
        }
    //}
    //firstAppearance = false;
    
    // ----------------------------------------------------------------------------

    // ****************************************************************************************************

}*/
/*------------------------- FIN ESCOGER CRUDO/COCINADO ---------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------- COLOCAR ALIMENTO ---------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   pedirAlimento(): Pide colocar alimento sobre la báscula, tras haber seleccionado crudo o cocinado.
----------------------------------------------------------------------------------------------------------*/
void pedirAlimento(){ 
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    // ************ TEXTO ********************************************************************************
    // ----- FONDO VERDE -----------
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1

    // ----- INTERNAL FIXED 12x24 X3 --------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(160, 50);                                         tft.println("COLOQUE UN ALIMENTO");
    tft.setCursor(150, tft.getCursorY() + tft.getTextSizeY()-20);   tft.print("EN LA ZONA DE PESADA"); 
    // ****************************************************************************************************

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;

    // ************ LÍNEAS ********************************************************************************
    tft.fillRoundRect(0,270,276,278,3,WHITE);
    tft.fillRoundRect(748,517,1024,525,3,WHITE);
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;
    // **************************************************************************************************** 
    
      
    // ************ SCALE *********************************************************************************
    // Imagen scale 
    if(slowAppearanceImage(SLOW_APPEAR_SCALE)) return; // Imagen de scale apareciendo
    // **************************************************************************************************** 
    
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;

}

/*------------------------- FIN COLOCAR ALIMENTO --------------------------------------------------------*/



/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------- SUGERIR ACCIONES ---------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
// TODO: DIBUJAR BORDE DE RECTANGULO REDONDEADO (VARIAS VECES POR GROSOR) DEL COLOR DEL FONDO Y PONERLO ENCIMA 
//        DE LAS IMÁGENES PARA TAPAR LAS SOMBRAS EN LAS ESQUINAS.
void sugerirAccion(){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    // ***** TEXTO (PREGUNTA) ****************************************************************************
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(80, 50);
    tft.println("\xBF""QU\xC9"" QUIERE HACER AHORA\x3F""");
    
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;
    // **************************************************************************************************** 


    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    // ***** POSIBLES ACCIONES ****************************************************************************
    // 30 pixeles entre cuadro y cuadro
    // Mostrar en PAGE1 (copiar de PAGE3 a PAGE1)

    // ***** MÁS ALIMENTO *************************************************
    // Añadir más cantidad de alimento --> scaleG (150x150) 
    tft.setCursor(90, 377);                                       tft.println("A\xD1""ADIR");
    tft.setCursor(115, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("M\xC1""S");
    tft.setCursor(75, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("ALIMENTO");
    // Apareciendo y recortando bordes de add/delete/save
    if(slowAppearanceImage(SLOW_APPEAR_SCALE_SUGERENCIA)) return;
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;
    // ********************************************************************


    // ***** NUEVO GRUPO **************************************************
    // Escoger nuevo grupo --> grupo1 (130x1125) 
    tft.setCursor(250, 377);                                       tft.println("ESCOGER");
    tft.setCursor(275, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("OTRO");
    tft.setCursor(270, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("GRUPO");
    // Apareciendo y recortando bordes de add/delete/save
    if(slowAppearanceImage(SLOW_APPEAR_GRUPO1_SUGERENCIA)) return;
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;
    // ********************************************************************


    // ***** AÑADIR PLATO *************************************************
    // Añadir plato --> anadir (172x130) 
    tft.setCursor(430, 377);                                       tft.println("A\xD1""ADIR");
    tft.setCursor(445, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("OTRO");
    tft.setCursor(440, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("PLATO");
    // Apareciendo y recortando bordes de add/delete/save
    if(slowAppearanceImage(SLOW_APPEAR_ANADIR_SUGERENCIA)) return;
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;
    // ********************************************************************


    // ***** BORRAR PLATO *************************************************
    // Borrar plato --> borrar (172x130) 
    tft.setCursor(617, 377);                                       tft.println("BORRAR");
    tft.setCursor(627, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("PLATO");
    tft.setCursor(617, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("ACTUAL");
    // Apareciendo y recortando bordes de add/delete/save
    if(slowAppearanceImage(SLOW_APPEAR_BORRAR_SUGERENCIA)) return;
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return;
    // ********************************************************************


    // ***** GUARDAR COMIDA ***********************************************
    // Guardar comida --> guardar (172x130) 
    tft.setCursor(795, 377);                                       tft.println("GUARDAR");
    tft.setCursor(800, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("COMIDA");
    // Apareciendo y recortando bordes de add/delete/save
    if(slowAppearanceImage(SLOW_APPEAR_GUARDAR_SUGERENCIA)) return;
    // ----- ESPERA E INTERRUPCION ----------------
    //if(doubleDelayAndCheckInterrupt(1000)) return;
    // ********************************************************************

    // **************************************************************************************************** 

}

/*------------------------- FIN SUGERIR ACCIONES --------------------------------------------------------*/



/*-------------------------------------------------------------------------------------------------------*/
/*--------------------------------- CONFIRMACIÓN DE ACCIONES --------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   pedirConfirmacion(): Pregunta genérica para confirmar acción (añadir, eliminar o guardar) 
        Parámetros: 
            - option -> 1: botón añadir   2: botón eliminar   3: botón guardar
----------------------------------------------------------------------------------------------------------*/
void pedirConfirmacion(int option){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO); // Fondo amarillo en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO); 

    if(option == ASK_CONFIRMATION_SAVE) tft.setCursor(30, 20); // Guardar
    else tft.setCursor(30, 30); // Añadir y eliminar
    tft.println("\xBF""EST\xC1"" SEGURO DE QUE QUIERE");

    // -------- INT -------------------
    if(eventOccurred()) return; // Evento de interrupción (botonera o báscula)  

    switch (option){
      case ASK_CONFIRMATION_ADD:    tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-40); tft.print("A\xD1""ADIR UN NUEVO PLATO\x3F"""); break; // BOTÓN AÑADIR
      case ASK_CONFIRMATION_DELETE: tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-40); tft.print("BORRAR EL PLATO ACTUAL\x3F""");     break; // BOTÓN ELIMINAR
      case ASK_CONFIRMATION_SAVE: // BOTÓN GUARDAR
              tft.setCursor(80, tft.getCursorY() + tft.getTextSizeY()-40);  tft.print("GUARDAR LA COMIDA ACTUAL\x3F""");
              delay(500);
              // -------- INT -------------------
              if(eventOccurred()) return; // Evento de interrupción (botonera o báscula)  
              // ----- TEXTO (COMENTARIO) ---------
              tft.selectInternalFont(RA8876_FONT_SIZE_32);
              tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
              tft.setCursor(100, 180);  tft.println("LOS VALORES NUTRICIONALES PASAR\xC1""N AL ACUMULADO DE HOY");
              // -----------------------------------
              break;
    }

    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return; 
    // ----------------------------------------------------------------------------------------------------

    // ------------ LINEA --------------------------------------------------------------------------------
    if(option == ASK_CONFIRMATION_SAVE) tft.fillRoundRect(252,235,764,243,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    else tft.fillRoundRect(252,205,764,213,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    // ----------------------------------------------------------------------------------------------------
    
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(400)) return; 

    // ----- TEXTO (CONFIRMACIÓN) -------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    if(option == ASK_CONFIRMATION_SAVE) tft.setCursor(150, 275); // Guardar
    else tft.setCursor(150, 245); // Añadir y eliminar
    tft.println("PARA CONFIRMAR, PULSE DE NUEVO");
    tft.setCursor(400, tft.getCursorY() + tft.getTextSizeY()-10);   tft.print("EL BOT\xD3""N"); 
    // ----------------------------------------------------------------------------------------------------

    // -------- INT -------------------
    if(doubleDelayAndCheckInterrupt(400)) return; // Evento de interrupción (botonera o báscula)  


    // ------------ BOTÓN A PULSAR ------------------------------------------------------------------------
    // Copiar de PAGE3 a PAGE1
    switch (option){
        case ASK_CONFIRMATION_ADD:    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); break; // Mostrar BOTÓN AÑADIR (172x130) en PAGE1      
        case ASK_CONFIRMATION_DELETE: tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); break; // Mostrar BOTÓN ELIMINAR (172x130) en PAGE1   
        case ASK_CONFIRMATION_SAVE:   tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,400,172,130); break; // Mostrar BOTÓN GUARDAR (172x130) en PAGE1  
        default: break;       
    }
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(800)) return; 
    // ----------------------------------------------------------------------------------------------------
    

    // ------------ DESPLAZAR MANO PARA SIMULAR MOVIMIENTO ------------------------------------------------
    // MANO por el camino
    desplazar_mano(option+1); // P.ej. option = ASK_CONFIRMATION_ADD = 1 --> desplazar_mano(2) --> añadir plato
    // ----------------------------------------------------------------------------------------------------
  

    // ******** ALTERNANCIA 2 PULSACIONES *****************************************************************
    // Tras trasladar la mano, no hay pulsación. Se van a hacer dos alternancias de pulsación,
    // es decir: pulsacion - no pulsacion - pulsacion - no pulsacion
    // ----- ESPERA E INTERRUPCION ----------------
    if(doubleDelayAndCheckInterrupt(1000)) return; 

    bool pulsacion = true;

    for(int i = 0; i < 4; i++){
        if(pulsacion) con_pulsacion(option+1); // Simular pulsación (p.ej. option = ASK_CONFIRMATION_ADD = 1 --> con_pulsacion(2) --> añadir plato
        else sin_pulsacion(option+1);          // Eliminar pulsación 
        // ----- ESPERA E INTERRUPCION ----------------
        if(doubleDelayAndCheckInterrupt(1000)) return; 
        pulsacion = !pulsacion;
    }
    // ****************************************************************************************************


}



/*-------------------------------------------------------------------------------------------------------*/
/*--------------------------------- REALIZACIÓN DE ACCIONES ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   showAccionRealizada(): Confirmación general de acción (añadir, eliminar o guardar) 
        Parámetros: 
            - option -> 1: botón añadir   2: botón eliminar   3: botón guardar
----------------------------------------------------------------------------------------------------------*/
void showAccionRealizada(int option){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    tft.clearScreen(VERDE_PEDIR);

    // ------------ LINEA ---------------------------------------------------------------------------------
    tft.fillRoundRect(252,150,764,158,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (ACCION REALIZADA) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 

    switch (option){
      case ADD_EXECUTED:    tft.setCursor(170, 208); tft.println("NUEVO PLATO A\xD1""ADIDO"); break; // PLATO AÑADIDO
      case DELETE_EXECUTED: tft.setCursor(100, 208); tft.println("PLATO ACTUAL ELIMINADO");   break; // PLATO ELIMINADO
      case SAVE_EXECUTED:   tft.setCursor(120, 208); tft.println("COMIDA ACTUAL GUARDADA");   break; // COMIDA GUARDADA
    }
    // ----------------------------------------------------------------------------------------------------
    
    // ------------ LINEA ---------------------------------------------------------------------------------
    tft.fillRoundRect(252,330,764,338,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 

     switch (option){
      case ADD_EXECUTED:    tft.setCursor(240, 388); tft.println("RETIRE EL PLATO PARA COMENZAR OTRO");                 break; // AÑADIDO

      case DELETE_EXECUTED: tft.setCursor(160, 388); tft.println("RETIRE EL PLATO ELIMINADO PARA COMENZAR DE NUEVO");   break; // ELIMINADO
              
      case SAVE_EXECUTED: // GUARDADA
              // No se pone if(pesoARetirar ...) porque aún no ha dado tiempo a actualizar 'pesoARetirar' y puede ser incorrecto
              //if((pesoRecipiente + pesoPlato) == 0.0){
              if(lastValidState == STATE_Init){
                  // Puede ser que se quiera guardar desde el STATE_Init, tras añadir o borrar. Si es así,
                  // la báscula estará vacía (pesoARetirar = 0).
                  tft.setCursor(190, 388); tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO");
                  tft.setCursor(350, tft.getCursorY() + tft.getTextSizeY()+40); tft.print("AL ACUMULADO DE HOY"); 
              }
              else{ 
                  // El siguiente mensaje solo se mostrará si se ha querido guardar tras conformar el plato,
                  // estando aún en la báscula.
                  tft.setCursor(30, 388); tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO AL ACUMULADO DE HOY");  
                  tft.setCursor(200,450); tft.println("RETIRE EL PLATO PARA COMENZAR DE NUEVO"); 
              }
              break; 
    }
    // ----------------------------------------------------------------------------------------------------

}




/*-------------------------------------------------------------------------------------------------------*/
/*--------------------------------- CANCELACIÓN DE ACCIONES ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   showAccionCancelada(): Indica que se ha cancelado la acción de añadir, eliminar o guardar.
                          Puede ser indicado por el usuario o por time-out de 10 segundos.
----------------------------------------------------------------------------------------------------------*/
void showAccionCancelada(){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    // ----- TEXTO (ACCION CANCELADA) -----------------------------------
    tft.clearScreen(RED);
    // ------ LINEA ---------
    tft.fillRoundRect(252,200,764,208,3,WHITE);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(220, 258);  tft.println("ACCI\xD3""N CANCELADA"); // 12x24 escalado x3
    // ------ LINEA ---------
    tft.fillRoundRect(252,380,764,388,3,WHITE);
    // -------------------------------------------------------------------
}




/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------------- AVISOS POR OBJETO VACÍO --------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------
   showWarning(): Pantalla genérica para avisar de que no se ha realizado la acción porque 
                  no era necesaria. Solo para casos de añadir, eliminar o guardar. 
        Parámetros: 
            option  - int -> 1: añadir   2: eliminar   3: guardar
----------------------------------------------------------------------------------------------------------*/
void showWarning(int option){
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

    // ----- TEXTO (AVISO) -------------------------------------------------------------------------------
    //tft.clearScreen(RED); // Fondo rojo en PAGE1
    //tft.clearScreen(DARKORANGE); // Fondo amarillo oscuro en PAGE1
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO); // Fondo amarillo en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO); 

    tft.setCursor(384, 100);
    tft.println("\xA1""AVISO\x21""");
    // ---------------------------------------------------------------------------------------------------


    // ------------ LINEA --------------------------------------------------------------------------------
    tft.fillRoundRect(252,286,764,294,3,ROJO_TEXTO_CONFIRM_Y_AVISO); // Cruza la imagen de aviso por detrás
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
      case WARNING_NOT_ADDED: // AÑADIR
              tft.setCursor(150, 410);                                      tft.println("NO SE HA CREADO UN NUEVO PLATO"); 
              tft.setCursor(180, tft.getCursorY() + tft.getTextSizeY());    tft.print("PORQUE EL ACTUAL EST\xC1"" VAC\xCD""O");  
              break;

      case WARNING_NOT_DELETED: // ELIMINAR
              tft.setCursor(180, 410);                                      tft.println("NO SE HA ELIMINADO EL PLATO"); 
              tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY());    tft.print("PORQUE EST\xC1"" VAC\xCD""O"); 
              break;

      case WARNING_NOT_SAVED: // GUARDAR
              tft.setCursor(190, 410);                                      tft.println("NO SE HA GUARDADO LA COMIDA"); 
              tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY());    tft.print("PORQUE EST\xC1"" VAC\xCD""A"); 
              break;
    }
    // ----------------------------------------------------------------------------------------------------  


    
}




/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- ERRORES POR ESTADO ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------
   showError(): Pantalla genérica de error que indica una acción incorrecta. Según la opción indicada como
                parámetro, dependiente del estado actual, se muestra un mensaje diferente de acción posible.
          Parámetros:
                option - int -> 1: Init         2: Plato          3: grupoA       4: grupoB   5: Crudo    
                                6: Cocinado     7: Pesado         8: add_check    9: Added    10: delete_check     
                                11: Deleted     12: save_check    13: Saved
----------------------------------------------------------------------------------------------------------*/
void showError(int option){ 
    showingTemporalScreen = true; // Activar flag de estar mostrando pantalla temporal/transitoria

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
      case ERROR_STATE_INIT: // Init
              tft.setCursor(250, 420);                                     tft.println("COLOQUE UN RECIPIENTE"); 
              //tft.setCursor(160, 420);                                     tft.println("COLOQUE UN RECIPIENTE ANTES DE"); 
              //tft.setCursor(180, tft.getCursorY() + tft.getTextSizeY());   tft.print("ESCOGER UN GRUPO DE ALIMENTOS"); // "O GUARDE LA COMIDA"
              break;

      case ERROR_STATE_PLATO: // Plato
              tft.setCursor(160, 420);                                     tft.println("SELECCIONE GRUPO DE ALIMENTOS"); 
              //tft.setCursor(120, tft.getCursorY() + tft.getTextSizeY());   tft.print("Y DESPU\xC9""S ESCOJA COCINADO O CRUDO"); 
              // Los errores que se pueden cometer en STATE_PLATO son pulsar crudo, cocinado, añadir, borrar o guardar.
              // Modificar el mensaje para que indique "No puede realizar esa acción. Seleccione grupo y después crudo/cocinado"??
              // Puede que el mensaje actual no sea lo suficiente claro porque no indica específicamente qué se ha hecho mal.
              break;

      case ERROR_STATE_GROUPS: // Grupos (grupoA o groupB)
              if(keepErrorScreen){ // Se ha colocado peso sin crudo/cocinado. Se mantiene la pantalla de error hasta que se retire el plato entero
                  tft.setCursor(10, 420); tft.println("SELECCIONE COCINADO O CRUDO ANTES DE PESAR");  
                  tft.selectInternalFont(RA8876_FONT_SIZE_32);
                  tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
                  tft.setCursor(200,tft.getCursorY() + tft.getTextSizeY()); tft.println("RETIRE EL PLATO PARA COMENZAR DE NUEVO"); 
              }
              else{ // Se ha pulsado botón que no toca (añadir, borrar o guardar)
                  tft.setCursor(190, 420);                                     tft.println("SELECCIONE COCINADO O CRUDO"); 
                  //tft.setCursor(200, tft.getCursorY() + tft.getTextSizeY());   tft.print("ANTES DE PESAR EL ALIMENTO"); 
              }
              break;

      case ERROR_STATE_PROCESAMIENTO: // Crudo o Cocinado
              tft.setCursor(100, 450);                                      tft.println("COLOQUE UN ALIMENTO SOBRE LA B\xC1""SCULA");  
              break;

      case ERROR_STATE_WEIGHTED: // Pesado
              tft.setCursor(140, 420);                                      tft.println("ESCOJA GRUPO PARA OTRO ALIMENTO,"); 
              tft.setCursor(100, tft.getCursorY() + tft.getTextSizeY());    tft.print("A\xD1""ADA OTRO PLATO O GUARDE LA COMIDA"); 
              break;

      case ERROR_STATE_ADD_CHECK: // add_check
              tft.setCursor(70, 420);                                       tft.println("PULSE \"A\xD1""ADIR\" DE NUEVO PARA CONFIRMAR"); 
              tft.setCursor(100, tft.getCursorY() + tft.getTextSizeY());    tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              break;
      
      case ERROR_STATE_ADDED: // Added
              tft.setCursor(50, 450);                                       tft.println("RETIRE EL PLATO PARA COMENZAR UNO NUEVO");  
              break;

      case ERROR_STATE_DELETE_CHECK: // delete_check
              tft.setCursor(60, 420);                                       tft.println("PULSE \"BORRAR\" DE NUEVO PARA CONFIRMAR"); 
              tft.setCursor(90, tft.getCursorY() + tft.getTextSizeY());     tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              break;

      case ERROR_STATE_DELETED: // Deleted
              tft.setCursor(130, 450);                                      tft.println("RETIRE EL PLATO QUE HA ELIMINADO"); 
              break;

      case ERROR_STATE_SAVE_CHECK: // save_check
              tft.setCursor(50, 420);                                       tft.println("PULSE \"GUARDAR\" DE NUEVO PARA CONFIRMAR"); 
              tft.setCursor(90, tft.getCursorY() + tft.getTextSizeY());     tft.print("O CUALQUIER OTRO BOT\xD3""N PARA CANCELAR"); 
              break;

      case ERROR_STATE_SAVED: // Saved
              tft.setCursor(155, 450);                                      tft.println("RETIRE EL PLATO PARA CONTINUAR");  
              break;
      
      case ERROR_STATE_CANCEL: // Cancelado
              tft.setCursor(90, 450);                                      tft.println("ESPERE A QUE TERMINE LA CANCELACI\xD3""N");  
              break;

      case ERROR_STATE_AVISO: // Aviso
              tft.setCursor(155, 450);                                      tft.println("ESPERE A QUE TERMINE EL AVISO");  
              break;

      default: break;
    }
    // ---------------------------------------------------------------------------------------------------- 

    if(eventOccurred()) return; // Evento de interrupción (botonera o báscula)   

}





/*-------------------------------------------------------------------------------------------------------*/
/*------------------------------ APARICIÓN/DESAPARICIÓN IMÁGENES ----------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   slowAppearanceImage(): Mostrar imagen de cocinado (escoger procesamiento) o de scale (colocar alimento)
                          apareciendo poco a poco en pantalla cuando corresponda.
        Parámetros: 
            - option -> 1: cocinado  2: scale

        Return:   true: ha habido interrupción    false: no ha habido interrupción
            Devuelve bool para salir de la función que llamó de slowAppearanceImage() si hubo interrupción.  
----------------------------------------------------------------------------------------------------------*/
bool slowAppearanceImage(int option){
    uint8_t i;
    switch(option){
        case SLOW_APPEAR_COCINADO: // Cocinado
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar cociGra apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // Pantalla completa:
                //tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160,i);
                // Sobre Dashboard:
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,800,350,PAGE1_START_ADDR,SCREEN_WIDTH,280,350,177,160,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            // Pantalla completa:
            //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); // Mostrar sin transparencia
            // Sobre Dashboard
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,280,350,177,160); // Mostrar sin transparencia

            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)

            break;


        case SLOW_APPEAR_SCALE: // ScaleG
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar scaleG apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,372,293,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,437,320,146,147,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,373,293,PAGE1_START_ADDR,SCREEN_WIDTH,437,320,146,147); // Mostrar sin transparencia
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        case SLOW_APPEAR_GRUPO1: // grupo1
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar grupo1 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,288,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125); // x = 236  ->  y = 288
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        case SLOW_APPEAR_GRUPO2: // grupo2
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar grupo2 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,288,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125); // x = <grupo1(236) + grupo1(130) + 30 = 396  ->  y = 288
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        case SLOW_APPEAR_GRUPO3: // grupo3
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar grupo3 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,288,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); // x = <grupo2(396) + grupo2(130) + 30 = 556  ->  y = 288
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        case SLOW_APPEAR_GRUPO4: // grupo4
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar grupo4 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,288,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125); // x = <grupo3(556) + grupo3(130) + 30 = 716  ->  y = 288
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        case SLOW_APPEAR_SCALE_SUGERENCIA: // ScaleG sugerencias
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar scale apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,372,293,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,69,200,146,147,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,373,293,PAGE1_START_ADDR,SCREEN_WIDTH,69,200,146,147); // Mostrar scaleG (150x150)
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        case SLOW_APPEAR_GRUPO1_SUGERENCIA: // grupo1 sugerencias
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar grupo1 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,1,1,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,245,213,129,124,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,1,1,PAGE1_START_ADDR,SCREEN_WIDTH,245,213,129,124); // Mostrar grupo1 (130x125)
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        case SLOW_APPEAR_ANADIR_SUGERENCIA: // añadir sugerencias
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar añadir apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,652,0,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,404,206,158,130,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,652,0,PAGE1_START_ADDR,SCREEN_WIDTH,404,206,158,130); // Mostrar añadir (172x130)
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        case SLOW_APPEAR_BORRAR_SUGERENCIA: // borrar sugerencias
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar borrar apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,825,0,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,592,206,158,130,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,825,0,PAGE1_START_ADDR,SCREEN_WIDTH,592,206,158,130); // Mostrar borrar (172x130) 
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        case SLOW_APPEAR_GUARDAR_SUGERENCIA: // guardar sugerencias
            for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
                // Mostrar guardar apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,7,131,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,780,206,158,130,i);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,7,131,PAGE1_START_ADDR,SCREEN_WIDTH,780,206,158,130); // Mostrar guardar (172x130)
  
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
  
            break;


        default: break;
    }

    return false; // Si no ha ocurrido ninguna interrupción mientras aparecía la imagen,
                  // se continúa con la función que llamó a esta.
}


/*---------------------------------------------------------------------------------------------------------
   slowAppearanceAndDisappareanceProcesamiento(): Mostrar imagen de crudo desapareciendo y cocinado apareciendo 
                                                  o viceversa.
        Parámetros: 
            - option -> 1: desaparecer crudo y aparecer cocinado    2: desaparecer cocinado y aparecer crudo

        Return:   true: ha habido interrupción    false: no ha habido interrupción
            Devuelve bool para salir de la función que llamó de slowAppearanceAndDisappareanceProcesamiento() si hubo interrupción.  
----------------------------------------------------------------------------------------------------------*/
bool slowAppearanceAndDisappareanceProcesamiento(int option){
    uint8_t i,j;
    /*
    // PANTALLA COMPLETA:
    switch(option){
        case SLOW_DISAPPEAR_CRUDO_APPEAR_COCINADO: // Desaparecer CRUDO y aparecer COCINADO 
            for (i = 4, j = 30; i <= 30 && j >= 4; i++, j--) {  // i|j = 16  --> RA8876_ALPHA_OPACITY_16
                // Mostrar crudoGra desapareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160,i);
                // Mostrar cociGra apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160,j);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.clearArea(500,280,724,480,VERDE_PEDIR); // Borrar crudoGra
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); // Mostrar cociGra (177x160) en PAGE1
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            break;

        case SLOW_DISAPPEAR_COCINADO_APPEAR_CRUDO: // Desaparecer COCINADO y aparecer CRUDO
            for (i = 4, j = 30; i <= 30 && j >= 4; i++, j--) {  // i|j = 16  --> RA8876_ALPHA_OPACITY_16
                // Mostrar cociGra desapareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160,i);
                // Mostrar crudoGra apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160,j);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.clearArea(280,280,497,470,VERDE_PEDIR); // Borrar cociGra
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,527,300,177,160); // Mostrar crudoGra (177x160) en PAGE1
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            break;

        default: break;
    }
    */


    // SOBRE DASHBOARD
     switch(option){
        case SLOW_DISAPPEAR_CRUDO_APPEAR_COCINADO: // Desaparecer CRUDO y aparecer COCINADO 
            for (i = 4, j = 30; i <= 30 && j >= 4; i++, j--) {  // i|j = 16  --> RA8876_ALPHA_OPACITY_16
                // Mostrar crudoGra desapareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,800,350,PAGE1_START_ADDR,SCREEN_WIDTH,567,350,177,160,i);
                // Mostrar cociGra apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,800,350,PAGE1_START_ADDR,SCREEN_WIDTH,280,350,177,160,j);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.clearArea(562,345,749,515,VERDE_PEDIR); // Borrar crudoGra
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,280,350,177,160); // Mostrar cociGra (177x160) en PAGE1
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            break;

        case SLOW_DISAPPEAR_COCINADO_APPEAR_CRUDO: // Desaparecer COCINADO y aparecer CRUDO
            for (i = 4, j = 30; i <= 30 && j >= 4; i++, j--) {  // i|j = 16  --> RA8876_ALPHA_OPACITY_16
                // Mostrar cociGra desapareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,800,350,PAGE1_START_ADDR,SCREEN_WIDTH,280,350,177,160,i);
                // Mostrar crudoGra apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,800,350,PAGE1_START_ADDR,SCREEN_WIDTH,567,350,177,160,j);
                delay(10);
                // ----- INT -------------------
                if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            }
            tft.clearArea(275,345,462,515,VERDE_PEDIR); // Borrar cociGra
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,567,350,177,160); // Mostrar crudoGra (177x160) en PAGE1
            // ----- INT -------------------
            if(eventOccurred()) return true; // Evento de interrupción (botonera o báscula)
            break;

        default: break;
    }

    return false; // Si no ha ocurrido ninguna interrupción mientras aparecían/desaparecían las imágenes,
                  // se continúa con la función que llamó a esta.
}




/*-------------------------------------------------------------------------------------------------------*/
/*------------------------------ BORRADO FICHERO CSV ----------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   pedirConfirmacion_DELETE_CSV(): Pantalla de confirmación de borrado del csv
----------------------------------------------------------------------------------------------------------*/
void pedirConfirmacion_DELETE_CSV(){
    // ----- TEXTO (CONFIRMAR BORRADO CSV) -----------------------------------
    tft.clearScreen(BLACK);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(220, 200);  tft.println("\xBF""BORRAR FICHERO CSV\x3F"""); // 12x24 escalado x3
    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setCursor(270, tft.getCursorY() + tft.getTextSizeY()+50); tft.println("GRUPO 20 PARA CONFIRMAR");
    tft.setCursor(220, tft.getCursorY() + tft.getTextSizeY()+70); tft.println("ESPERE 5 SEGUNDOS PARA CANCELAR");
    // -------------------------------------------------------------------
}

/*---------------------------------------------------------------------------------------------------------
   showAcumuladoBorrado(): Pantalla con mensaje de "Fichero borrado" o "Error en el borrado" 
        Parámetros: 
            - exito -> true: éxito borrando csv    false: error en el borrado
----------------------------------------------------------------------------------------------------------*/
void showAcumuladoBorrado(bool exito){
    // ----- TEXTO (CONFIRMAR BORRADO CSV) -----------------------------------
    tft.clearScreen(BLACK);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setTextForegroundColor(WHITE); 

    tft.setCursor(220, 200);  
    if(exito) tft.println("FICHERO BORRADO CON EXITO");
    else tft.println("ERROR BORRANDO EL FICHERO");

    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    tft.setCursor(270, tft.getCursorY() + tft.getTextSizeY()+50); tft.println("REGRESANDO A INIT...");
    // -------------------------------------------------------------------
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
        brain1  ->  (0,170)   brain2G  ->  (121,170)   [[NO[[logFull ->  (242,170)]]NO]]
        reloj1 -> (0,279)   reloj2 -> (66,279)   reloj3 -> (132,279)   reloj4 -> (198,279)   relGir1 -> (264,279)    relGir2 -> (360,279)   relGir3 -> (473,279)    relGir4 -> (587,279)    relGir5 -> (688,279)    relGir6 -> (802,279)
        
    PAGE 3:
        grupo1 -> (0,0)   grupo2 -> (131,0)   grupo3 -> (262,0)   grupo4 -> (393,0)   manoGppt -> (524,0)    anadir -> (645,0)    borrar -> (818,0)
        guardar -> (0,131)   cociGra -> (173,131)   crudoGra -> (351,131)    cociPeq -> (529,131)   crudoPeq -> (577,131)
                                                                             kcal -> (529,174)   cuadroGris -> (610,174)
        cruz -> (0,292)   aviso -> (115,292)   manoWppt -> (251,292)   scale -> (372,292)
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
      tft.sdCardDraw16bppBIN256bits(372,292,150,150,fileScale); // Cargar scaleG (150x150) en PAGE3 => x =  <manoY(251) + manoY(120) + 1 = 372   ->  y = 292

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
