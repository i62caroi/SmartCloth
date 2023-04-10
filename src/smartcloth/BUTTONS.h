#ifndef BUTTONS_H
#define BUTTONS_H

#include "ISR.h" 
#include "State_Machine.h"

/* Keyboards variables */
byte iRow = 0, iCol = 0;//, keyMain = 0;
const byte countRows = 4;
const byte countColumns = 5;
//const byte countButtons = 5; /* Main keyboard */

/* Buttons */
const byte rowsPins[countRows] = {32,33,30,31};
const byte columnsPins[countColumns] = {49,50,51,52,53};

/* Buttons info */
int buttons[countRows][countColumns] = {{1,2,3,4,5},
                                        {6,7,8,9,10},
                                        {11,12,13,14,15},
                                        {16,17,18,19,20}};

//String buttonsMainKeyboard[countButtons] = {"AMARILLO", "BLANCO","VERDE","ROJO","NEGRO"};
//int buttonsMainKeyboard[countButtons] = {1,2,3,4,5};




/*---------------------------------------------------------------------------------------------------------
   readButtonsGrande(): Función para encontrar botón pulsado en botonera Grande
----------------------------------------------------------------------------------------------------------*/
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



/*---------------------------------------------------------------------------------------------------------
   checkAllButtons(): Chequeo de ISR de pulsaciones y polling de botoneras
----------------------------------------------------------------------------------------------------------*/
void checkAllButtons(){
        /*--------------------------------------------------------------*/
        /* ---------------    PULSACIONES BOTONERAS   ----------------- */
        /*--------------------------------------------------------------*/

        /* ---------------    TECLADO MAIN   ----------------- */
        if(buttonMain != 0){
            switch (buttonMain) {
                case 1:   eventoMain = CRUDO;           break;  // AMARILLO       Crudo -> opción predeterminada
                case 2:   eventoMain = COCINADO;        if(eventoGrande == TIPO_A) getGrupoAlimentos(buttonGrande+20);     break;  // BLANCO 
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
            
            /* ----- EVENTO ------- */ 
            if (((buttons[iRow][iCol] >= 7) and (buttons[iRow][iCol] <= 9)) or ((buttons[iRow][iCol] >= 16) and 
                (buttons[iRow][iCol] <= 18))){
                       eventoGrande = TIPO_A; // Grupo A (necesita crudo/cocinado)
            }
            else if(((buttons[iRow][iCol] >= 1) and (buttons[iRow][iCol] <= 6)) or ((buttons[iRow][iCol] >= 10) and 
                   (buttons[iRow][iCol] <= 15)) or (buttons[iRow][iCol] >=19)){
                       eventoGrande = TIPO_B;  // Grupo B 
            }
            addEventToBuffer(eventoGrande);

            /*----- Grupo alimentos ---- */
            getGrupoAlimentos(buttonGrande);
            
            pulsandoGrande = false;
        }

}









#endif
