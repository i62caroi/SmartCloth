
// Borrar contenido de fichero csv

#include <SPI.h>
#include <SD.h>


const int chipSelect = 4;

char fileCSV[30] = "data/daTest.csv";

File myFile;


void setup() {
  // Open serial communications
    Serial.begin(115200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
      }

    Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    while (1);
  }
  Serial.println("card initialized.");


 /* if(!SD.exists(fileCSV)){ 
        escribirFichero();
  }
  else leerFichero();*/

  //borrarContenido();

  deleteFile();

  leerFichero();

}



void loop() {

}


void deleteFile(){
    Serial.println(F("Borrando fichero...\n"));
    SD.remove(fileCSV);
    if(!SD.exists(fileCSV)){
        Serial.println(F("Fichero borrado con éxito\n"));
    }
    else{
      Serial.println(F("Error al borrar el fichero\n"));
    }    
}


void escribirFichero(){
    Serial.print(F("\n Creando fichero ")); Serial.print(fileCSV); Serial.println(F(" ...\n"));

    String dataString = "hola;que;tal"; 

    myFile = SD.open(fileCSV, FILE_WRITE);    
    if (myFile){
        myFile.println(dataString);
        myFile.close(); // close the file
    }
    else{
        Serial.println("Error opening file for writing!");
    }
    Serial.println();
}



void leerFichero(){
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



void borrarContenido(){
    // Borrar contenido borrando fichero y creándolo de nuevo no se debe hacer, especialmente
    // si se va a repetir muchas veces, pero creo que para las pruebas no habrá problema.

    // 1 - Borro el fichero
    Serial.println("Borrando fichero csv...");
    SD.remove(fileCSV);

    if (!SD.exists(fileCSV)) {
        Serial.println("Fichero borrado");
    }
    else  {
        Serial.println("Error borrando fichero.");
    }


    // 2 - Creo uno nuevo con el mismo nombre (writeHeader())
    Serial.println("\nCreando fichero de nuevo..");
    writeHeader();

}


void writeHeader(){
    String header = "fecha;hora;carb;carb_R;lip;lip_R;prot;prot_R;kcal;peso";

    myFile = SD.open(fileCSV, FILE_WRITE);  
    if (myFile){
        myFile.println(header);
        myFile.close(); // close the file
    }
    else{
        Serial.println("Error opening file for writing!");
    }
}






