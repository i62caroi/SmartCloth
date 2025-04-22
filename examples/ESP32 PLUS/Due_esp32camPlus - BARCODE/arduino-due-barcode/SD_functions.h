
/** 
 * @file SD_functions.h
 * @brief Módulo Tarjeta SD
 *
 * @author Irene Casares Rodríguez
 * @date 17/07/23
 * @version 1.0
 *
 * Se ha utilizado el módulo de SD integrado en la pantalla TFT:
 *   Pantalla modelo: ER-TFTM070-6 de BuyDisplay
 * 
 * Este módulo no está conectado directamente con el micro de la pantalla,
 * por lo que se debe conectar de forma independiente al bus SPI del Arduino.
 * Esto produce que 2 dispositivos, la pantalla y la SD, estén compartiendo el
 * mismo bus SPI.
 *
 * Resulta que el micro RA8876 tiene el mismo problema en relación al pin MISO del SPI
 * que el micro RA8875 (véase https://github.com/wwatson4506/Ra8876LiteTeensy), y es que
 * no se "suelta" el dispositivo (su chipselect) tras finalizar la transacción SPI,
 * lo cual debería hacerse automáticamente. Esto crea un problema a la hora de usar la
 * misma interfaz SPI para diferentes dispositivos.
 *
 * Lo mejor sería utilizar diferentes pines para el CS de cada dispositivo, lo que se
 * ha hecho en este caso (SD en pin 4 y TFT en pin 12). Otra opción sería utilizar un
 * chip externo de buffer triestado como el 74HCT125.
 *
 * @see https://cs.stanford.edu/people/miles/iso8859.html Colección de caracteres ASCII/ISO 8859-1 (Latin-1) en HEX
 * @see https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf Table 14-1 (pag 99) del datasheet de RA8876 para caracteres Latin-1
 * @see https://www.buydisplay.com/spi-7-inch-tft-lcd-dislay-module-1024x600-ra8876-optl-touch-screen-panel  SPI 7"TFT LCD Dislay 1024x600 OPTL Capacitive Touch Screen
 *
 */


/*
  --------- CONEXIÓN SD CON ARDUINO DUE -----------


  Tarjeta SD conectada al bus SPI de la siguiente forma:
  
    (pines de la SD en el JP2 de la TFT, conectados al módulo SD 
     integrado directamente en la pantalla)
     
      ------------------------------------------------------------
      |  SPI  |        SD (JP2 en TFT)   |      Arduino Due      |
      ------------------------------------------------------------
      |  MOSI |      pin 3  (amarillo)   |      pin 4 (SPI)      |    
      |  MISO |      pin 6  (azul)       |      pin 1 (SPI)      |
      |  CLK  |      pin 4  (verde)      |      pin 3 (SPI)      |
      |  CS   |      pin 2  (blanco)     |   pin 13 (digital PWM) | 
      ------------------------------------------------------------

              -----------------------
              |   SD (JP2 en TFT)   |
      -------------------------------
      |  GND  |      pin 5 (negro)  |
      -------------------------------

-----------------------------------------------------
*/

#ifndef SD_FUNCTIONS_H
#define SD_FUNCTIONS_H

#include <SD.h>
#include "Serial_functions.h" // SerialPC

// ------- ChipSelect de SD --------------
#define SD_CARD_SCS  13 
// ---------------------------------------


// LIBRERÍA SD LIMITA LA LONGITUD DEL NOMBRE DEL FICHERO A 8 CARACTERES Y 3 PARA EXTENSION
// Los 8 caracteres no incluyen el path hasta el fichero

// --- FICHERO BARCODE ---
char    productsFile[30] = "data/barcodes.csv";       // Archivo CSV para guardar la información de los barcodes




/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
// -- Inicializar SD --
bool    setupSDcard();              // Inicializar tarjeta SD
// --------------------

// -- Fichero barcodes --
bool productsFileExists();                                      // Comprobar si existe el fichero barcodes.csv
bool writeHeaderProductsFile();                                 // Escribir encabezado del archivo barcodes.csv
bool searchBarcodeInSD(String &barcode, String &productInfo);   // Buscar barcode en el fichero barcodes.csv
void writeProductInfoInSD(String &productInfo);                 // Escribir información de un producto en el fichero barcodes.csv
// ----------------------


// -------------------------------------------------------------------------
/******************************************************************************/
/******************************************************************************/






/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/


/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicializa la tarjeta SD y realiza la configuración necesaria para su uso.
 * 
 * @return true si la tarjeta SD se inicializa correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool setupSDcard()
{
    SerialPC.println(F("Inicializando tarjeta SD..."));

    if(!SD.begin(SD_CARD_SCS))
    {
        SerialPC.println(F("SD card failure!\n"));
        return false;
    }
    else 
    {
        SerialPC.println(F("SD card initialized\n"));
    }    
    
    return true;
}






/*-----------------------------------------------------------------------------*/
/**
 * @brief Verifica si el fichero barcodes.csv existe en la tarjeta SD.
 * 
 * @return true si el fichero existe, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool productsFileExists()
{
    File file = SD.open(productsFile, FILE_READ);
    if (file)
    {
        file.close();
        return true;
    }
    return false;
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Escribe el encabezado del archivo de barcodes en la tarjeta SD.
 * El encabezado contiene los nombres de las columnas separados por ';'.
 */
/*-----------------------------------------------------------------------------*/
bool writeHeaderProductsFile() 
{
    SerialPC.print(F("\n Creando fichero ")); SerialPC.print(productsFile); SerialPC.println(F(" ...\n"));

    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente:

    // La información de los productos viene como "<barcode>;<nombreProducto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
    String header = "barcode;nombre_producto;carb_1g;lip_1g;prot_1g;kcal_1g";

    File myFile = SD.open(productsFile, FILE_WRITE); 
    if (myFile)
    {
        myFile.println(header);
        myFile.close(); 
        return true;
    }
    else
    {
        SerialPC.println(F("Error abriendo archivo CSV de barcodes!"));
        return false;
    }
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Busca el barcode en el fichero barcodes.csv de la tarjeta SD.
 * 
 * @param barcode El código de barras a buscar.
 * @param productInfo La información del producto encontrada.
 * @return true si el código de barras se encuentra en el fichero, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool searchBarcodeInSD(String &barcode, String &productInfo)
{
    File file = SD.open(productsFile, FILE_READ);
    if(!file)
    {
        SerialPC.println(F("Error al abrir el fichero barcodes.csv, puede que no exista"));
        productInfo = "";
        return false;
    }

    String line;
    while(file.available())
    {
        line = file.readStringUntil('\n'); // Leer línea completa, eliminando el salto de línea
        if(line.startsWith(barcode + ";")) // Si la línea empieza con el barcode. 
        {                                  // Se incluye el ';' para que no se confunda con una cadena parcial de otro barcode, en caso de que coincidan
            productInfo = line; // "<barcode>;<nombreProducto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
            file.close();
            return true; // Producto encontrado en SD (ya se ha leído antes)
        }
    }

    productInfo = "";
    file.close();
    return false;
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Escribe la información de un producto en el fichero barcodes.csv de la tarjeta SD.
 * 
 * @param productInfo La información del producto a escribir
 */
/*-----------------------------------------------------------------------------*/
void writeProductInfoInSD(String &productInfo)
{
    // -- 1. COMPROBAR SI EXISTE EL FICHERO --
    // Si no existe, se crea el fichero y se escribe el encabezado
    if (!productsFileExists())
    {
        if (!writeHeaderProductsFile())
        {
            SerialPC.println(F("Error creando el archivo CSV de barcodes con el encabezado!"));
            return;
        }
    }
    // -----------------------------------------

    // -- 2. ESCRIBIR LA INFO DEL PRODUCTO EN EL FICHERO --

    // La información del producto viene como "PRODUCT:<barcode>;<nombreProducto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
    
    // Eliminar la parte de "PRODUCT:"
    String productData = productInfo.substring(8);

    // Escribir información en el fichero
    File myFile = SD.open(productsFile, FILE_WRITE); 
    if (myFile)
    {
        myFile.println(productData); // "<barcode>;<nombreProducto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>\n"
        myFile.close(); 
        SerialPC.println(F("Nuevo producto guardado!"));
    }
    else
    {
        SerialPC.println(F("Error abriendo archivo CSV de barcodes!"));
    }
    // -----------------------------------------------------
}


/******************************************************************************/
/******************************************************************************/



#endif
