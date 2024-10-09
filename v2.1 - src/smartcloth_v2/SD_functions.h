
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
      |  CS   |      pin 2  (blanco)     |   pin 13 (digital PWM) | 
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
#include "debug.h" // SM_DEBUG --> SerialPC
#include "Serial_functions.h" // SerialESP32 y resultados de subir a database (WAITING_FOR_DATA, UPLOADING_DATA, MEAL_UPLOADED, MEALS_LEFT, ERROR_READING_TXT, NO_INTERNET_CONECTION, HTTP_ERROR, TIMEOUT, UNKNOWN_ERROR)



// --- Mostrar en pantalla que se ha subido (o no) info ---
// Usados en handleResponseFromESP32AfterUpload()
#define SHOW_SCREEN_UPLOAD_DATA   1
#define NOT_SHOW_SCREEN_UPLOAD_DATA 0


// --- TIPO DE GUARDADO HECHO (GENERAL) ---
// Usados en saveComida() y luego en showAccionRealizada()
#define  SAVE_EXECUTED_FULL                       3  // Se ha guardado en CSV y en la database
#define  SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP      4  // Solo se ha guardado en CSV. Fallo en petición HTTP
#define  SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI         5  // Solo se ha guardado en CSV. No hay WiFi
#define  SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT         6  // Solo se ha guardado en CSV. Timeout de respuesta del esp32
#define  SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR   7  // Solo se ha guardado en CSV. Error desconocido
#define  SAVE_EXECUTED_ONLY_DATABASE              8  // Solo en la database porque falló el CSV
#define  ERROR_SAVING_DATA                        9  // Error al guardar. Falló el CSV y la database



// ------- ChipSelect de SD --------------
#define SD_CARD_SCS  13 
// ---------------------------------------


/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
// -- Inicializar SD --
bool    setupSDcard();              // Inicializar tarjeta SD
// --------------------

// -- Guardar en CSV y TXT --
byte    saveComida(bool &hayConexionWifi);               // Guardar valores de la comida en fichero CSV y TXT
// --------------------------


// -- CSV: crear con header, leer, sumar comidas del día y guardar comida --
bool    writeHeaderFileCSV();       // Crear fichero CSV y escribir header 
void    getAcumuladoHoyFromSD();    // Sumar comidas del día desde CSV y mostrar en "Acumulado Hoy"
bool    saveComidaInCSV();          // Guardar comida en el fichero CSV
bool    deleteFileCSV();            // Borrar contenido del fichero CSV
// -------------------------------------------------------------------------


// -- TXT: guardar (TXT o database), leer, enviar al ESP32, borrar ---------
// --- Guardar comida actual (database o TXT) ---
byte    saveComidaInDatabase_or_TXT(bool &hayConexionWifi);  // Guardar lista en base de datos o en el fichero TXT, según valor de 'hayConexionWifi'
void    saveMealListInTXT();                                    // Guardar lista de la comida en el fichero TXT
bool    isFileTXTEmpty();                                   // Comprobar si el fichero TXT del ESP32 está vacío
#if defined(SM_DEBUG)
void    readFileTXT();                  // Leer contenido del fichero TXT del ESP32 y mostrarlo por terminal
#endif 
bool    deleteFileTXT();                // Borrar contenido del fichero TXT del ESP32
// ----------------------------------------------

// --- Actualizar SmartCloth --------------
byte            sendTXTFileToESP32ToUpdateWeb();           // Enviar fichero TXT al ESP32 línea a línea     

// Vector de comida actual leída del TXT
inline void     addLineToActualMeal(std::vector<String> &actualMeal, String &line){ actualMeal.push_back(line); }; // Añadir línea a 'actualMeal'
inline void     clearActualMeal(std::vector<String> &actualMeal){ actualMeal.clear(); }; // Limpiar 'actualMeal'

// Fichero auxiliar de comidas no subidas
inline void     saveMealToAuxFile(std::vector<String> &actualMeal, File& auxFile){ // Guardar comida actual no subida en fichero auxiliar
                for (const auto& line : actualMeal) 
                    auxFile.println(line); 
                }; 
inline bool     hayMealsToUploadLaterTXT(){ return SD.exists(auxFileTXT); };        // Comprobar si se ha escrito algo en el fichero auxiliar de comidas no subidas
void            updateFileTXTFromAuxFile();                                         // Actualizar fichero TXT con comidas no subidas y que se han guardado en un fichero auxiliar
bool            deleteAuxFileTXT();                                                 // Borrar fichero auxiliar TXT
void            readAuxFileTXT();                                                   // Leer contenido del fichero auxiliar TXT y mostrarlo por terminal
// ---- Fin actualizar SM -----------------
// -------------------------------------------------------------------------
/******************************************************************************/
/******************************************************************************/






/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/


/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicializa la tarjeta SD y realiza la configuración necesaria para su uso.
 * 
 * @return true si la tarjeta SD se inicializa correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool setupSDcard()
{
    #if defined(SM_DEBUG)
        SerialPC.println(F("\nInit SD..."));
    #endif

    if(!SD.begin(SD_CARD_SCS))
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("SD card failure!"));
            SerialPC.println("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        #endif
        return false;
    }
    else 
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("SD card initialized"));
            SerialPC.println("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        #endif
    }

    if(!SD.exists(fileCSV)) //Si no existe ya, se incorpora el encabezado. Todo se va a ir guardando en el mismo fichero.
    {
        if(writeHeaderFileCSV()){
            getAcumuladoHoyFromSD();   // Leer fichero csv de la SD y sumar los valores nutricionales y el peso de las 
                               // comidas guardadas en el día de hoy
        }
        else return false;
    }

    
    
    return true;
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda la información de la comida en la tarjeta SD.
 * La información se guarda en un archivo CSV y en un archivo TXT para el ESP32.
 */
/*-----------------------------------------------------------------------------*/
byte saveComida(bool &hayConexionWifi)
{
    #if defined(SM_DEBUG)
    SerialPC.println("--------------------------------------------------");
    SerialPC.println(F("Guardando info...\n"));
    #endif

    // ---- 1. GUARDADO LOCAL ------------------------
    bool savedCSV = saveComidaInCSV(); // TRUE o FALSE. Si fallara el guardado local, habría que arreglar SmartCloth
    // -----------------------------------------------

    // ---- 2. GUARDADO EN DATABASE O FICHERO TXT ----
    byte saveDatabaseOrTXT = saveComidaInDatabase_or_TXT(hayConexionWifi); // MEAL_UPLOADED, NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT, UNKNOWN_ERROR
    // -----------------------------------------------

    // ---- 3. RESULTADO DEL GUARDADO ----------------
    // Según si se ha guardado solo en local, solo en database o si no se ha guardado nada.

    if(savedCSV) // Se ha guardado en el CSV (acumulado actualizado)
    {
        switch(saveDatabaseOrTXT)
        {
            // A) Se ha guardado en el CSV (acumulado actualizado) y en la database. PERFECTO!!
            case MEAL_UPLOADED:             return SAVE_EXECUTED_FULL;                          
            
            // B) Se ha guardado en el CSV (acumulado actualizado) pero no en la database
            // Cuando se vuelva a encender el SM, se intentará subir la info que haya quedado a la espera en el TXT
            case HTTP_ERROR:                return SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP;         
            case NO_INTERNET_CONNECTION:    return SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI;            
            case TIMEOUT:                   return SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT;            
            case UNKNOWN_ERROR:             return SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR;     
        }
    }
    else // No se ha guardado en el CSV (el acumulado no se ha actualizado). FALLO ENORME!!!
    {    // Si no se guarda en el CSV, habría que arreglar el SM porque el usuario no puede ver su acumulado
        switch(saveDatabaseOrTXT)
        {
            // A) No se ha guardado en el CSV (el acumulado no se ha actualizado), pero si en la database
            case MEAL_UPLOADED:             return SAVE_EXECUTED_ONLY_DATABASE;         
            
            // B) No se ha guardado en el CSV ni en la database
            case HTTP_ERROR: 
            case NO_INTERNET_CONNECTION: 
            case TIMEOUT: 
            case UNKNOWN_ERROR:             return ERROR_SAVING_DATA;                        
        }
    }    
    // -----------------------------------------------

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Escribe el encabezado del archivo en la tarjeta SD.
 * El encabezado contiene los nombres de las columnas separados por ';'.
 */
/*-----------------------------------------------------------------------------*/
bool writeHeaderFileCSV() 
{
    #if defined(SM_DEBUG)
        SerialPC.print(F("\n Creando fichero ")); SerialPC.print(fileCSV); SerialPC.println(F(" ...\n"));
    #endif

    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente:

    String header = "fecha;hora;carb;carb_R;lip;lip_R;prot;prot_R;kcal;peso";

    File myFile = SD.open(fileCSV, FILE_WRITE);    // Todo se va a ir guardando en el mismo fichero ==> 'fileCSV' en Files.h
    if (myFile)
    {
        myFile.println(header);
        myFile.close(); // close the file
        return true;
    }
    else
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("Error abriendo archivo CSV!"));
        #endif
        return false;
    }
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
void getAcumuladoHoyFromSD()
{

    char *today = rtc.getDateStr(); // Es posible que se cambie de día durante el cocinado?? Si no, hacer en setupRTC(). 

    // SUMAS
    float sumCarb = 0.0, sumLip = 0.0, sumProt = 0.0, sumKcal = 0.0, sumPeso = 0.0;    
    float sumCarb_R = 0, sumLip_R = 0, sumProt_R = 0;
    byte nComidas = 0;

    char lineBuffer[128];  

    char *token;
    byte fieldIndex;

    bool msg = true;


    File myFile = SD.open(fileCSV, FILE_READ);
    if (myFile)
    {
        while (myFile.available()) 
        {
            myFile.readBytesUntil('\n', lineBuffer, sizeof(lineBuffer) - 1); // Leer línea completa hasta el tamaño máximo del búfer
            lineBuffer[sizeof(lineBuffer) - 1] = '\0'; // Asegurar terminación nula

            //SerialPC.println(lineBuffer);
            
            token = strtok(lineBuffer, ";"); // Separar campos de la línea utilizando el delimitador ';'
            fieldIndex = 0;

            if(strcmp(today, token) == 0) // si 'today' igual que primer token ==> comida guardada hoy
            {
                if(msg)
                {
                    #if defined SM_DEBUG
                        //SerialPC.println(F("Obteniendo Acumulado Hoy..."));
                    #endif
                    msg = false; // Solo imprimir una vez y si hay algo que sumar
                }

                nComidas++; // Incrementar numero comidas guardadas hoy

                float value;

                while (token != NULL) 
                {
                    switch (fieldIndex) // fieldIndex = 0 => fecha     fieldIndex = 1 => hora    fieldIndex = 2 => carb  ...
                    {
                        case 2:   value = atof(token);     sumCarb   += value;     break;    // Carbohidratos
                        case 3:   value = atof(token);     sumCarb_R += value;     break;    // Raciones de carbohidratos
                        case 4:   value = atof(token);     sumLip    += value;     break;    // Lípidos (Grasas)
                        case 5:   value = atof(token);     sumLip_R  += value;     break;    // Raciones de lípidos
                        case 6:   value = atof(token);     sumProt   += value;     break;    // Proteínas
                        case 7:   value = atof(token);     sumProt_R += value;     break;    // Raciones de proteínas
                        case 8:   value = atof(token);     sumKcal   += value;     break;    // Kilocalorías
                        case 9:   value = atof(token);     sumPeso   += value;     break;    // Peso
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
    else
    {
        #if defined SM_DEBUG
            //SerialPC.println(F("Error abriendo archivo CSV!"));
        #endif
    }

}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda la comida en el archivo CSV
 *      El acumulado se obtiene de este CSV, no de la base de datos, para ahorrar tiempo.
 * 
 * @note Guarda la información de forma "fecha;hora;carb;carb_R;lip;lip_R;prot;prot_R;kcal;peso"
 */
/*-----------------------------------------------------------------------------*/
bool saveComidaInCSV()
{
    // Se ha utilizado un RTC para conocer la fecha a la que se guarda la comida

    // Debe separarse por ';' para que Excel abra el fichero csv separando las
    // columnas directamente:
    //    "fecha;hora;carb;carb_R;lip;lip_R;prot;prot_R;kcal;peso"

    String comidaValues = comidaActual.getComidaAllValues(); // carb;carb_R;lip;lip_R;prot;prot_R;kcal
    String peso = String(comidaActual.getPesoComida());

    String dataString = String(rtc.getDateStr()) + ";" + String(rtc.getTimeStr()) + ";" + comidaValues + ";" + peso; 


    File myFile = SD.open(fileCSV, FILE_WRITE); // Todo se va a ir guardando en el mismo fichero ==> 'fileCSV' en Files.h
    if (myFile)
    {
        myFile.println(dataString);
        myFile.close(); 
        #if defined(SM_DEBUG)
            SerialPC.println(F("Comida guardada correctamente en el CSV"));
        #endif
        return true;
    }
    else
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("Error abriendo archivo CSV!"));
        #endif
        return false;
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el fichero CSV existente y crea uno nuevo con el mismo nombre.
 * 
 * @return true si se borra y crea el fichero correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool deleteFileCSV()
{
    // Borrar contenido borrando fichero y creándolo de nuevo no se debe hacer, 
    // especialmente si se va a repetir muchas veces, pero creo que para las 
    // pruebas no habrá problema.

    // -------- BORRAR FICHERO CSV --------------------------
    #if defined(SM_DEBUG)
        SerialPC.println(F("Borrando fichero csv..."));
    #endif
    SD.remove(fileCSV);

    if (!SD.exists(fileCSV)) 
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("Fichero CSV borrado"));
        #endif
    }
    else  
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("Error borrando fichero CSV!"));
        #endif
        return false;
    }
    // -------- FIN BORRAR FICHERO CSV ----------------------


    // -------- CREAR NUEVO FICHERO CSV ---------------------
    // Creo uno nuevo con el mismo nombre (writeHeader())
    #if defined(SM_DEBUG)
        SerialPC.println(F("\nCreando fichero de nuevo.."));
    #endif
    writeHeaderFileCSV();      // Crear fichero de nuevo e incluir el header
    getAcumuladoHoyFromSD();  // Actualizar acumulado (ahora debe ser 0)
    return true;
    // -------- FIN CREAR NUEVO FICHERO CSV -----------------
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda la lista de comida en la base de datos o en un archivo TXT dependiendo de la conexión a internet.
 * 
 * Esta función primero cierra la lista de comida actual y luego verifica si hay conexión a internet. Si hay conexión,
 * intenta enviar la lista de comida al ESP32 para que este la suba a la base de datos. Si el ESP32 responde correctamente,
 * la lista se envía y se espera una confirmación de subida exitosa. Si la subida falla, la lista se guarda en un archivo TXT.
 * Si no hay conexión a internet desde el principio, la lista se guarda directamente en un archivo TXT.
 * 
 * @param hayConexionWifi Referencia a un booleano que indica si hay conexión WiFi.
 * @return byte Código de estado indicando el resultado de la operación:
 *         - MEAL_UPLOADED: Comida subida a la base de datos.
 *         - NO_INTERNET_CONNECTION: No hay conexión a internet, comida guardada en TXT.
 *         - HTTP_ERROR: Error HTTP al subir la información, comida guardada en TXT.
 *         - TIMEOUT: No se recibió respuesta del ESP32 en el tiempo esperado, comida guardada en TXT.
 *         - UNKNOWN_ERROR: Error desconocido, comida guardada en TXT.
 */
/*-----------------------------------------------------------------------------*/
byte saveComidaInDatabase_or_TXT(bool &hayConexionWifi)
{

    // --- CERRAR LISTA DE COMIDA -------
    listaComidaESP32.finishComida();  // Escribir FIN-COMIDA,<fecha>,<hora> en la lista
    // ----------------------------------

    // --- COMPROBAR SI HAY CONEXIÓN A INTERNET -----
    // --- HAY INTERNET ---
    if(hayConexionWifi) // Se pregunta por WiFi en actStateSaved() para saber qué mensaje poner en pantalla y se pasa a saveComida(), 
    {                   // que lo pasa a esta función saveComidaInDatabase_or_TXT(), así que no hace falta preguntar de nuevo.

        // -----  ENVIAR INFORMACION AL ESP32  ------------------        
        // Avisar al ESP32 de que le va a enviar info. El ESP32 debe autenticarse en database y responder
        byte responseFromESP32ToSavePrompt = prepareSaving();   // Envía "SAVE" al ESP32, espera su respuesta y devuelve WAITING_FOR_DATA o 
                                                                // algún valor indicando error (NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT o UNKNOWN_ERROR)

        // ----- EXITO: ESP32 EN ESPERA --------------------------
        if(responseFromESP32ToSavePrompt == WAITING_FOR_DATA) // El ESP32 se ha autenticado correctamente en la database y está esperando información
        {
            // ---- ENVIAR LISTA DE COMIDA AL ESP32 ----
            listaComidaESP32.sendListToESP32(); // Envía la lista de la comida actual línea a línea
            // -----------------------------------------

            // ----- SUBIDA A DATABASE --------------------------------
            // ---- ESPERAR RESPUESTA DEL ESP32 ---------------
            // Tras enviar las líneas de toda la comida actual, espera hasta 10 segundos a que el ESP32 responda si se ha podido subir la comida a la database
            String msgFromESP32;
            unsigned long timeout = 12000; // Tiempo de espera máximo de 12 segundos para que el esp32 responda (espera hasta 10 segundos a que el servidor responda)
                                    // No hace falta esperar más tiempo porque el ESP32 espera hasta 10 segundos a que el servidor responda y los mensajes que se pueden 
                                    // recibir "SAVED-OK", "HTTP-ERROR" o "NO-WIFI" son cortos, por lo que no debería tardar demasiado waitResponseFromESP32() en recibirlos.
            waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
            // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
            // ---- FIN ESPERAR RESPUESTA ESP32 ---------------

            // ---- ANALIZAR RESPUESTA DEL ESP32 --------------
            // --- EXITO: COMIDA SUBIDA ----------------
            if (msgFromESP32 == "SAVED-OK") // El ESP32 ha creado el JSON y ha subido la comida a la database
            {
                #if defined(SM_DEBUG)
                    SerialPC.println(F("\nLista completa enviada"));
                    SerialPC.println(F("Limpiando la lista..."));
                #endif

                // Limpiar la lista para la próxima comida
                listaComidaESP32.clearList(); 

                #if defined SM_DEBUG
                    SerialPC.println("--------------------------------------------------");
                #endif

                // PANTALLA: en la pantalla de "COMIDA GUARDADA" aparece "Subido a web" en la esquina para indicar la sincronización correcta.

                return MEAL_UPLOADED; // Comida subida a database
            } 
            // ---- FIN DE ÉXITO EN SUBIDA -------------

            // --- ERRORES -----------------------------
            else // Falló subir la comida a database
            {
                // PANTALLA: en la pantalla de "COMIDA GUARDADA" aparece el error en la esquina para indicar el fallo de sincronización.

                // --- GUARDAR LISTA EN TXT -------
                #if defined SM_DEBUG
                    SerialPC.println("\nGuardando la lista en el TXT hasta que el ESP32 pueda subir la info...");
                #endif

                // Copiar lista en TXT y limpiarla
                saveMealListInTXT();

                #if defined SM_DEBUG
                    SerialPC.println("--------------------------------------------------");
                #endif
                // --------------------------------

                // Según el error ocurrido al subir la info a database, se devuelve un valor:

                if(msgFromESP32 == "NO-WIFI") // Se ha perdido la conexion WiFi
                {
                    #if defined SM_DEBUG
                        SerialPC.println(F("Se ha perdido la conexión WiFi..."));
                    #endif

                    return NO_INTERNET_CONNECTION; // Solo CSV. No hay WiFi
                }
                else if(msgFromESP32.startsWith("HTTP-ERROR")) // Error HTTP al subir la info al ESP32
                {
                    #if defined SM_DEBUG
                        SerialPC.println(F("Error HTTP al subir la comida a database..."));
                    #endif

                    return HTTP_ERROR; // Solo CSV. Error HTTP
                }
                else if(msgFromESP32 == "TIMEOUT") // No se recibió nada en 'timeout' segundos en waitResponseFromESP32()
                {
                    #if defined SM_DEBUG
                        SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
                    #endif
                    
                    return TIMEOUT; // Solo CSV. Timeout
                } 
                else 
                {
                    #if defined(SM_DEBUG)
                        SerialPC.println("Error desconocido al subir la comida a database...\n");
                    #endif

                    return UNKNOWN_ERROR; // Solo CSV. Error desconocido
                }
            }
            // ---- FIN DE ERRORES EN SUBIDA -----------
            // ---- FIN DE ANALIZAR RESPUESTA DEL ESP32 -------
            // ---- FIN DE SUBIDA A DATABASE ---------------------------

            // ---- INDICAR FIN DE INFO -----------------------
            // Tras enviar la comida, se envía al ESP32 un mensaje de fin de transmisión
            #if defined SM_DEBUG
                SerialPC.println(F("Indicando al ESP32 que terminó la transmisión..."));
            #endif
            sendMsgToESP32(F("FIN-TRANSMISION"));
            // ------------------------------------------------
        }
        // ------- FIN DE EXITO: ESP32 EN ESPERA ------------------

        // ---- ERROR: ESP32 NO RESPONDE O FALLA AUTENTICACIÓN ----
        else // Al avisar del guardado, ha fallado la comunicación con el ESP32 o la autenticación en la base de datos, previa a subir información
        {
            // Entra aquí si recibe EL ESP32 NO RESPONDE o FALLA AUTENTICACIÓN y responde con:
            //      NO_INTERNET_CONNECTION --> Perdió la conexión WiFi
            //      HTTP_ERROR --> Error al autenticar
            //      TIMEOUT --> ESP32 no responde
            //      UNKNOWN_ERROR
            #if defined SM_DEBUG
                SerialPC.println("\nEl ESP32 no respondio WAITING-FOR-DATA cuando se quiso subir lista comida actual!");
                SerialPC.println("Guardando la lista en el TXT hasta que el ESP32 pueda subir la info...");
            #endif

            // --- GUARDAR LISTA EN TXT -------
            saveMealListInTXT(); // Copiar lista en TXT y limpiarla
            // --------------------------------
            
            // Se devuelve el mismo valor de 'responseFromESP32ToSavePrompt', que será valor de error (NO_INTERNET_CONNECTION, HTTP_ERROR, TIMEOUT o UNKNOWN_ERROR)
            return responseFromESP32ToSavePrompt; // Solo guardado en CSV
        }
        // ------------------------------------------------------
    }
    // --- FIN HAY INTERNET --

    // --- NO HAY INTERNET ---
    else // Si no hay WiFi, se guarda la lista en el TXT y se limpia la lista para la próxima comida
    {
        #if defined SM_DEBUG
            SerialPC.println("\nGuardando la lista en el TXT hasta que el ESP32 pueda subir la info...");
        #endif

        // --- GUARDAR LISTA EN TXT -------
        saveMealListInTXT(); // Copiar lista en TXT y limpiarla
        // --------------------------------

        #if defined SM_DEBUG
            SerialPC.println("--------------------------------------------------");
        #endif

        return NO_INTERNET_CONNECTION;
    }
    // --- FIN NO HAY INTERNET ---
    // ---- FIN CHEQUEO INTERNET --------------------

}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda la lista de comida en el archivo TXT que se enviará al ESP32
 */
/*-----------------------------------------------------------------------------*/
void saveMealListInTXT()
{
    File myFile = SD.open(fileTXT, FILE_WRITE);

    if (myFile) 
    {
        // Escribe cada línea en el archivo
        for (byte i = 0; i < listaComidaESP32.getListSize(); i++) 
            myFile.println(listaComidaESP32.getItem(i));
        
        // Cierra el archivo
        myFile.close();

        // Limpiar la lista para la próxima comida
        listaComidaESP32.clearList();       

        #if defined(SM_DEBUG)
        SerialPC.println(F("Comida guardada correctamente en fichero TXT"));
        #endif
    } 
    else 
    {
        // Si el archivo no se abre, imprime un error:
        #if defined(SM_DEBUG)
        SerialPC.println(F("Error abriendo archivo TXT!"));
        #endif
    }
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si el archivo TXT del ESP32 está vacío.
 * 
 * @return true si el archivo está vacío, false si tiene contenido.
 */
/*-----------------------------------------------------------------------------*/
bool isFileTXTEmpty() 
{
    #if defined(SM_DEBUG)
        SerialPC.println("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++");
        SerialPC.println(F("Comprobando contenido del fichero TXT del ESP32..."));
    #endif

    File myFile = SD.open(fileTXT, FILE_READ);
    if (myFile && myFile.size() > 0)  // Si el archivo se puede abrir y su tamaño es mayor que 0, tiene contenido (isFileTXTEmpty() devuelve false)
    {
        myFile.close();
        return false;
    } 
    else  // Si no se puede abrir el archivo o su tamaño es 0, está vacío (isFileTXTEmpty() devuelve true)
    {
        #if defined(SM_DEBUG)
            if (!myFile) {
                SerialPC.println(F("Error abriendo fichero TXT! Puede que se borrara, asumimos vacio"));
                SerialPC.println("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
            }
        #endif
        if (myFile) {
            myFile.close();
        }
        return true;
    }
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Lee un archivo de texto del ESP32 y muestra su contenido en el monitor serie.
 */
/*-----------------------------------------------------------------------------*/
#if defined(SM_DEBUG)
void readFileTXT()
{
    SerialPC.println(F("\n\nLeyendo fichero TXT del ESP32...\n"));
    File myFile = SD.open(fileTXT, FILE_READ);
    if (myFile)
    {
        while (myFile.available()) 
        {
            SerialPC.write(myFile.read());
        }
        myFile.close();
    }
    else
    {
        SerialPC.println(F("Error abriendo fichero TXT!"));
    }
}
#endif



/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra un fichero en el ESP32.
 * 
 * Esta función borra un fichero en el ESP32 utilizando la tarjeta SD.
 * 
 * @return true si el fichero se borra correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool deleteFileTXT()
{
    // -------- BORRAR FICHERO ESP32 ------------------------
    if (SD.exists(fileTXT))
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("Borrando fichero TXT (ESP32)..."));
        #endif
        SD.remove(fileTXT);

        if (!SD.exists(fileTXT)) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Fichero TXT (ESP32) borrado"));
            #endif
            return true;
        }
        else  
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Error borrando fichero ESP32!"));
            #endif
            return false;
        }
    }
    else
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nNo existe el fichero TXT (ESP32)"));
        #endif
        return true;
    }
    // ------------------------------------------------------

    // En este caso no hace falta crearlo aquí, como sí ocurría con el CSV, porque
    // cuando se vaya a escribir algo al guardar comida, ya se creará.
}








// **********************************************************************************************************
// ************************ FICHERO TXT AUXILIAR PARA COMIDAS NO GUARDADAS **********************************
// **********************************************************************************************************

/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un archivo de comidas al ESP32 de forma progresiva, enviando cada línea por separado.
 * 
 *          Si una comida no se puede subir a la base de datos, se guarda en un archivo auxiliar.
 *          Al final se sustituye el TXT original por el auxiliar con las comidas que aún no se han
 *          podido subir. Se utiliza un fichero auxiliar en lugar del vector de String para evitar
 *          desbordamiento de la memoria RAM, aunque el uso inteso de la SD aumente la latencia del programa.
 */
/*-----------------------------------------------------------------------------*/
byte sendTXTFileToESP32ToUpdateWeb()
{
    #if defined SM_DEBUG
        SerialPC.println(F("\nEnviando TXT al esp32..."));
    #endif

    std::vector<String> actualMeal;   // Vector que guarda los datos de la comida actual leída del fichero TXT

    File originalFile = SD.open(fileTXT);   // Fichero original con las comidas a subir a la base de datos

    File auxFile;                       // Fichero TXT auxiliar donde guardar las comidas que no se han podido subir a la base de datos
    bool auxFileCreated = false;        // Flag para saber si se ha creado el fichero TXT auxiliar de comidas sin guardar

    if (originalFile) 
    {
        // Lee el archivo línea por línea y envía cada línea al ESP32
        while (originalFile.available()) 
        {
            // ----- LEER LÍNEA DEL TXT -------------
            String line = originalFile.readStringUntil('\n');
            line.trim();
            /*#if defined(SM_DEBUG)
                SerialPC.print("Linea del TXT: "); SerialPC.println(line);
            #endif*/
            // -------------------------------------

            // ----- AÑADIR A actualMeal -----------
            // Añade la línea al vector de la comida actual 
            // (si la línea es INICIO-COMIDA, el vector debe estar vacío antes de insertarla)
            addLineToActualMeal(actualMeal, line);
            // -------------------------------------

            // ----- ENVIAR A ESP32 ----------------
            // Envía la línea al ESP32 a través de Serial
            sendMsgToESP32(line);
            // -------------------------------------

            // ----- SUBIDA A DATABASE ------------------------
            // Si se ha llegado al final de la comida, se espera si se ha subido correctamente
            if (line.startsWith("FIN-COMIDA")) 
            {
                // ---- ESPERAR RESPUESTA DEL ESP32 -----
                String msgFromESP32 = "";
                unsigned long timeout = 12000; // Tiempo de espera máximo de 12 segundos para que el esp32 responda (tiene 10 segundos para subir info)
                                    // No hace falta esperar más tiempo porque el ESP32 espera hasta 10 segundos a que el servidor responda y los mensajes que se pueden 
                                    // recibir "SAVED-OK", "HTTP-ERROR" o "NO-WIFI" son cortos, por lo que no debería tardar demasiado waitResponseFromESP32() en recibirlos.
                waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
                // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
                // --------------------------------------

                // ---- ANALIZAR RESPUESTA DEL ESP32 ----
                // --- EXITO: COMIDA SUBIDA ------
                if(msgFromESP32 == "SAVED-OK")
                {
                    // Si se recibió SAVED-OK, no se añade a unsavedMeals 
                    #if defined(SM_DEBUG)
                        SerialPC.println("Comida guardada correctamente\n");
                    #endif

                    // Se resetea la comida actual (final del if)
                    // No se hace return porque se sigue con el bucle de lectura de otras comidas del fichero TXT
                    // Al terminar el fichero, se comprueba si se han podido subir las comidas y entonces se devuelve un resultado
                }
                // -------------------------------
                // --- ERRORES -------------------
                else
                {
                     // -- AÑADIR A unsavedMeals --
                    // Si no hay conexión WiFi o ha habido un error en la petición HTTP, se añade la comida actual al TXT auxiliar de comidas no subidas
                    if(!auxFileCreated) // Crear fichero TXT auxiliar la primera vez que falla una subida de comida
                    {
                        auxFile = SD.open(auxFileTXT, FILE_WRITE);  // Crear fichero TXT auxiliar
                        auxFileCreated = true;                      // Fichero auxiliar creado
                    }
                    saveMealToAuxFile(actualMeal, auxFile); // Escribir líneas de la comida no subida (actualMeal) en el TXT auxiliar
                    // ---------------------------

                    // No se hace return porque se sigue con el bucle de lectura de otras comidas del fichero TXT
                    // Al terminar el fichero, se comprueba si se han podido subir las comidas y entonces se devuelve un resultado
                    
                    #if defined SM_DEBUG
                        if(msgFromESP32 == "NO-WIFI")                   SerialPC.println(F("Se ha perdido la conexión WiFi al subir una comida en la actualización de SM..."));
                        else if(msgFromESP32.startsWith("HTTP-ERROR"))  SerialPC.println(F("Error HTTP al subir la info a database en la actualización de SM..."));
                        else if(msgFromESP32 == "TIMEOUT")              SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32 en la actualización de SM"));
                        else                                            SerialPC.println("Error desconocido al subir la comida a database en la actualización de SM...\n");
                    #endif

                    
                }
                // -------------------------------


                // ---- REINICIAR actualMeal -----------------------
                clearActualMeal(actualMeal); // Reseta el vector de la comida actual
                // ------------------------------------------------

            }
            // ------------------------------------------------

        } // Fichero 'originalFile' completo

        // ---- CERRAR FICHEROS ----------        
        originalFile.close();
        if(auxFileCreated) auxFile.close(); // Cerrar el archivo auxiliar si se ha creado
        // -------------------------------

        // --- TERMINAR ENVIO DE INFO ----
        // Tras enviar todas las comidas, se envía un mensaje de fin de transmisión
        sendMsgToESP32(F("FIN-TRANSMISION"));
        // -------------------------------

        // ------------------------------------------------------
        // Una vez enviado todo el fichero, se comprueba si ha quedado 
        // algo en el TXT auxiliar para actualizar el fichero TXT de comidas a enviar al ESP32
        // ------------------------------------------------------
        if(!auxFileCreated)  // Si no se ha creado el fichero auxiliar, significa que se han subido todas las comidas sin problema
        {
            #if defined(SM_DEBUG)
                SerialPC.println("\nINFO COMPLETA GUARDADA!");
                SerialPC.println(F("Paso a Init tras subir la info y borrar TXT...\n"));
            #endif

            // ---- BORRAR FICHERO TXT --------
            deleteFileTXT(); // Borrar fichero TXT
            // --------------------------------

            #if defined(SM_DEBUG)
                readFileTXT();   // Debe mostrar que no hay fichero
            #endif

            return ALL_MEALS_UPLOADED; // Se subieron todas las comidas
        }
        // Si no se ha subido todo (se ha creado el fichero auxiliar), se actualiza el fichero TXT (borrar y crear nuevo) con las comidas 
        // no subidas, escritas en el TXT auxiliar
        else if(hayMealsToUploadLaterTXT()) // Comprobar que realmente existe, por si fallara su creación
        {                    
            #if defined(SM_DEBUG) 
                SerialPC.println("\nNo se ha podido subir todo");
            #endif

            // ---- ACTUALIZAR FICHERO TXT ----
            updateFileTXTFromAuxFile();     // Actualizar fichero TXT con comidas sin guardar
            // --------------------------------
            
            #if defined(SM_DEBUG) 
                readAuxFileTXT();           // Mostrar el contenido del archivo auxiliar
                readFileTXT();              // Debe mostrar las comidas no subidas tras actualizarse
            #endif

            // ---- BORRAR FICHERO AUX --------
            deleteAuxFileTXT();             // Eliminar archivo auxiliar
            // --------------------------------

            return MEALS_LEFT; // Quedan comidas para subir más tarde
        }
        // ------------------------------------------------------

    }
    else 
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nError al abrir el archivo data-ESP.txt\n"));
        #endif

        return ERROR_READING_TXT; // Error al abrir el archivo data-ESP.txt
    }

}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el fichero TXT y crea un nuevo con las comidas que no se han podido subir
 *          y que se han guardado, temporalmente, en un fichero auxiliar.
 * 
 */
/*-----------------------------------------------------------------------------*/
void updateFileTXTFromAuxFile() 
{
    SerialPC.println(F("\nActualizando fichero TXT (ESP32)..."));

    // Borrar fichero
    deleteFileTXT(); // Ya comprueba si existe el TXT original antes de borrarlo

    // Abrir el archivo auxiliar en modo lectura
    File auxFile = SD.open(auxFileTXT, FILE_READ);
    if (!auxFile) {
        SerialPC.println(F("Error al abrir el archivo auxiliar para lectura"));
        return;
    }

    // Crear un nuevo archivo para el archivo original actualizado
    File originalFile = SD.open(fileTXT, FILE_WRITE);
    if (!originalFile) {
        SerialPC.println(F("Error al crear de nuevo el archivo TXT"));
        auxFile.close();
        return;
    }

    // Leer el archivo auxiliar línea por línea y escribir en el nuevo archivo
    while (auxFile.available()) 
    {
        String line = auxFile.readStringUntil('\n');
        originalFile.println(line); 
    }

    // Cerrar los archivos
    auxFile.close();
    originalFile.close();

    SerialPC.println(F("Archivo TXT actualizado con comidas no subidas"));
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el fichero auxiliar TXT que guarda las comidas cuya subida a la database ha fallado.
 * 
 * Esta función borra el fichero TXT auxiliar de la tarjeta SD.
 * 
 * @return true si el fichero se borra correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool deleteAuxFileTXT()
{
    if (SD.exists(auxFileTXT))
    {
        SerialPC.println(F("\nBorrando fichero auxiliar TXT (comidas no guardadas)..."));
        SD.remove(auxFileTXT);

        if (!SD.exists(auxFileTXT)) 
        {
            SerialPC.println(F("Fichero auxiliar TXT (comidas no guardadas) borrado"));
            return true;
        }
        else  
        {
            SerialPC.println(F("Error borrando fichero auxiliar TXT (comidas no guardadas)!"));
            return false;
        }
    }
    else{
        SerialPC.println(F("\nNo existe el auxiliar TXT (comidas no guardadas)"));
        return true;
    }

}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Lee el contenido de un archivo auxiliar TXT desde la tarjeta SD y lo imprime en el puerto serie.
 *
 * Esta función verifica si el archivo auxiliar TXT existe en la tarjeta SD. Si el archivo existe, 
 * lo abre en modo de lectura y lee su contenido, enviando los datos leídos al puerto serie. 
 * Si el archivo no puede ser abierto, se imprime un mensaje de error en el puerto serie. 
 * Si el archivo no existe, también se imprime un mensaje de error en el puerto serie.
 */
/*-----------------------------------------------------------------------------*/
void readAuxFileTXT()
{
    if (SD.exists(fileTXT)){
        Serial.println(F("\n\nLeyendo fichero auxiliar...\n"));

        File myFile = SD.open(auxFileTXT, FILE_READ);
        
        if (myFile){
            while (myFile.available()) {
                Serial.write(myFile.read());
            }
            myFile.close();
        }
        else{
            Serial.println("\nError abriendo el fichero auxiliar TXT para lectura!");
        }
    }
    else{
        Serial.println("\nNo se encuentra el fichero auxiliar TXT!\n");
    }
}






/******************************************************************************/
/******************************************************************************/



#endif
