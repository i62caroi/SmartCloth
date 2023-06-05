
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

#ifndef SD_FUNCTIONS_H
#define SD_FUNCTIONS_H

#include <SD.h>
#include "RTC.h"
//#include "Variables.h"

#define SD_CARD_SCS  4


File myFile;


/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
void    setupSDcard();                   // Inicializar tarjeta SD
void    writeHeaderFileSD();             // Crear fichero CSV y escribir header 
//void    saveDataSD(ValoresNutricionales val, float peso);
void    saveComidaSD();                  // Guardar valores de la comida en fichero CSV
void    getAcumuladoHoyFromSD();         // Sumar comidas del día desde CSV y mostrar en "Acumulado Hoy"
/*-----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------
   setupSDcard(): Inicializar tarjeta SD
-------------------------------------------------------------------------------*/
void setupSDcard(){
    if(!SD.begin(SD_CARD_SCS)){
        Serial.println(F("SD card failure!"));
        //while(1);
    }
    else Serial.println(F("SD card initialized"));

    if(!SD.exists(fileCSV)){ //Si no existe ya, se incorpora el encabezado. Todo se va a ir guardando en el mismo fichero.
        writeHeaderFileSD();
    }

    getAcumuladoHoyFromSD();   // Leer fichero csv de la SD y sumar los valores nutricionales y el peso de las 
                               // comidas guardadas en el día de hoy
}




/*-----------------------------------------------------------------------------
   writeHeaderFileSD(): Crear fichero en SD y escribir header
-------------------------------------------------------------------------------*/
void writeHeaderFileSD(){
    Serial.print(F("\n Creando fichero ")); Serial.print(fileCSV); Serial.println(F(" ...\n"));

    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente:

    String header = "fecha;hora;carb;carb_R;lip;lip_R;prot;prot_R;kcal;peso";

    myFile = SD.open(fileCSV, FILE_WRITE);    // Todo se va a ir guardando en el mismo fichero ==> 'fileCSV' en Variables.h
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
//void saveDataSD(ValoresNutricionales val, float peso){
  void saveComidaSD(){
    Serial.println(F("Guardando info...\n"));

    // Se ha utilizado un RTC para conocer la fecha a la que se guarda la comida

    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente:
    //    "fecha;hora;carb;carb_R;lip;lip_R;prot;prot_R;kcal;peso"

    String comidaValues = comidaActual.getComidaAllValues(); // carb;carb_R;lip;lip_R;prot;prot_R;kcal
    String peso = String(comidaActual.getPesoComida());

    String dataString = String(rtc.getDateStr()) + ";" + String(rtc.getTimeStr()) + ";" + comidaValues + ";" + peso; 


    myFile = SD.open(fileCSV, FILE_WRITE); // Todo se va a ir guardando en el mismo fichero ==> 'fileCSV' en Variables.h
    if (myFile){
        myFile.println(dataString);
        myFile.close(); // close the file
    }
    else{
        Serial.println("Error opening file for writing!");
    }
}





/*-----------------------------------------------------------------------------
   getAcumuladoHoy(): Leer info del fichero de la SD e inicializar el "Acumulado Hoy"
                      con las comidas guardadas en la fecha de hoy
-------------------------------------------------------------------------------*/
void getAcumuladoHoyFromSD(){

    char *today = rtc.getDateStr(); // Es posible que se cambie de día durante el cocinado?? Si no, hacer en setupRTC(). 

    // SUMAS
    float sumCarb = 0.0, sumLip = 0.0, sumProt = 0.0, sumKcal = 0.0, sumPeso = 0.0;    
    int sumCarb_R = 0, sumLip_R = 0, sumProt_R = 0;
    int nComidas = 0;

    char lineBuffer[128];  

    char *token;
    int fieldIndex;

    bool msg = true;


    myFile = SD.open(fileCSV, FILE_READ);
    if (myFile){
        while (myFile.available()) {
            
            myFile.readBytesUntil('\n', lineBuffer, sizeof(lineBuffer) - 1); // Leer línea completa hasta el tamaño máximo del búfer
            lineBuffer[sizeof(lineBuffer) - 1] = '\0'; // Asegurar terminación nula

            //Serial.println(lineBuffer);
            
            token = strtok(lineBuffer, ";"); // Separar campos de la línea utilizando el delimitador ';'
            fieldIndex = 0;

            if(strcmp(today, token) == 0){ // today == primer token ==> comida guardada hoy
                
                if(msg){
                    Serial.println(F("Obteniendo Acumulado Hoy..."));
                    msg = false; // Solo imprimir una vez y si hay algo que sumar
                }

                nComidas++; // Incrementar numero comidas guardadas hoy

                float valueFloat;
                int valueInt;

                while (token != NULL) {

                    switch (fieldIndex){ // fieldIndex = 0 => fecha     fieldIndex = 1 => hora
                        case 2:   valueFloat = atof(token);     sumCarb   += valueFloat;     break;    // Carbohidratos
                        case 3:   valueInt   = atoi(token);     sumCarb_R += valueInt;       break;    // Raciones de carbohidratos
                        case 4:   valueFloat = atof(token);     sumLip    += valueFloat;     break;    // Lípidos (Grasas)
                        case 5:   valueInt   = atoi(token);     sumLip_R  += valueInt;       break;    // Raciones de lípidos
                        case 6:   valueFloat = atof(token);     sumProt   += valueFloat;     break;    // Proteínas
                        case 7:   valueInt   = atoi(token);     sumProt_R += valueInt;       break;    // Raciones de proteínas
                        case 8:   valueFloat = atof(token);     sumKcal   += valueFloat;     break;    // Kilocalorías
                        case 9:   valueFloat = atof(token);     sumPeso   += valueFloat;     break;    // Peso
                    }

                    // Obtener el siguiente campo
                    token = strtok(NULL, ";");
                    fieldIndex++;
                }
            }
         
        }

        myFile.close();
      

        // Imprimir las sumas de cada columna
        /*Serial.print("\nSuma carb: ");    Serial.println(sumCarb);
        Serial.print("Suma carb_R: ");  Serial.println(sumCarb_R);
        Serial.print("Suma lip: ");     Serial.println(sumLip); 
        Serial.print("Suma lip_R: ");   Serial.println(sumLip_R); 
        Serial.print("Suma prot: ");    Serial.println(sumProt);
        Serial.print("Suma prot_R: ");  Serial.println(sumProt_R);
        Serial.print("Suma kcal: ");    Serial.println(sumKcal);
        Serial.print("Suma peso: ");    Serial.println(sumPeso);
        Serial.print("N Comidas: ");    Serial.println(nComidas);*/

        // ----- ACTUALIZAR ACUMULADO HOY -----
        ValoresNutricionales valAux(sumCarb, sumLip, sumProt, sumKcal);   
        diaActual.setValoresDiario(valAux);                               // Inicializar valores nutricionales del Acumulado Hoy
        diaActual.setPesoDiario(sumPeso);                                 // Actualizar peso del Acumulado Hoy
        diaActual.setNumComidas(nComidas);                                // Actualizar nº de comidas del Acumulado Hoy
        
    }
    else{
        Serial.println("Error opening file for reading!");
    }

}



#endif
