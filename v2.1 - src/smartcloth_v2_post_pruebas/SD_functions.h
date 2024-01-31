
/** 
 * @file SD_functions.h
 * @brief Módulo Tarjeta SD
 *
 * @author Irene Casares Rodríguez
 * @date 17/07/23
 * @version 1.0
 *
 * Se ha utilizado el módulo de SD integrado en la pantalla TFT:
 *   Pantalla modelo: ER-TFTM070-6 de BuyDisplay
 * 
 * Este módulo no está conectado directamente con el micro de la pantalla,
 * por lo que se debe conectar de forma independiente al bus SPI del Arduino.
 * Esto produce que 2 dispositivos, la pantalla y la SD, estén compartiendo el
 * mismo bus SPI.
 *
 * Resulta que el micro RA8876 tiene el mismo problema en relación al pin MISO del SPI
 * que el micro RA8875 (véase https://github.com/wwatson4506/Ra8876LiteTeensy), y es que
 * no se "suelta" el dispositivo (su chipselect) tras finalizar la transacción SPI,
 * lo cual debería hacerse automáticamente. Esto crea un problema a la hora de usar la
 * misma interfaz SPI para diferentes dispositivos.
 *
 * Lo mejor sería utilizar diferentes pines para el CS de cada dispositivo, lo que se
 * ha hecho en este caso (SD en pin 4 y TFT en pin 12). Otra opción sería utilizar un
 * chip externo de buffer triestado como el 74HCT125.
 *
 * @see https://cs.stanford.edu/people/miles/iso8859.html Colección de caracteres ASCII/ISO 8859-1 (Latin-1) en HEX
 * @see https://www.raio.com.tw/data_raio/Datasheet/RA887677/RA8876_Brief_DS_V11_Eng.pdf Table 14-1 (pag 99) del datasheet de RA8876 para caracteres Latin-1
 * @see https://www.buydisplay.com/spi-7-inch-tft-lcd-dislay-module-1024x600-ra8876-optl-touch-screen-panel  SPI 7"TFT LCD Dislay 1024x600 OPTL Capacitive Touch Screen
 *
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

#ifndef SD_FUNCTIONS_H
#define SD_FUNCTIONS_H

#include <SD.h>
#include "RTC.h"
#include "Diario.h" // incluye Comida.h
#include "Files.h"
#include "lista_Comida.h"

#define SerialPC Serial

#define SD_CARD_SCS  4


/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
bool    setupSDcard();              // Inicializar tarjeta SD
void    writeHeaderFileSD();        // Crear fichero CSV y escribir header 
void    saveComidaSD();             // Guardar valores de la comida en fichero CSV
void    getAcumuladoHoyFromSD();    // Sumar comidas del día desde CSV y mostrar en "Acumulado Hoy"

bool    deleteFileCSV();            // Borrar contenido del fichero CSV

bool    checkFileESP32();           // Comprobar si el fichero TXT del ESP32 está vacío o si hay algo que subir
void    readFileESP32();            // Leer contenido del fichero TXT del ESP32 y mostrarlo por terminal
void    sendFileToESP32();          // Enviar fichero TXT al ESP32 línea a línea     
bool    deleteFileESP32();          // Borrar contenido del fichero TXT del ESP32
/*-----------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicializa la tarjeta SD y realiza la configuración necesaria para su uso.
 * 
 * @return true si la tarjeta SD se inicializa correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool setupSDcard(){
    if(!SD.begin(SD_CARD_SCS)){
        SerialPC.println(F("SD card failure!"));
        return false;
    }
    else SerialPC.println(F("SD card initialized"));

    if(!SD.exists(fileCSV)){ //Si no existe ya, se incorpora el encabezado. Todo se va a ir guardando en el mismo fichero.
        writeHeaderFileSD();
    }

    getAcumuladoHoyFromSD();   // Leer fichero csv de la SD y sumar los valores nutricionales y el peso de las 
                               // comidas guardadas en el día de hoy
    
    return true;
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Escribe el encabezado del archivo en la tarjeta SD.
 * El encabezado contiene los nombres de las columnas separados por ';'.
 */
/*-----------------------------------------------------------------------------*/
void writeHeaderFileSD(){
    SerialPC.print(F("\n Creando fichero ")); SerialPC.print(fileCSV); SerialPC.println(F(" ...\n"));

    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente:

    String header = "fecha;hora;carb;carb_R;lip;lip_R;prot;prot_R;kcal;peso";

    File myFile = SD.open(fileCSV, FILE_WRITE);    // Todo se va a ir guardando en el mismo fichero ==> 'fileCSV' en Files.h
    if (myFile){
        myFile.println(header);
        myFile.close(); // close the file
    }
    else{
        SerialPC.println("Error abriendo archivo CSV!");
    }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda la información de la comida en la tarjeta SD.
 * La información se guarda en un archivo CSV y en un archivo TXT para el ESP32.
 */
/*-----------------------------------------------------------------------------*/
void saveComidaSD(){
    SerialPC.println(F("Guardando info...\n"));

    // Se ha utilizado un RTC para conocer la fecha a la que se guarda la comida

    // ----- 1. GUARDADO LOCAL ------------------------------------------------------------
    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente:
    //    "fecha;hora;carb;carb_R;lip;lip_R;prot;prot_R;kcal;peso"

    String comidaValues = comidaActual.getComidaAllValues(); // carb;carb_R;lip;lip_R;prot;prot_R;kcal
    String peso = String(comidaActual.getPesoComida());

    String dataString = String(rtc.getDateStr()) + ";" + String(rtc.getTimeStr()) + ";" + comidaValues + ";" + peso; 


    File myFile = SD.open(fileCSV, FILE_WRITE); // Todo se va a ir guardando en el mismo fichero ==> 'fileCSV' en Files.h
    if (myFile){
        myFile.println(dataString);
        myFile.close(); 
        SerialPC.println("Comida guardada correctamente en la SD");
    }
    else{
        SerialPC.println("Error abriendo archivo CSV!");
    }
    // --------------------------------------------------------------------------------


    // ----- 2. GUARDADO FICHERO ESP32 -------------------------------------------------
    // Guardar información en el fichero que se pasará al esp32 cuando haya WIFI

    // Escribir FIN-COMIDA,<fecha>,<hora> en la lista
    listaComidaESP32.saveComida();

    // Guardar lista en fichero
    myFile = SD.open(fileESP32, FILE_WRITE);

    if (myFile) {
        // Escribe cada línea en el archivo
        for (int i = 0; i < listaComidaESP32.getListSize(); i++) {
            myFile.println(listaComidaESP32.getItem(i));
        }
        
        // Cierra el archivo
        myFile.close();

        // Limpia la lista para la próxima comida
        listaComidaESP32.clearList();

        SerialPC.println("Comida guardada correctamente en fichero TXT");
    } else {
        // Si el archivo no se abre, imprime un error:
        SerialPC.println("Error abriendo archivo TXT!");
    }
    // --------------------------------------------------------------------------------
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Obtiene los valores acumulados de hoy desde una tarjeta SD.
 * 
 * Esta función lee un archivo CSV en una tarjeta SD y suma los valores nutricionales correspondientes al día actual.
 * Los valores acumulados incluyen carbohidratos, lípidos, proteínas, kilocalorías y peso.
 * 
 * @note Esta función asume que el archivo CSV tiene un formato específico y que la tarjeta SD está correctamente configurada.
 */
/*-----------------------------------------------------------------------------*/
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


    File myFile = SD.open(fileCSV, FILE_READ);
    if (myFile){
        while (myFile.available()) {
            
            myFile.readBytesUntil('\n', lineBuffer, sizeof(lineBuffer) - 1); // Leer línea completa hasta el tamaño máximo del búfer
            lineBuffer[sizeof(lineBuffer) - 1] = '\0'; // Asegurar terminación nula

            //SerialPC.println(lineBuffer);
            
            token = strtok(lineBuffer, ";"); // Separar campos de la línea utilizando el delimitador ';'
            fieldIndex = 0;

            if(strcmp(today, token) == 0){ // si 'today' igual que primer token ==> comida guardada hoy
                
                if(msg){
                    SerialPC.println(F("Obteniendo Acumulado Hoy..."));
                    msg = false; // Solo imprimir una vez y si hay algo que sumar
                }

                nComidas++; // Incrementar numero comidas guardadas hoy

                float valueFloat;
                int valueInt;

                while (token != NULL) {

                    switch (fieldIndex){ // fieldIndex = 0 => fecha     fieldIndex = 1 => hora    fieldIndex = 2 => carb  ...
                        case 2:   valueFloat = atof(token);     sumCarb   += valueFloat;     break;    // Carbohidratos
                        case 3:   valueInt   = atoi(token);     sumCarb_R += valueInt;       break;    // Raciones de carbohidratos
                        case 4:   valueFloat = atof(token);     sumLip    += valueFloat;     break;    // Lípidos (Grasas)
                        case 5:   valueInt   = atoi(token);     sumLip_R  += valueInt;       break;    // Raciones de lípidos
                        case 6:   valueFloat = atof(token);     sumProt   += valueFloat;     break;    // Proteínas
                        case 7:   valueInt   = atoi(token);     sumProt_R += valueInt;       break;    // Raciones de proteínas
                        case 8:   valueFloat = atof(token);     sumKcal   += valueFloat;     break;    // Kilocalorías
                        case 9:   valueFloat = atof(token);     sumPeso   += valueFloat;     break;    // Peso
                        default:  break;
                    }

                    // Obtener el siguiente campo
                    token = strtok(NULL, ";");
                    fieldIndex++;
                }
            }
         
        }

        myFile.close();


        // ----- ACTUALIZAR ACUMULADO HOY -----
        ValoresNutricionales valAux(sumCarb, sumLip, sumProt, sumKcal);   
        diaActual.setValoresDiario(valAux);                               // Inicializar valores nutricionales del Acumulado Hoy
        diaActual.setPesoDiario(sumPeso);                                 // Actualizar peso del Acumulado Hoy
        diaActual.setNumComidas(nComidas);                                // Actualizar nº de comidas del Acumulado Hoy
        
    }
    else{
        SerialPC.println("Error abriendo archivo CSV!");
    }

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el fichero CSV existente y crea uno nuevo con el mismo nombre.
 * 
 * @return true si se borra y crea el fichero correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool deleteFileCSV(){
    // Borrar contenido borrando fichero y creándolo de nuevo no se debe hacer, 
    // especialmente si se va a repetir muchas veces, pero creo que para las 
    // pruebas no habrá problema.

    // -------- BORRAR FICHERO CSV --------------------------
    SerialPC.println("Borrando fichero csv...");
    SD.remove(fileCSV);

    if (!SD.exists(fileCSV)) {
        SerialPC.println("Fichero CSV borrado");
    }
    else  {
        SerialPC.println("Error borrando fichero CSV!");
        return false;
    }
    // -------- FIN BORRAR FICHERO CSV ----------------------


    // -------- CREAR NUEVO FICHERO CSV ---------------------
    // Creo uno nuevo con el mismo nombre (writeHeader())
    SerialPC.println("\nCreando fichero de nuevo..");
    writeHeaderFileSD();      // Crear fichero de nuevo e incluir el header
    getAcumuladoHoyFromSD();  // Actualizar acumulado (ahora debe ser 0)
    return true;
    // -------- FIN CREAR NUEVO FICHERO CSV -----------------
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si el archivo TXT del ESP32 contiene contenido.
 * 
 * @return true si el archivo tiene contenido, false si está vacío o no se puede abrir.
 */
/*-----------------------------------------------------------------------------*/
bool checkFileESP32(){
    SerialPC.println(F("\n\nComprobando contenido del fichero TXT del ESP32...\n"));
    File myFile = SD.open(fileESP32, FILE_READ);
    if (myFile){
        if (myFile.size() > 0) { // Si el tamaño del archivo es mayor que 0, tiene contenido (true)
            myFile.close();
            return true;
        } 
        else { // Si el tamaño del archivo es 0, está vacío (false)
            myFile.close();
            return false;
        }
    }
    else{
        SerialPC.println("Error abriendo fichero TXT!");
        return false; // Si hubo error, lo tratamos como vacío para que no intente acceder 
    }
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Lee un archivo de texto del ESP32 y muestra su contenido en el monitor serie.
 */
/*-----------------------------------------------------------------------------*/
void readFileESP32(){
    SerialPC.println(F("\n\nLeyendo fichero TXT del ESP32...\n"));
    File myFile = SD.open(fileESP32, FILE_READ);
    if (myFile){
        while (myFile.available()) {
            SerialPC.write(myFile.read());
        }
        myFile.close();
    }
    else{
        SerialPC.println("Error abriendo fichero TXT!");
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía el archivo TXT al ESP32 línea por línea a través de Serial.
 * 
 */
/*-----------------------------------------------------------------------------*/
void sendFileToESP32(){
    File dataFile = SD.open(fileESP32);

    if (dataFile) {
        // Lee el archivo línea por línea y envía cada línea al ESP32
        while (dataFile.available()){
            String line = dataFile.readStringUntil('\n');
            line.trim();

            // Envía la línea al ESP32 a través de Serial
            SerialDueESP32.println(line);
        }
        dataFile.close();

        SerialDueESP32.println("FIN-TRANSMISION");

        SerialPC.println("\nFichero completo enviado");
    }
    else {
        SerialPC.println("\nError al abrir el archivo data-ESP.txt");
    }
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra un fichero en el ESP32.
 * 
 * Esta función borra un fichero en el ESP32 utilizando la tarjeta SD.
 * 
 * @return true si el fichero se borra correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool deleteFileESP32(){

    // -------- BORRAR FICHERO ESP32 ------------------------
    SerialPC.println("Borrando fichero ESP32...");
    SD.remove(fileESP32);

    if (!SD.exists(fileESP32)) {
        SerialPC.println("Fichero ESP32 borrado");
        return true;
    }
    else  {
        SerialPC.println("Error borrando fichero ESP32!");
        return false;
    }
    // ------------------------------------------------------

    // En este caso no hace falta crearlo aquí, como sí ocurría con el CSV, porque
    // cuando se vaya a escribir algo al guardar comida, ya se creará.
}






#endif
