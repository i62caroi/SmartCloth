/**
 * @file Buttons.h
 * @brief Este archivo contiene las definiciones de los botones y las funciones para interactuar con ellos.
 * 
 * @author Irene Casares Rodríguez
 * @date 01/12/2023
 */

/* --------- CONEXIÓN PLACAS BOTONERAS CON ARDUINO DUE -----------
  
  BOTONERA GRANDE conectada de la siguiente forma (conexiones hacia arriba
  y de iquierda a derecha):
      ----------------------------------------------------------------
      | VCC | GND | Int | C1 | C2 | C3 | C4 | C5 | F1 | F2 | F3 | F4 |
      ----------------------------------------------------------------
      | 3.3 | GND |  37 | 44 | 46 | 48 | 50 | 52 | 26 | 28 | 30 | 32 |       
      ----------------------------------------------------------------

  BOTONERA MAIN conectada de la siguiente forma (conexiones hacia arriba
  y de iquierda a derecha):
      ----------------------------------------------------------------------------
      | VCC |     B1     |    B2   |    B3    |    B4    |     B5    |  -  | GND |
      |     | (cocinado) | (crudo) | (añadir) | (borrar) | (guardar) |     |     |
      ----------------------------------------------------------------------------
      | 3.3 |   33    |     31     |    29    |    27    |     25    |  -  | GND |       
      ----------------------------------------------------------------------------

------------------------------------------------------------------
*/



#ifndef BUTTONS_H
#define BUTTONS_H


/* Keyboards variables */
byte iRow = 0, iCol = 0; ///< Variables para almacenar la fila y columna del botón pulsado
const byte countRows = 4; ///< Número de filas de botones
const byte countColumns = 5; ///< Número de columnas de botones


/* Buttons */
const byte rowsPins[countRows] = {26,28,30,32}; ///< Pines de las filas de botones
const byte columnsPins[countColumns] = {44,46,48,50,52}; ///< Pines de las columnas de botones


/* Buttons info => IDs de grupo (crudo) */
int buttons[countRows][countColumns] = {{1,2,3,4,5}, ///< Matriz de IDs de los botones
                                        {6,7,8,9,10},
                                        {11,12,13,14,15},
                                        {16,17,18,19,20}};


int  grupoAlimento = 0; ///< ID del grupo de alimento seleccionado
float pesoAlimento = 0; ///< Peso del alimento colocado (en este ejemplo es un valor aleatorio al pulsar COCINADO)



#include "ISR.h" 


/*-----------------------------------------------------------------------------
                          DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
int   checkButtonsMain();
bool  checkButtonsGrande();
void  readButtonsGrande();
/*-----------------------------------------------------------------------------*/



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para chequear los botones de Main (botonera B)
 * @return El ID del botón pulsado, o 0 por cuestión de debugging
 */
/*-----------------------------------------------------------------------------*/
int checkButtonsMain()
{
    if(buttonMain != 0){
        int aux = buttonMain;
        buttonMain = 0;
        return aux;
    }
    
    return 0;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para chequear los botones de Grande (botonera A)
 * @return Verdadero si se ha cambiado de grupo, falso en caso contrario
 */
/*-----------------------------------------------------------------------------*/
bool checkButtonsGrande()
{
    if (pulsandoGrande){ // Se está pulsando una tecla
        readButtonsGrande(); // Qué tecla se está pulsando. Establece valores de iRow e iCol para saber qué tecla es.

        int grupoPrev = grupoAlimento;
        grupoAlimento = buttons[iRow][iCol];

        if (grupoPrev != grupoAlimento){
            return true; // Cambio de grupo
        }
        else 
            return false; // No se ha cambiado de grupo

        
        pulsandoGrande = false;
        
    }

    return false;

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para leer el botón pulsado en la botonera grande
 * 
 * Esta función realiza la lectura del botón pulsado en la botonera grande
 * utilizando el método de polling. Se busca el botón activo en cada fila y columna
 * y se actualizan las variables iRow e iCol con la posición del botón pulsado.
 */
/*-----------------------------------------------------------------------------*/
void readButtonsGrande()
{
    for (byte c = 0; c < countColumns; c++){  
        pinMode(columnsPins[c], INPUT); //Para proteger eléctricamente los puertos de los botones y que no llegue 0 y 1 a la vez
    }
    
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT); 
        digitalWrite(columnsPins[c], HIGH);
        for (byte r = 0; r < countRows; r++){
            if (digitalRead(rowsPins[r]) == HIGH){ // Activo en HIGH 
                  iRow = r;
                  iCol = c;
            }
        }
        pinMode(columnsPins[c], INPUT); 
    }
    
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT);
        digitalWrite(columnsPins[c], HIGH);
    }

}





#endif
