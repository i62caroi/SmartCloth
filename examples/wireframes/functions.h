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

// --- MENSAJE DE AVISO ---
#define  WARNING_NOT_ADDED                  1   // Aviso: plato no añadido porque está vacío
#define  WARNING_NOT_DELETED                2   // Aviso: plato no borrado porque está vacío
#define  WARNING_NOT_SAVED                  3   // Aviso: comida no guardada porque está vacía
#define  WARNING_RAW_COOKED_NOT_NEEDED      4   // Aviso: no hace falta crudo/cocinado para producto con código de barras
#define  WARNING_BARCODE_NOT_READ           5   // Aviso: código de barras no leído
#define  WARNING_PRODUCT_NOT_FOUND          6   // Aviso: producto no encontrado
#define  WARNING_MEALS_LEFT                 7   // Aviso: algunas comidas no sincronizadas
#define  WARNING_NO_INTERNET_NO_BARCODE     8   // Aviso: no se puede leer barcode porque no hay conexión a internet

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

// --- MENSAJE DE GUARDADO ---
#define  SAVING_ONLY_LOCAL          0   // FALSE. Solo se puede guardar localmente porque no hay conexión a internet
#define  SAVING_WIFI                1   // TRUE. Guardado completo: local y en la database

// --- RESULTADOS DE SUBIR INFO A DATABASE --- 
// Usadas en:
//      prepareSaving()                 -->     WAITING_FOR_DATA, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT y UNKNOWN_ERROR
//      sendTXTFileToESP32()            -->     ALL_MEALS_UPLOADED, MEALS_LEFT y ERROR_READING_TXT
//      saveComidaInDatabase_or_TXT()   -->     MEAL_UPLOADED, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT y UNKNOWN_ERROR
//      showSyncState()              -->     UPLOADING_DATA, ALL_MEALS_UPLOADED, MEALS_LEFT, ERROR_READING_TXT, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT y UNKNOWN_ERROR
#define  WAITING_FOR_DATA        1
#define  UPLOADING_DATA          2
#define  ALL_MEALS_UPLOADED      3 // Comidas guardadas en sincronización inicial
#define  MEAL_UPLOADED           3 // Comida guardada al pulsar "Guardar comida"
#define  MEALS_LEFT              4
#define  ERROR_READING_TXT       5
#define  NO_INTERNET_CONNECTION  6
#define  HTTP_ERROR              7
#define  TIMEOUT                 8
#define  UNKNOWN_ERROR           9



// --- TIPO DE GUARDADO HECHO (GENERAL) ---
// Usados en saveComida() y luego en showAccionRealizada()
#define  SAVE_EXECUTED_FULL                       3  // Se ha guardado en CSV y en la database
#define  SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP      4  // Solo se ha guardado en CSV. Fallo en petición HTTP
#define  SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI         5  // Solo se ha guardado en CSV. No hay WiFi
#define  SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT         6  // Solo se ha guardado en CSV. Timeout de respuesta del esp32
#define  SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR   7  // Solo se ha guardado en CSV. Error desconocido
#define  SAVE_EXECUTED_ONLY_DATABASE              8  // Solo en la database porque falló el CSV
#define  ERROR_SAVING_DATA                        9  // Error al guardar. Falló el CSV y la database







RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);

/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
// --- CONVERSIÓN DE TEXTO ---
void    convertSpecialCharactersToHEX_ref(String &input);       // Convierte a HEX caracteres especiales en una cadena de texto pasada por referencia
String  convertSpecialCharactersToHEX(String input);            // Convierte a HEX caracteres especiales en una cadena de texto y devuelve la cadena modificada


// Configuración
bool    setupSD();
void    setupScreen();

// Barcode
void    scanningBarcode();                      // Pantalla: Escaneando código de barras
void    showSearchingProductInfo();             // Pantalla: Buscando información del producto
void    showProductInfo_1(String barcode, String name, float carb_1gr, float protein_1gr, float fat_1gr, float kcal_1gr); // Pantalla: Información del producto
//void    esp32_sinWifi();                        // Pantalla: ESP32 sin WiFi
//void    showBarcodeNotRead();                   // Pantalla: Código de barras no leído
//void    showProductNotFound(String barcode);    // Pantalla: Producto no encontrado

// Fallo SD
void    showCriticFailureSD();              // Pantalla: Fallo en SD (no se encuentra)

// Sincronizar SM
void    showSyncState(byte option);
void    drawSmartClothDevice();            // Dibujar dispositivo SmartCloth (Sincronizando...)

// Nombre SmartCloth
void    arranque();                         // Pantalla: Mostrar logo SmartCloth letra a letra
void    arranqueSlowOpacity();              // Pantalla: Mostrar logo SmartCloth con distinta opacidad

// Pedir recipiente
void    pantalla_inicial();                 // Pantalla: Coloque Recipiente

// Escoger grupo
void    select_Grupo();                     // Pantalla: Seleccione grupo
// Escoger crudo/cocinado
void    crudo_cocinado();                   // Pantalla: Escoja crudo/cocinado (simple)
void    crudo_cocinado_sobre_dashboard();   // Pantalla: Escoja crudo/cocinado (sobre dashboard)

// Coloar alimento
void    colocar_alimento();                 // Pantalla: Coloque alimento sobre báscula

// Dashboard
void    dashboard();                        // Pantalla: Dashboard completo 

// Sugerir acción
void    suggestAction();                    // Pantalla: Sugerencia acción

// Añadir, borrar, guardar
void    add_Plato();                        // Pantalla: Pedir confirmación y confirmado añadir plato
void    delete_Plato();                     // Pantalla: Pedir confirmación y confirmado borrar plato
void    save_Comida(bool conexion);         // Pantalla: Pedir confirmación y confirmado guardar comida
void    showSavingMeal(bool conexion);         // Pantalla: Guardando comida

// Acción realizada (añadir, borrar, guardar)
void    showAccionRealizada(int option);    // Pantalla: Acción realizada (añadir, borrar, guardar)

// Error
void    showError(int option);              // Pantalla: Error y sugerencia de solución

// Aviso
void    showWarning(byte option, String barcode = "");           // Pantalla: Aviso

// Borrar CSV/TXT
void    pedirConfirmacion_DELETE_CSV();     // Pantalla: Pedir confirmacion y confirmado borrar CSV


// -- Auxiliares --
void    slowDisappearance(int option);      // Desaparecer paulatinamente imagen de crudo o cocinado (option)
void    slowAppearanceImage(int option);    // Aparecer paulatinamente imagen de crudo/cocinado, sugerencias o grupos (option)
void    slowAppearanceAndDisappareanceProcesamiento(int option); // Aparecer y desaparecer simultáneo de crudo y cocinado
void    desplazar_mano(int option);         // Desplazar mano hasta "botón" 
void    sin_pulsacion(int option);          // Fin movimiento mano sin simular pulsación sobre "botón"
void    con_pulsacion(int option);          // Fin movimiento mano simulando pulsación sobre "botón"
void    drawBigX(uint16_t centerX, uint16_t centerY, uint16_t length, uint16_t angleLine1, uint16_t angleLine2, uint16_t thickness, uint16_t lineColor);
// -- Carga imagenes --
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
 * @brief Convierte los caracteres especiales en una cadena de texto.
 * 
 * Esta función reemplaza los caracteres especiales en una cadena de texto con sus correspondientes
 * códigos de caracteres en formato hexadecimal. Los caracteres especiales que se reemplazan son:
 * 
 * - Minúsculas: vocales acentuadas (á, é, í, ó, ú) y la letra 'ñ'.
 * - Mayúsculas: vocales acentuadas (Á, É, Í, Ó, Ú) y la letra 'Ñ'.
 * - Signos de interrogación (¿), exclamación (¡), escape (/) y puntos suspensivos (...).
 * 
 * Se utilizan los caracteres especiales en ISO 8859-1 (Latin-1) en HEX.
 * 
 * @param input La cadena de texto en la que se reemplazarán los caracteres especiales.
 * 
 * @see https://cs.stanford.edu/people/miles/iso8859.html Colección de caracteres ASCII/ISO 8859-1 (Latin-1) en HEX
 * @see https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf Table 14-1 (pag 99) del datasheet de RA8876 para caracteres Latin-1
 */
/*-----------------------------------------------------------------------------*/
void convertSpecialCharactersToHEX_ref(String &input) 
{
    // Minúsculas: vocales y 'ñ'
    input.replace("á", "\xE1");
    input.replace("é", "\xE9");
    input.replace("í", "\xED");
    input.replace("ó", "\xF3");
    input.replace("ú", "\xFA");
    input.replace("ñ", "\xF1");

    // Mayúsculas: vocales y 'Ñ'
    input.replace("Á", "\xC1");
    input.replace("É", "\xC9");
    input.replace("Í", "\xCD");
    input.replace("Ó", "\xD3");
    input.replace("Ú", "\xDA");
    input.replace("Ñ", "\xD1");

    // Signos de interrogación y exclamación
    input.replace("¿", "\xBF");
    input.replace("¡", "\xA1");

    // Caracter especial '/'
    input.replace("/", "\x2F");

    // Puntos suspensivos
    input.replace("...", "\x85");
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte los caracteres especiales en una cadena de texto utilizando una función auxiliar.
 * 
 * Esta función llama a `convertSpecialCharactersToHEX_ref` para realizar la conversión de los caracteres
 * especiales en la cadena de texto. La función auxiliar modifica la cadena directamente a través de una
 * referencia.
 * 
 * @param input La cadena de texto en la que se van a reemplazar los caracteres especiales.
 * @return La cadena de texto con los caracteres especiales reemplazados.
 */
/*-----------------------------------------------------------------------------*/
String convertSpecialCharactersToHEX(String input) 
{
    convertSpecialCharactersToHEX_ref(input);
    return input;
}







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




void scanningBarcode()
{
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AZUL_PROCESO);
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO ----------------------------------------------------------------------------------
    // ---- RECUADRO ----
    // Borde:
    /*tft.drawRect(50,110,974,278,WHITE);
    tft.drawRect(51,111,973,277,WHITE);
    tft.drawRect(52,112,972,276,WHITE);
    tft.drawRect(53,113,971,275,WHITE);
    tft.drawRect(54,114,970,274,WHITE);
    // Relleno:
    tft.fillRect(55, 115, 969, 273, AZUL_RECUADRO_SINCRONIZANDO); // Rellenar el rectángulo más interno con DARK_BLUE
    // ------------------

    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(WHITE);*/
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO DEL CÓDIGO DE BARRAS FICTICIO -----------------------------------------------------
    int startX = 400; // Posición inicial X del código de barras
    int startY = 130; // Posición inicial Y del código de barras
    int barWidth = 10;   // Ancho de cada barra
    int barHeight = 150; // Altura del código de barras
    int numBars = 20; // Número de barras en el código de barras

    for (int i = 0; i < numBars; i++) {
        if (i % 2 == 0) {
            tft.fillRect(startX + (i * barWidth), startY, startX + (i * barWidth) + barWidth, startY + barHeight, BLACK); // Barra negra
        } else {
            tft.fillRect(startX + (i * barWidth), startY, startX + (i * barWidth) + barWidth, startY + barHeight, WHITE); // Barra blanca
        }
    }
    // -----------------------------------------------------------------------------------------------


    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    /*tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    //tft.setCursor(230, 158);     
    tft.setCursor(210, 90);    
    tft.println("ESCANEANDO...");*/
    // -----------------------------------------------------------------------------------------------


    // ------ TEXTO (COMENTARIO) ---------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    //tft.setCursor(70, 358);      
    tft.setCursor(70, 400);   
    tft.println("COLOQUE EL PRODUCTO SOBRE EL LECTOR");
    // -----------------------------------------------------------------------------------------------
}


void showNewScanningBarcode()
{
    // ---- COLOR FONDO -----------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AZUL_PROCESO); // 0x037F en RGB565 (#006CFF en HEX) es el color que dio Guillermo, pero por el ángulo se ve celeste
    // ----------------------------------------------------------------------------------------------------

    // ------ ICONO (CODIGO DE BARRAS) --------------------------------------------------------------------
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,137,PAGE1_START_ADDR,SCREEN_WIDTH,427,100,171,128); // Mostrar scan (171x128) en PAGE1
    // ----------------------------------------------------------------------------------------------------

    // ------------ LINEA ---------------------------------------------------------------------------------
    tft.fillRoundRect(210,280,814,288,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);
    tft.setCursor(140, 338);                                            tft.println(convertSpecialCharactersToHEX("COLOQUE EL CÓDIGO DE BARRAS DEL"));
    tft.setCursor(130,tft.getCursorY() + tft.getTextSizeY() - 10);      tft.println("PRODUCTO A UNOS 10cms DEL LECTOR");
    
    //tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(180,tft.getCursorY() + tft.getTextSizeY() + 10);      tft.println(convertSpecialCharactersToHEX("El lector cambiará de color"));
    tft.setCursor(300,tft.getCursorY() + tft.getTextSizeY() - 20);      tft.println(convertSpecialCharactersToHEX("al leer el código"));
    // ----------------------------------------------------------------------------------------------------

}


void showSearchingProductInfo() {
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AZUL_PROCESO);
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO ----------------------------------------------------------------------------------
    // ---- RECUADRO ----
    // Borde:
    tft.drawRect(50,110,974,278,WHITE);
    tft.drawRect(51,111,973,277,WHITE);
    tft.drawRect(52,112,972,276,WHITE);
    tft.drawRect(53,113,971,275,WHITE);
    tft.drawRect(54,114,970,274,WHITE);
    // Relleno:
    tft.fillRect(55, 115, 969, 273, AZUL_RECUADRO_PROCESO); // Rellenar el rectángulo más interno con DARK_BLUE
    // ------------------

    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(WHITE);
    // -----------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    tft.setCursor(150, 158);    tft.println("BUSCANDO PRODUCTO...");
    // -----------------------------------------------------------------------------------------------

    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    tft.setCursor(50, 358);   tft.println("ESPERE MIENTRAS SE BUSCA LA INFORMACI\xD3""N");
    tft.setCursor(200, tft.getCursorY() + tft.getTextSizeY()+10);   tft.println("NUTRICIONAL DEL PRODUCTO"); 
    // ----------------------------------------------------------------------------------------------------

}


void showNewSearchingProduct()
{
    // ---- COLOR FONDO -----------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AZUL_PROCESO); // 0x037F en RGB565 (#006CFF en HEX) es el color que dio Guillermo, pero por el ángulo se ve celeste
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);
    tft.setTextForegroundColor(WHITE);

    tft.setCursor(350, 30);    tft.println("BUSCANDO...");
    // ----------------------------------------------------------------------------------------------------

    // ------ ICONO (LUPA) --------------------------------------------------------------------------------
    // Toma desde y=138 (137+1) para quitar linea de basura y, para evitar la linea de debajo, hacemos como que es de 129 píxeles de alto
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,172,138,PAGE1_START_ADDR,SCREEN_WIDTH,471,120,82,129); // Mostrar lupa (82x130) en PAGE1
    // ----------------------------------------------------------------------------------------------------

    // ------------ LINEA ---------------------------------------------------------------------------------
    tft.fillRoundRect(210,280,814,288,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);
    tft.setCursor(140, 338);                                         tft.println("ESPERE MIENTRAS LOCALIZAMOS LA");
    tft.setCursor(160,tft.getCursorY() + tft.getTextSizeY() - 10);   tft.println(convertSpecialCharactersToHEX("INFORMACIÓN NUTRICIONAL DEL"));
    tft.setCursor(420,tft.getCursorY() + tft.getTextSizeY() - 10);   tft.println("PRODUCTO");
    // ----------------------------------------------------------------------------------------------------

}


void showProductInfo_1(String barcode, String name, float carb_1gr, float protein_1gr, float fat_1gr, float kcal_1gr) {
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(VERDE_PEDIR_Y_EXITO);
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO ----------------------------------------------------------------------------------
    tft.fillRoundRect(252,90,764,98,3,WHITE); // Línea superior
    tft.fillRoundRect(252,230,764,238,3,WHITE); // Línea inferior

    tft.fillRoundRect(60, 288, 924, 570, 10, GRIS_CUADROS_VALORES); // Recuadro para la información del producto
    // ------------------
    // -----------------------------------------------------------------------------------------------

    // ----- TEXTO (TÍTULO) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(WHITE);

    tft.setCursor(150, 128); // Ajustar la posición según sea necesario
    tft.println("PRODUCTO ENCONTRADO");
    // -----------------------------------------------------------------------------------------------

    // ------ TEXTO (INFORMACIÓN DEL PRODUCTO) -------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);
    

    // Establecer el cursor y mostrar el código de barras
    tft.setTextForegroundColor(ROJO_PESO);
    tft.setCursor(90, 300); tft.println("C\xF3""digo: " + barcode);

    // Mostrar el nombre del producto
    tft.setTextForegroundColor(WHITE); 
    tft.setCursor(90, 342); tft.println("Nombre: " + name);

    // Mostrar los valores nutricionales como valores flotantes
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.setCursor(90, 382); tft.print("Carbohidratos (por gr): "); tft.println(carb_1gr, 2);

    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.setCursor(90, 422); tft.print("Prote\xED""nas (por gr): "); 
    tft.println(protein_1gr, 2);

    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.setCursor(90, 462); tft.print("Grasas (por gr): "); tft.println(fat_1gr, 2);

    tft.setTextForegroundColor(ROJO_KCAL);
    tft.setCursor(90, 502); tft.print("Calor\xED""as (por gr): "); tft.println(kcal_1gr, 2);
    // -----------------------------------------------------------------------------------------------

    // Dibujar líneas divisorias entre cada campo de información
    tft.drawLine(60, 344, 924, 344, GRIS_CAMPOS_INFO); // Línea debajo del código de barras
    tft.drawLine(60, 384, 924, 384, GRIS_CAMPOS_INFO); // Línea debajo del nombre
    tft.drawLine(60, 425, 924, 425, GRIS_CAMPOS_INFO); // Línea debajo de los carbohidratos
    tft.drawLine(60, 465, 924, 465, GRIS_CAMPOS_INFO); // Línea debajo de las proteínas
    tft.drawLine(60, 505, 924, 505, GRIS_CAMPOS_INFO); // Línea debajo de las grasas
    
}

void showProductInfo_2(String barcode, String name, float carb_1gr, float protein_1gr, float fat_1gr, float kcal_1gr) {
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(VERDE_PEDIR_Y_EXITO);
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO ----------------------------------------------------------------------------------
    tft.fillRoundRect(252,90,764,98,3,WHITE); // Línea superior
    tft.fillRoundRect(252,230,764,238,3,WHITE); // Línea inferior

    // Recuadro para la información del producto
    uint16_t VERDE_OSCURO = 0x03E0; // Verde oscuro (RGB565)
    tft.fillRoundRect(60, 288, 924, 570, 10, VERDE_OSCURO);
    // -----------------------------------------------------------------------------------------------

    // ----- TEXTO (TÍTULO) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    // Mostrar el nombre del producto como título principal
    tft.setTextForegroundColor(WHITE);
    tft.setCursor(150, 128); // Ajustar la posición según sea necesario
    tft.println(name);
    // -----------------------------------------------------------------------------------------------

    // ------ TEXTO (INFORMACIÓN DEL PRODUCTO) -------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);

    // Establecer el cursor y mostrar el código de barras
    tft.setTextForegroundColor(ROJO_PESO);
    tft.setCursor(90, 300); tft.println("C\xF3""digo: " + barcode);

    // Mostrar los valores nutricionales como valores flotantes por cada 100 gramos
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.setCursor(90, 342); tft.print("Carbohidratos (por 100gr): "); tft.println(carb_1gr * 100, 2);

    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.setCursor(90, 382); tft.print("Prote\xED""nas (por 100gr): "); tft.println(protein_1gr * 100, 2);

    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.setCursor(90, 422); tft.print("Grasas (por 100gr): "); tft.println(fat_1gr * 100, 2);

    tft.setTextForegroundColor(ROJO_KCAL);
    tft.setCursor(90, 462); tft.print("Calor\xED""as (por 100gr): "); tft.println(kcal_1gr * 100, 2);
    // -----------------------------------------------------------------------------------------------

    // Dibujar líneas divisorias entre cada campo de información
    tft.drawLine(60, 344, 924, 344, GRIS_CAMPOS_INFO); // Línea debajo del código de barras
    tft.drawLine(60, 384, 924, 384, GRIS_CAMPOS_INFO); // Línea debajo de los carbohidratos
    tft.drawLine(60, 425, 924, 425, GRIS_CAMPOS_INFO); // Línea debajo de las proteínas
    tft.drawLine(60, 465, 924, 465, GRIS_CAMPOS_INFO); // Línea debajo de las grasas
}

void showProductInfo_3(String barcode, String name, float carb_1gr, float protein_1gr, float fat_1gr, float kcal_1gr) {
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(VERDE_PEDIR_Y_EXITO);
    // -----------------------------------------------------------------------------------------------

    // ----- TEXTO (TÍTULO) --------------------------------------------------------------------------
    //tft.fillRoundRect(252,90,764,98,3,WHITE); // Línea superior
    tft.fillRoundRect(252,230,764,238,3,WHITE); // Línea inferior

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(WHITE);

    tft.setCursor(50, 128);  tft.println(name);
    // -----------------------------------------------------------------------------------------------

    // ------ TEXTO (INFORMACIÓN DEL PRODUCTO) -------------------------------------------------------
    tft.fillRoundRect(60, 288, 924, 570, 10, GRIS_CUADROS_VALORES); // Recuadro para la información del producto

    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);

    // Establecer el cursor y mostrar el código de barras
    //tft.setTextForegroundColor(ROJO_PESO);
    //tft.setCursor(90, 300); tft.println("C\xF3""digo: " + barcode);

    // Mostrar los valores nutricionales como valores flotantes
    tft.setTextForegroundColor(AZUL_CARB); 
    tft.setCursor(90, 300); tft.print("Carbohidratos (por gr): "); tft.println(carb_1gr, 2);

    tft.setTextForegroundColor(NARANJA_PROT); 
    tft.setCursor(90, 342); tft.print("Prote\xED""nas (por gr): "); 
    tft.println(protein_1gr, 2);

    tft.setTextForegroundColor(AMARILLO_GRASAS); 
    tft.setCursor(90, 382); tft.print("Grasas (por gr): "); tft.println(fat_1gr, 2);

    tft.setTextForegroundColor(ROJO_KCAL);
    tft.setCursor(90, 422); tft.print("Calor\xED""as (por gr): "); tft.println(kcal_1gr, 2);
    // -----------------------------------------------------------------------------------------------

    // Dibujar líneas divisorias entre cada campo de información
    //tft.drawLine(60, 344, 924, 344, GRIS_CAMPOS_INFO); // Línea debajo del código de barras
    //tft.drawLine(60, 384, 924, 384, GRIS_CAMPOS_INFO); // Línea debajo del nombre
    tft.drawLine(60, 303, 924, 303, GRIS_CAMPOS_INFO); // Línea debajo de los carbohidratos
    tft.drawLine(60, 345, 924, 345, GRIS_CAMPOS_INFO); // Línea debajo de las proteínas
    tft.drawLine(60, 385, 924, 385, GRIS_CAMPOS_INFO); // Línea debajo de las grasas
    
}

void showNewProductFound()
{
    // ---- COLOR FONDO -----------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AZUL_PROCESO); // 0x037F en RGB565 (#006CFF en HEX) es el color que dio Guillermo, pero por el ángulo se ve celeste
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);
    tft.setTextForegroundColor(WHITE);

    tft.setCursor(125, 30);    tft.println(convertSpecialCharactersToHEX("¡PRODUCTO ENCONTRADO!"));
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (COMENTARIO) ---------------------------------------------------------------------------
    //tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);

    tft.setCursor(150, 125);                                        tft.print(convertSpecialCharactersToHEX("Pulse el botón de lectura si es"));
    tft.setCursor(265, tft.getCursorY() + tft.getTextSizeY() + 15); tft.println("el producto correcto");
    // ----------------------------------------------------------------------------------------------------

    // ------ ICONO (PRODUCTO + BOTÓN BARCODE) ------------------------------------------------------------
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,369,137,PAGE1_START_ADDR,SCREEN_WIDTH,350,240,297,104); // Mostrar producto encontrado (297x104) en PAGE1
    // ----------------------------------------------------------------------------------------------------

    // ------------ LINEA ---------------------------------------------------------------------------------
    tft.fillRoundRect(210,370,814,378,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    // ------ RECUADRO REDONDEADO -------------------------------------------------------------------------
    // Para encuadrar el texto con el código y el nombre del producto
    // Tres recuadros para simular grosor
    tft.drawRoundRect(95, 420, 905, 570, 20, WHITE);
    tft.drawRoundRect(96, 421, 904, 569, 19, WHITE);
    tft.drawRoundRect(97, 422, 903, 568, 18, WHITE);
    // ----------------------------------------------------------------------------------------------------

    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);

    String barcode = "8437002353025";
    String nombreProducto = "Tortas de aceite";

    tft.setCursor(110, 440);                                         tft.print(convertSpecialCharactersToHEX("Código: "));  tft.println(barcode);
    tft.setCursor(110,tft.getCursorY() + tft.getTextSizeY() - 10);   tft.print("Nombre:");                                  tft.println(nombreProducto);
    // ----------------------------------------------------------------------------------------------------

}


/*
void esp32_sinWifi()
{
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO);
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO ----------------------------------------------------------------------------------
    tft.fillRoundRect(252,110,764,118,3,ROJO_TEXTO_CONFIRM_Y_AVISO); // Línea superior
    tft.fillRoundRect(252,270,764,278,3,ROJO_TEXTO_CONFIRM_Y_AVISO); // Línea inferior
    // ------------------

    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO);
    // -----------------------------------------------------------------------------------------------


    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    tft.setCursor(100, 158);        tft.println("SIN CONEXI\xD3""N A INTERNET");
    // -----------------------------------------------------------------------------------------------


    // ------ TEXTO (COMENTARIO) ---------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    tft.setCursor(100, 358);         tft.println("NO SE PUEDE LEER EL C\xD3""DIGO DE BARRAS");
    // -----------------------------------------------------------------------------------------------

}


void showBarcodeNotRead()
{
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO);
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO ----------------------------------------------------------------------------------
    tft.fillRoundRect(252,110,764,118,3,ROJO_TEXTO_CONFIRM_Y_AVISO); // Línea superior
    tft.fillRoundRect(252,270,764,278,3,ROJO_TEXTO_CONFIRM_Y_AVISO); // Línea inferior
    // ------------------

    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO);
    // -----------------------------------------------------------------------------------------------


    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    tft.setCursor(100, 158);        tft.println("PRODUCTO NO DETECTADO");
    // -----------------------------------------------------------------------------------------------


    // ------ TEXTO (COMENTARIO) ---------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    tft.setCursor(100, 358);         tft.println("NO SE HA LE\xCD""DO EL C\xD3""DIGO DE BARRAS");
    // -----------------------------------------------------------------------------------------------
}


void showProductNotFound(String barcode)
{
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO);
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO ----------------------------------------------------------------------------------
    tft.fillRoundRect(252,110,764,118,3,ROJO_TEXTO_CONFIRM_Y_AVISO); // Línea superior
    tft.fillRoundRect(252,270,764,278,3,ROJO_TEXTO_CONFIRM_Y_AVISO); // Línea inferior
    // ------------------

    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO);
    // -----------------------------------------------------------------------------------------------


    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    tft.setCursor(100, 158);        tft.println("PRODUCTO NO ENCONTRADO");
    // -----------------------------------------------------------------------------------------------


    // ------ TEXTO (COMENTARIO) ---------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    tft.setCursor(100, 358);         tft.println("NO SE HA ENCONTRADO EL PRODUCTO");

    String cad = "CON EL C\xD3""DIGO: " + barcode;
    tft.setCursor(150, tft.getCursorY() + tft.getTextSizeY()+10);       tft.println(cad);
    // -----------------------------------------------------------------------------------------------
}
*/





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
 * @param option La opción seleccionada: DATA_TO_UPLOAD, UPLOADING_DATA o NO_INTERNET_CONNECTION
 */
/*-----------------------------------------------------------------------------*/
/*void showSyncState(int option){

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
        case NO_INTERNET_CONNECTION:     tft.setCursor(300, 158);    tft.println("SIN CONEXI\xD3""N");                       break;
        case UPLOADING_DATA:            tft.setCursor(220, 158);    tft.println("SINCRONIZANDO...");                        break; 

        case UPLOADED_DATA:             tft.fillRect(55,115,969,273,VERDE_PEDIR_Y_EXITO); // Verde de éxito
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
        
        case NO_INTERNET_CONNECTION:     tft.setCursor(200, 358);                                        tft.println("NO HAY CONEXI\xD3""N A INTERNET");
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

}*/


/**
 * Muestra la información y los elementos gráficos correspondientes a la opción de subida de datos.
 * 
 * @param option La opción seleccionada para mostrar los datos de subida.
 * 
 *               - UPLOADING_DATA: "Sincronizando..." con fondo azul dentro de un recuadro azul oscuro indicando 
 *                                  al usuario que espere a que termine de subirse toda la info
 * 
 *               - ALL_MEALS_UPLOADED: "¡SmartCloth sincronizado!" con fondo verde entre dos líneas blancas indicando 
 *                                      al usuario que se ha subido toda la info
 * 
 *               - MEALS_LEFT: "Sincronización parcial" con fondo amarillo entre dos líneas rojas indicando al usuario 
 *                              que se han subido algunas comidas pero otras no se ha podido y se intentará más adelante
 * 
 *               - ERROR_READING_TXT, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT, UNKNOWN_ERROR: "¡Error del sistema!" con comentario
 * 
 */
void showSyncState(byte option)
{
    // ---- COLOR FONDO, RECUADRO Y TEXTO ------------------------------------------------------------
    uint16_t backgroundColor, lineColor, textColor;
    
    switch(option)
    {
        case UPLOADING_DATA:            backgroundColor = AZUL_PROCESO;                 lineColor = textColor = WHITE;                          break;
        case ALL_MEALS_UPLOADED:        backgroundColor = VERDE_PEDIR_Y_EXITO;                  lineColor = textColor = WHITE;                          break;
        case MEALS_LEFT:                
        //case NO_INTERNET_CONNECTION:    backgroundColor = AMARILLO_CONFIRM_Y_AVISO;     lineColor = textColor = ROJO_TEXTO_CONFIRM_Y_AVISO;     break;
        
        // Error: option en rango [5, 9]
        default:                        backgroundColor = RED_ERROR_Y_CANCEL;           lineColor = textColor = WHITE;                          break;

    }

    // Aplicar color al fondo
    tft.clearScreen(backgroundColor);

    // El color del texto se establece después de dibujar (recuadro o líneas) porque al dibujar se modifica '_textForeColor'
    // -----------------------------------------------------------------------------------------------


    // ----- DIBUJO ----------------------------------------------------------------------------------
    switch (option)
    {
        case UPLOADING_DATA:
            // ---- RECUADRO ----
            // Borde:
            tft.drawRect(50,110,974,278,lineColor);
            tft.drawRect(51,111,973,277,lineColor);
            tft.drawRect(52,112,972,276,lineColor);
            tft.drawRect(53,113,971,275,lineColor);
            tft.drawRect(54,114,970,274,lineColor);
            // Relleno:
            tft.fillRect(55, 115, 969, 273, AZUL_RECUADRO_PROCESO); // Rellenar el rectángulo más interno con DARK_BLUE
            // ------------------
            // ----- NUBE -------
            // Parte superior de la nube
            /*tft.fillCircle(50, 148, 70, WHITE);  // Extremo izquierdo superior, más grande
            tft.fillCircle(175, 128, 100, WHITE); // Círculo central superior, más grande
            tft.fillCircle(300, 148, 100, WHITE);  // Círculo medio derecho superior, más grande
            tft.fillCircle(425, 128, 100, WHITE); // Círculo extremo derecho superior, más grande
            tft.fillCircle(550, 148, 70, WHITE);  // Nuevo círculo extremo derecho para extender la nube

            // Base de la nube
            tft.fillCircle(50, 178, 60, WHITE);  // Círculo izquierdo de la base, más grande
            tft.fillCircle(175, 198, 90, WHITE); // Círculo central de la base, más grande
            tft.fillCircle(300, 178, 90, WHITE); // Círculo medio derecho de la base, más grande
            tft.fillCircle(425, 198, 90, WHITE); // Círculo extremo derecho de la base, más grande
            tft.fillCircle(550, 178, 60, WHITE); // Nuevo círculo extremo derecho de la base para extender la nube
            */
            // ------------------
            break;

        case MEALS_LEFT:
            tft.fillRoundRect(252,110,764,118,3,lineColor); // Línea superior
            tft.fillRoundRect(252,270,764,278,3,lineColor); // Línea inferior
            break;

        case ALL_MEALS_UPLOADED:
            tft.fillRoundRect(252,110,764,118,3,lineColor); // Línea superior
            tft.fillRoundRect(252,270,764,278,3,lineColor); // Línea inferior
            break;
            
        case ERROR_READING_TXT:
        case NO_INTERNET_CONNECTION:
            // Parte superior de la nube
            /*tft.fillCircle(900, 65, 18, WHITE); // Círculo izquierdo superior
            tft.fillCircle(920, 60, 20, WHITE); // Círculo central superior 
            tft.fillCircle(940, 65, 18, WHITE); // Círculo derecho superior 
            // Base de la nube 
            tft.fillCircle(890, 80, 15, WHITE); // Círculo izquierdo de la base
            tft.fillCircle(910, 80, 15, WHITE); // Círculo central de la base 
            tft.fillCircle(930, 80, 15, WHITE); // Círculo derecho de la base 

            // Dibujar línea diagonal para simbolizar NO_INTERNET_CONNECTION
            //tft.drawLine(390, 70, 460, 110, RED); // Línea diagonal roja cruzando la nube
            tft.drawLine(960, 30, 880, 110, RED); // Línea diagonal roja cruzando la nube
            tft.drawLine(961, 31, 881, 111, RED);
            tft.drawLine(962, 32, 882, 112, RED);
            break;*/
        case HTTP_ERROR:
        case TIMEOUT:
        case UNKNOWN_ERROR:
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,292,PAGE1_START_ADDR,SCREEN_WIDTH,451,231,114,127); // Mostrar cruz (114x127) en PAGE1
            tft.fillRoundRect(252,290,764,298,3,lineColor); // Dibujar líea por encima de la imagen de cruz para no tener que cuadrarla
            break;

        default:
            break;
    }
    
    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(textColor);
    // -----------------------------------------------------------------------------------------------
    



    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    switch (option)
    {
        // CREO QUE ES MEJOR HACERLO EN UN SEGUNDO PLANO Y SOLO MOSTRAR INFO DE LA SINCRONIZACIÓN SI OCURRE,
        // OSEA SI HAY ALGO QUE SUBIR Y SE PUEDE SUBIR (HAY WIFI)
        //case DATA_TO_UPLOAD:            tft.setCursor(80, 158);         tft.println("SINCRONIZACI\xD3""N NECESARIA");           break; 

        case UPLOADING_DATA:            tft.setCursor(230, 158);        tft.println("SINCRONIZANDO...");                        break; 

        case ALL_MEALS_UPLOADED:        tft.setCursor(70, 158);         tft.println("\xA1""SMARTCLOTH SINCRONIZADO\x21""");     break;

        case MEALS_LEFT:                tft.setCursor(100, 158);         tft.println("SINCRONIZACI\xD3""N PARCIAL");             break;

        case ERROR_READING_TXT:         
        case NO_INTERNET_CONNECTION:     
        case HTTP_ERROR:                tft.setCursor(180,100);         tft.println("\xA1""ERROR DEL SISTEMA\x21""");            break;

        //case TIMEOUT:                   tft.setCursor(30,158);         tft.println("ERROR DEL SISTEMA: TIMEOUT");                                 break;

        //case UNKNOWN_ERROR:             tft.setCursor(30,158);         tft.println("ERROR DEL SISTEMA: DESCONOCIDO");                       break;
        
        default:    break;
    }
    // ----------------------------------------------------------------------------------------------------


    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    switch (option){
        //case DATA_TO_UPLOAD:            tft.setCursor(110, 358);                                        tft.println("COMPROBANDO CONEXI\xD3""N A INTERNET...");         break; 

        case UPLOADING_DATA:            tft.setCursor(70, 358);                                         tft.println("ESPERE MIENTRAS SE COMPLETA LA SUBIDA");           break; 

        case ALL_MEALS_UPLOADED:        tft.setCursor(210, 358);                                        tft.println("LA WEB SE HA ACTUALIZADO");                        break;

        case MEALS_LEFT:                tft.setCursor(50, 358);                                         tft.println("ALGUNAS COMIDAS NO SE HAN SINCRONIZADO");                
                                        tft.setCursor(100, tft.getCursorY() + tft.getTextSizeY()+20);   tft.println("SE INTENTAR\xC1"" DE NUEVO M\xC1""S ADELANTE");    break;

        case ERROR_READING_TXT:         tft.setCursor(60, 420);                                         tft.println("FALL\xD3"" LA LECTURA DEL FICHERO DE COMIDAS");    break;
        
        case NO_INTERNET_CONNECTION:    tft.setCursor(125, 420);                                        tft.println("SE PERDI\xD3"" LA CONEXI\xD3""N A INTERNET");
                                        tft.setCursor(50, tft.getCursorY() + tft.getTextSizeY()+20);    tft.println("NO SE PUEDE SINCRONIZAR LA INFORMACI\xD3""N");     break;
              
        case HTTP_ERROR:                tft.setCursor(80, 358);                                         tft.println("FALL\xD3"" LA SINCRONIZACI\xD3""N CON LA WEB");    break;

        // case TIMEOUT:                   tft.setCursor(100, 358);                                        tft.println("M\xD3""DULO WIFI NO RESPONDE");    break;

        default:    break;
    }
    // ----------------------------------------------------------------------------------------------------

}



/**
 * Muestra el estado de sincronización en la pantalla.
 * 
 * @param option Opción que indica el estado de sincronización.
 *               Los valores posibles son:
 *               - UPLOADING_DATA: Sincronización en progreso.
 *               - ALL_MEALS_UPLOADED: Todas las comidas han sido sincronizadas.
 *               - MEALS_LEFT: Algunas comidas no se han sincronizado.
 *               - ERROR_READING_TXT: Error al leer el archivo de comidas.
 *               - NO_INTERNET_CONNECTION: Se perdió la conexión a internet.
 *               - HTTP_ERROR: Error de sincronización con la web.
 *               - TIMEOUT: El módulo WiFi no responde.
 *               - UNKNOWN_ERROR: Error desconocido del sistema.
 */
void showNewSyncState(byte option)
{
    // ---- COLOR FONDO Y TEXTO ------------------------------------------------------------
    uint16_t backgroundColor, lineColor, textColor;
    
    switch(option)
    {
        case UPLOADING_DATA:            backgroundColor = AZUL_PROCESO;                 lineColor = textColor = WHITE;                          break;
        case ALL_MEALS_UPLOADED:        backgroundColor = VERDE_PEDIR_Y_EXITO;          lineColor = textColor = WHITE;                          break;
        case MEALS_LEFT:                backgroundColor = AMARILLO_CONFIRM_Y_AVISO;     lineColor = textColor = ROJO_TEXTO_CONFIRM_Y_AVISO;     break;
        
        // Error: option en rango [5, 9]
        default:                        backgroundColor = RED_ERROR_Y_CANCEL;           lineColor = textColor = WHITE;                          break;

    }

    tft.clearScreen(backgroundColor);       // Aplicar color al fondo
    tft.setTextForegroundColor(textColor);  // Aplicar color al texto
    // -----------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    switch (option)
    {
        case UPLOADING_DATA:            tft.setCursor(270, 30);    tft.println("SINCRONIZANDO...");                                             break; 

        case ALL_MEALS_UPLOADED:        tft.setCursor(70, 30);     tft.println(convertSpecialCharactersToHEX("¡SMARTCLOTH SINCRONIZADO!"));     break;

        case MEALS_LEFT:                tft.setCursor(100, 158);   tft.println("SINCRONIZACI\xD3""N PARCIAL");                                  break;

        case ERROR_READING_TXT:         
        case NO_INTERNET_CONNECTION:     
        case HTTP_ERROR:                tft.setCursor(180,100);    tft.println("\xA1""ERROR DEL SISTEMA\x21""");                                break;

        //case TIMEOUT:                   tft.setCursor(30,158);         tft.println("ERROR DEL SISTEMA: TIMEOUT");                             break;

        //case UNKNOWN_ERROR:             tft.setCursor(30,158);         tft.println("ERROR DEL SISTEMA: DESCONOCIDO");                         break;
        
        default:    break;
    }
    // ----------------------------------------------------------------------------------------------------


    // ----- DIBUJO O ICONO ----------------------------------------------------------------------------------
    switch (option)
    {
        case UPLOADING_DATA: // Icono sincronizando SM
            // Toma desde y=1 para quitar linea de basura y, para evitar la linea de debajo, hacemos como que es de 122 píxeles de alto
            tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,1,PAGE1_START_ADDR,SCREEN_WIDTH,420,130,188,122); // Mostrar sincronizando (188x123) en PAGE1
            //tft.fillRoundRect(252,280,764,288,3,lineColor); // Línea bajo el icono
            tft.fillRoundRect(210,280,814,288,3,lineColor);
            break;

        case ALL_MEALS_UPLOADED: // Icono SM sincronizado
            tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,189,0,PAGE1_START_ADDR,SCREEN_WIDTH,402,147,220,136); // Mostrar SM sincronizado (220x136) en PAGE1
            tft.fillRoundRect(210,300,814,308,3,lineColor); // Línea bajo el icono
            break;

        case MEALS_LEFT:
            tft.fillRoundRect(252,110,764,118,3,lineColor); // Línea superior
            tft.fillRoundRect(252,270,764,278,3,lineColor); // Línea inferior
            break;

        
        case ERROR_READING_TXT:
        case NO_INTERNET_CONNECTION:
        case HTTP_ERROR:
        case TIMEOUT:
        case UNKNOWN_ERROR:
            tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,292,PAGE1_START_ADDR,SCREEN_WIDTH,451,231,114,127); // Mostrar cruz (114x127) en PAGE1
            tft.fillRoundRect(252,290,764,298,3,lineColor); // Dibujar líea por encima de la imagen de cruz para no tener que cuadrarla
            break;

        default:
            break;
    }


    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(textColor);
    // -----------------------------------------------------------------------------------------------
    


    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    switch (option)
    {
        case UPLOADING_DATA:            
            tft.setCursor(70, 338);                                             tft.println("ESPERE MIENTRAS SE FINALIZA LA SUBIDA");
            tft.setCursor(300,tft.getCursorY() + tft.getTextSizeY()-10);        tft.println(convertSpecialCharactersToHEX("DE INFORMACIÓN"));
            tft.setCursor(200, tft.getCursorY() + tft.getTextSizeY() + 20);     tft.println(convertSpecialCharactersToHEX("No retire el teléfono móvil"));              break; 

        case ALL_MEALS_UPLOADED:        tft.setCursor(215, 388);                                        tft.println("LA WEB SE HA ACTUALIZADO");                        break;

        case MEALS_LEFT:                tft.setCursor(50, 358);                                         tft.println("ALGUNAS COMIDAS NO SE HAN SINCRONIZADO");                
                                        tft.setCursor(100, tft.getCursorY() + tft.getTextSizeY()+20);   tft.println("SE INTENTAR\xC1"" DE NUEVO M\xC1""S ADELANTE");    break;

        case ERROR_READING_TXT:         tft.setCursor(60, 420);                                         tft.println("FALL\xD3"" LA LECTURA DEL FICHERO DE COMIDAS");    break;
        
        case NO_INTERNET_CONNECTION:    tft.setCursor(125, 420);                                        tft.println("SE PERDI\xD3"" LA CONEXI\xD3""N A INTERNET");
                                        tft.setCursor(50, tft.getCursorY() + tft.getTextSizeY()+20);    tft.println("NO SE PUEDE SINCRONIZAR LA INFORMACI\xD3""N");     break;
              
        case HTTP_ERROR:                tft.setCursor(80, 358);                                         tft.println("FALL\xD3"" LA SINCRONIZACI\xD3""N CON LA WEB");    break;

        // case TIMEOUT:                   tft.setCursor(100, 358);                                        tft.println("M\xD3""DULO WIFI NO RESPONDE");    break;

        default:    break;
    }
    // ----------------------------------------------------------------------------------------------------

}


void showSincronizandoSM()
{
    // ---- COLOR FONDO -----------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AZUL_PROCESO); // 0x037F en RGB565 (#006CFF en HEX) es el color que dio Guillermo, pero por el ángulo se ve celeste
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);
    tft.setTextForegroundColor(WHITE);

    tft.setCursor(270, 30);    tft.println("SINCRONIZANDO...");
    // ----------------------------------------------------------------------------------------------------

    // ------ ICONO (SINCRONIZANDO SM) --------------------------------------------------------------------
    // Toma desde y=1 para quitar linea de basura y, para evitar la linea de debajo, hacemos como que es de 122 píxeles de alto
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,0,1,PAGE1_START_ADDR,SCREEN_WIDTH,420,130,188,122); // Mostrar sincronizando (188x123) en PAGE1
    // ----------------------------------------------------------------------------------------------------

    // ------------ LINEA ---------------------------------------------------------------------------------
    //tft.fillRoundRect(252,280,764,288,3,WHITE);
    tft.fillRoundRect(210,280,814,288,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);
    tft.setCursor(70, 338);    tft.println("ESPERE MIENTRAS SE FINALIZA LA SUBIDA");
    tft.setCursor(300,tft.getCursorY() + tft.getTextSizeY()-10);    tft.println(convertSpecialCharactersToHEX("DE INFORMACIÓN"));

    tft.setCursor(200, tft.getCursorY() + tft.getTextSizeY() + 20);    tft.println(convertSpecialCharactersToHEX("No retire el teléfono móvil"));
    // ----------------------------------------------------------------------------------------------------

    
}


void showSMSincronizado()
{
    // ---- COLOR FONDO -----------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(VERDE_PEDIR_Y_EXITO); 
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);
    tft.setTextForegroundColor(WHITE);

    tft.setCursor(70, 30);    tft.println(convertSpecialCharactersToHEX("¡SMARTCLOTH SINCRONIZADO!"));
    // ----------------------------------------------------------------------------------------------------

    // ------ ICONO (SM SINCRONIZADO) --------------------------------------------------------------------
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,189,0,PAGE1_START_ADDR,SCREEN_WIDTH,402,147,220,136); // Mostrar SM sincronizado (220x136) en PAGE1
    // ----------------------------------------------------------------------------------------------------

    // ------------ LINEA ---------------------------------------------------------------------------------
    tft.fillRoundRect(210,300,814,308,3,WHITE);
    // ----------------------------------------------------------------------------------------------------


    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);
    tft.setCursor(215, 388);    tft.println("LA WEB SE HA ACTUALIZADO");
    // ----------------------------------------------------------------------------------------------------

    
}





void drawSmartClothDevice() 
{
    // Limpiar la pantalla
    tft.clearScreen(AZUL_PROCESO); // Fondo azul

    // Dibujar el borde exterior del dispositivo
    tft.drawRoundRect(10, 10, 1004, 580, 20, WHITE); // Borde exterior con radio de 20

    // Dibujar la pantalla del dispositivo
    tft.drawRoundRect(30, 30, 400, 200, 10, WHITE); // Pantalla con radio de 10

    // Dibujar el botón de activar el lector de código de barras
    tft.drawRoundRect(80, 250, 50, 50, 5, WHITE); // Botón de activar el lector de código de barras con radio de 5

    // Dibujar el lector de código de barras
    tft.drawCircle(60, 275, 10, WHITE); // Lector de código de barras

    // Dibujar la botonera con los grupos de alimentos
    tft.drawRoundRect(30, 320, 400, 200, 10, WHITE); // Botonera con radio de 10

    // Dibujar el plato de la báscula
    tft.drawRoundRect(500, 30, 300, 300, 20, WHITE); // Plato de la báscula con radio de 20
    tft.drawCircle(650, 180, 5, WHITE); // Centro del plato

    // Dibujar la botonera de las acciones
    tft.drawRoundRect(500, 350, 300, 100, 10, WHITE); // Botonera de las acciones con radio de 10
}


void drawCenteredSmartClothDevice() {
    // Dimensiones de la pantalla
    const int screenWidth = 1024;
    const int screenHeight = 600;

    // Color de fondo y de los bordes
    const uint16_t borderColor = WHITE; // Blanco

    // Limpiar la pantalla y establecer el color de fondo
    tft.clearScreen(AZUL_PROCESO);

    // Dimensiones del dispositivo SmartCloth
    const int deviceWidth = 300;
    const int deviceHeight = 200;

    // Calcular las coordenadas de la esquina superior izquierda del dispositivo para centrarlo
    int deviceX1 = (screenWidth - deviceWidth) / 2;
    int deviceY1 = (screenHeight - deviceHeight) / 2;
    int deviceX2 = deviceX1 + deviceWidth;
    int deviceY2 = deviceY1 + deviceHeight;

    // Dibujar el borde del dispositivo
    tft.drawRoundRect(deviceX1, deviceY1, deviceX2, deviceY2, 10, borderColor);

    // Dibujar la pantalla
    tft.drawRoundRect(deviceX1 + 20, deviceY1 + 20, deviceX1 + 120, deviceY1 + 70, 10, borderColor);

    // Dibujar el lector de códigos de barras
    tft.drawCircle(deviceX1 + 160, deviceY1 + 45, 10, borderColor);
    tft.drawRoundRect(deviceX1 + 180, deviceY1 + 35, deviceX1 + 200, deviceY1 + 55, 5, borderColor);

    // Dibujar la botonera de grupos de alimentos
    tft.drawRoundRect(deviceX1 + 20, deviceY1 + 100, deviceX1 + 120, deviceY1 + 150, 10, borderColor);

    // Dibujar el plato de la báscula
    tft.drawRoundRect(deviceX1 + 180, deviceY1 + 20, deviceX1 + 280, deviceY1 + 120, 10, borderColor);
    tft.drawCircle(deviceX1 + 230, deviceY1 + 70, 5, borderColor);

    // Dibujar la botonera de acciones
    tft.drawRoundRect(deviceX1 + 180, deviceY1 + 150, deviceX1 + 280, deviceY1 + 180, 5, borderColor);
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
    tft.clearScreen(VERDE_PEDIR_Y_EXITO); // Fondo verde en PAGE1

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
    tft.clearScreen(VERDE_PEDIR_Y_EXITO); // Fondo verde en PAGE1

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
    tft.clearScreen(VERDE_PEDIR_Y_EXITO); // Fondo verde en PAGE1

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
    tft.clearScreen(VERDE_PEDIR_Y_EXITO); // Fondo verde en PAGE1

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
  //int x1, y1, x2, y2;

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
        tft.fillRoundRect(30,145,994,580,20,VERDE_PEDIR_Y_EXITO); // 964 x 435
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
              tft.drawRect(x1,y1,x2,y2,VERDE_PEDIR_Y_EXITO); // Alrededor de botón
              //tft.drawRoundRect(x1,y1,x2,y2,20,VERDE_PEDIR_Y_EXITO);
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
              tft.drawRect(x1,y1,x2,y2,VERDE_PEDIR_Y_EXITO); // Alrededor de botón
              //tft.drawRoundRect(x1,y1,x2,y2,20,VERDE_PEDIR_Y_EXITO);
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
    tft.clearScreen(VERDE_PEDIR_Y_EXITO); // Fondo verde en PAGE1

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
    tft.clearScreen(VERDE_PEDIR_Y_EXITO);
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
    tft.clearScreen(VERDE_PEDIR_Y_EXITO);
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
        tft.setTextColor(WHITE,VERDE_PEDIR_Y_EXITO,RA8876_TEXT_TRANS_OFF); // Texto blanco remarcado con fondo morado oscuro, se superpone al fondo verde del canvas (RA8876_TEXT_TRANS_OFF)
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
    tft.clearScreen(VERDE_PEDIR_Y_EXITO);
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

void showSavingMeal(bool conexion) 
{
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AZUL_PROCESO);
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO ----------------------------------------------------------------------------------
    // ---- RECUADRO ----
    /*
    // Borde:
    tft.drawRect(50,110,974,278,WHITE);
    tft.drawRect(51,111,973,277,WHITE);
    tft.drawRect(52,112,972,276,WHITE);
    tft.drawRect(53,113,971,275,WHITE);
    tft.drawRect(54,114,970,274,WHITE);
    // Relleno:
    tft.fillRect(55, 115, 969, 273, AZUL_RECUADRO_PROCESO); // Rellenar el rectángulo más interno con DARK_BLUE
    */
    uint16_t xOrigin = 50, yOrigin = 110, ancho = 924, alto = 168, grosor = 5; 
    tft.drawThickFillRect(xOrigin, yOrigin, ancho, alto, grosor, WHITE, AZUL_RECUADRO_PROCESO);
    // ------------------


    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(WHITE);
    // -----------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    tft.setCursor(150, 158);    tft.println("GUARDANDO COMIDA...");
    // -----------------------------------------------------------------------------------------------

    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    tft.setCursor(100, 358);   tft.println("ESPERE MIENTRAS SE GUARDA LA COMIDA");
    if(conexion == SAVING_ONLY_LOCAL){ tft.setCursor(150, tft.getCursorY() + tft.getTextSizeY()+20);   tft.println("NO SE PUEDE GUARDAR EN LA WEB"); }


    // ----- CONEXION A INTERNET O NO----------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    if(conexion == SAVING_WIFI) // CON WIFI
    { 
        // "Resaltar" texto:
        tft.setTextColor(WHITE,VERDE_PEDIR_Y_EXITO,RA8876_TEXT_TRANS_OFF); // Texto blanco remarcado con fondo morado oscuro, se superpone al fondo verde del canvas (RA8876_TEXT_TRANS_OFF)
        // Texto
        tft.setCursor(20,550);    tft.println(" CONECTADO A INTERNET ");
        // Eliminar "resaltado" del texto de aquí en adelante:
        tft.ignoreTextBackground(); // Ignorar el color de background del texto que haya y mostrar fondo canvas
    }
    else // SIN INTERNET
    {
        // "Resaltar" texto:
        tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_OFF); // Texto blanco remarcado con fondo rojo, se superpone al fondo verde del canvas (RA8876_TEXT_TRANS_OFF)
        // Texto
        tft.setCursor(840,550); tft.println(" SIN INTERNET "); 
        // Eliminar "resaltado" del texto de aquí en adelante:
        tft.ignoreTextBackground(); // Ignorar el color de background del texto que haya y mostrar fondo canvas
    }
    // -----------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------


    

}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra una pantalla de "Guardando comida..." para que el usuario espere mientras se completa el guardado.
 */
/*-----------------------------------------------------------------------------*/
void showSavingMealBase() 
{
    // ---- COLOR FONDO ------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AZUL_PROCESO);
    // -----------------------------------------------------------------------------------------------

    // ----- DIBUJO ----------------------------------------------------------------------------------
    // ---- RECUADRO ----
    // Borde:
    tft.drawRect(50,110,974,278,WHITE);
    tft.drawRect(51,111,973,277,WHITE);
    tft.drawRect(52,112,972,276,WHITE);
    tft.drawRect(53,113,971,275,WHITE);
    tft.drawRect(54,114,970,274,WHITE);
    // Relleno:
    tft.fillRect(55, 115, 969, 273, AZUL_RECUADRO_PROCESO); // Rellenar el rectángulo más interno con DARK_BLUE
    // ------------------


    // Restablecer color de texto. Al dibujar (recuadro o líneas), el foregroundColor se cambia a lineColor
    tft.setTextForegroundColor(WHITE);
    // -----------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);

    tft.setCursor(150, 158);    tft.println("GUARDANDO COMIDA...");
    // -----------------------------------------------------------------------------------------------

    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 

    tft.setCursor(100, 358);   tft.println("ESPERE MIENTRAS SE GUARDA LA COMIDA");
    
    // En la función completeSavingMeal() se escribe el comentario de si se está guardando solo localmente o también en la database
    // ----------------------------------------------------------------------------------------------------


}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Completa la pantalla de "Guardando comida..." con información según si solo se guarda localmente o
 *          también en la database.
 * 
 * @param option Opción que indica el tipo de guardado
 *               - SAVING_ONLY_LOCAL: Solo se guarda localmente porque no hay conexión a internet
 * 
 *               - SAVING_WIFI: se guarda localmente y también en la database, aprovechando la conexión a internet
 */
/*-----------------------------------------------------------------------------*/
void completeSavingMeal(byte option) 
{
    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextForegroundColor(WHITE);
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    
    // ----- CONEXION A INTERNET O NO----------------------------
    if(option == SAVING_WIFI) // CON WIFI
    { 
        tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
        tft.setTextColor(WHITE,VERDE_PEDIR_Y_EXITO,RA8876_TEXT_TRANS_OFF); // "Resaltar" texto: Texto blanco remarcado con fondo morado oscuro, se superpone al fondo verde del canvas (RA8876_TEXT_TRANS_OFF)
        // Comentario esquina inferior izquierda
        tft.setCursor(20,550);    tft.println(" CONECTADO A INTERNET ");
    }
    else if(option == SAVING_ONLY_LOCAL) // SIN INTERNET
    {
        // Comentario
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setCursor(150, 440);   tft.println("NO SE PUEDE GUARDAR EN LA WEB");

        // Comentario esquina inferior derecha
        tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
        tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_OFF); // "Resaltar" texto: Texto blanco remarcado con fondo rojo, se superpone al fondo verde del canvas (RA8876_TEXT_TRANS_OFF)
        tft.setCursor(840,550); tft.println(" SIN INTERNET "); 
    }

    // Eliminar "resaltado" del texto de aquí en adelante:
    tft.ignoreTextBackground(); // Ignorar el color de background del texto que haya y mostrar fondo canvas
    // -----------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------


}



void showNewSavingMeal(bool conexion)
{
    // ---- COLOR FONDO -----------------------------------------------------------------------------------
    // Aplicar color al fondo
    tft.clearScreen(AZUL_PROCESO); // 0x037F en RGB565 (#006CFF en HEX) es el color que dio Guillermo, pero por el ángulo se ve celeste
    // ----------------------------------------------------------------------------------------------------

    // ----- TEXTO (INFORMACIÓN) --------------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3);
    tft.setTextForegroundColor(WHITE);

    tft.setCursor(200, 30);    tft.println("GUARDANDO COMIDA...");
    // ----------------------------------------------------------------------------------------------------

    // ------ ICONO (GUARDANDO) ---------------------------------------------------------------------------
    tft.bteMemoryCopy(PAGE4_START_ADDR,SCREEN_WIDTH,554,0,PAGE1_START_ADDR,SCREEN_WIDTH,450,150,91,98); // Mostrar disquete (91x98) en PAGE1
    // ----------------------------------------------------------------------------------------------------

    // ------------ LINEA ---------------------------------------------------------------------------------
    //tft.fillRoundRect(252,280,764,288,3,WHITE);
    tft.fillRoundRect(210,280,814,288,3,WHITE);
    // ----------------------------------------------------------------------------------------------------

    // ------ TEXTO (COMENTARIO) --------------------------------------------------------------------------
    tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2);
    tft.setCursor(100, 338);    tft.println("ESPERE MIENTRAS SE GUARDA LA COMIDA");
    if(!conexion)
    {
        /*tft.selectInternalFont(RA8876_FONT_SIZE_16);
        tft.setCursor(170,tft.getCursorY() + tft.getTextSizeY() + 10);   
        tft.println(convertSpecialCharactersToHEX("Se guardará en SMARTCLOTH pero no en la web"));*/

        tft.setCursor(220, tft.getCursorY() + tft.getTextSizeY()); tft.print(convertSpecialCharactersToHEX("Se guardará en SMARTCLOTH"));
        tft.setCursor(280, tft.getCursorY() + tft.getTextSizeY() + 10); tft.println("pero no en la web");
    }
    // ----------------------------------------------------------------------------------------------------

    // ----- ICONO CONEXION A INTERNET O NO ---------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
    if(conexion) // Mostrar icono de conexión a internet y texto
    {
        // Toma desde y=1 para quitar linea de basura y, para evitar la linea de debajo, hacemos como que es de 66 píxeles de alto
        tft.bteMemoryCopy(PAGE4_START_ADDR, SCREEN_WIDTH, 646, 1, PAGE1_START_ADDR, SCREEN_WIDTH, 30, 500, 43, 66); // Mostrar conexión (43x67) en PAGE1. 
        tft.setCursor(85,520);    tft.println(convertSpecialCharactersToHEX("CON CONEXIÓN A INTERNET"));
    }
    else // Mostrar icono de sin conexión a internet y texto
    {   
        tft.bteMemoryCopy(PAGE4_START_ADDR, SCREEN_WIDTH, 689, 0, PAGE1_START_ADDR, SCREEN_WIDTH, 30, 480, 54, 85); // Mostrar no conexión (54x85) en PAGE1
        tft.setCursor(90,520);    tft.println(convertSpecialCharactersToHEX("SIN CONEXIÓN A INTERNET"));
    
    }
    // ----------------------------------------------------------------------------------------------------

    

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
    // ------------ COLOR FONDO Y TEXTO ------------------------------------------------------------------
    switch (option)
    {
        case SAVE_EXECUTED_ONLY_DATABASE:   tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO);  break; // Comida guardado solo en database. ERROR IMPORTANTE EN MEMORIA (acumulado local no actualizado)
        case ERROR_SAVING_DATA:             tft.clearScreen(RED_ERROR_Y_CANCEL);        break; // ERROR IMPORTANTE EN MEMORIA (acumulado local no actualizado)
        default:                            tft.clearScreen(VERDE_PEDIR_Y_EXITO);               break; // Éxito completo o parcial (guardado solo local): option en rango [1, 7]
                                                                                               //     Si falla la subida a database pero se puede guardar en local, también es éxito
    }
    // ----------------------------------------------------------------------------------------------------


    // ------------ LINEA ---------------------------------------------------------------------------------
    // Al dibujar la línea, ya se cambia el color del texto según el caso
    if(option == SAVE_EXECUTED_ONLY_DATABASE)   tft.fillRoundRect(252,150,764,158,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    else                                        tft.fillRoundRect(252,150,764,158,3,WHITE);
    // ----------------------------------------------------------------------------------------------------
    

    // ----- TEXTO (ACCION REALIZADA) ---------------------------------------------------------------------
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 

    switch (option)
    {
        case ADD_EXECUTED:               tft.setCursor(170, 208);   tft.println("NUEVO PLATO A\xD1""ADIDO");  break; // PLATO AÑADIDO
        
        case DELETE_EXECUTED:            tft.setCursor(100, 208);   tft.println("PLATO ACTUAL ELIMINADO");    break; // PLATO ELIMINADO
        
        case SAVE_EXECUTED_FULL:  
        case SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP:   
        case SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI:    
        case SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT:
        case SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR:
                                        tft.setCursor(120, 208);   tft.println("COMIDA ACTUAL GUARDADA");    break; // COMIDA GUARDADA AL MENOS EN LOCAL
    
        case SAVE_EXECUTED_ONLY_DATABASE:   tft.setCursor(120, 208);   tft.println("COMIDA ACTUAL GUARDADA");                break;  // Comida guardada solo en database. Error en acumulado local
        case ERROR_SAVING_DATA:             tft.setCursor(100, 208);   tft.println("\xA1""ERROR AL GUARDAR DATOS\x21""");    break;  // Error al guardar datos

        default: break;
    }
    // ----------------------------------------------------------------------------------------------------
    
    // ------------ LINEA ---------------------------------------------------------------------------------
    // Al dibujar la línea, ya se cambia el color del texto según el caso
    if(option == SAVE_EXECUTED_ONLY_DATABASE)   tft.fillRoundRect(252,330,764,338,3,ROJO_TEXTO_CONFIRM_Y_AVISO);
    else                                        tft.fillRoundRect(252,330,764,338,3,WHITE);
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
      case SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT:  // TIMEOUT EN LA RESPUESTA DEL ESP32. NO SABEMOS SI HA GUARDADO O NO, PERO ASUMIMOS QUE NO
      case SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR:  // GUARDADA SOLO EN LOCAL POR UN ERROR DESCONOCIDO AL GUARDAR EN DATABASE
                // Puede ser que se quiera guardar desde el STATE_Init, tras añadir o borrar. Si es así, la báscula estará vacía (pesoARetirar = 0).
                // No se pone if(pesoARetirar ...) porque aún no ha dado tiempo a actualizar 'pesoARetirar' y puede ser incorrecto
                /*if(lastValidState == STATE_Init)
                {
                    tft.setCursor(190, 388); tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO");
                    tft.setCursor(350, tft.getCursorY() + tft.getTextSizeY()+40); tft.print("AL ACUMULADO DE HOY"); 
                }
                else // Si se guarda tras conformar el plato, estando aún en la báscula, indicando que se retire
                {*/ 
                    tft.setCursor(30, 388); tft.println("LOS VALORES NUTRICIONALES SE HAN A\xD1""ADIDO AL ACUMULADO DE HOY");  
                    tft.setCursor(200,450); tft.println("RETIRE EL PLATO PARA COMENZAR DE NUEVO"); 
                //}

                // --- MENSAJE DE SUBIDA EN ESQUINA --------------
                // Tamaño de texto para mensaje en la esquina
                tft.selectInternalFont(RA8876_FONT_SIZE_24);
                tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
            
                // Color de texto según mensaje
                if (option == SAVE_EXECUTED_FULL) // SE HA GUARDADO LOCAL Y DATABASE
                    tft.setTextColor(WHITE,DARKPURPLE,RA8876_TEXT_TRANS_OFF); // Texto blanco sobre fondo morado oscuro
                else // SOLO SE HA GUARDADO EN LOCAL
                    tft.setTextColor(WHITE,RED,RA8876_TEXT_TRANS_OFF); // Texto blanco sobre fondo rojo
            
                // Mensaje según tipo de guardado
                switch(option)
                {
                    case SAVE_EXECUTED_FULL:                        tft.setCursor(20,550);    tft.println(" SUBIDO A WEB ");                        break;
                    case SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP:       tft.setCursor(785,550);   tft.println(" ERROR EN EL ENV\xCD""O ");              break;
                    case SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI:          tft.setCursor(810,550);   tft.println(" NO SUBIDO A WEB ");                     break;
                    case SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT:          tft.setCursor(705,550);   tft.println(" ERROR EN ENV\xCD""O (TIMEOUT) ");       break;
                    case SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR:    tft.setCursor(790,550);   tft.println(" ERROR DESCONOCIDO ");                   break;
                    default: break;
                }


                // Eliminar "resaltado" del texto de aquí en adelante:
                tft.ignoreTextBackground(); // Ignorar el color de background del texto que haya y mostrar fondo canvas
                // ---- FIN MENSAJE DE SUBIDA EN ESQUINA ---------

                break; 


        case SAVE_EXECUTED_ONLY_DATABASE:
        case ERROR_SAVING_DATA:
                if(option == SAVE_EXECUTED_ONLY_DATABASE){ tft.setCursor(220, 388); tft.println("ACUMULADO NO ACTUALIZADO, SOLO WEB"); }
                else if(option == ERROR_SAVING_DATA){ tft.setCursor(85, 388); tft.println("FALLO IMPORTANTE EN MEMORIA. ACUMULADO NO ACTUALIZADO"); }

                // Si se guarda tras conformar el plato, estando aún en la báscula, indicando que se retire
                //if(lastValidState != STATE_Init){ tft.setCursor(50,450); tft.println("RETIRE EL PLATO Y CONTACTE CON EL EQUIPO DE SMARTCLOTH");}
                tft.setCursor(80,450); tft.println("RETIRE EL PLATO Y CONTACTE CON EL EQUIPO DE SMARTCLOTH");

                break;

        default: break;

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
    //int integerPart;

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
    /*tft.fillRoundRect(30,145,504,580,20,GRIS_CUADROS); // 474 x 425
    tft.drawRoundRect(30,145,504,580,20,AZUL_BORDE_CUADRO); // Borde => 474 x 425
    tft.drawRoundRect(31,146,503,579,20,AZUL_BORDE_CUADRO); // Borde => 473 x 424
    */
    uint16_t xOrigin = 30, yOrigin = 145, ancho = 474, alto = 435, radio = 20, grosor = 2; 
    tft.drawThickRoundFillRect(xOrigin,yOrigin,ancho,alto,radio,grosor,AZUL_BORDE_CUADRO, GRIS_CUADROS); // Recuadro azul de 474x435 con grosor de 2 pixeles

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
    //tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,0,292,PAGE1_START_ADDR,SCREEN_WIDTH,451,231,114,127); // Mostrar cruz (114x127) en PAGE1
    //drawBigX(512, 300, 100, 45, 135, 5, RED);
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




/*---------------------------------------------------------------------------------------------------------
   showWarning(): Pantalla genérica de aviso. Según la opción indicada como parámetro se muestra un mensaje 
                    diferente.
          Parámetros:
                option - 1: Plato no añadido                                2: Plato no borrado                     3: Comida no guardada          
                         4: No hace falta crudo/cocinado para barcode       5: Barcode no detectado                 6: Producto no encontrado    
                         7: Comidas sin guardar en web                      8: Sin internet para buscar barcode         
----------------------------------------------------------------------------------------------------------*/
void showWarning(byte option, String barcode)
{
    // ----- TEXTO (AVISO) -------------------------------------------------------------------------------
    tft.clearScreen(AMARILLO_CONFIRM_Y_AVISO); // Fondo amarillo en PAGE1

    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.setTextScale(RA8876_TEXT_W_SCALE_X3, RA8876_TEXT_H_SCALE_X3); 
    tft.setTextForegroundColor(ROJO_TEXTO_CONFIRM_Y_AVISO); 

    // Título principal de la pantalla
    switch(option){
        case WARNING_BARCODE_NOT_READ:      tft.setCursor(100, 100);    tft.println("\xA1""PRODUCTO NO DETECTADO\x21""");           break;
        case WARNING_PRODUCT_NOT_FOUND:     tft.setCursor(100, 100);    tft.println("\xA1""PRODUCTO NO ENCONTRADO\x21""");          break;
        case WARNING_MEALS_LEFT:            tft.setCursor(100, 100);    tft.println("\xA1""SINCRONIZACI\xD3""N PARCIAL\x21""");     break;
        case WARNING_NO_INTERNET_NO_BARCODE:           tft.setCursor(70, 100);    tft.println("\xA1""SIN CONEXI\xD3""N A INTERNET\x21""");    break;
        // ADD, DELETE, SAVE
        default:                            tft.setCursor(384, 100);    tft.println("\xA1""AVISO\x21""");                           break;
    }
    
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

    switch (option)
    {
        case WARNING_NOT_ADDED: // AÑADIR
            tft.setCursor(150, 410);                                        tft.println("NO SE HA CREADO UN NUEVO PLATO"); 
            tft.setCursor(180, tft.getCursorY() + tft.getTextSizeY());      tft.print("PORQUE EL ACTUAL EST\xC1"" VAC\xCD""O");  
            break;

        case WARNING_NOT_DELETED: // ELIMINAR
            tft.setCursor(180, 410);                                        tft.println("NO SE HA ELIMINADO EL PLATO"); 
            tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY());      tft.print("PORQUE EST\xC1"" VAC\xCD""O"); 
            break;

        case WARNING_NOT_SAVED: // GUARDAR
            tft.setCursor(190, 410);                                        tft.println("NO SE HA GUARDADO LA COMIDA"); 
            tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY());      tft.print("PORQUE EST\xC1"" VAC\xCD""A"); 
            break;

        case WARNING_RAW_COOKED_NOT_NEEDED: // NO HACE FALTA CRUDO/COCINADO PARA PRODUCTO BARCODE
            tft.setCursor(50, 410);                                         tft.println("NO HACE FALTA ESCOGER GRUPO O COCINADO"); 
            tft.setCursor(300, tft.getCursorY() + tft.getTextSizeY());      tft.print("PARA ESTE PRODUCTO");  
            break;

        case WARNING_BARCODE_NOT_READ: // CÓDIGO DE BARRAS NO LEÍDO
            tft.setCursor(100, 410);                                        tft.println("NO SE HA LE\xCD""DO EL C\xD3""DIGO DE BARRAS");
            break;

        case WARNING_PRODUCT_NOT_FOUND: // PRODUCTO NO ENCONTRADO
            tft.setCursor(150, 410);                                        tft.println("NO SE HA ENCONTRADO EL PRODUCTO");
            //String cad = "CON EL C\xD3""DIGO: " + barcode;
            tft.setCursor(180, tft.getCursorY() + tft.getTextSizeY()+10);   tft.println("CON EL C\xD3""DIGO: " + barcode); 
            break;

        case WARNING_MEALS_LEFT: // SINCRONIZACIÓN PARCIAL
            tft.setCursor(50, 410);                                         tft.println("ALGUNAS COMIDAS NO SE HAN SINCRONIZADO"); 
            tft.setCursor(100, tft.getCursorY() + tft.getTextSizeY()+20);   tft.println("SE INTENTAR\xC1"" DE NUEVO M\xC1""S ADELANTE");
            break;

        case WARNING_NO_INTERNET_NO_BARCODE: // SIN CONEXIÓN A INTERNET
            tft.setCursor(140, 410);         tft.println("NO SE PUEDE BUSCAR EL PRODUCTO");
            break;

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


/**
 * @brief Función que realiza el arranque del programa con las letras apareciendo poco a poco.
*/
void arranqueSlowOpacity(){

    uint8_t i; // Variable i de los bucles for para ir modificando la opacidad de las imágenes y que aparezcan poco a poco

    tft.clearScreen(WHITE); // Fondo blanco en PAGE1
    delay(200);

    // S M A R T C L O T H ==> S T O T M L R A C H (orden de aparición)

    // ----------- LETRA S ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar S apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,40,150,95,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,0,0,PAGE1_START_ADDR,SCREEN_WIDTH,40,150,95,159);       // S (95x159) 
    delay(80);

    // ----------- LETRA T1 ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar T1 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,428,150,104,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,428,150,104,159);   // T1 (104x159)
    delay(80);

    // ----------- LETRA O ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar  apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,669,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,702,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,669,0,PAGE1_START_ADDR,SCREEN_WIDTH,702,150,85,159);    // O (85x159)
    delay(80);
    
    // ----------- LETRA T2 ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar T2 apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,787,150,104,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,392,0,PAGE1_START_ADDR,SCREEN_WIDTH,787,150,104,159);   // T2 (104x159) 
    delay(80);

    // ----------- LETRA M ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar M apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,96,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,135,150,104,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,96,0,PAGE1_START_ADDR,SCREEN_WIDTH,135,150,104,159);    // M (104x154) 
    delay(80);
    
    // ----------- LETRA L ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar L apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,583,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,617,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,583,0,PAGE1_START_ADDR,SCREEN_WIDTH,617,150,85,159);    // L (85x159) 
    delay(80);

    // ----------- LETRA R ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar R apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,306,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,343,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,306,0,PAGE1_START_ADDR,SCREEN_WIDTH,343,150,85,159);    // R (85x159) 
    delay(80);

    // ----------- LETRA A ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar A apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,201,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,239,150,104,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,201,0,PAGE1_START_ADDR,SCREEN_WIDTH,239,150,104,159);   // A (104x159) 
    delay(80);

    // ----------- LETRA C ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar C apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,497,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,532,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,497,0,PAGE1_START_ADDR,SCREEN_WIDTH,532,150,85,159);    // C (85x159) 
    delay(80);

    // ----------- LETRA H ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar H apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,755,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,891,150,85,159,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,755,0,PAGE1_START_ADDR,SCREEN_WIDTH,891,150,85,159);    // H (85x159) 
    delay(80);   

    
    // ----------- LOGO ----------------------
    for(i = 32; i >= 1; i--){ // i = 16 --> RA8876_ALPHA_OPACITY_16
        // Mostrar LOGO apareciendo con opacidad a nivel i/32. Utiliza el propio fondo verde de la page1 como S1.
        tft.bteMemoryCopyWithOpacity(PAGE2_START_ADDR,SCREEN_WIDTH,841,0,PAGE1_START_ADDR,SCREEN_WIDTH,0,400,PAGE1_START_ADDR,SCREEN_WIDTH,417,350,162,169,i);
        delay(10);
    }
    tft.bteMemoryCopy(PAGE2_START_ADDR,SCREEN_WIDTH,841,0,PAGE1_START_ADDR,SCREEN_WIDTH,417,350,162,169);   // Logo (162x169) ==> debajo
    
    delay(500);
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
            tft.clearArea(500,280,724,480,VERDE_PEDIR_Y_EXITO); // Borrar crudoGra
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
            tft.clearArea(280,280,497,470,VERDE_PEDIR_Y_EXITO); // Borrar cociGra
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
                  tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,posY,119,127,VERDE_PEDIR_Y_EXITO); // Transparencia manoGppt (120x128)
                  delay(50);
                  if(posY < 413) posY = 413; // Solo afecta al penúltimo movimiento de la mano, para evitar que se borre parte del grupo3 que está debajo
                  tft.clearArea(556,posY,690,posY + alto+5,VERDE_PEDIR_Y_EXITO); // Desaparece de esa zona para aparecer en otra --> se mueve
                  posY -= 10; // Subimos verticalmente la imagen 10 píxeles
              }
              posY = 400;
              while(posY >= 380){
                  // Mostrar grupo3 (130x125). Para superponerse a la mano, que aún se está "moviendo"
                  tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 
                  // Mostrar mano (manoGppt)
                  tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,posY,119,127,VERDE_PEDIR_Y_EXITO); // Transparencia manoGppt (120x128)
                  delay(50);
                  if(posY == 400) tft.clearArea(556,413,690,528,VERDE_PEDIR_Y_EXITO); // Se borra desde y = 413 para no borrar parte del botón. Solo se borra en la primera iteración del bucle
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
              tft.clearArea(546,278,690,528,VERDE_PEDIR_Y_EXITO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano

              // 2 - Mostrar grupo3 (130x125)
              tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 

              // 3 - Mostrar mano (manoGppt)
              tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,119,127,VERDE_PEDIR_Y_EXITO); // Transparencia manoGppt (120x128)

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


/*void drawThickLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t thickness, uint16_t color) 
{
    // Calcular la dirección de la línea
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;

    // Normalizar la dirección
    float length = sqrt(deltaX * deltaX + deltaY * deltaY);
    float dirX = deltaX / length;
    float dirY = deltaY / length;

    // Calcular el vector perpendicular a la dirección de la línea
    float perpX = -dirY;
    float perpY = dirX;

    // Dibujar líneas paralelas desplazadas por el vector perpendicular para crear el grosor
    for (int i = 0; i < thickness; i++) {
        float offset = i - thickness / 2.0;
        uint16_t newX1 = x1 + offset * perpX;
        uint16_t newY1 = y1 + offset * perpY;
        uint16_t newX2 = x2 + offset * perpX;
        uint16_t newY2 = y2 + offset * perpY;
        tft.drawLine(newX1, newY1, newX2, newY2, color);
    }
}*/

void drawHorizontalThickLine(uint16_t x1, uint16_t y1, uint16_t length, uint16_t thickness, uint16_t lineColor) 
{
    for (int i = 0; i < thickness; i++) {
        // Dibujar líneas horizontales desplazadas en y para crear el grosor
        tft.drawLine(x1, y1 + i, x1 + length, y1 + i, lineColor);
        if (i != 0) { // Evita dibujar la línea central dos veces
            tft.drawLine(x1, y1 - i, x1 + length, y1 - i, lineColor);
        }
    }
}

void drawVerticalThickLine(uint16_t x1, uint16_t y1, uint16_t length, uint16_t thickness, uint16_t lineColor) 
{
    for (int i = 0; i < thickness; i++) {
        // Dibujar líneas verticales desplazadas en x para crear el grosor
        tft.drawLine(x1 + i, y1, x1 + i, y1 + length, lineColor);
        if (i != 0) { // Evita dibujar la línea central dos veces
            tft.drawLine(x1 - i, y1, x1 - i, y1 + length, lineColor);
        }
    }
}

void drawThickLineWithAngle(uint16_t x1, uint16_t y1, float length, float angle, uint16_t thickness, uint16_t lineColor)
{
    // Convertir el ángulo de grados a radianes
    float radian = angle * (M_PI / 180.0);

    // Calcular las coordenadas finales usando longitud y ángulo
    uint16_t x2 = x1 + length * cos(radian);
    uint16_t y2 = y1 - length * sin(radian);

    // Dibujar líneas desplazadas para crear un grosor
    for (int i = 0; i < thickness; i++) {
        tft.drawLine(x1 + i, y1, x2 + i, y2, lineColor); // Desplazamiento positivo
        tft.drawLine(x1 - i, y1, x2 - i, y2, lineColor); // Desplazamiento negativo
    }
}

void drawThickLineWithAngle_2(uint16_t x1, uint16_t y1, float length, float angle, uint16_t thickness, uint16_t lineColor)
{
    // Convertir el ángulo de grados a radianes
    float radian = angle * (M_PI / 180.0);

    // Calcular las coordenadas finales usando longitud y ángulo
    uint16_t x2 = x1 + length * cos(radian);
    uint16_t y2 = y1 - length * sin(radian);

    // Dibujar líneas desplazadas para crear un grosor
    for (int i = 0; i < thickness; i++) {
        if (angle == 0 || angle == 180) {
            // Para líneas horizontales, desplazamiento vertical
            tft.drawLine(x1, y1 + i, x2, y2 + i, lineColor); // Desplazamiento positivo
            tft.drawLine(x1, y1 - i, x2, y2 - i, lineColor); // Desplazamiento negativo
        } else if (angle == 90 || angle == 270) {
            // Para líneas verticales, desplazamiento horizontal
            tft.drawLine(x1 + i, y1, x2 + i, y2, lineColor); // Desplazamiento positivo
            tft.drawLine(x1 - i, y1, x2 - i, y2, lineColor); // Desplazamiento negativo
        } else {
            // Para líneas inclinadas, desplazamiento en ángulo perpendicular
            tft.drawLine(x1 + i, y1, x2 + i, y2, lineColor); // Desplazamiento positivo
            tft.drawLine(x1 - i, y1, x2 - i, y2, lineColor); // Desplazamiento negativo
        }
    }
}


// Función para dibujar una "X" grande
/*
x1 = 512 - 50 * cos(45°)
y1 = 300 + 50 * sin(45°)

x2 = 512 - 50 * cos(135°)
y2 = 300 + 50 * sin(135°)
*/
void drawBigX(uint16_t centerX, uint16_t centerY, uint16_t length, uint16_t angleLine1, uint16_t angleLine2, uint16_t thickness, uint16_t lineColor) 
{
    float halfLength = length / 2.0;

    // Convertir el ángulo de grados a radianes
    float angleLine1Rad = angleLine1 * (M_PI / 180.0);
    float angleLine2Rad = angleLine2 * (M_PI / 180.0);

    // Calcular el punto inicial para la primera línea (grados en radianes)
    uint16_t x1 = centerX - halfLength * cos(angleLine1Rad);
    uint16_t y1 = centerY + halfLength * sin(angleLine1Rad);

    // Calcular el punto inicial para la segunda línea (grados en radianes)
    uint16_t x2 = centerX - halfLength * cos(angleLine2Rad);
    uint16_t y2 = centerY + halfLength * sin(angleLine2Rad);

    // Dibujar la primera línea diagonal ('angleLine1' grados)
    //drawThickLineWithAngle(x1, y1, length, angleLine1, thickness, lineColor);
    drawThickLineWithAngle_2(x1, y1, length, angleLine1, thickness, lineColor);

    // Dibujar la segunda línea diagonal ('angleLine2' grados)
    //drawThickLineWithAngle(x2, y2, length, angleLine2, thickness, lineColor);
    drawThickLineWithAngle_2(x2, y2, length, angleLine2, thickness, lineColor);



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
    uint16_t xOrigin, yOrigin, ancho, alto, grosor; 

    switch(option){
        case 1: // Grupos
                // 1 - Borrar todo (grupo, mano y pulsación)
                tft.clearArea(546,278,690,528,VERDE_PEDIR_Y_EXITO); // Empieza en la esquina superior izquierda de la pulsación y termina al final de la mano
                
                // 2 - Volver a mostrar grupo3 (130x125)
                tft.bteMemoryCopy(PAGE3_START_ADDR,SCREEN_WIDTH,262,0,PAGE1_START_ADDR,SCREEN_WIDTH,556,288,130,125); 
                
                // 3 - Pulsación
                // ------------ CUADRADO ESQUINADO (PULSACION) --------------------------------------------------------   
                // No se puede modificar el grosor de las líneas ni de los bordes de las figuras. Por eso se dibujan varios
                // cuadrados normales, separados por 1 píxel en cada dirección, para simular un grosor mayor.
                /*for (int i = 0; i <= 10; i++) {
                    x1 = 556 - i;   y1 = 288 - i;   
                    x2 = 680 + i;   y2 = 413 + i;
                    tft.drawRect(x1,y1,x2,y2,RED_BUTTON); // Alrededor de grupo3
                }*/
                xOrigin = 546, yOrigin = 278, ancho = 144, alto = 145, grosor = 11; 
                tft.drawThickRect(xOrigin,yOrigin,ancho,alto,grosor,RED_BUTTON); // Recuadro rojo de 144x145 con grosor de 11 pixeles
                // ----------------------------------------------------------------------------------------------------

                // 4 - Mano
                // ------------ MANO (120x129) ------------------------------------------------------------------------
                // Mano final pulsando (manoGppt)
                tft.bteMemoryCopyWithChromaKey(PAGE3_START_ADDR,SCREEN_WIDTH,525,1,PAGE1_START_ADDR,SCREEN_WIDTH,566,380,119,127,VERDE_PEDIR_Y_EXITO); // Transparencia manoGppt (120x128)
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
 * @brief Carga en SDRAM las imágenes necesarias para todas las pantallas de SmartCloth mientras muestra 
 * un reloj de arena en la pantalla. Las imágenes del reloj se muestran en una secuencia específica para 
 * lograr el efecto de giro del reloj.
 * La carga de las imágenes se hace por orden de peso para dar la sensación de una carga cada vez más rápida.
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
        brain1  ->  (0,170)   brain2G  ->  (121,170)   
        reloj1 -> (0,279)   reloj2 -> (66,279)   reloj3 -> (132,279)   reloj4 -> (198,279)   relGir1 -> (264,279)    relGir2 -> (360,279)   relGir3 -> (473,279)    relGir4 -> (587,279)    relGir5 -> (688,279)    relGir6 -> (802,279)
        
    PAGE 3:
        grupo1 -> (0,0)   grupo2 -> (131,0)   grupo3 -> (262,0)   grupo4 -> (393,0)   manoGppt -> (524,0)    anadir -> (645,0)    borrar -> (818,0)
        guardar -> (0,131)   cociGra -> (173,131)   crudoGra -> (351,131)    cociPeq -> (529,131)   crudoPeq -> (577,131)
                                                                             kcal -> (529,174)   cuadroGris -> (610,174)
        cruz -> (0,292)   aviso -> (115,292)   manoWppt -> (251,292)   scale -> (372,292)

    PAGE 4:   
        sync -> (0,0)       sync_ok -> (189,0)      aviso -> (410,0)        disquete -> (554,0)     conexion -> (646,0)     no_conex -> (689,0)
        scan -> (0,137)     lupa -> (172,137)      pr_found -> (369,137)    
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


    // AHORA SE CARGAN ALGUNAS IMÁGENES DE LA PÁGINA 4 PORQUE SON MÁS PESADAS QUE LAS DE LA PÁGINA 3 Y ASÍ PARECE QUE EL RELOJ GIRA MÁS RÁPIDO

    // ----------------- SINCRONIZAR SMARTCLOTH ----------------------------------------------------------
        // Sincronizando SM
        tft.canvasImageStartAddress(PAGE4_START_ADDR); // Ir a PAGE4
        tft.clearScreen(BLACK);
        tft.sdCardDraw16bppBIN256bits(0,0,188,123,fileSincronizando);  // Cargar sincronizando (188x123) en PAGE4 =>  x  =  0      ->   y = 0 

        putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1

        // SM sincronizado
        tft.canvasImageStartAddress(PAGE4_START_ADDR); // Regresar a PAGE4
        tft.sdCardDraw16bppBIN256bits(189,0,220,136,fileSMSincronizado);  // Cargar SM sincronizado (220x136) en PAGE4 =>  x  =  <sincronizando(0) + sincronizando(188) + 1 = 189      ->   y = 0

        putReloj2(); // Mostrar reloj2 en PAGE1 

        // Aviso nuevo para sincronización y barcode
        tft.canvasImageStartAddress(PAGE4_START_ADDR); // Regresar a PAGE4
        tft.sdCardDraw16bppBIN256bits(410,0,143,126,fileAvisoNew);  // Cargar aviso nuevo (143x126) en PAGE4 =>  x  =  <sync_ok(189) + sync_ok(220) + 1 = 410      ->   y = 0

        putReloj3(); // Mostrar reloj3 en PAGE1 

    // ----------------- FIN SINCRONIZAR SMARTCLOTH ------------------------------------------------------


    // -------------- LEER BARCODE, BUSCAR PRODUCTO Y CONFIRMAR ------------------------------------------
        // Escanear
        tft.canvasImageStartAddress(PAGE4_START_ADDR); // Regresar a PAGE4
        tft.sdCardDraw16bppBIN256bits(0,137,171,128,fileScanBarcode); // Cargar scan (171x128) en PAGE4 =>  x  =  0  ->   y = <sync_ok(0) + sync_ok(136) + 1 = 137  

        putReloj4(); // Mostrar reloj4 en PAGE1 => borrar PAGE1

        // Lupa
        tft.canvasImageStartAddress(PAGE4_START_ADDR); // Regresar a PAGE4
        tft.sdCardDraw16bppBIN256bits(172,137,82,130,fileSearchProduct); // Cargar lupa (82x130) en PAGE4 =>  x  =  <scan(0) + scan(171) + 1 = 172  ->   y = 137

        putRelojGirado1(); // Mostrar relGir1 en PAGE1

        // Producto encontrado
        tft.canvasImageStartAddress(PAGE4_START_ADDR); // Regresar a PAGE4
        tft.sdCardDraw16bppBIN256bits(369,137,297,104,fileProductFound); // Cargar producto encontrado (297x104) en PAGE4 =>  x  =  <lupa(172) + lupa(82) + 1 = 369  ->   y = 137

        putRelojGirado2(); // Mostrar relGir2 en PAGE1
        
    // --------- FIN LEER BARCODE, BUSCAR PRODUCTO Y CONFIRMAR ------------------------------------------


    // ----------------- AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO ----------------------------------------
      // añadir
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.clearScreen(BLACK);
      tft.sdCardDraw16bppBIN256bits(645,0,172,130,fileBotonAnadir);  // Cargar anadir (172x130) en PAGE3 =>  x  =  <manoR(524) + manoR(120) + 1 = 645      ->   y = 0  
      
       putRelojGirado3(); // Mostrar relGir3 en PAGE1 
      
      // borrar
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(818,0,172,130,fileBotonEliminar);  // Cargar borrar (172x130) en PAGE3 =>  x  = <anadir(645) + anadir(172) + 1 = 818    ->   y = 0  
      
        putRelojGirado4(); // Mostrar relGir4 en PAGE1

      // guardar
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(0,131,172,130,fileBotonGuardar); // Cargar guardar (172x130) en PAGE3 =>  x  =   0  ->   y = <borrar(0) + borrar(130) + 1 = 131 
     
        putRelojGirado5(); // Mostrar relGir5 en PAGE1
     
      // cociGra
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(173,131,177,160,fileCocinadoGrande); // Cargar cociGra (177x160) en PAGE3 =>  x  =  <guardar(0) + guardar(172) + 1 = 173    ->   y = 131  
     
        putRelojGirado6(); // Mostrar relGir6 en PAGE1
     
      // crudoGra
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(351,131,177,160,fileCrudoGrande); // Cargar crudoGra (177x160) en PAGE3 =>  x  =  <cociGra(131) + cociGra(177) + 1 = 351  ->   y = 131  
     
        putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1

    // --------- FIN AÑADIR, BORRAR, GUARDAR Y CRUDO/COCINADO --------------------------------------------



    // -------------- COLOCAR ALIMENTO -------------------------------------------------------------------
      // scale
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(372,292,150,150,fileScale); // Cargar scaleG (150x150) en PAGE3 => x =  <manoY(251) + manoY(120) + 1 = 372   ->  y = 292

        putReloj2(); // Mostrar reloj2 en PAGE1 
    // ----------- FIN COLOCAR ALIMENTO ------------------------------------------------------------------


    // ----------------- ESCOGER GRUPO -------------------------------------------------------------------
      // -------- GRUPOS  -------
      // grupo1
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Canvas inicia en PAGE3
      tft.sdCardDraw16bppBIN256bits(0,0,130,125,fileGrupo1);     // Cargar grupo1 (130x125) en PAGE3  =>  x = 0   ->   y = 0 
      
        putReloj3(); // Mostrar reloj3 en PAGE1 => borrar PAGE1
      
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

      // mano con fondo verde ==> usada en grupos
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(524,0,120,128,fileManoGreenIcon);    // Cargar manoGppt (120x128) en PAGE3  =>  x  =  <manoG  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  
      //tft.sdCardDraw16bppBIN256bits(524,0,120,129,fileManoGreen);    // Cargar manoG (120x129) en PAGE3  =>  x  =  <manoG  =  <grupo4(393) + grupo4(130) + 1 = 524   ->   y = 0  

        putRelojGirado3(); // Mostrar relGir3 en PAGE1
    // --------------- FIN ESCOGER GRUPO -----------------------------------------------------------------

    
    // --------- ERROR / AVISO ---------------------------------------------------------------------------
      // error
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(0,292,114,127,fileCruz); // Cargar cruz (114x127) en PAGE3 =>  x  =  0  ->   y = <cociGra(131) + cociGra(160) + 1 = 292

        putRelojGirado4(); // Mostrar relGir4 en PAGE1

      // aviso
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      // Imagen aviso con fondo naranja oscuro
      //tft.sdCardDraw16bppBIN256bits(115,292,135,113,fileAvisoDarkOrange); // Cargar avisoO (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292
      // Imagen aviso con fondo amarillo
      tft.sdCardDraw16bppBIN256bits(115,292,135,113,fileAvisoYellow); // Cargar avisoY (135x113) en PAGE3 =>  x  =  <cruz(0) + cruz(114) + 1 = 115  ->   y = 292
      
        putRelojGirado5(); // Mostrar relGir5 en PAGE1

      // Mano con fondo blanco: usada en botones add/delete/save
      //      La imagen de la mano no es blanco puro, por eso se puede filtrar el fondo blanco y que se siga viendo
      //      la mano. Así no aparecen los píxeles de color, como sí ocurre con los píxeles amarillos de manoY.
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(251,292,120,128,fileManoWhiteIcon);    // Cargar manoWppt (120x128) en PAGE3  =>  x  =  <avisoY(115) + avisoY(135) + 1 = 251   ->   y = 292
      //tft.sdCardDraw16bppBIN256bits(251,292,120,129,fileManoYellow);    // Cargar manoY (120x129) en PAGE3  =>  x  =  <avisoY(115) + avisoY(135) + 1 = 251   ->   y = 292

        putRelojGirado6(); // Mostrar relGir6 en PAGE1

    // --------- FIN ERROR / AVISO -----------------------------------------------------------------------


    // ----------------- PANTALLA INICIAL ----------------------------------------------------------------
      // brain1
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(0,170,120,108,fileBrain1);    // Cargar brain1 (120x108) en PAGE2 => x = 0 ->   y = <Log(0) + Log(169) + 1 = 170  
      
        putReloj1(); // Mostrar reloj1 en PAGE1 => borrar PAGE1
      
      // brain2G
      tft.canvasImageStartAddress(PAGE2_START_ADDR); // Regresar a PAGE2
      tft.sdCardDraw16bppBIN256bits(121,170,120,108,fileBrain2Green);  // Cargar brain2G (120x108) en PAGE2 => x = <brain1(0)  + brain1(120) + 1 = 121  ->  y = 170  

        putReloj2(); // Mostrar reloj2 en PAGE1 
    // ------------- FIN PANTALLA INICIAL ----------------------------------------------------------------


    // ----------- GUARDAR COMIDA ------------------------------------------------------------------------
    // guardar (disquete)
    tft.canvasImageStartAddress(PAGE4_START_ADDR); // Regresar a PAGE4
    tft.sdCardDraw16bppBIN256bits(554,0,91,98,fileGuardando); // Cargar disquete (91x98) en PAGE4 =>  x  = <aviso_new(410)  + aviso_new(143) + 1 = 554  ->   y = 0

    putReloj3(); // Mostrar reloj3 en PAGE1 

    // Hay conexión WiFi
    tft.canvasImageStartAddress(PAGE4_START_ADDR); // Regresar a PAGE4
    tft.sdCardDraw16bppBIN256bits(646,0,43,67,fileGuardandoWifi); // Cargar conexión (43x67) en PAGE4 =>  x  = <disquete(554) + disquete(91) + 1 = 646  ->   y = 0

    putReloj4(); // Mostrar reloj4 en PAGE1 

    // no conexión
    tft.canvasImageStartAddress(PAGE4_START_ADDR); // Regresar a PAGE4
    tft.sdCardDraw16bppBIN256bits(689,0,54,85,fileGuardandoNoWifi); // Cargar no_conex (54x85) en PAGE4 =>  x  = <conexion(646) + conexion(42) + 1 = 689  ->   y = 0

    putRelojGirado1(); // Mostrar relGir1 en PAGE1

    // ----------- FIN GUARDAR COMIDA --------------------------------------------------------------------


    // --------- DASHBOARD -------------------------------------------------------------------------------
      // cociPeq
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(529,131,47,42,fileCocinadoPeq); // Cargar cociPeq (47x42) en PAGE3 =>  x  =  <crudoGra(351) + crudoGra(177) + 1 = 529  ->   y = 131  

        putRelojGirado2(); // Mostrar relGir2 en PAGE1

      // crudoPeq
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(577,131,47,42,fileCrudoPeq); // Cargar crudoPeq (47x42) en PAGE3 =>  x  =  <cociPeq(529) + crudoGra(47) + 1 = 577  ->   y = 131  

        putRelojGirado3(); // Mostrar relGir3 en PAGE1

      // kcal
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.sdCardDraw16bppBIN256bits(529,174,60,65,fileKcal); // Cargar kcal (60x65) en PAGE3 =>  x = <crudoGra(351) + crudoGra(177) + 1 = 529   ->   y = <cociPeq(131) + cociPeq(42) + 1 = 174

        putRelojGirado4(); // Mostrar relGir4 en PAGE1

      // Recuadro azul utilizado para la transparencia de crudo/cocinado en dashboard. Tiene el mismo tamaño que esas imágenes.
      tft.canvasImageStartAddress(PAGE3_START_ADDR); // Regresar a PAGE3
      tft.fillRect(610,174,657,216,GRIS_CUADROS); // Cargar cuadro (47x42) en PAGE3 => x = <kcal(529) + kcal(80) + 1 = 610    ->  y = <kcal = 174
    // --------- FIN DASHBOARD ---------------------------------------------------------------------------




    //---------------------------------------------------------------------------------------------------
    // ----- REGRESAR A LA PÁGINA 1 ---------------------------------------------------------------------
    // Regresamos la dirección de inicio del canvas a la PAGE1 para que lo que se escriba a partir de ahora se muestre en pantalla
    tft.canvasImageStartAddress(PAGE1_START_ADDR); 
    // ---------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------

}





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




#endif