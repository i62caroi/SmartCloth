/*
    Este programa crea un fichero TXT en la SD y le introduce un ejemplo de 3 comidas 
    de un día cualquiera 
*/

#include <SD.h>


// Simular la lectura del archivo
String string1 = "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,5,53.50\n"
                "ALIMENTO,7,23.50\n"
                "ALIMENTO,50,120.5,8424621001093\n"  // Línea con barcode (Mini Tostas de Trigo)
                "FIN-COMIDA,03.07.2024,08:55:36\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,20,24.40\n"
                "FIN-COMIDA,03.07.2024,18:25:25\n";
                
                

String string2 = "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "FIN-COMIDA,02.05.2024,17:55:36\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,24.40\n"
                "FIN-COMIDA,02.05.2024,18:25:25\n"
                "INICIO-COMIDA\n"
                "INICIO-PLATO\n"
                "ALIMENTO,15,53.50\n"
                "ALIMENTO,9,53.50\n"
                "INICIO-PLATO\n"
                "ALIMENTO,3,32.07\n"
                "ALIMENTO,27,46.65\n"
                "ALIMENTO,10,16.23\n"
                "FIN-COMIDA,02.05.2024,10:39:36\n";



String invalidString = "INICIO-COMIDA\n"
                        "INICIO-PLATO\n"
                        "ALIMENTO,5,53.50\n"
                        "ALIMENTO,7,23.50\n"
                        "ALIMENTO,50,120.5,8424621001093\n"  // Línea con barcode (Mini Tostas de Trigo)
                        "FIN-COMIDA,31.12.2024,10:08:36\n"

                        "INICIO-COMIDA\n"
                        "INICIO-PLATO\n"
                        "ALIMENTO,30,24.40\n" // Grupo no válido, toda esta comida debería fallar y guardarse en TXT auxiliar
                        "FIN-COMIDA,31.12.2024,14:28:36\n";




//#define SD_CARD_SCS  4      // SmartCloth v2.1
#define SD_CARD_SCS  13   // SmartCloth v2.2



// Fichero donde ir escribiendo la info para cuando haya wifi
char fileTXT[30] = "data/test.txt";
//char    fileTXT[30] = "data/data-esp.txt"; 




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    setupSDcard();     // Inicializar tarjeta SD
void    createFileTXT();   // Crear fichero TXT con comidas de ejemplo
void    createFileTXT(String lines);
void    addInfoToFileTXT(String lines);     
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

    // Si ya existe, se borra
    if (SD.exists(fileTXT)){
        Serial.println(F("Borrando fichero TXT (ESP32)..."));
        SD.remove(fileTXT);
    }

    // Crear fichero TXT con comidas de ejemplo
    //createFileTXT();
    //createFileTXT(invalidString);

    // Añadir más comidas al fichero TXT
    addInfoToFileTXT(string2);

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



void createFileTXT(String lines)
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
        int start = 0;
        int end = lines.indexOf('\n');

        while (end != -1) 
        {
            String line = lines.substring(start, end);
            line.trim();

            dataFile.println(line); // Escribir línea en TXT

            start = end + 1;
            end = lines.indexOf('\n', start);
        }

        // Escribe la última línea si existe
        if (start < lines.length()) {
            String line = lines.substring(start);
            line.trim();
            dataFile.println(line);
        }

        dataFile.close();

        Serial.println("\nFichero TXT creado con comidas de ejemplo");
        
    }
    else 
    {
        Serial.println(F("Error al abrir el archivo test.txt"));
    }
}



void addInfoToFileTXT(String lines)
{
    File dataFile = SD.open(fileTXT, FILE_WRITE);

    if (dataFile) 
    {
        int start = 0;
        int end = lines.indexOf('\n');

        while (end != -1) 
        {
            String line = lines.substring(start, end);
            line.trim();

            dataFile.println(line); // Escribir línea en TXT

            start = end + 1;
            end = lines.indexOf('\n', start);
        }

        // Escribe la última línea si existe
        if (start < lines.length()) {
            String line = lines.substring(start);
            line.trim();
            dataFile.println(line);
        }

        dataFile.close();

        Serial.println("\nFichero TXT actualizado con más comidas de ejemplo");
        
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