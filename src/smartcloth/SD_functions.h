
/* ------------- MÓDULO TARJETA SD ---------------------

  Se ha utilizado el módulo de SD integrado en la pantalla TFT:
    Pantalla modelo: ER-TFTM070-6 de BuyDisplay
      - SPI 7"TFT LCD Dislay 1024x600 OPTL Capacitive Touch Screen
      https://www.buydisplay.com/spi-7-inch-tft-lcd-dislay-module-1024x600-ra8876-optl-touch-screen-panel
   
  Este módulo no está conectado directamente con el micro de la pantalla,
  por lo que se debe conectar de forma independiente al bus SPI del Arduino.
  Esto produce que 2 dispositivos, la pantalla y la SD, estén compartiendo el
  mismo bus SPI. 
  
  Resulta que el micro RA8876 tiene el mismo problema en relación al pin MISO del SPI
  que el micro RA8875 (véase https://github.com/wwatson4506/Ra8876LiteTeensy), y es que 
  no se "suelta" el dispositivo (su chipselect) tras finalizar la transacción SPI, 
  lo cual debería hacerse automáticamente. Esto crea un problema a la hora de usar la 
  misma interfaz SPI para diferentes dispositivos.
  
  Lo mejor sería utilizar diferentes pines para el CS de cada dispositivo, lo que se
  ha hecho en este caso (SD en pin 4 y TFT en pin 12). Otra opción sería utilizar un
  chip externo de buffer triestado como el 74HCT125.

  Véase el tema "Fix compatibility with other SPI devices" de esta wiki:
    https://github.com/sumotoy/RA8875/wiki/Fix-compatibility-with-other-SPI-devices
  
  También véase el apartado "SDO Defect" de la siguiente fuente:
    https://os.mbed.com/components/RA8875-Based-Display/

  Aun así, el micro SAM3X del Arduino Due tiene capacidades SPI avanzadas, por lo que 
  puede manejar automáticamente la selección de chipselect entre diferentes dispositivos que 
  compartan el bus SPI, incluso teniendo diferentes atributos como velocidad y datamode. 
  En este caso no ha hecho falta utilizar esta API extendida del SPI, sino que ha bastado con
  utilizar diferentes pines para el CS de cada dispositivo. 

  Véase el tema "Extended SPI Library Usage with the Arduino Due" de la documentación oficial
  de Arduino:
    (https://docs.arduino.cc/tutorials/due/due-extended-spi)


  --------- CONEXIÓN SD CON ARDUINO DUE -----------


  Tarjeta SD conectada al bus SPI de la siguiente forma:
  
    (pines de la SD en el JP2 de la TFT, conectados al módulo SD 
     integrado directamente en la pantalla)
     
      -------------------------------------------------------
      |  SPI  |   SD (JP2 en TFT)   |      Arduino Due      |
      -------------------------------------------------------
      |  MOSI |      pin 3          |      pin 4 (SPI)      |    
      |  MISO |      pin 6          |      pin 1 (SPI)      |
      |  CLK  |      pin 4          |      pin 3 (SPI)      |
      |  CS   |      pin 2          |   pin 4 (digital PWM) | 
      -------------------------------------------------------

              -----------------------
              |   SD (JP2 en TFT)   |
      -------------------------------
      |  GND  |      pin 5          |
      -------------------------------

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
void saveDataSD(ValoresNutricionales val);
//void readDataSD();
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
   setupSDcard(): Inicializar tarjeta SD
-------------------------------------------------------------------------------*/
void setupSDcard(){
    // initialize the bus for a device on pin 4 ==> Extended SPI Library Usage with the Arduino Due
                                                //   (https://docs.arduino.cc/tutorials/due/due-extended-spi)
    //SPI.begin(SD_CARD_SCS);

    if(!SD.begin(SD_CARD_SCS)){
        Serial.println(F("SD card failure!"));
        while(1);
    }
    Serial.println(F("SD card initialized"));

    if(!SD.exists(fileCSV)){ //Si no existe ya, se incorpora el encabezado. Todo se va a ir guardando en el mismo fichero.
        writeHeaderFileSD();
    }
}


/*-----------------------------------------------------------------------------
   writeHeaderFileSD(): Escribir header en fichero en SD
-------------------------------------------------------------------------------*/
void writeHeaderFileSD(){
    Serial.println(F("\n Creando fichero data.csv ...\n"));

    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente. Si se separa por comas, no divide las columnas porque
    // en la región de España las comas se usan para los decimales, aunque se haya
    // indicado en las preferencias que se use el punto para separar la parte decimal.
    String header = "carb;carb_r;lip;lip_R;prot;prot_R;kcal";

    File myFile = SD.open(fileCSV, FILE_WRITE);    // Todo se va a ir guardando en el mismo fichero ==> 'fileCSV' en Variables.h
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
void saveDataSD(ValoresNutricionales val){
    Serial.println(F("Guardando info...\n"));
    // Lo suyo sería usar un RTC (Real Time Clock) para incluir
    // la fecha en que se guarda la comida y llevar un control de varios días.
    // Si se hiciera eso, al volver a encender el Arduino se podría acceder al fichero
    // y coger la información que ya se tiene del día e incluirlo desde el principio
    // en 'diaActual'. De esta forma, cada vez que se encendiera el dispositivo se tendría
    // la información ya incluida anteriormente en el día, aunque el arduino "borre" la info
    // al apagarse.
    
    // ---------- RACIONES ------------------
    // 0.3 <= carb_R <= 0.7 ==> carb_R = 0.5
    int carb_R = round(2.0*(val.getCarbValores()/10));
    carb_R = carb_R/2;

    // 0.3 <= lip_R <= 0.7 ==> lip_R = 0.5
    int lip_R = round(2.0*(val.getLipValores()/10));
    lip_R = lip_R/2;

    // 0.3 <= prot_R <= 0.7 ==> prot_R = 0.5
    int prot_R = round(2.0*(val.getProtValores()/10));
    prot_R = prot_R/2;
    // -----------------------------------------

    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente. Si se separa por comas, no divide las columnas porque
    // en la región de España las comas se usan para los decimales, aunque se haya
    // indicado en las preferencias que se use el punto para separar la parte decimal.
    String dataString = String(val.getCarbValores()) + ";" + String(carb_R) + ";" + 
                        String(val.getLipValores()) + ";" + String(lip_R) + ";" + 
                        String(val.getProtValores()) + ";" + String(prot_R) + ";" + 
                        String(val.getKcalValores()); 


    File myFile = SD.open(fileCSV, FILE_WRITE); // Todo se va a ir guardando en el mismo fichero ==> 'fileCSV' en Variables.h
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
