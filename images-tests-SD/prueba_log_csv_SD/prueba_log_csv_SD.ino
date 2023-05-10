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
//

const int CSpin = 4;

char charRead;


String dataString ="";          // holds the data to be written to the SD card

float carb = 15.03;    // value read from your first sensor
int carb_R = 2;

float lip = 2.00;    // value read from your second sensor
int lip_R = 0;

float prot = 54.40;    // value read from your third sensor
int prot_R = 1;

float kcal = 546.09;

int veces = 3;
bool borrado = false;

File myFile;
//
//
void setup()
{
    // Open serial communications
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
      }
     
    // see if the card is present and can be initialized:
    if (!SD.begin(CSpin)) {
        Serial.println("Card failed, or not present");
        // don't do anything more:
        return;
    }
    Serial.println("card initialized.");

    Serial.println("\nPulse una tecla para comenzar");
    while(Serial.available() == 0){;}
    charRead = Serial.read();  
    Serial.write(charRead); //write it back to Serial window
    Serial.println();

    writeHeader();
    readData();
  
}

void loop(){
  
  if(veces != 0){
    // build the data string
    dataString = String(carb) + ";" + String(carb_R) + ";" + 
                 String(lip) + ";" + String(lip_R) + ";" + 
                 String(prot) + ";" + String(prot_R) + ";" + 
                 String(kcal); // convertir a CSV con ';' como delimitador para que Excel lo abra directamente
                 
    saveData(); // save to SD card
    readData();
    delay(5000);

    veces--;
  }
  //else{
      //if(!borrado) deleteData();
  //}
    
}

void writeHeader(){
    Serial.println(F("\n Creando header del fichero...\n"));
    String header = "carb;carb_r;lip;lip_R;prot;prot_R;kcal";
    myFile = SD.open("data.csv", FILE_WRITE);
    if (myFile){
        myFile.println(header);
        myFile.close(); // close the file
    }
    else{
        Serial.println("Error opening file for writing!");
    }
}

void saveData(){
    Serial.println(F("Guardando info...\n"));
    myFile = SD.open("data.csv", FILE_WRITE);
    if (myFile){
        myFile.println(dataString);
        myFile.close(); // close the file
    }
    else{
        Serial.println("Error opening file for writing!");
    }
}

void readData(){
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
}

void deleteData(){
    Serial.println(F("Borrando fichero...\n"));
    SD.remove("data.csv");
    if(!SD.exists("data.csv")){
        Serial.println(F("Fichero borrado con éxito\n"));
        borrado = true;
    }
    else{
      Serial.println(F("Error al borrar el fichero\n"));
    }
    
}
