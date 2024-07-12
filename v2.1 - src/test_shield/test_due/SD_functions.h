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
#define SerialPC Serial


// ------- ChipSelect de SD --------------
#define SD_CARD_SCS  13 
// ---------------------------------------


// ---- FICHEROS ------
char   testTXT[30] = "test.txt";
// --------------------

/******************************************************************************/
inline bool    setupSDcard() { return SD.begin(SD_CARD_SCS); };      
             
void           checkSD();
bool           writeTXT();    
bool           readTXT();
void           deleteTXT();
/******************************************************************************/



void checkSD()
{
    if(writeTXT())
    {
        SerialPC.println("  Fichero TXT creado correctamente");
        SerialPC.println("  Leyendo fichero TXT...\n");
        if(!readTXT()) SerialPC.println("       Error al leer fichero TXT");
        else{ 
            SerialPC.println("    Borrando fichero TXT..."); 
            deleteTXT(); 
        }
    }
    else SerialPC.println("   Error al escribir fichero TXT");
}



bool writeTXT()
{
    File myFile = SD.open(testTXT, FILE_WRITE);

    if (myFile) 
    {
        myFile.println("Esto es una prueba");
        myFile.close();

        return true;
    } 
    else return false;
}


bool readTXT()
{
    File myFile = SD.open(testTXT, FILE_READ);
    if (myFile)
    {
        while (myFile.available()) 
        {
            SerialPC.write(myFile.read());
        }
        myFile.close();

        return true;
    }
    else return false;
}

void deleteTXT()
{
    if (SD.exists(testTXT)) SD.remove(testTXT);
}


/******************************************************************************/
/******************************************************************************/



#endif
