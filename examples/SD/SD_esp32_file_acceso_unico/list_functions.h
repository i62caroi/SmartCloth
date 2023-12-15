#ifndef LIST_FUNCTIONS_H
#define LIST_FUNCTIONS_H


#include <vector>
#include <string>
#include "SD_functions_ESP32.h"

// Define un vector para almacenar las líneas
std::vector<String> lines;


/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
void    addLine(const String& line);
void    iniciarComida();                            // Escribir INICIO-COMIDA 
void    iniciarPlato();                             // Escribir INICIO-PLATO 
void    addAlimento();                             // Escribir ALIMENTO,<grupo>,<peso>

void    borrarLastPlato();

void    leerLista();                                // Mostrar todos los elementos de la lista
/*-----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------*/
/**
 * @brief Añade una línea (String) al vector
 * @param line Cadena a añadir al vector
 */
/*-----------------------------------------------------------------------------*/
void addLine(const String& line) 
{
    lines.push_back(line);
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicia una nueva comida.
 */
/*-----------------------------------------------------------------------------*/
void iniciarComida()
{
    // Comprobar si la lista está vacía para iniciar la comida. Si no lo está,
    // significa que la comida ya ha comenzado, por lo que no hace falta escribirlo
    // de nuevo.
    // La lista estará vacía si se acaba de encender SM o si se acaba de guardar comida.
    if (lines.empty()) {
        Serial.println(F("\nIniciando comida..."));
        // Añadir cadena a la lista
        addLine("INICIO-COMIDA");
    }
    else{
        Serial.println(F("\nLa COMIDA ya ha comenzado"));
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Inicia un nuevo plato.
 */
/*-----------------------------------------------------------------------------*/
void iniciarPlato()
{   
    // Comprobar si el último elemento no es "INICIO-PLATO"
    if (lines.back() != "INICIO-PLATO") 
    {
        Serial.println(F("\nIniciando plato..."));

        // Añadir cadena a la lista
        addLine("INICIO-PLATO");
    }
    else
    {
        Serial.println(F("\nEl PLATO ya ha comenzado"));
    }
}






/*-----------------------------------------------------------------------------*/
/**
 * @brief Añade un alimento con su peso correspondiente.
 */
/*-----------------------------------------------------------------------------*/
void addAlimento()
{
    Serial.println(F("Guardando alimento y peso...\n"));

    // Obtener cadena ALIMENTO,<grupo>,<peso>
    String cadGrupoPeso = getParGRupoPeso();

    // Añadir cadena a la lista
    addLine(cadGrupoPeso);
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Borra la información del plato actual hasta el último "INICIO-PLATO" inclusive
 */
/*-----------------------------------------------------------------------------*/
void borrarLastPlato() 
{

  // Buscar el último INICIO-PLATO en la lista
  int lastPlatoIndex = -1;
  for (int i = 0; i < lines.size(); i++) 
  {
    if (lines[i] == "INICIO-PLATO") 
      lastPlatoIndex = i;
  }

  if (lastPlatoIndex != -1) 
    // Borrar todas las líneas desde el último INICIO-PLATO
    lines.erase(lines.begin() + lastPlatoIndex, lines.end());
  
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Guardar comida con fecha y hora
 */
/*-----------------------------------------------------------------------------*/
void saveComida()
{
    // No se comprueba si la comida está vacía (último escrito es INICIO-COMIDA),
    // porque si lo estuviera, no se llegaría a hacer el guardado.

    Serial.println(F("Finalizando y guardando comida...\n"));

    char *today = rtc.getDateStr();
    char *time = rtc.getTimeStr();

    String cadFin = "FIN-COMIDA," + String(today) + "," + String(time); // FIN-COMIDA,<fecha>,<hora>

    // Añadir cadena a la lista
    addLine(cadFin);
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Muestra todos los elementos de la lista
 */
/*-----------------------------------------------------------------------------*/
void leerLista()
{
    Serial.println(F("\n\nContenido de la Lista:\n"));
    for (int i = 0; i < lines.size(); i++) {
        Serial.println(lines[i]);
    }
    Serial.println("\n");
}


#endif