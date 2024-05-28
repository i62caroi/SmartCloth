/*
Writing Sensor Data to an SD card
//
This example shows how to write data
to an SD card using the SD library.
//
The circuit:
* SD card attached to SPI bus as follows:
** MOSI - pin 11
** MISO - pin 12
** CLK - pin 13
** CS - pin 10 Uno (53 on Mega)
Based on code by Tom Igoe
*/
//
#include "SD.h"
#include"SPI.h"

#include <DS3231.h> // RTC

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);
// Arduino Due:
// ----------------------
// DS3231:  SDA pin   -> Arduino Digital 20 (SDA) or the dedicated SDA1 (Digital 70) pin
//          SCL pin   -> Arduino Digital 21 (SCL) or the dedicated SCL1 (Digital 71) pin
//
//

const int CSpin = 13;

char charRead;


String dataString ="";          // holds the data to be written to the SD card

// --------------
float carb = 15.03;    // value read from your first sensor
int carb_R = 2;

float lip = 2.00;    // value read from your second sensor
int lip_R = 0;

float prot = 54.40;    // value read from your third sensor
int prot_R = 1;

float kcal = 546.09;

float peso = 150.78;
// ----------------

int veces = 3;
bool borrado = false;

char fileCSV[30] = "data/data-SC2.csv";
File myFile;

char *today;

void setup()
{
    // Open serial communications
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
      }
     
    // see if the card is present and can be initialized:
    if (!SD.begin(CSpin)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("\n\ncard initialized.");

    // Initialize the rtc object
    rtc.begin();
    Serial.println("RTC initialized.");

    Serial.println("\nPulse una tecla para comenzar");
    while(Serial.available() == 0){;}
    charRead = Serial.read();  
    Serial.write(charRead); //write it back to Serial window
    Serial.println();

    /*if(SD.exists(fileCSV)){
        deleteFile();
    }
    else{*/
    if(!SD.exists(fileCSV)){ //Si no existe ya, se crea e incorpora el encabezado. Todo se va a ir guardando en el mismo fichero.
        writeHeader();
    }

    today = rtc.getDateStr();
    Serial.print("Hoy es "); Serial.print(today); Serial.print(" y son las "); Serial.println(rtc.getTimeStr());
    
    readData();
    
    getAcumuladoHoy();
  
}




void loop(){
  
  /*if(veces != 0){
    // build the data string
    dataString = String(rtc.getDateStr()) + ";" + String(rtc.getTimeStr()) + ";" +
                 String(carb) + ";" + String(carb_R) + ";" + 
                 String(lip) + ";" + String(lip_R) + ";" + 
                 String(prot) + ";" + String(prot_R) + ";" + 
                 String(kcal) + ";" + String(peso); // convertir a CSV con ';' como delimitador para que Excel lo abra directamente
                 
    saveData(); // save to SD card
    readData();
    delay(5000);

    veces--;
  }*/
  //else{
      //if(!borrado) deleteData();
  //}
    
}


/* *************************************************************
   ************************************************************* */
void getAcumuladoHoy(){

    //char *today = rtc.getDateStr();

    // SUMAS
    float carb = 0.0, lip = 0.0, prot = 0.0, kcal = 0.0, peso = 0.0;    
    int carb_R = 0, lip_R = 0, prot_R = 0;
    int nComidas = 0;

    char lineBuffer[128];  

    char *token;
    int fieldIndex;


    //Serial.print("Hoy es "); Serial.println(today);

    myFile = SD.open(fileCSV, FILE_READ);
    if (myFile){
        Serial.print(fileCSV); Serial.println(": ");

        while (myFile.available()) {
            
            myFile.readBytesUntil('\n', lineBuffer, sizeof(lineBuffer) - 1); // Leer línea completa hasta el tamaño máximo del búfer
            lineBuffer[sizeof(lineBuffer) - 1] = '\0'; // Asegurar terminación nula

            //Serial.println(lineBuffer);
            
            token = strtok(lineBuffer, ";"); // Separar campos de la línea utilizando el delimitador ';'
            fieldIndex = 0;

            if(strcmp(today, token) == 0){ // today = primer token ==> comida guardada hoy

                nComidas++; // Incrementar numero comidas guardadas hoy

                float valueFloat;
                int valueInt;

                while (token != NULL) {

                    switch (fieldIndex){ // fieldIndex = 0 => fecha     fieldIndex = 1 => hora
                        case 2:   valueFloat = atof(token);     carb   += valueFloat;     break;    // Carbohidratos
                        case 3:   valueInt   = atoi(token);     carb_R += valueInt;       break;    // Raciones de carbohidratos
                        case 4:   valueFloat = atof(token);     lip    += valueFloat;     break;    // Lípidos (Grasas)
                        case 5:   valueInt   = atoi(token);     lip_R  += valueInt;       break;    // Raciones de lípidos
                        case 6:   valueFloat = atof(token);     prot   += valueFloat;     break;    // Proteínas
                        case 7:   valueInt   = atoi(token);     prot_R += valueInt;       break;    // Raciones de proteínas
                        case 8:   valueFloat = atof(token);     kcal   += valueFloat;     break;    // Kilocalorías
                        case 9:   valueFloat = atof(token);     peso   += valueFloat;     break;    // Peso
                    }

                    // Obtener el siguiente campo
                    token = strtok(NULL, ";");
                    fieldIndex++;
                }
            }
         
        }

        myFile.close();

        // Imprimir las sumas de cada columna
        Serial.print("\nSuma carb: ");    Serial.println(carb);
        Serial.print("Suma carb_R: ");  Serial.println(carb_R);
        Serial.print("Suma lip: ");     Serial.println(lip); 
        Serial.print("Suma lip_R: ");   Serial.println(lip_R); 
        Serial.print("Suma prot: ");    Serial.println(prot);
        Serial.print("Suma prot_R: ");  Serial.println(prot_R);
        Serial.print("Suma kcal: ");    Serial.println(kcal);
        Serial.print("Suma peso: ");    Serial.println(peso);
        Serial.print("N Comidas: ");    Serial.println(nComidas);
        
    }
    else{
        Serial.println("Error opening file for reading!");
    }

}





/* *************************************************************
   ************************************************************* */
void writeHeader(){
    Serial.print(F("\n Creando fichero ")); Serial.print(fileCSV); Serial.println(F(" ...\n"));
    
    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente. Si se separa por comas, no divide las columnas porque
    // en la región de España las comas se usan para los decimales, aunque se haya
    // indicado en las preferencias que se use el punto para separar la parte decimal.

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



/* *************************************************************
   ************************************************************* */
void saveData(){
    Serial.println(F("Guardando info...\n"));
    myFile = SD.open(fileCSV, FILE_WRITE);
    if (myFile){
        myFile.println(dataString);
        myFile.close(); // close the file
    }
    else{
        Serial.println("Error opening file for writing!");
    }
}


/* *************************************************************
   ************************************************************* */
void readData(){
    Serial.println(F("Leyendo fichero...\n"));
    myFile = SD.open(fileCSV, FILE_READ);
    if (myFile){
        while (myFile.available()) {
            Serial.write(myFile.read());
        }
        myFile.close();
    }
    else{
        Serial.println("Error opening file for reading!");
    }
    Serial.println();
}


/* *************************************************************
   ************************************************************* */

void deleteFile(){
    Serial.println(F("Borrando fichero...\n"));
    SD.remove(fileCSV);
    if(!SD.exists(fileCSV)){
        Serial.println(F("Fichero borrado con éxito\n"));
        borrado = true;
    }
    else{
      Serial.println(F("Error al borrar el fichero\n"));
    }    
}
