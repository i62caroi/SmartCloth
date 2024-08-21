/*
    Este programa crea un fichero TXT en la SD y le introduce un ejemplo de 3 comidas 
    de un día cualquiera 
*/

#include <SD.h>


//#define SD_CARD_SCS  4      // SmartCloth v2.1
#define SD_CARD_SCS  13   // SmartCloth v2.2



// Fichero donde ir escribiendo la info para cuando haya wifi
char fileTXT[30] = "data/test.txt";



/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    setupSDcard();     // Inicializar tarjeta SD
void    createFileTXT();   // Crear fichero TXT con comidas de ejemplo
void    readFileTXT();     // Leer contenido del fichero TXT
/*-----------------------------------------------------------------------------*/



void setup()
{
    Serial.begin(115200);
    while (!Serial);
     
    // Inicializar SD
    if (!setupSDcard()){
        Serial.println(F("Card failed, or not present"));
        return;
    }
    delay(100);

    // Crear fichero TXT con comidas de ejemplo
    createFileTXT();

    // Leer contenido del fichero TXT
    readFileTXT();

}


void loop()
{
    // No hace falta hacer nada más
    delay(1000);
}








/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicializar la tarjeta SD.
 * @return true: inicialización correcta  false: fallo al inicializar la SD
 */
/*-----------------------------------------------------------------------------*/
bool setupSDcard()
{
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
 * @brief Crea el fichero TXT y escribe en él un ejemplo de 3 comidas de un día cualquiera.
 * 
 */
/*-----------------------------------------------------------------------------*/
void createFileTXT()
{
    // Si ya existe, se borra
    if (SD.exists(fileTXT)){
        Serial.println(F("Borrando fichero TXT (ESP32)..."));
        SD.remove(fileTXT);
    }

    // Crear nuevo fichero
    File dataFile = SD.open(fileTXT, FILE_WRITE);

    if (dataFile) 
    {
        // Escribir en el fichero las comidas de ejemplo
        dataFile.println("INICIO-COMIDA");
        dataFile.println("INICIO-PLATO");
        dataFile.println("ALIMENTO,15,53.50");
        dataFile.println("ALIMENTO,9,53.50");
        dataFile.println("FIN-COMIDA,23.06.2024,10:18:36");
        dataFile.println("INICIO-COMIDA");
        dataFile.println("INICIO-PLATO");
        dataFile.println("ALIMENTO,15,24.40");
        dataFile.println("FIN-COMIDA,23.06.2024,15:25:25");
        dataFile.println("INICIO-COMIDA");
        dataFile.println("INICIO-PLATO");
        dataFile.println("ALIMENTO,15,53.50");
        dataFile.println("ALIMENTO,9,53.50");
        dataFile.println("INICIO-PLATO");
        dataFile.println("ALIMENTO,3,32.07");
        dataFile.println("ALIMENTO,27,46.65");
        dataFile.println("ALIMENTO,10,16.23");
        dataFile.println("FIN-COMIDA,23.06.2024,22:39:36");

        dataFile.close();

        Serial.println("\nFichero TXT creado con comidas de ejemplo");
        
    }
    else 
    {
        Serial.println(F("Error al abrir el archivo test.txt"));
    }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Lee el contenido del archivo en la tarjeta SD.
 */
/*-----------------------------------------------------------------------------*/
void readFileTXT()
{
    if (SD.exists(fileTXT)){
        Serial.println(F("\n\nLeyendo fichero...\n"));

        File myFile = SD.open(fileTXT, FILE_READ);
        
        if (myFile){
            while (myFile.available()) {
                Serial.write(myFile.read());
            }
            myFile.close();
        }
        else{
            Serial.println("Error abriendo el fichero TXT para lectura!");
        }
    }
    else{
        Serial.println("\nNo se encuentra el fichero TXT!\n");
    }
}