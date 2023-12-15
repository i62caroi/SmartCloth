/**
 * @file SD_functions_ESP32.h
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

#ifndef SD_FUNCTIONS_ESP32_H
#define SD_FUNCTIONS_ESP32_H

#include <SD.h>
#include "RTC.h"
#include "functions.h"

#define SD_CARD_SCS  4 ///< Define el pin CS para la tarjeta SD

// Fichero donde ir escribiendo la info para cuando haya wifi
char fileESP32[30] = "data/data-ESP.txt";

// Posicion de la línea de la última aparición de INICIO-PLATO, para saber desde dónde borrar.
int lastPlatoStartPosition = 0; 



/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
bool    setupSDcard();                   // Inicializar tarjeta SD

void    iniciarElemento(String cadInicio);      // Escribir INICIO-COMIDA o INICIO-PLATO en file para ESP32
void    saveAlimento();                         // Guardar par <grupo>,<peso>

void    borrarLastPlato(bool writeNuevoPlato);  // Borrar información del plato actual (hasta último INICIO-PLATO)
                                                // writeNuevoPlato --> true: escribir de nuevo INICIO-PLATO     false: no escribirlo

void    saveComida();                           // Escribir FIN-COMIDA,<fecha>,<hora>

void    readFileESP32();
void    borrarFileESP32();
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
 * @brief Inicia un nuevo elemento (comida o plato).
 * @param cadInicio Cadena que indica el tipo de inicio ("INICIO-COMIDA" o "INICIO-PLATO").
 */
/*-----------------------------------------------------------------------------*/
void iniciarElemento(String cadInicio)
{
    if(cadInicio == "INICIO-COMIDA") Serial.println(F("\nIniciando comida..."));
    else if(cadInicio == "INICIO-PLATO") Serial.println(F("\nIniciando plato..."));

    File myFile = SD.open(fileESP32, FILE_WRITE);

    if (myFile){
        // Guardar la posición del último INICIO-PLATO escrito para saber desde dónde borrar
        // al borrar el plato actual.
        // Se pone antes de escribir la línea porque se añade un '\n' al final,
        // por lo que si se obtuviera la posición después de escribir, se estaría
        // cogiendo la línea posterior a INICIO-PLATO.
        if(cadInicio == "INICIO-PLATO") {
            lastPlatoStartPosition = myFile.position();
        }

        myFile.println(cadInicio);
        myFile.close(); // close the file
        
        if(cadInicio == "INICIO-COMIDA") Serial.println(F("Creado INICIO-COMIDA en la SD"));
        else if(cadInicio == "INICIO-PLATO") Serial.println(F("Creado INICIO-PLATO en la SD"));
    }
    else{
        Serial.println("Error opening file for writing!");
    }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda un alimento con su peso correspondiente.
 */
/*-----------------------------------------------------------------------------*/
void saveAlimento()
{
    Serial.println(F("Guardando alimento y peso...\n"));

    String cadGrupoPeso = getParGRupoPeso();

    File myFile = SD.open(fileESP32, FILE_WRITE);
    if (myFile){
        myFile.println(cadGrupoPeso);
        myFile.close(); // close the file
        Serial.println("Creado ALIMENTO,<grupo>,<peso> en la SD");
    }
    else{
        Serial.println("Error opening file for writing!");
    }
}



/*-----------------------------------------------------------------------------*/
/*
    Debido a las limitaciones de memoria y procesamiento de las placas Arduino, no todas las 
    bibliotecas de C++ están disponibles para su uso. En particular, las bibliotecas que permiten 
    manipulaciones de archivos más avanzadas, como leer desde el final de un archivo, generalmente 
    no están disponibles.
    
    Por tanto, no hay una forma directa de eliminar una línea específica de un archivo en la tarjeta SD,
    sino que la forma estándar de "eliminar" líneas es crear un archivo temporal al que copiar
    las líneas que se quieran mantener y luego reemplazar el archivo original con el nuevo archivo.

    Para eso, se irá actualizando la variable lastPlatoStartPosition con la posición de la línea
    donde se escriba el último INICIO-PLATO para saber en qué punto dejar de copiar información.
*/
/**
 * @brief Borra la información del plato actual hasta el último "INICIO-PLATO" inclusive
 * @param writeNuevoPlato Indica si se debe escribir un nuevo "INICIO-PLATO" después de borrar.
 */
/*-----------------------------------------------------------------------------*/
void borrarLastPlato(bool writeNuevoPlato)
{
    Serial.println(F("Borrando plato actual...\n"));

    if (lastPlatoStartPosition > 0) {
        File originalFile = SD.open(fileESP32, FILE_READ);
        File tempFile = SD.open("temp.txt", FILE_WRITE);

        // ----- 1. COPIAR AL ARCHIVO TEMPORAL --------------------------------------------
        // Primera parte: copiar al archivo temporal
        // Copiar en 'tempFile' todas las líneas hasta la línea con la última aparición
        // de INICIO-PLATO, marcada por su posición en 'lastPlatoStartPosition'.
        if (originalFile && tempFile) {
            int pos = 0;
            while (originalFile.available() && pos < lastPlatoStartPosition) {
                tempFile.write(originalFile.read());
                pos++;
            }
            originalFile.close();
            tempFile.close();
        } else {
            Serial.println("Error opening file!");
        }
        // --------------------------------------------------------------------------------

        // ----- 2. COPIAR AL ARCHIVO ORIGINAL --------------------------------------------
        // Segunda parte: copiar de nuevo al archivo original
        // La librería SD.h no tiene un método para renombrar ficheros, así que debemos
        // borrar el fichero original y volver a crear uno con el mismo nombre donde
        // copiar las líneas que no se han "borrado" y que están en el 'tempFile'.
        if (SD.exists(fileESP32)) {
            SD.remove(fileESP32);
        }
        originalFile = SD.open(fileESP32, FILE_WRITE);
        tempFile = SD.open("temp.txt", FILE_READ);

        if (originalFile && tempFile) {
            while (tempFile.available()) {
                originalFile.write(tempFile.read());
            }
            originalFile.close();
            tempFile.close();
        } else {
            Serial.println("Error opening file!");
        }

        // Elimina el archivo temporal
        if (SD.exists("temp.txt")) {
            SD.remove("temp.txt");
        }
        // --------------------------------------------------------------------------------


        // ----- 3. NUEVO PLATO -----------------------------------------------------------
        // Crear nuevo plato con INICIO-PLATO para que pueda continuar añadiendo alimentos 
        // sin tener que indicar explícitamente "añadir plato" tras borrar el contenido del actual.
        // SmartCloth ya hace esto: tras borrar el plato, deja listo el objeto platoActual
        // para continuar con la comida.

        // En el caso en que se borre el plato pero no se continúe con la comida, sino que
        // se guarde tal como está, antes de escribir FIN-COMIDA habría que comprobar
        // si la última línea es INICIO-PLATO en lugar de ALIMENTO, lo que significaría
        // que el último plato está vacío. Si esto ocurriera, habría que borrar esa línea
        // escrita con INICIO-PLATO (usar misma función borrarLastPlato()) antes de escribir
        // la línea de FIN-COMIDA para evitar enviar información vacía.

        // Si en lugar de escribir de nuevo INICIO-PLATO tras borrarlo, se hubiera mantenido y
        // se hubiera borrado desde su línea posterior, al querer borrarlo antes de escribir
        // FIN-COMIDA (usando la misma función borrarLastPlato()) no se podría hacer porque
        // la función intentaría borrar desde su línea posterior y, en ese caso, no es lo deseado.

        // Se hace esta comprobación para, al comprobar antes de FIN-COMIDA, no volverlo a escribir
        // tras borrarlo.
        if(writeNuevoPlato) iniciarElemento("INICIO-PLATO"); 


        // --------------------------------------------------------------------------------

    }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Escribe "FIN-COMIDA" junto con la fecha y hora actuales.
 */
/*-----------------------------------------------------------------------------*/
void saveComida()
{
    Serial.println(F("Finalizando y guardando comida...\n"));

    // ----- 1. COMPROBAR ULTIMO INICIO-PLATO --------------------------------------------
    // Comprobar si la penúltima línea es INICIO-PLATO y borrarla.
    // Se mira la penúltima porque en iniciarElemento() se escribe "INICIO-PLATO\n", por lo que 
    // la última estará vacía tras hacer el salto de línea.

    File myFile = SD.open(fileESP32, FILE_READ);

    if (myFile) 
    {
        // Colocarse en la línea con el último INICIO-PLATO:
        myFile.seek(lastPlatoStartPosition);

        // No se puede simplemente hacer 'lastPlatoStartPosition + 1' para moverse a la siguiente
        // línea porque cada línea puede tener un número diferente de caracteres, por lo que hacer 
        // eso no llevaría a la siguiente.
        // En su lugar, se debe leer la línea completa para mover el cursor al inicio de la siguiente línea:
        myFile.readStringUntil('\n');

        // Si el siguiente carácter es el final del archivo, significa que lo último escrito en el
        // fichero es INICIO-PLATO, por lo que habría que borrarlo.
        if (myFile.peek() == -1) { 
            myFile.close();
            borrarLastPlato(false); // Borrar último INICIO-PLATO y no volverlo a escribir (false)
        } else {
            myFile.close();
        }
    } else {
        Serial.println("Error opening file for reading!");
    }
    // --------------------------------------------------------------------------------


    // ----- 2. FIN-COMIDA ------------------------------------------------------------
    char *today = rtc.getDateStr();
    char *time = rtc.getTimeStr();

    String cad = "FIN-COMIDA," + String(today) + "," + String(time); // FIN-COMIDA,<fecha>,<hora>

    myFile = SD.open(fileESP32, FILE_WRITE);
    if (myFile){
        myFile.println(cad);
        myFile.close(); // close the file
        Serial.println("Creado FIN-COMIDA,<fecha>,<hora> en la SD");
    }
    else{
        Serial.println("Error opening file for writing!");
    }
    // --------------------------------------------------------------------------------
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
 * @brief Borra el archivo en la tarjeta SD.
 */
/*-----------------------------------------------------------------------------*/
void borrarFileESP32(){

    Serial.println("Borrando fichero ESP32...");
    SD.remove(fileESP32);

    if (!SD.exists(fileESP32)) {
        Serial.println("Fichero ESP32 borrado");
    }
    else  {
        Serial.println("Error borrando fichero.");
    }
}








#endif
