
/** 
 * @file lista_Comida.h
 * @brief Lista de proceso de la comida
 *
 * @author Irene Casares Rodríguez
 * @date 05/12/23
 * @version 1.0
 *
 *  Este archivo contiene las funciones para manejar una lista de cadenas que representan las
 *  líneas a escribir en el fichero para el ESP32.
 *  Para cada comida, se van a ir guardando en una lista (vector de String) las cadenas a escribir 
 *  en el fichero cuya información se enviará al esp32 cuando haya wifi para que conforme un JSON y suba
 *  la información a la base de datos.
 *  Estas cadenas (líneas) solo se escribirán en el fichero txt al guardar la comida. 
 *  
 */

/* 
   Guardar las cadenas en una lista en lugar de escribirlas directamente en el fichero agiliza mucho 
   el procesamiento y evita accesos reiterados a la SD al modificar el fichero (adición y borrado).

  STATE_Init:
        - Si la lista no está vacía, escribe "INICIO-COMIDA". Solo estará vacía al inicio y tras guardar comida.
        - Si se ha retirado plato de repente, se borra el último plato (como en STATE_deleted)

    STATE_Plato:
        - Si lo último escrito no es "INICIO-PLATO", se escribe. Se comprueba para no volver a escribirlo mientras se coloca el plato.

    STATE_groupA/B:
        - Escribir "ALIMENTO,<grupoAnterior>,<pesoBascula>" al incluir alimento anterior en plato

    STATE_added:
        - Al actualizar plato actual previo al nuevo plato, escribir "ALIMENTO,<grupoActual>,<pesoBascula>"

    STATE_deleted:
        - Borra todas las lineas finales de la lista hasta la última aparición de "INICIO-PLATO", inclusive.

    STATE_saved:
        - Al actualizar plato actual previo a guardar comida, escribir "ALIMENTO,<grupoActual>,<pesoBascula>"
        - Si se llega a guardar porque la comida no está vacía, escribir "FIN-COMIDA,<fecha>,<hora>"" en la lista, 
        copiar la lista en el fichero y limpiar la lista.
*/

#ifndef LISTA_H
#define LISTA_H

#include <vector>
#include <string>
#include "RTC.h"


#include "debug.h" // SM_DEBUG --> SerialPC
#include "Serial_functions.h" // SerialESP32 y resultados de subir a database (WAITING_FOR_DATA, UPLOADING_DATA, MEAL_UPLOADED, MEALS_LEFT, ERROR_READING_TXT, NO_INTERNET_CONECTION, HTTP_ERROR, TIMEOUT, UNKNOWN_ERROR)


void waitResponseFromESP32(String &msgFromESP32, unsigned long &timeout);



// **************************************************************************************************************************
// *****************      DECLARACIÓN CLASE 'LISTA'       *******************************************************************
// **************************************************************************************************************************




/**
 * @class Lista
 * @brief Clase para manejar una lista de comidas y platos.
 */
class Lista 
{
    private:
        
        std::vector<String> _lines; // Vector para almacenar las líneas de la lista.

        /**
         * @brief Obtiene el último elemento de la lista.
         * @return El último elemento de la lista.
         */
        inline String getLastItem() { return _lines.back(); };

        /**
         * @brief Comprueba si la lista está vacía.
         * @return Verdadero si la lista está vacía, falso en caso contrario.
         */
        inline bool isListEmpty() { return _lines.empty(); };

        /**
         * @brief Añade una línea a la lista.
         * @param line La línea a añadir.
         */
        inline void addLineToList(const String& line) { _lines.push_back(line); };

        /**
         * @brief Borra elementos de la lista desde el índice 'fromIndex' hasta el final de la lista
         * @param fromIndex Índice de la lista desde donde borrar elementos
        */
        inline void deleteItemsFrom(byte& fromIndex) { _lines.erase(_lines.begin() + fromIndex, _lines.end()); };


    public:
        
        /**
         * @brief Obtiene el tamaño de la lista
         * @return El tamaño de la lista
        */
        inline byte getListSize() { return _lines.size(); };

        /**
         * @brief Obtiene el elemento de la lista en el índice 'index'
         * @param index El índice del elemento en la lista
         * @return El elemento en la lista según el índice
        */
        inline String getItem(byte& index) { return _lines[index]; };

        /**
         * @brief Limpiar lista
        */
        inline void clearList() { _lines.clear(); };


        void iniciarComida();                                               // Inicia una comida.
        void iniciarPlato();                                                // Inicia un plato.
        void addAlimento(byte grupo, float peso);                           // Añade un alimento a la lista.
        void addAlimentoBarcode(byte grupo, float peso, String barcode);    // Añade un alimento de tipo barcode a la lista.
        void borrarLastPlato();                                             // Borra el último plato de la lista.
        void finishComida();                                                // Finaliza la comida añadiendo la fecha y hora
        void sendListToESP32();                                             // Envía la lista elemento a elemento al ESP32 por Serial


        #if defined(SM_DEBUG)
            void leerLista(); // Lee la lista.
        #endif //SM_DEBUG

};




/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicia una comida nueva.
 */
/*-----------------------------------------------------------------------------*/
void Lista::iniciarComida() 
{
    // Comprobar si la lista está vacía para iniciar la comida. Si no lo está,
    // significa que la comida ya ha comenzado, por lo que no hace falta escribirlo
    // de nuevo.

    if (isListEmpty()) {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nIniciando comida..."));
        #endif 
        
        // Añadir cadena a la lista
        addLineToList("INICIO-COMIDA");
    }
    else{
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nLa COMIDA ya ha comenzado"));
        #endif 
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicia un plato nuevo.
 */
/*-----------------------------------------------------------------------------*/
void Lista::iniciarPlato() 
{
    // Comprobar si el último elemento no es "INICIO-PLATO"   
    if (getLastItem() != "INICIO-PLATO") {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nIniciando plato..."));
        #endif
        
        // Añadir cadena a la lista
        addLineToList("INICIO-PLATO"); 
    }
    else {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nEl PLATO ya ha comenzado"));
        #endif
    }
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Añade un alimento a la lista.
 * 
 * Al escoger un nuevo grupo se guarda el alimento del grupo anterior
 * colocado en la báscula en la iteración anterior. Por eso se utiliza 'grupoAnterior',
 * porque 'grupoActual' ya ha tomado el valor del nuevo grupo.
 *  
 * Si se está actualizando el plato antes de añadir otro o de guardar la comida,
 * en esos casos sí se usa 'grupoActual', porque es el que actualmente se 
 * tiene marcado.
 *      
 * También se pasa el peso de la báscula (pesoBascula) para no tener que incluir 
 * todo el Scale.h solo para eso.
 * 
 * @param grupo El grupo del alimento (grupoAnterior).
 * @param peso El peso del alimento (pesoBascula).
 * 
 */
/*-----------------------------------------------------------------------------*/
void Lista::addAlimento(byte grupo, float peso) 
{
    #if defined(SM_DEBUG)
        SerialPC.println(F("Guardando alimento y peso en lista...\n"));
    #endif
    
    // Obtener cadena "ALIMENTO,<grupo>,<peso>"
    String cad = "ALIMENTO," + String(grupo) + "," + String(peso);
    
    // Añadir cadena a la lista
    addLineToList(cad);
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Agrega un alimento con código de barras a la lista.
 * 
 * Esta función agrega un alimento a la lista utilizando su código de barras.
 * El alimento se identifica por su grupo, peso y código de barras.
 * 
 * Al escoger un nuevo grupo se guarda el alimento del grupo anterior
 * colocado en la báscula en la iteración anterior. Por eso se utiliza 'grupoAnterior',
 * porque 'grupoActual' ya ha tomado el valor del nuevo grupo.
 *  
 * Si se está actualizando el plato antes de añadir otro o de guardar la comida,
 * en esos casos sí se usa 'grupoActual', porque es el que actualmente se 
 * tiene marcado.
 *      
 * También se pasa el peso de la báscula (pesoBascula) para no tener que incluir 
 * todo el Scale.h solo para eso.
 * 
 * @param grupo El grupo al que pertenece el alimento (grupoAnterior).
 * @param peso El peso del alimento (pesoBascula).
 * @param barcode El código de barras del alimento (barcode).
 * 
 */
/*-----------------------------------------------------------------------------*/
void Lista::addAlimentoBarcode(byte grupo, float peso, String barcode) 
{
    #if defined(SM_DEBUG)
        SerialPC.println(F("Guardando alimento tipo barcode con peso e EAN en lista...\n"));
    #endif
    
    // Obtener cadena "ALIMENTO,<grupo>,<peso>,<ean>"
    String cad = "ALIMENTO," + String(grupo) + "," + String(peso) + "," + barcode;
    
    // Añadir cadena a la lista
    addLineToList(cad);
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra el último plato de la lista.
 *
 * Busca el índice del último "INICIO-PLATO" en la lista y borra todos los 
 * elementos desde ese índice hasta el final de la lista.
 */
/*-----------------------------------------------------------------------------*/
void Lista::borrarLastPlato() 
{
    // Buscar el último INICIO-PLATO en la lista
    byte lastPlatoIndex = -1;
    for (byte i = 0; i < getListSize(); i++) 
    {
        if (getItem(i) == "INICIO-PLATO"){
            lastPlatoIndex = i;
        }
    }

    // Borrar todas las líneas desde el último INICIO-PLATO
    if (lastPlatoIndex != -1){
        deleteItemsFrom(lastPlatoIndex);
    }
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Finaliza la comida actual.
 *
 * Añade una línea "FIN-COMIDA,<fecha>,<hora>" al final de la lista para indicar 
 * el final de la comida.
 */
/*-----------------------------------------------------------------------------*/
void Lista::finishComida() 
{
    // No se comprueba si la comida está vacía (último escrito es INICIO-COMIDA),
    // porque si lo estuviera, no se llegaría a hacer el guardado.

    #if defined(SM_DEBUG)
        SerialPC.println(F("\nFinalizando comida en lista...\n"));
    #endif 

    char *today = rtc.getDateStr();
    char *time = rtc.getTimeStr();

    // Obtener cadena "FIN-COMIDA,<fecha>,<hora>"
    String cadFin = "FIN-COMIDA," + String(today) + "," + String(time);
    
    // Añadir cadena a la lista
    addLineToList(cadFin);
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Envia el contenido de la lista al ESP32.
 *
 * Recorre la lista y envía cada línea a través de SerialESP32.
 */
/*-----------------------------------------------------------------------------*/
/*byte Lista::sendListToESP32() 
{
    #if defined SM_DEBUG
        SerialPC.println(F("Enviando lista al esp32 para subir la info"));
    #endif

    for (byte i = 0; i < getListSize(); i++) {
        //SerialESP32.println(getItem(i));
        sendMsgToESP32(getItem(i));
    }


    // ----- SUBIDA A DATABASE ------------------------
    // ---- ESPERAR RESPUESTA DEL ESP32 -----
    String msgFromESP32;
    unsigned long timeout = 10000; // Tiempo de espera máximo de 10 segundos para que el esp32 responda
    waitResponseFromESP32(msgFromESP32, timeout); // Espera la respuesta del ESP32 y la devuelve en msgFromESP32
    // Cuando se recibe mensaje o se pasa el timout, entonces se comprueba la respuesta
    // --------------------------------------

    // ---- INDICAR FIN DE INFO -------------
    // Se indica después de esperar la respuesta de si se ha subido la info a database para
    // mantener un orden en la comunicación
    //SerialESP32.println(F("FIN-TRANSMISION"));
    sendMsgToESP32(F("FIN-TRANSMISION"));
    // --------------------------------------

    // ---- ANALIZAR RESPUESTA DEL ESP32 ----
    // --- EXITO: COMIDA SUBIDA ------
    if (msgFromESP32 == "SAVED-OK") // El ESP32 ha creado el JSON y subido a la database
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nLista completa enviada"));
            SerialPC.println(F("Limpiando la lista..."));
        #endif

        // Limpiar la lista para la próxima comida
        clearList(); 

        //return MEAL_UPLOADED; // Comida subida a database
        return SAVE_EXECUTED_FULL; // CSV y database
    } 
    // --------------------------------------
    // --- ERRORES -------------------
    else // Falló subir la comida a database
    {
        if(msgFromESP32 == "NO-WIFI") // Se ha perdido la conexion WiFi
        {
            #if defined SM_DEBUG
                SerialPC.println(F("Se ha perdido la conexión WiFi..."));
            #endif

            return SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI; // Solo CSV. No hay WiFi
        }
        else if(msgFromESP32.startsWith("HTTP-ERROR")) // Error HTTP al subir la info al ESP32
        {
            #if defined SM_DEBUG
                SerialPC.println(F("Error HTTP al subir la info a database..."));
            #endif

            return SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP; // Solo CSV. Error HTTP
        }
        else if(msgFromESP32 == "TIMEOUT") // No se recibió nada en 'timeout' segundos en waitResponseFromESP32()
        {
            #if defined SM_DEBUG
                SerialPC.println(F("TIMEOUT. No se ha recibido respuesta del ESP32"));
            #endif
            
            return SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT; // Solo CSV. Timeout
        } 
        else 
        {
            #if defined(SM_DEBUG)
                SerialPC.println("Error desconocido al subir la comida a database...\n");
            #endif

            return SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR; // Solo CSV. Error desconocido
        }
    }
    // -------------------------------

    // ------------------------------------------------
      
}*/

void Lista::sendListToESP32() 
{
    #if defined SM_DEBUG
        SerialPC.println(F("Enviando lista al esp32 para subir la info"));
    #endif

    for (byte i = 0; i < getListSize(); i++) {
        //SerialESP32.println(getItem(i));
        sendMsgToESP32(getItem(i));
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Imprime el contenido de la lista.
 *
 * Recorre la lista e imprime cada línea en la consola SerialPC.
 */
/*-----------------------------------------------------------------------------*/
#if defined(SM_DEBUG)
void Lista::leerLista() 
{
    SerialPC.println(F("\nContenido de la Lista:\n"));

    for (byte i = 0; i < getListSize(); i++) {
        SerialPC.println(getItem(i));
    }

    SerialPC.println("\n");
}
#endif





// **************************************************************************************************************************
// *****************      OBJETOS 'LISTA'      ******************************************************************************
// **************************************************************************************************************************


Lista    listaComidaESP32;  // Objeto de la lista a guardar en el fichero TXT que se enviará al ESP32 para actualizar la base de datos 
                            // con la información guardada localmente en SmartCloth cuando no había conexión a internet.




#endif