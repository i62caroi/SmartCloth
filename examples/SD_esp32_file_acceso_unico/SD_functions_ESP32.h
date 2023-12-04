/**
 * @file SD_functions_ESP32.h
 * @brief Este archivo contiene las definiciones de las funciones utilizadas para interactuar con la tarjeta SD.
 * 
 * @author Irene Casares Rodríguez
 * @date 01/12/2023
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
      |  CS   |      pin 2  (blanco)     |   pin 4 (digital PWM) | 
      ------------------------------------------------------------

              -----------------------
              |   SD (JP2 en TFT)   |
      -------------------------------
      |  GND  |      pin 5 (negro)  |
      -------------------------------

-----------------------------------------------------
*/

#ifndef SD_FUNCTIONS_ESP32_H
#define SD_FUNCTIONS_ESP32_H

#include <SD.h>
#include "RTC.h"
#include "functions.h"
#include "list_functions.h"


#define SD_CARD_SCS  4 ///< Define el pin CS para la tarjeta SD

// Fichero donde ir escribiendo la info para cuando haya wifi
char fileESP32[30] = "data/data-ESP.txt";





/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
bool    setupSDcard();                   // Inicializar tarjeta SD

void    saveComida();                           // Escribir FIN-COMIDA,<fecha>,<hora>

void    readFileESP32();
void    borrarFileESP32();
/*-----------------------------------------------------------------------------*/





/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicializar la tarjeta SD.
 * @return true: inicialización correcta  false: fallo al inicializar la SD
 */
/*-----------------------------------------------------------------------------*/
bool setupSDcard(){
    if(!SD.begin(SD_CARD_SCS)){
        Serial.println(F("SD card failure!"));
        return false;
    }
    else{ 
        Serial.println(F("SD card initialized"));
        return true;
    }
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Escribe "FIN-COMIDA" junto con la fecha y hora actuales.
 */
/*-----------------------------------------------------------------------------*/
void saveComida()
{
    Serial.println(F("Finalizando y guardando comida...\n"));

    // ----- 1. COMPROBAR ULTIMO INICIO-PLATO ------------------------
    // Comprobar si la última línea es INICIO-PLATO y borrarla si es así
    if (!lines.empty() && lines.back() == "INICIO-PLATO") {
        lines.pop_back();
    }
    // ---------------------------------------------------------------


    // ----- 2. FIN-COMIDA -------------------------------------------
    char *today = rtc.getDateStr();
    char *time = rtc.getTimeStr();

    String cadFin = "FIN-COMIDA," + String(today) + "," + String(time); // FIN-COMIDA,<fecha>,<hora>

    // Añadir cadena a la lista
    addLine(cadFin);
    // ---------------------------------------------------------------


    // ----- 3. GUARDAR EN FICHERO -----------------------------------
    File myFile = SD.open(fileESP32, FILE_WRITE);

    if (myFile) {
        // Escribe cada línea en el archivo
        for (int i = 0; i < lines.size(); i++) {
            myFile.println(lines[i]);
        }
        // Cierra el archivo
        myFile.close();
        // Limpia el vector para la próxima comida
        lines.clear(); 
    } else {
        // Si el archivo no se abre, imprime un error:
        Serial.println("error opening comida.txt");
    }
    // ---------------------------------------------------------------
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Lee el contenido del archivo en la tarjeta SD.
 */
/*-----------------------------------------------------------------------------*/
void readFileESP32(){
    Serial.println(F("\n\nLeyendo fichero...\n"));
    File myFile = SD.open(fileESP32, FILE_READ);
    if (myFile){
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        myFile.close();
    }
    else{
        Serial.println("Error opening file for reading!");
    }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el archivo en la tarjeta SD.
 */
/*-----------------------------------------------------------------------------*/
void borrarFileESP32(){

    Serial.println("Borrando fichero ESP32...");
    SD.remove(fileESP32);

    if (!SD.exists(fileESP32)) {
        Serial.println("Fichero ESP32 borrado");
    }
    else  {
        Serial.println("Error borrando fichero.");
    }
}








#endif
