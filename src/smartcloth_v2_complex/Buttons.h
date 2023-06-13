/**
 * @file Buttons.h
 * @brief Librería para el manejo de las placas botoneras
 *
 * @author Irene Casares Rodríguez
 * @date 23/05/2023
 * @version 1.0
 * 
 * Este archivo contiene las definiciones y funciones necesarias para el manejo
 * de las placas botoneras. Se definen las conexiones de los pines, se implementa
 * la lectura de los botones y se asignan eventos según las pulsaciones.
 * 
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

#include "ISR.h" 
#include "State_Machine.h"


/* Keyboards variables */
byte iRow = 0, iCol = 0;//, keyMain = 0;
const byte countRows = 4;
const byte countColumns = 5;

/* Buttons */
const byte rowsPins[countRows] = {26,28,30,32};
const byte columnsPins[countColumns] = {44,46,48,50,52};

/* Buttons info => IDs de grupo crudo */
int buttons[countRows][countColumns] = {{1,2,3,4,5},
                                        {6,7,8,9,10},
                                        {11,12,13,14,15},
                                        {16,17,18,19,20}};

//String buttonsMainKeyboard[countButtons] = {"AMARILLO", "BLANCO","VERDE","ROJO","NEGRO"};


/*-----------------------------------------------------------------------------
                          DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void  readButtonsGrande();     // Polling de botonera grande tras saltar interrupción de pulsación
void  checkAllButtons();       // Asignación de eventos según botón pulsado en Grande o Main
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para leer el botón pulsado en la botonera grande
 * 
 * Esta función realiza la lectura del botón pulsado en la botonera grande
 * utilizando el método de polling. Se busca el botón activo en cada fila y columna
 * y se actualizan las variables iRow e iCol con la posición del botón pulsado.
 */
 /*-----------------------------------------------------------------------------*/
 void readButtonsGrande(){
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


 /*-----------------------------------------------------------------------------*/
/**
 * @brief Función para chequear los botones de ambas botoneras
 * 
 * Esta función comprueba las interrupciones de pulsaciones de los botones en
 * ambas botoneras (grande y main). Dependiendo del botón pulsado, se asigna
 * un evento correspondiente y se agrega al búfer de eventos.
 */
  /*-----------------------------------------------------------------------------*/
 void checkAllButtons(){
        /*--------------------------------------------------------------*/
        /* ---------------    PULSACIONES BOTONERAS   ----------------- */
        /*--------------------------------------------------------------*/

        /* ---------------    TECLADO MAIN   ----------------- */
        if(buttonMain != 0){
            switch (buttonMain) {
                case 1:   eventoMain = CRUDO;           if(eventoGrande == TIPO_A) setGrupoAlimentos(buttonGrande);        break;  // AMARILLO  Crudo -> opción predeterminada
                case 2:   eventoMain = COCINADO;        if(eventoGrande == TIPO_A) setGrupoAlimentos(buttonGrande+20);     break;  // BLANCO 
                case 3:   eventoMain = ADD_PLATO;       break;  // VERDE 
                case 4:   eventoMain = DELETE_PLATO;    break;  // ROJO 
                case 5:   eventoMain = GUARDAR;         break;  // NEGRO 
            }
            addEventToBuffer(eventoMain);
            
            buttonMain = 0;
        }


        /* ---------------    TECLADO GRANDE   ----------------- */
        
        if (pulsandoGrande){ // Se está pulsando una tecla
            readButtonsGrande(); // Qué tecla se está pulsando 
            buttonGrande = buttons[iRow][iCol];
            Serial.print(F("Grupo: ")); Serial.println(buttonGrande);
            /* ----- EVENTO ------- */ 
            if (((buttons[iRow][iCol] >= 7) and (buttons[iRow][iCol] <= 9)) or ((buttons[iRow][iCol] >= 16) and 
                (buttons[iRow][iCol] <= 18))){
                       eventoGrande = TIPO_A; // Grupo A (necesita crudo/cocinado)
            }
            else if(((buttons[iRow][iCol] >= 1) and (buttons[iRow][iCol] <= 6)) or ((buttons[iRow][iCol] >= 10) and 
                   (buttons[iRow][iCol] <= 15)) or (buttons[iRow][iCol] >=19)){
                       eventoGrande = TIPO_B;  // Grupo B (no necesita crudo/cocinado pero se permite "escoger" de forma ficticia)  
            }
            addEventToBuffer(eventoGrande);

            /*----- Grupo alimentos ---- */
            setGrupoAlimentos(buttonGrande);
            
            pulsandoGrande = false;
        }

}









#endif
