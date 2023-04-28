
/* --------- CONEXIÓN SD CON ARDUINO DUE -----------
   
  Tarjeta SD conectada al bus SPI de la siguiente forma:
  
    (pines de la SD en el JP2 de la TFT, conectados al módulo SD 
     integrado directamente en la pantalla)
     
      ------------------------------------------
      |  SPI  |   SD   |      Arduino Due      |
      ------------------------------------------
      |  MOSI |  pin 3 |      pin 4 (SPI)      |    
      |  MISO |  pin 6 |      pin 1 (SPI)      |
      |  CLK  |  pin 4 |      pin 3 (SPI)      |
      |  CS   |  pin 2 |   pin 4 (digital PWM) | 
      ------------------------------------------

              ----------
              |   SD   |
      ------------------
      |  GND  |  pin 5 |
      ------------------

-----------------------------------------------------
*/

#ifndef SD_FUNCTIONS_H
#define SD_FUNCTIONS_H

#include <SD.h>

#define SD_CARD_SCS  4


/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
void setupSDcard();                   // Inicializar tarjeta SD
void writeHeaderFileSD();
void saveDataSD();
//void readDataSD();
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
   setupSDcard(): Inicializar tarjeta SD
-------------------------------------------------------------------------------*/
void setupSDcard(){
    if(!SD.begin(SD_CARD_SCS)){
        Serial.println(F("SD card failure!"));
        while(1);
    }
    Serial.println(F("SD card initialized"));

    if(!SD.exists("data.csv")){ //Si no existe ya, se incorpora el encabezado. Todo se va a ir guardando en el mismo fichero.
        writeHeaderFileSD();
    }
}


/*-----------------------------------------------------------------------------
   writeHeaderFileSD(): Escribir header en fichero en SD
-------------------------------------------------------------------------------*/
void writeHeaderFileSD(){
    Serial.println(F("\n Creando fichero data.csv ...\n"));
    String header = "carb,carb_r,lip,lip_R,prot,prot_R,kcal";
    File myFile = SD.open("data.csv", FILE_WRITE);    // Todo se va a ir guardando en el mismo fichero
    if (myFile){
        myFile.println(header);
        myFile.close(); // close the file
    }
    else{
        Serial.println("Error opening file for writing!");
    }
}

/*-----------------------------------------------------------------------------
   saveDataSD(): Guardar información de la comida actual en SD
          Parámetros:
              val - ValoresNutricionales de la comida a guardar
-------------------------------------------------------------------------------*/
void saveDataSD(ValoresNutricionales &val){
    Serial.println(F("Guardando info...\n"));
    // Lo suyo sería usar un RTC (Real Time Clock) para incluir
    // la fecha en que se guarda la comida y llevar un control de varios días.
    // Si se hiciera eso, al volver a encender el Arduino se podría acceder al fichero
    // y coger la información que ya se tiene del día e incluirlo desde el principio
    // en 'diaActual'. De esta forma, cada vez que se encendiera el dispositivo se tendría
    // la información ya incluida anteriormente en el día, aunque el arduino "borre" la info
    // al apagarse.
    
    int carb_R = round(val.getCarbValores()/10);
    int lip_R = round(val.getLipValores()/10);
    int prot_R = round(val.getProtValores()/10);

    String dataString = String(val.getCarbValores()) + "," + String(carb_R) + "," + 
                        String(val.getLipValores()) + "," + String(lip_R) + "," + 
                        String(val.getProtValores()) + "," + String(prot_R) + "," + 
                        String(val.getKcalValores()); 

    File myFile = SD.open("data.csv", FILE_WRITE);
    if (myFile){
        myFile.println(dataString);
        myFile.close(); // close the file
    }
    else{
        Serial.println("Error opening file for writing!");
    }
}


/*-----------------------------------------------------------------------------
   readDataSD(): Leer info del fichero de la SD
-------------------------------------------------------------------------------*/
/*void readDataSD(){
    Serial.println(F("Leyendo fichero...\n"));
    myFile = SD.open("data.csv", FILE_READ);
    if (myFile){
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        myFile.close();
    }
    else{
        Serial.println("Error opening file for reading!");
    }
}*/


#endif
