
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
        - Al actualizar plato actual previo al nuevo plato, escribir "ALIMENTO,<grupoEscogido>,<pesoBascula>"

    STATE_deleted:
        - Borra todas las lineas finales de la lista hasta la última aparición de "INICIO-PLATO", inclusive.

    STATE_saved:
        - Al actualizar plato actual previo a guardar comida, escribir "ALIMENTO,<grupoEscogido>,<pesoBascula>"
        - Si se llega a guardar porque la comida no está vacía, escribir "FIN-COMIDA,<fecha>,<hora>"" en la lista, 
        copiar la lista en el fichero y limpiar la lista.
*/

#ifndef LISTA_H
#define LISTA_H

#include <vector>
#include <string>
#include "RTC.h"


#define SerialPC Serial


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
        
        std::vector<String> _lines; ///< Vector para almacenar las líneas de la lista.

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
        inline void deleteItemsFrom(int& fromIndex) { _lines.erase(_lines.begin() + fromIndex, _lines.end()); };


    public:
        
        /**
         * @brief Obtiene el tamaño de la lista
         * @return El tamaño de la lista
        */
        inline int getListSize() { return _lines.size(); };

        /**
         * @brief Obtiene el elemento de la lista en el índice 'index'
         * @param index El índice del elemento en la lista
         * @return El elemento en la lista según el índice
        */
        inline String getItem(int& index) { return _lines[index]; };

        /**
         * @brief Limpiar lista
        */
        inline void clearList() { _lines.clear(); };

        void iniciarComida(); ///< Inicia una comida.
        void iniciarPlato(); ///< Inicia un plato.
        void addAlimento(int grupo, float peso); ///< Añade un alimento a la lista.
        void borrarLastPlato(); ///< Borra el último plato de la lista.
        void saveComida(); ///< Guarda la comida.
        void leerLista(); ///< Lee la lista.
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
        SerialPC.println(F("\nIniciando comida..."));
        
        // Añadir cadena a la lista
        addLineToList("INICIO-COMIDA");
    }
    else{
        SerialPC.println(F("\nLa COMIDA ya ha comenzado"));
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
        SerialPC.println(F("\nIniciando plato..."));
        
        // Añadir cadena a la lista
        addLineToList("INICIO-PLATO"); 
    }
    else {
        SerialPC.println(F("\nEl PLATO ya ha comenzado"));
    }
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Añade un alimento a la lista.
 * @param grupo El grupo del alimento.
 * @param peso El peso del alimento.
 * 
 * Al escoger un nuevo grupo se guarda el alimento del grupo anterior
 * colocado en la iteración anterior. Por eso se utiliza 'grupoAnterior',
 * porque 'grupoEscogido' ya ha tomado el valor del nuevo grupo.
 *  
 * Si se está actualizando el plato antes de añadir otro o de guardar la comida,
 * en esos casos sí se usa 'grupoEscogido', porque es el que actualmente se 
 * tiene marcado.
 *      
 * También se pasa el peso de la báscula (pesoBascula) para no tener que incluir 
 * todo el Scale.h solo para eso.
 */
/*-----------------------------------------------------------------------------*/
void Lista::addAlimento(int grupo, float peso) 
{
    SerialPC.println(F("Guardando alimento y peso...\n"));
    
    // Obtener cadena "ALIMENTO,<grupo>,<peso>"
    String parGrupoPeso = "ALIMENTO," + String(grupo) + "," + String(peso);
    
    // Añadir cadena a la lista
    addLineToList(parGrupoPeso);
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
    int lastPlatoIndex = -1;
    for (int i = 0; i < getListSize(); i++) 
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
 * @brief Guarda la comida actual.
 *
 * Añade una línea "FIN-COMIDA,<fecha>,<hora>" al final de la lista para indicar 
 * el final de la comida.
 */
/*-----------------------------------------------------------------------------*/
void Lista::saveComida() 
{
    // No se comprueba si la comida está vacía (último escrito es INICIO-COMIDA),
    // porque si lo estuviera, no se llegaría a hacer el guardado.

    SerialPC.println(F("\nFinalizando y guardando comida...\n"));

    char *today = rtc.getDateStr();
    char *time = rtc.getTimeStr();

    // Obtener cadena "FIN-COMIDA,<fecha>,<hora>"
    String cadFin = "FIN-COMIDA," + String(today) + "," + String(time);
    
    // Añadir cadena a la lista
    addLineToList(cadFin);
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Imprime el contenido de la lista.
 *
 * Recorre la lista e imprime cada línea en la consola SerialPC.
 */
/*-----------------------------------------------------------------------------*/
void Lista::leerLista() 
{
    SerialPC.println(F("\n\nContenido de la Lista:\n"));

    for (int i = 0; i < getListSize(); i++) {
        SerialPC.println(getItem(i));
    }

    SerialPC.println("\n");
}





// **************************************************************************************************************************
// *****************      OBJETOS 'LISTA'      ******************************************************************************
// **************************************************************************************************************************


Lista    listaComidaESP32; // Objeto de la lista a guardar en el fichero ESP32




#endif