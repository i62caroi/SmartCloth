#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "RA8876_v2.h"
#include "Files.h"

// ------- ChipSelect de SD --------------
// SmartCloth v2.1
//#define SD_CARD_SCS  4 
// SmartCloth v2.2
#define SD_CARD_SCS  13 
// ---------------------------------------

// Screen circuit wiring
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10


int SCREEN_WIDTH; // X (1024)
int SCREEN_HEIGHT; // Y (600)


// --- ACCIÓN CONFIRMADA ---
#define  ADD_EXECUTED                             1
#define  DELETE_EXECUTED                          2
#define  SAVE_EXECUTED_FULL                       3  
#define  SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP      4
#define  SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI         5
#define  SAVE_ESP32_TIMEOUT                       6
#define  SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR   7


#define  DATA_TO_UPLOAD         1
#define  NO_INTERNET_CONECTION  2
#define  UPLOADING_DATA         3
#define  UPLOADED_DATA          4
#define  HTTP_ERROR             5



// --- MENSAJE DE ERROR ----
#define  ERROR_STATE_INIT            1
#define  ERROR_STATE_PLATO           2
#define  ERROR_STATE_GROUP           3  
#define  ERROR_STATE_BARCODE         4
#define  ERROR_STATE_PROCESAMIENTO   5  // raw o cooked
#define  ERROR_STATE_WEIGHTED        6
#define  ERROR_STATE_ADD_CHECK       7
#define  ERROR_STATE_ADDED           8
#define  ERROR_STATE_DELETE_CHECK    9
#define  ERROR_STATE_DELETED         10
#define  ERROR_STATE_SAVE_CHECK      11
#define  ERROR_STATE_SAVED           12
#define  ERROR_STATE_CANCEL          13
#define  ERROR_STATE_AVISO           14



RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);

/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    setupSD();
void    setupScreen();

void    showCriticFailureSD();              // Pantalla: Fallo en SD (no se encuentra)
void    showDataToUpload(int option);
void    pedirConfirmacion_DELETE_CSV();     // Pantalla: Pedir confirmacion y confirmado borrar CSV

void    suggestAction();                    // Pantalla: Sugerencia acción
void    pantalla_inicial();                 // Pantalla: Coloque Recipiente
void    select_Grupo();                     // Pantalla: Seleccione grupo
void    crudo_cocinado();                   // Pantalla: Escoja crudo/cocinado (simple)
void    crudo_cocinado_sobre_dashboard();   // Pantalla: Escoja crudo/cocinado (sobre dashboard)
void    colocar_alimento();                 // Pantalla: Coloque alimento sobre báscula
void    add_Plato();                        // Pantalla: Pedir confirmación y confirmado añadir plato
void    delete_Plato();                     // Pantalla: Pedir confirmación y confirmado borrar plato
void    save_Comida();                      // Pantalla: Pedir confirmación y confirmado guardar comida
void    showAccionRealizada(int option);    // Pantalla: Acción realizada (añadir, borrar, guardar)
void    arranque();                         // Pantalla: Mostrar logo SmartCloth letra a letra
void    dashboard();                        // Pantalla: Dashboard completo 
void    showError(int option);              // Pantalla: Error y sugerencia de solución

// Auxiliares
void    slowDisappearance(int option);      // Desaparecer paulatinamente imagen de crudo o cocinado (option)
void    slowAppearanceImage(int option);    // Aparecer paulatinamente imagen de crudo/cocinado, sugerencias o grupos (option)
void    slowAppearanceAndDisappareanceProcesamiento(int option); // Aparecer y desaparecer simultáneo de crudo y cocinado
void    desplazar_mano(int option);         // Desplazar mano hasta "botón" 
void    sin_pulsacion(int option);          // Fin movimiento mano sin simular pulsación sobre "botón"
void    con_pulsacion(int option);          // Fin movimiento mano simulando pulsación sobre "botón"
// Carga imagenes
void    loadPicturesShowHourglass();        // Cargar imágenes de la SD a memoria SDRAM mientras se dibuja un reloj de arena
void    putReloj1();                        // Cantidad arena en reloj (1)
void    putReloj2();                        // Cantidad arena en reloj (2)
void    putReloj3();                        // Cantidad arena en reloj (3)
void    putReloj4();                        // Cantidad arena en reloj (4)
void    putRelojGirado1();                  // Girando reloj (1)
void    putRelojGirado2();                  // Girando reloj (2)
void    putRelojGirado3();                  // Girando reloj (3)
void    putRelojGirado4();                  // Girando reloj (4)
void    putRelojGirado5();                  // Girando reloj (5)
void    putRelojGirado6();                  // Girando reloj (6)
/*-----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la tarjeta SD.
 * 
 * @return true si la configuración es exitosa, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool setupSD(){
    if(SD.begin(SD_CARD_SCS)) return true;
    else return false;
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura la pantalla y realiza la inicialización necesaria.
 * 
 * Esta función configura el pin de retroiluminación como salida y lo enciende.
 * Luego, inicializa la pantalla RA8876 y verifica si se pudo inicializar correctamente.
 * A continuación, limpia la pantalla y obtiene las dimensiones de la pantalla.
 * Finalmente, muestra un mensaje indicando que la pantalla ha sido inicializada.
 * 
 * @note Esta función debe ser llamada antes de utilizar la pantalla.
 */
/*-----------------------------------------------------------------------------*/
void setupScreen(){
    Serial.println(F("Iniciando pantalla..."));

    pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
    digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight

    if (!tft.init()){
      Serial.println(F("Could not initialize RA8876"));
      while(1);
    }

    // Limpiar pantalla mientras se cargan las imágenes
    tft.canvasImageStartAddress(PAGE1_START_ADDR); 
    tft.clearScreen(BLACK); 

    SCREEN_WIDTH = tft.getWidth(); // X
    SCREEN_HEIGHT = tft.getHeight(); // Y
    Serial.println(F("Screen initialized"));
    delay(200);
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra un fallo crítico en la memoria interna de SmartCloth.
 * 
 * Esta función muestra un mensaje de error en la pantalla LCD indicando un fallo en la memoria interna de SmartCloth.
 * También proporciona instrucciones sobre cómo reiniciar SmartCloth y contactar al equipo de SmartCloth si el fallo persiste.
 */
/*-----------------------------------------------------------------------------*/
void showCriticFailureSD(){

    // ----- TEXTO (FALLO EN MEMORIA INTERNA DE SM - SD) ---------------------
    tft.clearScreen(RED);
    // ------ LINEA ---------
    tft.fillRoundRect(252,110,764,118,3,WHITE);
    // ------ TEXTO ---------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.setCursor(60, 258);  tft.println("\xA1""FALLO EN MEMORIA INTERNA\x21"""); // 12x24 escalado x3
    tft.setCursor(200, 158);  tft.println("\xA1""FALLO EN MEMORIA\x21"""); // 12x24 escalado x3
    // ------ LINEA ---------
    tft.fillRoundRect(252,270,764,278,3,WHITE);
    // -------------------------------------------------------------------


    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setCursor(300, 358); tft.println("REINICIE SMARTCLOTH");  

    tft.setCursor(250,450); tft.println("SI SE MANTIENE EL FALLO,");
    tft.setCursor(100,tft.getCursorY() + tft.getTextSizeY()-10); tft.println("CONTACTE CON EL EQUIPO DE SMARTCLOTH"); 
    // ----------------------------------------------------------------------------------------------------
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra información indicando que se debe sincronizar SmartCloth con la web.
 * Es decir, que hay algo en el fichero TXT que se debe subir.
 * 
 * @param option La opción seleccionada: DATA_TO_UPLOAD, UPLOADING_DATA o NO_INTERNET_CONECTION
 */
/*-----------------------------------------------------------------------------*/
void showDataToUpload(int option){

    tft.clearScreen(WHITE);

    // ------------ RECUADRO ---------------------------------------------------------------------------------
    // Borde
    tft.drawRect(50,110,974,278,BLACK);
    tft.drawRect(51,111,973,277,BLACK);
    tft.drawRect(52,112,972,276,BLACK);
    tft.drawRect(53,113,971,275,BLACK);
    tft.drawRect(54,114,970,274,BLACK);

    // Relleno
    tft.fillRect(55,115,969,273,AMARILLO_CONFIRM_Y_AVISO);
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO);

    switch (option){
        case DATA_TO_UPLOAD:            tft.setCursor(80, 158);     tft.println("SINCRONIZACI\xD3""N NECESARIA");           break; 
        case NO_INTERNET_CONECTION:     tft.setCursor(300, 158);    tft.println("SIN CONEXI\xD3""N");                       break;
        case UPLOADING_DATA:            tft.setCursor(220, 158);    tft.println("SINCRONIZANDO...");                        break; 

        case UPLOADED_DATA:             tft.fillRect(55,115,969,273,VERDE_PEDIR); // Verde de éxito
                                        tft.setTextForegroundColor(WHITE); // Texto blanco
                                        tft.setCursor(80, 158);     tft.println("\xA1""SMARTCLOTH SINCRONIZADO\x21""");     break;
                                        
        case HTTP_ERROR:                tft.fillRect(55,115,969,273,RED); // Rojo de error
                                        tft.setTextForegroundColor(WHITE); // Texto blanco
                                        tft.setCursor(130,158);            tft.println("ERROR EN EL SERVIDOR");             break;
        
        default:    break;
    }
    // ----------------------------------------------------------------------------------------------------



    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setTextForegroundColor(BLACK);
    
    switch (option){
        case DATA_TO_UPLOAD:            tft.setCursor(110, 358);                                        tft.println("COMPROBANDO CONEXI\xD3""N A INTERNET...");         break; 
        
        case NO_INTERNET_CONECTION:     tft.setCursor(200, 358);                                        tft.println("NO HAY CONEXI\xD3""N A INTERNET");
                                        tft.setCursor(50, tft.getCursorY() + tft.getTextSizeY()+20);    tft.println("NO SE PUEDE SINCRONIZAR LA INFORMACI\xD3""N");     break;
        
        case UPLOADING_DATA:            tft.setCursor(50, 358);                                         tft.println("ESPERE MIENTRAS SE COMPLETA LA SUBIDA...");        break; 

        case UPLOADED_DATA:             tft.setCursor(260, 358);                                        tft.println("LA WEB SE HA ACTUALIZADO");                        break;

        case HTTP_ERROR:                tft.setCursor(100, 358);                                        tft.println("FALL\xD3"" LA SINCRONIZACI\xD3""N CON LA WEB");    break;

        default:    break;
    }
    // ----------------------------------------------------------------------------------------------------


    // ------ RELOJ (SUBIENDO DATA) ----------------------------------------------------------------------------
    if(option == UPLOADING_DATA){ // SINCRONIZANDO...

    }

    // ----------------------------------------------------------------------------------------------------

}


/*
void putNextReloj() {
    static int currentReloj = 0;
    currentReloj++;

    switch(currentReloj) {
        case 1:
            putReloj1();
            break;
        case 2:
            putReloj2();
            break;
        case 3:
            putReloj3();
            break;
        // Añade más casos si tienes más imágenes
        default:
            currentReloj = 0;
            putReloj1();
            break;
    }
}
*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Solicita confirmación para borrar un archivo CSV.
 * 
 * Esta función muestra un mensaje en la pantalla para solicitar la confirmación
 * del usuario antes de borrar un archivo CSV. Muestra un texto de confirmación
 * y proporciona instrucciones adicionales para cancelar la operación.
 */
/*-----------------------------------------------------------------------------*/
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




/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que muestra sugerencias de acción en la pantalla.
 * 
 * Esta función muestra una serie de opciones de acción en la pantalla, como añadir más cantidad de alimento, 
 * escoger un nuevo grupo, añadir un plato, borrar un plato actual y guardar la comida. 
 * Cada opción se muestra en una posición específica de la pantalla y se va apareciendo gradualmente.
 */
/*-----------------------------------------------------------------------------*/
void suggestAction(){
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    tft.clearScreen(VERDE_PEDIR); // Fondo verde en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(50, 50);
    tft.println("\xBF""QU\xC9"" QUIERE HACER AHORA\x3F""");
    delay(1000);
    // ----------------------------------------------------------------------------------------------------


    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    // ------------ POSIBLES ACCIONES ---------------------------------------------------------------------
    // 30 pixeles entre cuadro y cuadro
    // Mostrar en PAGE1 (copiar de PAGE3 a PAGE1)

    // ------ Añadir más cantidad de alimento --> scaleG (150x150) ---------
    // Sin recortar bordes de add/delete/save
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,373,293,PAGE1_START_ADDR,SCREEN_WIDTH,56,200,146,147); // Mostrar scaleG (150x150)
    // Recortando bordes de add/delete/save
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,373,293,PAGE1_START_ADDR,SCREEN_WIDTH,69,200,146,147); // Mostrar scaleG (150x150)
    
    tft.setCursor(90, 377);                                       tft.println("A\xD1""ADIR");
    tft.setCursor(115, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("M\xC1""S");
    tft.setCursor(75, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("ALIMENTO");
    // Apareciendo y recortando bordes de add/delete/save
    slowAppearanceImage(3);
    delay(800);


    // ------ Escoger nuevo grupo --> grupo1 (130x1125) ---------
    // Sin recortar bordes de add/delete/save
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,1,1,PAGE1_START_ADDR,SCREEN_WIDTH,232,214,129,124); // Mostrar grupo1 (130x125)
    // Recortando bordes de add/delete/save
//    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,1,1,PAGE1_START_ADDR,SCREEN_WIDTH,245,213,129,124); // Mostrar grupo1 (130x125)
    
    tft.setCursor(250, 377);                                       tft.println("ESCOGER");
    tft.setCursor(275, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("OTRO");
    tft.setCursor(270, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("GRUPO");
    // Apareciendo y recortando bordes de add/delete/save
    slowAppearanceImage(4);
    delay(800);


    // ------ Añadir plato --> anadir (172x130) ---------
    // Sin recortar bordes de add/delete/save
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,391,205,172,130); // Mostrar añadir (172x130)
    // Recortando bordes de add/delete/save
 //   tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,652,0,PAGE1_START_ADDR,SCREEN_WIDTH,404,206,158,130); // Mostrar añadir (172x130)
    
    tft.setCursor(430, 377);                                       tft.println("A\xD1""ADIR");
    tft.setCursor(445, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("OTRO");
    tft.setCursor(440, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("PLATO");
    // Apareciendo y recortando bordes de add/delete/save
    slowAppearanceImage(5);
    delay(800);


    // ------ Borrar plato --> borrar (172x130) ---------
    // Sin recortar bordes de add/delete/save
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,593,205,172,130); // Mostrar borrar (172x130) 
    // Recortando bordes de add/delete/save
 //   tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,825,0,PAGE1_START_ADDR,SCREEN_WIDTH,592,206,158,130); // Mostrar borrar (172x130) 
    
    tft.setCursor(617, 377);                                       tft.println("BORRAR");
    tft.setCursor(627, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("PLATO");
    tft.setCursor(617, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("ACTUAL");
    // Apareciendo y recortando bordes de add/delete/save
    slowAppearanceImage(6);
    delay(800);


    // ------ Guardar comida --> guardar (172x130) ---------
    // Sin recortar bordes de add/delete/save
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,795,205,172,130); // Mostrar guardar (172x130)
    // Recortando bordes de add/delete/save
  //  tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,7,131,PAGE1_START_ADDR,SCREEN_WIDTH,780,206,158,130); // Mostrar guardar (172x130)
    
    tft.setCursor(795, 377);                                       tft.println("GUARDAR");
    tft.setCursor(800, tft.getCursorY() + tft.getTextSizeY()-5);  tft.println("COMIDA");
    // Apareciendo y recortando bordes de add/delete/save
    slowAppearanceImage(7);
    delay(800);
    // ----------------------------------------------------------------------------------------------------
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que muestra la pantalla inicial.
 * 
 * Esta función muestra en la pantalla un mensaje de instrucción y dibuja diferentes elementos gráficos, como texto, círculos, cuadrados redondeados y palitos.
 * También copia una imagen de un cerebro en la pantalla.
 */
/*-----------------------------------------------------------------------------*/
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




/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que muestra una selección de grupos de alimentos en la pantalla.
 * 
 * Esta función muestra una serie de grupos de alimentos en la pantalla, permitiendo al usuario seleccionar uno de ellos.
 * Los grupos se muestran en forma de imágenes y se alternan entre una pulsación y una no pulsación para indicar la interacción del usuario.
 */
/*-----------------------------------------------------------------------------*/
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
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125); // x = 236  ->  y = 288
    slowAppearanceImage(8);
    delay(800);

    // ------ Grupo 2 (130x125) ---------
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125); // x = <grupo1(236) + grupo1(130) + 30 = 396  ->  y = 288
    slowAppearanceImage(9);
    delay(800);

    // ------ Grupo 3 (130x125) ---------
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); // x = <grupo2(396) + grupo2(130) + 30 = 556  ->  y = 288
    slowAppearanceImage(10);
    delay(800);

    // ------ Grupo 4 (130x125) ---------
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125); // x = <grupo3(556) + grupo3(130) + 30 = 716  ->  y = 288
    slowAppearanceImage(11);
    delay(800);
    // ----------------------------------------------------------------------------------------------------


    // ------------ DESPLAZAR MANO PARA SIMULAR MOVIMIENTO ------------------------------------------------
    // MANO por el camino
    desplazar_mano(1); // Grupos
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
            con_pulsacion(1); // Grupos
        }
        else{
          Serial.println("No pulsacion");
          sin_pulsacion(1); // Grupos
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



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que muestra una pregunta en la pantalla sobre si un alimento está cocinado o crudo.
 * 
 * Esta función muestra una pregunta en la pantalla sobre si un alimento está cocinado o crudo.
 * La pregunta se muestra en un fondo verde y utiliza una fuente de tamaño 32.
 * Además, se dibujan líneas y se muestran botones para alternar entre las opciones de cocinado y crudo.
 */
/*-----------------------------------------------------------------------------*/
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
    slowAppearanceImage(1); // option = 1 --> imagen cocinado con 16/32 a 7/32 de opacidad y luego sin opacidad
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



// SUPERPUESTO AL DASHBOARD PARA OBLIGAR A ESCOGER CRUDO/COCINADO
/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que muestra en la pantalla un dashboard con dos zonas: una para el grupo actual y ejemplos, y otra para las opciones de cocinado y crudo.
 * 
 * La función dibuja los recuadros y texto correspondientes a cada zona del dashboard, así como los botones para seleccionar entre cocinado y crudo.
 * También se aplican efectos visuales como resaltados y opacidad.
 */
/*-----------------------------------------------------------------------------*/
void crudo_cocinado_sobre_dashboard(){
  int x1, y1, x2, y2;

  tft.clearScreen(AZUL_FONDO); // Fondo azul oscuro en PAGE1

    // ---------- ZONAS QUE SE MANTIENEN  ---------------------------------------------------------------
      // -------------- ZONA 1 ----------------------------------------------------
        // Recuadro grupo y ejemplos
        tft.fillRoundRect(30,20,932,135,20,GRIS_CUADROS); // 902 x 115
        
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
      // -------------- FIN ZONA 1 ------------------------------------------------

      // -------------- ZONA 2 ----------------------------------------------------
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
      // -------------- FIN ZONA 2 ------------------------------------------------
    // ---------------------------------------------------------------------------------------------------


    delay(1000);

    // ---------- ZONAS QUE SE TAPAN ----------------------------------------------------------------------
      // -------------- ZONAS 3 Y 4 -----------------------------------------------
        // ------- GRÁFICOS -------------------------------------
        // Recuadro tapando zonas 3 y 4 del dashboard
        tft.fillRoundRect(30,145,994,580,20,VERDE_PEDIR); // 964 x 435
        tft.drawRoundRect(30,145,994,580,20,VERDE_BORDE_PEDIR_COCCION); // Borde => 964 x 435
        // ------ LINEAS -----
        tft.fillRoundRect(30,330,256,338,3,WHITE);
        tft.fillRoundRect(768,527,994,535,3,WHITE);
        // ------- FIN GRÁFICOS ---------------------------------

        delay(500);

        // ----- TEXTO (PREGUNTA) -------------------------------
        tft.selectInternalFont(RA8876_FONT_SIZE_32);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setTextForegroundColor(WHITE); 
        tft.setCursor(230, 185);                                        tft.println("\xBF""EL ALIMENTO EST\xC1""");
        tft.setCursor(250, tft.getCursorY() + tft.getTextSizeY()-20);   tft.print("COCINADO O CRUDO\x3F"""); 
        delay(500);
        // ------- FIN TEXTO ------------------------------------

        // ----- BOTONES ----------------------------------------
        // COCINADO
        //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,230,220,177,160); // Mostrar cociGra (177x160) en PAGE1
        tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,280,350,177,160); // Mostrar cociGra (177x160) en PAGE1
        delay(500);

        /* QUEDA FEO PORQUE PIERDE LAS ESQUINAS REDONDAS
        // CUADRADO REDONDEADO ALREDEDOR DE BOTONES PARA "ELIMINAR" ESQUINAS OSCURAS    
          // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
          // cuadrados separados por 1 píxel en cada dirección, para simular un grosor mayor.
        for (int i = 0; i <= 20; i++) {
              x1 = 284 - i;   y1 = 353 - i;   // x1 +4    y1 +3
              x2 = 452 + i;   y2 = 507 + i;   // x2 -5    y2 -3
              tft.drawRect(x1,y1,x2,y2,VERDE_PEDIR); // Alrededor de botón
              //tft.drawRoundRect(x1,y1,x2,y2,20,VERDE_PEDIR);
          }
        delay(500);
        */

        // CRUDO
        //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,617,220,177,160); // Mostrar crudoGra (177x160) en PAGE1
        tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,351,131,PAGE1_START_ADDR,SCREEN_WIDTH,567,350,177,160); // Mostrar crudoGra (177x160) en PAGE1
        //delay(500);

        /* QUEDA FEO PORQUE PIERDE LAS ESQUINAS REDONDAS
        // CUADRADO REDONDEADO ALREDEDOR DE BOTONES PARA "ELIMINAR" ESQUINAS OSCURAS    
          // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
          // cuadrados separados por 1 píxel en cada dirección, para simular un grosor mayor.
        for (int i = 0; i <= 20; i++) {
              x1 = 571 - i;   y1 = 353 - i;   
              x2 = 739 + i;   y2 = 507 + i;
              tft.drawRect(x1,y1,x2,y2,VERDE_PEDIR); // Alrededor de botón
              //tft.drawRoundRect(x1,y1,x2,y2,20,VERDE_PEDIR);
          }
          */
        // ----- FIN BOTONES ------------------------------------


      // -------------- FIN ZONAS 3 Y 4 -------------------------------------------
    // ----------------------------------------------------------------------------------------------------


}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para colocar un alimento en la zona de pesada.
 * 
 * Esta función muestra un mensaje en la pantalla indicando al usuario que coloque un alimento en la zona de pesada.
 * También dibuja líneas en la pantalla y muestra una imagen de una báscula.
 * 
 * @note Esta función tiene un retardo de 1 segundo después de mostrar cada elemento en la pantalla.
 */
/*-----------------------------------------------------------------------------*/
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
    // Scale 
    slowAppearanceImage(2); // option = 2 --> imagen scale con 16/32 a 7/32 de opacidad y luego sin opacidad
    delay(1000);

}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Agrega un nuevo plato.
 * 
 * Esta función muestra un mensaje de confirmación en la pantalla y permite al usuario
 * añadir un nuevo plato. Primero muestra un mensaje de pregunta en un fondo amarillo,
 * luego muestra un botón de confirmación en un fondo rojo. Después de que el usuario
 * confirma la acción, se muestra un botón de añadir en la pantalla y se simula el
 * movimiento de una mano. Finalmente, se muestra un mensaje de plato añadido en un
 * fondo verde.
 */
/*-----------------------------------------------------------------------------*/
void add_Plato(){ // Tb PAGE3, pero más abajo ==> HECHO
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
    desplazar_mano(2); // Añadir
    // ----------------------------------------------------------------------------------------------------
  

    // ******** ALTERNANCIA 2 PULSACIONES *****************************************************************
    // Tras trasladar la mano, no hay pulsación. Se van a hacer dos alternancias de pulsación,
    // es decir: pulsacion - no pulsacion - pulsacion - no pulsacion
    delay(1000);

    bool pulsacion = true;

    for(int i = 0; i < 4; i++){
        if(pulsacion) con_pulsacion(2); // Añadir
        else sin_pulsacion(2); // Añadir
        //if(i < 3){ // No hacer el delay si es la última pulsación/no pulsación (debería ser no pulsación)
            // ----- ESPERA E INTERRUPCION ----------------
            delay(1000);
        //}
        pulsacion = !pulsacion;
    }
    // ****************************************************************************************************

    // ----------------------------------------------------------------------------------------------------

    delay(2000);

    // ----- TEXTO (PLATO AÑADIDO) -------------------------------------------------------------------------
    tft.clearScreen(VERDE_PEDIR);
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



/*-----------------------------------------------------------------------------*/
/**
 * @brief Elimina el plato actual.
 * 
 * Esta función muestra un mensaje de confirmación para borrar el plato actual en la pantalla.
 * Luego, muestra un botón de eliminar y simula el movimiento de una mano para pulsar el botón.
 * Después de dos alternancias de pulsación y no pulsación, muestra un mensaje indicando que el plato ha sido eliminado.
 */
/*-----------------------------------------------------------------------------*/
void delete_Plato(){ // Tb PAGE3, pero más a la derecha ==> HECHO
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO); // Fondo rojo en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(30, 30);
    tft.println("\xBF""EST\xC1"" SEGURO DE QUE QUIERE");
    tft.setCursor(110, tft.getCursorY() + tft.getTextSizeY()-40); // -30
    tft.print("BORRAR EL PLATO ACTUAL\x3F"""); 
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
    tft.setCursor(400, tft.getCursorY() + tft.getTextSizeY()-10); // +10
    tft.print("EL BOT\xD3""N"); 
    // ----------------------------------------------------------------------------------------------------
    delay(500);

    // ------------ BOTÓN ELIMINAR ------------------------------------------------------------------------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar borrar (172x130) en PAGE1
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,410,172,130); // Mostrar borrar (172x130) en PAGE1
    delay(800);
    // ----------------------------------------------------------------------------------------------------

    // ------------ DESPLAZAR MANO PARA SIMULAR MOVIMIENTO ------------------------------------------------
    // MANO por el camino
    desplazar_mano(3); // Eliminar
    // ----------------------------------------------------------------------------------------------------
  

    // ******** ALTERNANCIA 2 PULSACIONES *****************************************************************
    // Tras trasladar la mano, no hay pulsación. Se van a hacer dos alternancias de pulsación,
    // es decir: pulsacion - no pulsacion - pulsacion - no pulsacion
    delay(1000);

    bool pulsacion = true;

    for(int i = 0; i < 4; i++){
        if(pulsacion) con_pulsacion(3); // Eliminar
        else sin_pulsacion(3); // Eliminar
        //if(i < 3){ // No hacer el delay si es la última pulsación/no pulsación (debería ser no pulsación)
            // ----- ESPERA E INTERRUPCION ----------------
            delay(1000);
        //}
        pulsacion = !pulsacion;
    }
    // ****************************************************************************************************


    delay(2000);

    // ----- TEXTO (PLATO BORRADO) -------------------------------------------------------------------------
    tft.clearScreen(VERDE_PEDIR);
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


/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda la comida actual y muestra una confirmación en la pantalla.
 * 
 * Esta función muestra un mensaje de confirmación en la pantalla para que el usuario confirme si desea guardar la comida actual.
 * Si el usuario confirma, se realiza la acción de guardar la comida y se muestra una animación de pulsación en el botón de guardar.
 * 
 * @note Esta función utiliza una serie de funciones auxiliares para mostrar texto, dibujar formas y realizar animaciones.
 * 
 * @note Esta función tiene una duración aproximada de 10 segundos debido a las pausas y animaciones.
 */
/*-----------------------------------------------------------------------------*/
void save_Comida(bool conexion){ // Tb PAGE3, pero más a la derecha
    // ----- TEXTO (PREGUNTA) ----------------------------------------------------------------------------
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO); // Fondo AMARILLO en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO); 
    //tft.ignoreTextBackground();       // Activa la transparencia igual que ==> tft.setTextBackgroundTrans(RA8876_TEXT_TRANS_ON);
    tft.setCursor(30, 20);
    tft.println("\xBF""EST\xC1"" SEGURO DE QUE QUIERE");
    tft.setCursor(80, tft.getCursorY() + tft.getTextSizeY()-40); // -30
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
    tft.fillRoundRect(252,235,764,243,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    //tft.fillRoundRect(252,250,764,258,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    // ----------------------------------------------------------------------------------------------------
     delay(500);


    // ----- TEXTO (CONFIRMACIÓN) -------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
    tft.setCursor(150, 275);
    //tft.setCursor(150, 300);
    tft.println("PARA CONFIRMAR, PULSE DE NUEVO");
    tft.setCursor(400, tft.getCursorY() + tft.getTextSizeY()-10); // -10
    tft.print("EL BOT\xD3""N"); 
    // ----------------------------------------------------------------------------------------------------
    delay(500);

    // ------------ BOTÓN GUARDAR ------------------------------------------------------------------------
    tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,400,172,130); // Mostrar guardar (172x130) en PAGE1
    // tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,450,172,130); // Mostrar guardar (172x130) en PAGE1
    delay(800);
    // ----------------------------------------------------------------------------------------------------

    // ------------ DESPLAZAR MANO PARA SIMULAR MOVIMIENTO ------------------------------------------------
    // MANO por el camino
    desplazar_mano(4); // Guardar
    // ----------------------------------------------------------------------------------------------------


    if(conexion){ // si hay conexion WiFi
        // ----- CON WIFI -----
        // "Resaltar" texto:
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
        tft.setTextColor(WHITE,VERDE_PEDIR,RA8876_TEXT_TRANS_OFF); // Texto blanco remarcado con fondo morado oscuro, se superpone al fondo verde del canvas (RA8876_TEXT_TRANS_OFF)
        // Texto
        tft.setCursor(20,550);    tft.println(" CONECTADO A INTERNET ");
        // Eliminar "resaltado" del texto de aquí en adelante:
        tft.ignoreTextBackground(); // Ignorar el color de background del texto que haya y mostrar fondo canvas
        // ------------------------
    }
    else{
        // ----- SIN INTERNET -----
        // "Resaltar" texto:
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
        tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_OFF); // Texto blanco remarcado con fondo rojo, se superpone al fondo verde del canvas (RA8876_TEXT_TRANS_OFF)
        // Texto
        tft.setCursor(850,550); tft.println(" SIN INTERNET "); 
        // Eliminar "resaltado" del texto de aquí en adelante:
        tft.ignoreTextBackground(); // Ignorar el color de background del texto que haya y mostrar fondo canvas
        // ------------------------
    }

    // ******** ALTERNANCIA 2 PULSACIONES *****************************************************************
    // Tras trasladar la mano, no hay pulsación. Se van a hacer dos alternancias de pulsación,
    // es decir: pulsacion - no pulsacion - pulsacion - no pulsacion
    delay(1000);

    bool pulsacion = true;

    for(int i = 0; i < 4; i++){
        if(pulsacion) con_pulsacion(4); // Guardar
        else sin_pulsacion(4); // Guardar
        //if(i < 3){ // No hacer el delay si es la última pulsación/no pulsación (debería ser no pulsación)
            // ----- ESPERA E INTERRUPCION ----------------
            delay(1000);
        //}
        pulsacion = !pulsacion;
    }
    // ****************************************************************************************************

  


/*
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
    tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,128,WHITE); // manoWppt (120x128)    
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,WHITE); // handW (120x129)
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,RED); // manoR
    //tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,524,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,129,GREEN_HAND); // manoG
    // ----------------------------------------------------------------------------------------------------


    delay(2000);

    // ----- TEXTO (COMIDA GUARDADA) -------------------------------------------------------------------------
    tft.clearScreen(VERDE_PEDIR);
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

*/
}



/*-----------------------------------------------------------------------------*/
/**
 * Muestra la acción realizada en la pantalla TFT.
 * 
 * @param option La opción de la acción realizada.
 *               Los valores posibles son:
 *               - ADD_EXECUTED: Plato añadido.
 *               - DELETE_EXECUTED: Plato eliminado.
 *               - SAVE_EXECUTED_FULL: Comida guardada en local y en la base de datos.
 *               - SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP: Comida guardada solo en local debido a un error en la petición HTTP.
 *               - SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI: Comida guardada solo en local debido a la falta de conexión WiFi.
 *               - SAVE_ESP32_TIMEOUT: Comida guardada solo en local debido a un timeout en la respuesta del ESP32.
 *               - SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR: Comida guardada solo en local debido a un error desconocido al guardar en la base de datos.
 */
/*-----------------------------------------------------------------------------*/
void showAccionRealizada(int option){
    tft.clearScreen(VERDE_PEDIR);

    // ------------ LINEA ---------------------------------------------------------------------------------
    tft.fillRoundRect(252,150,764,158,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (ACCION REALIZADA) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(WHITE); 

    switch (option){
      case ADD_EXECUTED:               tft.setCursor(170, 208);   tft.println("NUEVO PLATO A\xD1""ADIDO");  break; // PLATO AÑADIDO
      case DELETE_EXECUTED:            tft.setCursor(100, 208);   tft.println("PLATO ACTUAL ELIMINADO");    break; // PLATO ELIMINADO
      case SAVE_EXECUTED_FULL:  
      case SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP:   
      case SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI:    
      case SAVE_ESP32_TIMEOUT:
      case SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR:
                                       tft.setCursor(120, 208);   tft.println("COMIDA ACTUAL GUARDADA");    break; // COMIDA GUARDADA AL MENOS EN LOCAL
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
              
      case SAVE_EXECUTED_FULL: // GUARDADA EN LOCAL Y DATABASE
      case SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP:   // GUARDADA SOLO EN LOCAL POR FALLO EN PETICION HTTP
      case SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI:  // GUARDADA SOLO EN LOCAL POR NO TENER WIFI
      case SAVE_ESP32_TIMEOUT:  // TIMEOUT EN LA RESPUESTA DEL ESP32. NO SABEMOS SI HA GUARDADO O NO, PERO ASUMIMOS QUE NO
      case SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR:  // GUARDADA SOLO EN LOCAL POR UN ERROR DESCONOCIDO AL GUARDAR EN DATABASE
                // No se pone if(pesoARetirar ...) porque aún no ha dado tiempo a actualizar 'pesoARetirar' y puede ser incorrecto
                //if(lastValidState == STATE_Init){
                    // Puede ser que se quiera guardar desde el STATE_Init, tras añadir o borrar. Si es así,
                    // la báscula estará vacía (pesoARetirar = 0).
                    //tft.setCursor(190, 388); tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO");
                    //tft.setCursor(350, tft.getCursorY() + tft.getTextSizeY()+40); tft.print("AL ACUMULADO DE HOY"); 
                //}
                //else{ 
                    // El siguiente mensaje solo se mostrará si se ha querido guardar tras conformar el plato,
                    // estando aún en la báscula.
                    tft.setCursor(30, 388); tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO AL ACUMULADO DE HOY");  
                    tft.setCursor(200,450); tft.println("RETIRE EL PLATO PARA COMENZAR DE NUEVO"); 
                //}

                tft.selectInternalFont(RA8876_FONT_SIZE_24);
                tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
            
                if (option == SAVE_EXECUTED_FULL) // SE HA GUARDADO LOCAL Y DATABASE
                    tft.setTextColor(WHITE,DARKPURPLE,RA8876_TEXT_TRANS_OFF); // Texto blanco sobre fondo morado oscuro
                else // SOLO SE HA GUARDADO EN LOCAL
                    tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_OFF); // Texto blanco sobre fondo rojo
            

                switch(option){
                    case SAVE_EXECUTED_FULL:                        tft.setCursor(20,550);    tft.println(" SUBIDO A WEB ");                        break;
                    case SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP:       tft.setCursor(785,550);   tft.println(" ERROR EN EL ENV\xCD""O ");              break;
                    case SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI:          tft.setCursor(810,550);   tft.println(" NO SUBIDO A WEB ");                        break;
                    case SAVE_ESP32_TIMEOUT:                        tft.setCursor(705,550);   tft.println(" ERROR EN ENV\xCD""O (TIMEOUT) ");       break;
                    case SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR:    tft.setCursor(790,550);   tft.println(" ERROR DESCONOCIDO ");                   break;
                }

                // Eliminar "resaltado" del texto de aquí en adelante:
                tft.ignoreTextBackground(); // Ignorar el color de background del texto que haya y mostrar fondo canvas

                break; 

    }
    // ----------------------------------------------------------------------------------------------------

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que muestra el panel de control en la pantalla.
 * 
 * Esta función se encarga de mostrar en la pantalla el panel de control
 * con la información del grupo actual, el plato actual y el acumulado
 * del día. Se dibujan los gráficos y se muestran los valores correspondientes
 * a cada sección.
 */
/*-----------------------------------------------------------------------------*/
void dashboard(){ // OK ==> HECHO
    float raciones;
    int integerPart;

    // NO HACE FALTA VOLVER A PAG1, YA SE HACE EN loadPicturesShowHourglass()
    //tft.canvasImageStartAddress(PAGE1_START_ADDR); // Regresar a PAGE1
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

    // Raciones de Carbohidratos
    /*raciones = 1.0;
    integerPart = abs((int)raciones);
    if((integerPart == 0) or ((integerPart/10) == 0)){ // 1 cifra entera
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
    if((integerPart == 0) or ((integerPart/10) == 0)){ // 1 cifra entera
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
    tft.print("0.5"); // 24 escale x2*/

    // ------------ Raciones de Carbohidratos ------------
    /*
        CARB: 22.2 --> 2.20
        PROT: 38.3 --> 3.80
        LIP: 43.6 --> 4.40

        CARB: 1.3 --> 0.10
        PROT: 0.4 --> 0.00
        LIP: 9.9 --> 1.00
    */
    raciones = 50.50;
    // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
    if(abs((int)raciones) < 10){   // 1 cifra entera
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);
        tft.setCursor(406,293);
    } 
    else { // 2 o más cifras enteras
        tft.selectInternalFont(RA8876_FONT_SIZE_16);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X3); // Alargar
        //tft.setCursor(416,293); 
        tft.setCursor(408,291);
    }         
    tft.print(raciones,1); // Mostrar 1 decimal, para evitar el 0 que se queda al final (p.ej. 3.20 muestra 3.2)
    



    // ------------ Raciones de Proteinas ------------ 
    raciones = 0.00;
    // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
    if(abs((int)raciones) < 10){   // 1 cifra entera
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setCursor(406,370);
    } 
    else{ // 2 o más cifras enteras
        tft.selectInternalFont(RA8876_FONT_SIZE_16);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X3); // Alargar
        //tft.setCursor(416,370); 
        tft.setCursor(408,368);        
    } 
    tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura



    // ---------- Raciones de Grasas ------------
    raciones = 1.00;
    // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
    if(abs((int)raciones) < 10){   // 1 cifra entera
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setCursor(406,447);
    } 
    else{ // 2 o más cifras enteras
        tft.selectInternalFont(RA8876_FONT_SIZE_16);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X3); // Alargar
        //tft.setCursor(416,447);  
        tft.setCursor(408,445);         
    }   
    tft.print(raciones,1); // Si no termina en .0 , termina en .5 , entonces sí lo mostramos. 12x24 escale x2 solo altura

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
    //tft.selectInternalFont(RA8876_FONT_SIZE_24);
    //tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    // ------------ Raciones de Carbohidratos ------------
    /*
    // Raciones de Carbohidratos
    tft.setCursor(906,293);
    tft.setTextForegroundColor(WHITE); 
    tft.print("10"); // 24 escale x2
    */
    raciones = 20.50;
    // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
    if(abs((int)raciones) < 10){   // 1 cifra entera
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setCursor(896,293);
    } 
    else{ // 2 o más cifras enteras
        tft.selectInternalFont(RA8876_FONT_SIZE_16);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X3); // Alargar
        //tft.setCursor(906,293); 
        tft.setCursor(898,291);
    }          
    tft.print(raciones,1); // Mostrar 1 decimal, para evitar el 0 que se queda al final (p.ej. 3.20 muestra 3.2)
    

    // ------------ Raciones de Proteinas ------------ 
    /*
    // Raciones de Proteinas
    tft.setCursor(916,370);
    tft.setTextForegroundColor(WHITE); 
    tft.print("7"); // 24 escale x2
    */
    raciones = 7.60;
    // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
    if(abs((int)raciones) < 10){   // 1 cifra entera
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setCursor(896,370);
    } 
    else{ // 2 o más cifras enteras
        tft.selectInternalFont(RA8876_FONT_SIZE_16);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X3); // Alargar
        //tft.setCursor(906,370);
        tft.setCursor(898,368);        
    } 
    tft.print(raciones,1); // Mostrar 1 decimal, para evitar el 0 que se queda al final (p.ej. 3.20 muestra 3.2)

    
    // ---------- Raciones de Grasas ------------
    /*
    // Raciones de Grasas
    tft.setCursor(916,447);
    tft.setTextForegroundColor(WHITE); 
    tft.print("5"); // 24 escale x2
    */
    raciones = 1.00;
    // Cursor y tamaño según la cantidad de cifras enteras para centrar en el cuadro
    if(abs((int)raciones) < 10){   // 1 cifra entera
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setCursor(896,447);
    } 
    else{ // 2 cifras enteras
        tft.selectInternalFont(RA8876_FONT_SIZE_16);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X3); // Alargar
        //tft.setCursor(906,447);  
        tft.setCursor(898,445);         
    }   
    tft.print(raciones,1); // Mostrar 1 decimal, para evitar el 0 que se queda al final (p.ej. 3.20 muestra 3.2)
    // ---- FIN RACIONES ----
    // -------- FIN TEXTO --------------------
    // -------------- FIN ACUMULADO HOY ---------------------------------------



}






/*---------------------------------------------------------------------------------------------------------
   showError(): Pantalla genérica de error que indica una acción incorrecta. Según la opción indicada como
                parámetro, dependiente del estado actual, se muestra un mensaje diferente de acción posible.
          Parámetros:
                option - 1: Init         2: Plato          3: grupoA       4: grupoB   5: Crudo    
                         6: Cocinado     7: Pesado         8: add_check    9: Added    10: delete_check     
                         11: Deleted     12: save_check    13: Saved
----------------------------------------------------------------------------------------------------------*/
void showError(int option){ 

    // cruz
    //tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
    //tft.sdCardDraw16bppBIN256bits(0,292,114,127,fileCruz); // Cargar cruz (114x127) en PAGE3 =>  x  =  0  ->   y = <crudoGra(131) + crudoGra(160) + 1 = 292

    //tft.canvasImageStartAddress(PAGE1_START_ADDR); 

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

      case ERROR_STATE_GROUP: // Grupos (grupoA o groupB)
              //if(keepErrorScreen){ // Se ha colocado peso sin crudo/cocinado. Se mantiene la pantalla de error hasta que se retire el plato entero
                  tft.setCursor(80, 420); tft.println("HA OLVIDADO ESCOGER COCINADO O CRUDO"); 
                  /*
                  tft.selectInternalFont(RA8876_FONT_SIZE_32);
                  tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
                  tft.setCursor(200,tft.getCursorY() + tft.getTextSizeY()); tft.println("RETIRE EL PLATO PARA COMENZAR DE NUEVO"); 
                  
                  tft.selectInternalFont(RA8876_FONT_SIZE_32);
                  tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
                  tft.setCursor(130,tft.getCursorY() + tft.getTextSizeY()); tft.println("TAMBI\xC9""N PUEDE RETIRAR EL PLATO PARA COMENZAR DE NUEVO"); 
                  */
              /*}
              else{ // Se ha pulsado botón que no toca (añadir, borrar o guardar)
                  tft.setCursor(190, 420);                                     tft.println("SELECCIONE COCINADO O CRUDO"); 
                  //tft.setCursor(200, tft.getCursorY() + tft.getTextSizeY());   tft.print("ANTES DE PESAR EL ALIMENTO"); 
              }*/
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


}









/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que realiza el arranque del programa.
 * 
 * Esta función se encarga de limpiar la pantalla TFT y mostrar en orden las letras del texto "SMARTCLOTH" 
 * y el logo correspondiente.
 * 
 * @note Esta función utiliza la biblioteca RA8876_v2 para realizar las operaciones de copia de memoria.
 */
/*-----------------------------------------------------------------------------*/
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




//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


/*-----------------------------------------------------------------------------*/
/**
 * @brief Realiza una desaparición lenta de elementos en la pantalla.
 * 
 * @param option Opción que determina qué elemento se va a desvanecer.
 *               1: Cocinado
 *               2: Crudo
 */
/*-----------------------------------------------------------------------------*/
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


/*-----------------------------------------------------------------------------*/
/**
 * @brief Realiza un proceso de aparición y desaparición lento según la opción proporcionada.
 * 
 * @param option La opción que determina el proceso a realizar.
 *               1: Desaparecer CRUDO y aparecer COCINADO.
 *               2: Desaparecer COCINADO y aparecer CRUDO.
 */
/*-----------------------------------------------------------------------------*/
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


/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que muestra una imagen con una apariencia lenta y gradual.
 * 
 * @param option Opción que determina qué imagen mostrar.
 *               1: Cocinado
 *               2: ScaleG
 *               3: ScaleG sugerencias
 *               4: grupo1 sugerencias
 *               5: añadir sugerencias
 *               6: borrar sugerencias
 *               7: guardar sugerencias
 *               8: GRUPO1
 *               9: GRUPO2
 *               10: GRUPO3
 *               11: GRUPO4
 */
/*-----------------------------------------------------------------------------*/
void slowAppearanceImage(int option){
    uint8_t i;
    switch(option){
        case 1: // Cocinado
            for(i = 32; i >= 1; i--){
                // Mostrar cociGra apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,173,131,PAGE1_START_ADDR,SCREEN_WIDTH,300,300,177,160); // Mostrar sin transparencia
            break;

        case 2: // ScaleG
            for(i = 32; i >= 1; i--){
                // Mostrar scaleG apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,372,293,PAGE1_START_ADDR,SCREEN_WIDTH,1,320,PAGE1_START_ADDR,SCREEN_WIDTH,437,320,150,149,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,373,293,PAGE1_START_ADDR,SCREEN_WIDTH,437,320,146,147); // Mostrar sin transparencia
            break;

        case 3: // ScaleG sugerencias
            for(i = 32; i >= 1; i--){
                // Mostrar scaleG apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,372,293,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,69,200,146,147,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,373,293,PAGE1_START_ADDR,SCREEN_WIDTH,69,200,146,147); // Mostrar scaleG (150x150)
            break;

        case 4: // grupo1 sugerencias
            for(i = 32; i >= 1; i--){
                // Mostrar grupo1 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,1,1,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,245,213,129,124,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,1,1,PAGE1_START_ADDR,SCREEN_WIDTH,245,213,129,124); // Mostrar grupo1 (130x125)
            break;

        case 5: // añadir sugerencias
            for(i = 32; i >= 1; i--){
                // Mostrar añadir apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,652,0,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,404,206,158,130,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,652,0,PAGE1_START_ADDR,SCREEN_WIDTH,404,206,158,130); // Mostrar añadir (172x130)
            break;

        case 6: // borrar sugerencias
            for(i = 32; i >= 1; i--){
                // Mostrar borrar apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,825,0,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,592,206,158,130,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,825,0,PAGE1_START_ADDR,SCREEN_WIDTH,592,206,158,130); // Mostrar borrar (172x130) 
            break;

        case 7: // guardar sugerencias
            for(i = 32; i >= 1; i--){
                // Mostrar guardar apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,7,131,PAGE1_START_ADDR,SCREEN_WIDTH,873,450,PAGE1_START_ADDR,SCREEN_WIDTH,780,206,158,130,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,7,131,PAGE1_START_ADDR,SCREEN_WIDTH,780,206,158,130); // Mostrar guardar (172x130)
            break;

        case 8: // GRUPO1
            for(i = 32; i >= 1; i--){
                // Mostrar guardar apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,288,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,236,288,130,125); // x = 236  ->  y = 288
            break;


        case 9: // GRUPO2
            for(i = 32; i >= 1; i--){
                // Mostrar guardar apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,288,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,131,0,PAGE1_START_ADDR,SCREEN_WIDTH,396,288,130,125); // x = <grupo1(236) + grupo1(130) + 30 = 396  ->  y = 288
            break;


        case 10: // GRUPO3
            for(i = 32; i >= 1; i--){
                // Mostrar guardar apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,288,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); // x = <grupo2(396) + grupo2(130) + 30 = 556  ->  y = 288
            break;


        case 11: // GRUPO4
            for(i = 32; i >= 1; i--){
                // Mostrar guardar apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
                // i = 16 --> RA8876_ALPHA_OPACITY_16
                tft.bteMemoryCopyWithOpacity(PAGE3_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,288,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125,i);
                delay(10);
            }
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,393,0,PAGE1_START_ADDR,SCREEN_WIDTH,716,288,130,125); // x = <grupo3(556) + grupo3(130) + 30 = 716  ->  y = 288
            break;

        default: break;
    }
    
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Desplaza la mano en la pantalla según la opción seleccionada.
 * 
 * @param option La opción seleccionada.
 *               1: Desplazar la mano en grupos.
 *               2: Añadir.
 *               3: Eliminar.
 *               4: Guardar.
 */
/*-----------------------------------------------------------------------------*/
void desplazar_mano(int option){
    int alto, posY;

    switch(option){
        case 1: // Grupos
              // 1 - MANO por el camino
              alto = 127; posY = 480;
              while(posY >= 410){
                  // manoGppt
                  tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,posY,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)
                  delay(50);
                  if(posY < 413) posY = 413; // Solo afecta al penúltimo movimiento de la mano, para evitar que se borre parte del grupo3 que está debajo
                  tft.clearArea(556,posY,690,posY + alto+5,VERDE_PEDIR); // Desaparece de esa zona para aparecer en otra --> se mueve
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

              break;

        case 2: // Añadir
        case 3: // Eliminar
              // 1 - MANO por el camino
              alto = 128; posY = 580;
              while(posY >= 510){
                  // manoWppt
                  tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,posY,120,128,WHITE); // Mostrar manoWppt (120x128)
                  delay(50);
                  tft.clearArea(430,posY,567,posY + alto,AMARILLO_CONFIRM_Y_AVISO); // Desaparece de esa zona para aparecer en otra --> se mueve
                  posY -= 10; // Subimos verticalmente la imagen 10 píxeles
              }
              
              // 2 - Botón correspondiente --> para superponerse a la última mano y que desaparezca para simular el movimiento
              if(option == 2) tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) 
              else tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar borrar (172x130)
              
              // 3 - Movimiento final de la mano (manoWppt)
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,472,120,128,WHITE); // Mostrar manoWppt (120x128)
              delay(50);
              
              break;


        case 4: // Guardar
              // 1 - MANO por el camino
              alto = 128; posY = 590;
              while(posY >= 530){
                  // manoWppt
                  tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,420,posY,120,128,WHITE); // Mostrar manoWppt (120x128)
                  delay(50);
                  tft.clearArea(420,posY,557,posY + alto,AMARILLO_CONFIRM_Y_AVISO); // Desaparece de esa zona para aparecer en otra --> se mueve
                  posY -= 10; // Subimos verticalmente la imagen 10 píxeles
              }
              
              // 2 - Botón guardar --> para superponerse a la última mano y que desaparezca para simular el movimiento
              tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,131,PAGE1_START_ADDR,SCREEN_WIDTH,420,400,172,130); // Mostrar guardar (172x130)
              
              // 3 - Movimiento final de la mano (manoWppt)
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,420,492,120,128,WHITE); // Mostrar manoWppt (120x128)    
              delay(50);
              
              break;

        default: break;
    }

}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Realiza diferentes acciones dependiendo de la opción seleccionada.
 * 
 * @param option La opción seleccionada.
 *        - 1: Grupos
 *        - 2: Añadir
 *        - 3: Eliminar
 *        - 4: Guardar
 */
/*-----------------------------------------------------------------------------*/
void sin_pulsacion(int option){
    switch(option){
        case 1: // Grupos
              // 1 - Borrar todo (grupo, mano y pulsación)
              tft.clearArea(546,278,690,528,VERDE_PEDIR); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano

              // 2 - Mostrar grupo3 (130x125)
              tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 

              // 3 - Mostrar mano (manoGppt)
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,119,127,VERDE_PEDIR); // Transparencia manoGppt (120x128)

              break;


        case 2: // Añadir
        case 3: // Eliminar
              // 1 - Borrar todo (botón, mano y pulsación)
              tft.clearArea(400,370,612,620,AMARILLO_CONFIRM_Y_AVISO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
              
              // 2 - Botón correspondiente --> para superponerse a la última mano y que desaparezca para simular el movimiento
              if(option == 2) tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) 
              else tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,818,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar borrar (172x130)
              
              // 3 - Movimiento final de la mano (manoWppt)
              // manoWppt
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,251,292,PAGE1_START_ADDR,SCREEN_WIDTH,430,472,120,128,WHITE); // Transparencia manoWppt (120x128)
              //tft.clearArea(420,510,690,528,AMARILLO_CONFIRM_Y_AVISO); // Se borra desde y = 413 para no borrar parte del botón añadir
              
              break;


        case 4: // Guardar
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


/*-----------------------------------------------------------------------------*/
/**
 * @brief Realiza una serie de acciones dependiendo de la opción seleccionada.
 * 
 * @param option La opción seleccionada.
 *               1: Grupos
 *               2: Añadir
 *               3: Eliminar
 *               4: Guardar
 */
/*-----------------------------------------------------------------------------*/
void con_pulsacion(int option){
    int x1, y1, x2, y2;

    switch(option){
        case 1: // Grupos
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


        case 2: // Añadir
        case 3: // Eliminar
              // 1 - Borrar todo (botón, mano y pulsación)
              tft.clearArea(400,370,612,620,AMARILLO_CONFIRM_Y_AVISO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
              
              // 2 - Botón correspondiente --> para superponerse a la última mano y que desaparezca para simular el movimiento
              if(option == 2) tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,645,0,PAGE1_START_ADDR,SCREEN_WIDTH,420,380,172,130); // Mostrar añadir (172x130) 
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


        case 4: // Guardar
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




//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------- CARGA DE IMÁGENES EN MEMORIA --------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra el reloj1 en la página 1 de la pantalla TFT.
 * 
 * Esta función establece la dirección de inicio de la imagen de la página 1 en la pantalla TFT,
 * borra la pantalla TFT con color blanco y copia la imagen del reloj1 en la página 2 a la página 1.
 * La imagen del reloj1 tiene dimensiones de 260x410 píxeles y se muestra en la posición (480, 249) de la página 1.
 */
/*-----------------------------------------------------------------------------*/
void putReloj1(){ 
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE); 
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj1 (65x103) en PAGE1 -> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra el reloj2 en la página 1 de la pantalla TFT.
 * 
 * Esta función copia el contenido del reloj2 en la página 2 de la memoria de la pantalla TFT
 * y lo muestra en la página 1. No es necesario limpiar la página antes de copiar el contenido,
 * ya que ocupa el mismo espacio que el reloj1.
 */
/*-----------------------------------------------------------------------------*/
void putReloj2(){ 
  // No necesita limpiar porque ocupa el mismo espacio que reloj1
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,66,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj2 en PAGE1 --> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra el reloj3 en la pantalla.
 * 
 * Esta función copia el contenido del reloj3 en la memoria de la pantalla,
 * ocupando el mismo espacio que el reloj2. No es necesario limpiar la memoria
 * antes de copiar el contenido.
 */
/*-----------------------------------------------------------------------------*/
void putReloj3(){
  // No necesita limpiar porque ocupa el mismo espacio que reloj2
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,132,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj3 en PAGE1 --> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra el reloj4 en la pantalla.
 * 
 * Esta función copia el contenido del reloj4 en la memoria de la pantalla,
 * ocupando el mismo espacio que el reloj3. No es necesario limpiar la memoria
 * antes de copiar el contenido.
 */
/*-----------------------------------------------------------------------------*/
void putReloj4(){
  // No necesita limpiar porque ocupa el mismo espacio que reloj3
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,198,279,PAGE1_START_ADDR,SCREEN_WIDTH,480,249,65,103); // Mostrar reloj4 en PAGE1 --> x = 512 +/- 32 = 480     y = 300 +/- 51 = 249
}

/*-----------------------------------------------------------------------------*/
/**
 * Muestra el reloj girado 1 en la pantalla.
 * No es necesario limpiar la pantalla antes de mostrar el reloj, ya que ocupa más espacio que reloj4.
 */
/*-----------------------------------------------------------------------------*/
void putRelojGirado1(){
  // No necesita limpiar porque ocupa más espacio que reloj4
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,264,279,PAGE1_START_ADDR,SCREEN_WIDTH,465,243,95,115); // Mostrar relGir1 en PAGE1 --> x = 512 +/- 47 = 465     y = 300 +/- 57 = 243
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra un reloj girado en la pantalla TFT.
 * 
 * Esta función borra la pantalla TFT y muestra un reloj girado en la posición especificada.
 * Utiliza las direcciones de memoria PAGE1_START_ADDR y PAGE2_START_ADDR para copiar y mostrar el reloj.
 */
/*-----------------------------------------------------------------------------*/
void putRelojGirado2(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,360,279,PAGE1_START_ADDR,SCREEN_WIDTH,456,244,112,112); // Mostrar relGir2 --> x = 512 +/- 56 = 456     y = 300 +/- 56 = 244
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Coloca el reloj girado 3 en la pantalla.
 * 
 * Esta función borra la pantalla y copia la imagen del reloj girado 3 en la posición especificada.
 */
/*-----------------------------------------------------------------------------*/
void putRelojGirado3(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,473,279,PAGE1_START_ADDR,SCREEN_WIDTH,456,253,113,94); // Mostrar relGir3 en PAGE1 --> x = 512 +/- 56 = 456     y = 300 +/- 47 = 253
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Coloca y muestra un reloj girado en la pantalla.
 * El reloj se muestra en la página 1 de la pantalla TFT.
 * La posición del reloj en la pantalla se determina mediante las coordenadas x e y.
 */
/*-----------------------------------------------------------------------------*/
void putRelojGirado4(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,587,279,PAGE1_START_ADDR,SCREEN_WIDTH,462,268,100,65); // Mostrar relGir4 en PAGE1 --> x = 512 +/- 50 = 462     y = 300 +/- 32 = 268
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra el reloj girado 5 en la pantalla.
 * 
 * Esta función borra la pantalla y copia la imagen del reloj girado 5 en la posición especificada.
 */
/*-----------------------------------------------------------------------------*/
void putRelojGirado5(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,688,279,PAGE1_START_ADDR,SCREEN_WIDTH,456,253,113,94); // Mostrar relGir5 en PAGE1 --> x = 512 +/- 56 = 456   ->  y = 300 +/- 47 = 253
}

/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra el reloj girado 6 en la pantalla.
 * 
 * Esta función borra la pantalla y copia la imagen del reloj girado 6 en la posición especificada.
 */
/*-----------------------------------------------------------------------------*/
void putRelojGirado6(){
  tft.canvasImageStartAddress(PAGE1_START_ADDR); 
  tft.clearScreen(WHITE);
  tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,802,279,PAGE1_START_ADDR,SCREEN_WIDTH,463,244,99,113); // Mostrar relGir6 en PAGE1 --> x = 512 +/- 49 = 463  ->  y = 300 +/- 56 = 244
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Carga en SDRAM las imágenes necesarias para todas las pantallas de SmartCloth mientras muestra 
 * un reloj de arena en la pantalla. Las imágenes se cargan en una secuencia específica para lograr el 
 * efecto de giro del reloj.
 * 
 * @note Esta función asume que se ha configurado correctamente la pantalla TFT y se ha
 *       proporcionado la ruta de archivo correcta para cada imagen.
 */
/*-----------------------------------------------------------------------------*/
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


#endif