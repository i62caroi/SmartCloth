#ifndef BUTTONS_H
#define BUTTONS_H

#include "ISR.h" 
#include "STATE_MACHINE.h"
#include "aux.h"

/* Keyboards variables */
byte iRow = 0, iCol = 0, keyMain = 0;
const byte countRows = 4;
const byte countColumns = 5;
const byte countButtons = 5; /* Main keyboard */

/* Buttons */
const byte buttonsMain[countButtons] = {26,25,24,23,22};
const byte rowsPins[countRows] = {36,37,38,39};
const byte columnsPins[countColumns] = {49,50,51,52,53};

/* Buttons info */
String buttons[countRows][countColumns] = {{"gris1","gris2","gris3","azul1","azul2"},
                                         {"verde1","verde2","verde3","verde4","azul3"},
                                         {"amarillo1","amarillo2","amarillo3","amarillo4","azul4"},
                                         {"rojo1","rojo2","rojo3","rojo4","rojo5"}};

String buttonsMainKeyboard[countButtons] = {"AMARILLO", "BLANCO","VERDE","ROJO","NEGRO"};



/*---------------------------------------------------------------------------------------------------------
   readButtonsMain(): Función para encontrar botón pulsado en botonera Main
----------------------------------------------------------------------------------------------------------*/
void readButtonsMain(){
    for (byte i = 0; i < countButtons; i++){
          if (digitalRead(buttonsMain[i])){ /* Activo en HIGH */
                keyMain = i;
          }
    }
}


/*---------------------------------------------------------------------------------------------------------
   readButtonsGrande(): Función para encontrar botón pulsado en botonera Grande
----------------------------------------------------------------------------------------------------------*/
void readButtonsGrande(){
    for (byte c = 0; c < countColumns; c++){
        digitalWrite(columnsPins[c], LOW);
    }
    
    for (byte c = 0; c < countColumns; c++){
        digitalWrite(columnsPins[c], HIGH);
        for (byte r = 0; r < countRows; r++){
            if (digitalRead(rowsPins[r]) == HIGH){ /* Activo en HIGH */
                  iRow = r;
                  iCol = c;
            }
        }
        digitalWrite(columnsPins[c], LOW);
    }
    
    for (byte c = 0; c < countColumns; c++){
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
        
        if (pulsandoMain){ // Se está pulsando una tecla       
            readButtonsMain(); // Qué tecla se está pulsando 
            //Serial.print(F("TECLA PULSADA (MAIN): "));
            //Serial.println(buttonsMainKeyboard[keyMain]);
            //printButton(buttonsMainKeyboard[keyMain]);
            buttonMain = buttonsMainKeyboard[keyMain];
            
            /* ----- EVENTO ------- */ 
            switch (keyMain) {
                case 0:   eventoMain = CRUDO;           break;  // AMARILLO
                case 1:   eventoMain = COCINADO;        break;  // BLANCO 
                case 2:   eventoMain = ADD_PLATO;       break;  // VERDE 
                case 3:   eventoMain = DELETE_PLATO;    break;  // ROJO 
                case 4:   eventoMain = GUARDAR;         break;  // NEGRO 
            }
            addEventToBuffer(eventoMain);
            
            pulsandoMain = false;
        }


        /* ---------------    TECLADO GRANDE   ----------------- */
        
        if (pulsandoGrande){ // Se está pulsando una tecla
            readButtonsGrande(); // Qué tecla se está pulsando 
            //Serial.print(F("tecla pulsada (teclado grande): "));
            //Serial.println(buttons[iRow][iCol]);
            //printButton(buttons[iRow][iCol]); 
            buttonGrande = buttons[iRow][iCol];
            
            /* ----- EVENTO ------- */ 
            //
            //==> COMPROBAR GRUPOS DE ALIMENTOS!!!
            //
            if ((buttons[iRow][iCol] == "verde2") or (buttons[iRow][iCol] == "verde3") or (buttons[iRow][iCol] == "verde4") or 
                (buttons[iRow][iCol] == "rojo1") or (buttons[iRow][iCol] == "rojo2") or (buttons[iRow][iCol] == "rojo3")){
                       eventoGrande = TIPO_A; // Grupo A (necesita crudo/cocinado)
                       //Serial.println("\nEvento TIPO_A");
            }
            else if((buttons[iRow][iCol] == "gris1") or (buttons[iRow][iCol] == "gris2") or (buttons[iRow][iCol] == "gris3") or 
                   (buttons[iRow][iCol] == "azul1") or (buttons[iRow][iCol] == "azul2") or (buttons[iRow][iCol] == "azul3") or 
                   (buttons[iRow][iCol] == "azul4") or (buttons[iRow][iCol] == "verde1") or (buttons[iRow][iCol] == "amarillo1") or 
                   (buttons[iRow][iCol] == "amarillo2") or (buttons[iRow][iCol] == "amarillo3") or (buttons[iRow][iCol] == "amarillo4") or 
                   (buttons[iRow][iCol] == "rojo4") or (buttons[iRow][iCol] == "rojo5")){
                       eventoGrande = TIPO_B;  // Grupo B 
                       //Serial.println("\nEvento TIPO_B");
            }
            addEventToBuffer(eventoGrande);
            
            pulsandoGrande = false;
        }

}





#endif
