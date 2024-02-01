/**
 * @file SD_functions.h
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

#ifndef SD_FUNCTIONS
#define SD_FUNCTIONS

#include <SD.h>
// incluir esta línea en el includePath de c_cpp_properties.json 
//
// "/Applications/Arduino.app/Contents/Java/libraries/SD/src"


#define SD_CARD_SCS  4 ///< Define el pin CS para la tarjeta SD

// Fichero donde ir escribiendo la info para cuando haya wifi
char fileESP32[30] = "data/data-esp.txt";

#define SerialPC Serial
#define SerialDueESP32 Serial1




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    setupSDcard();           // Inicializar tarjeta SD
void    readFileESP32();         // Leer fichero esp32 
void    sendFileToEsp32ONE();       // Enviar fichero al esp32 
int     sendFileToEsp32();       // Enviar fichero al esp32 contando comidas enviadas
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
 * @brief Envía el fichero por Serial al esp32
 */
/*-----------------------------------------------------------------------------*/
void sendFileToEsp32ONE()
{
    File dataFile = SD.open(fileESP32);

    if (dataFile) 
    {
        // Lee el archivo línea por línea y envía cada línea al ESP32
        while (dataFile.available()) 
        {
            String line = dataFile.readStringUntil('\n');
            line.trim();

            // Envía la línea al ESP32 a través de Serial
            SerialDueESP32.println(line);
        }
        dataFile.close();

        SerialDueESP32.println("FIN-TRANSMISION");

        SerialPC.println("\nFichero completo enviado");
    }
    else 
    {
        SerialPC.println("\nError al abrir el archivo data-ESP.txt");
    }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía el fichero por Serial al esp32
 * @return Devuelve el número de comidas enviadas para saber si el esp32 las
 *          ha podido subir todas
 */
/*-----------------------------------------------------------------------------*/
int sendFileToEsp32()
{
    int nComidas = 0;

    File dataFile = SD.open(fileESP32);

    if (dataFile) 
    {
        
        // Lee el archivo línea por línea y envía cada línea al ESP32
        while (dataFile.available()) 
        {
            String line = dataFile.readStringUntil('\n');
            line.trim();

            if(line == "INICIO-COMIDA") nComidas++;

            // Envía la línea al ESP32 a través de Serial
            SerialDueESP32.println(line);
        }
        dataFile.close();

        SerialDueESP32.println("FIN-TRANSMISION");

        SerialPC.println("\nFichero completo enviado");
    }
    else 
    {
        SerialPC.println("\nError al abrir el archivo data-ESP.txt");
    }

    SerialPC.print("Numero de comidas a enviar: "); SerialPC.println(nComidas);

    return nComidas;
}





#endif
