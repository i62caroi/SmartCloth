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
void    iniciarElemento(const String& cadInicio);   // Escribir INICIO-COMIDA o INICIO-PLATO 
void    saveAlimento();                             // Escribir ALIMENTO,<grupo>,<peso>

void    borrarLastPlato(bool writeNuevoPlato);      // Borrar información del plato actual (hasta último INICIO-PLATO)
                                                    // writeNuevoPlato --> true: escribir de nuevo INICIO-PLATO     false: no escribirlo

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
 * @brief Inicia un nuevo elemento (comida o plato).
 * @param cadInicio Cadena que indica el tipo de inicio ("INICIO-COMIDA" o "INICIO-PLATO").
 */
/*-----------------------------------------------------------------------------*/
void iniciarElemento(const String& cadInicio)
{
    if(cadInicio == "INICIO-COMIDA") Serial.println(F("\nIniciando comida..."));
    else if(cadInicio == "INICIO-PLATO") Serial.println(F("\nIniciando plato..."));

    // Añadir cadena a la lista
    addLine(cadInicio);
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Guarda un alimento con su peso correspondiente.
 */
/*-----------------------------------------------------------------------------*/
void saveAlimento()
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
 * @param writeNuevoPlato Indica si se debe escribir un nuevo "INICIO-PLATO" después de borrar.
 */
/*-----------------------------------------------------------------------------*/
void borrarLastPlato(bool writeNuevoPlato) 
{

  // Buscar el último INICIO-PLATO en la lista
  int lastPlatoIndex = -1;
  for (int i = 0; i < lines.size(); i++) 
  {
    if (lines[i] == "INICIO-PLATO") 
      lastPlatoIndex = i;
  }

  if (lastPlatoIndex != -1) {

    // Borrar todas las líneas desde el último INICIO-PLATO
    lines.erase(lines.begin() + lastPlatoIndex, lines.end());

    // Crear nuevo plato con INICIO-PLATO para que pueda continuar añadiendo alimentos 
    // sin tener que indicar explícitamente "añadir plato" tras borrar el contenido del actual.
    // SmartCloth ya hace esto: tras borrar el plato, deja listo el objeto platoActual
    // para continuar con la comida.

    // En el caso en que se borre el plato pero no se continúe con la comida, sino que
    // se guarde tal como está, antes de escribir FIN-COMIDA habría que comprobar
    // si la última línea es INICIO-PLATO en lugar de ALIMENTO, lo que significaría
    // que el último plato está vacío. Si esto ocurriera, habría que borrar esa línea
    // escrita con INICIO-PLATO (usar misma función borrarLastPlato()) antes de escribir
    // la línea de FIN-COMIDA para evitar enviar información vacía y no volverla a 
    // escribir (writeNuevoPlato = false)

    if (writeNuevoPlato) 
      addLine("INICIO-PLATO");
    
  }
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