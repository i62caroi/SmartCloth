#ifndef LISTA_H
#define LISTA_H

#include <vector>
#include <string>
#include "RTC.h"




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
         * @brief Borra elementos de la lista desde el índice 'fromIndex'
         * @param fromIndex Índice de la lista desde donde borrar elementos hasta el final de la lista
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
        Serial.println(F("\nIniciando comida..."));
        
        // Añadir cadena a la lista
        addLineToList("INICIO-COMIDA");
    }
    else{
        Serial.println(F("\nLa COMIDA ya ha comenzado"));
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
        Serial.println(F("\nIniciando plato..."));
        
        // Añadir cadena a la lista
        addLineToList("INICIO-PLATO"); 
    }
    else {
        Serial.println(F("\nEl PLATO ya ha comenzado"));
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
    Serial.println(F("Guardando alimento y peso...\n"));
    
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

    Serial.println(F("\nFinalizando y guardando comida...\n"));

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
 * Recorre la lista e imprime cada línea en la consola serial.
 */
/*-----------------------------------------------------------------------------*/
void Lista::leerLista() 
{
    Serial.println(F("\n\nContenido de la Lista:\n"));

    for (int i = 0; i < getListSize(); i++) {
        Serial.println(getItem(i));
    }

    Serial.println("\n");
}





// **************************************************************************************************************************
// *****************      OBJETOS 'LISTA'      ******************************************************************************
// **************************************************************************************************************************


Lista    listaComidaESP32; // Objeto de la lista a guardar en el fichero ESP32




#endif