#ifndef BUTTONS_H
#define BUTTONS_H

#include "ISR.h" 

/* Keyboards variables */
byte iRow = 0, iCol = 0, keyMain = 0;
const byte countRows = 4;
const byte countColumns = 5;
const byte countButtons = 5; /* Main keyboard */

/* Buttons */
const byte buttonsMain[countButtons] = {26,25,24,23,22};
//const byte rowsPins[countRows] = {36,37,38,39};
//const byte columnsPins[countColumns] = {49,50,51,52,53};
//const byte rowsPins[countRows] = {41,42,43,44}; // F1, F2, F3, F4
//const byte columnsPins[countColumns] = {36,37,38,39,40}; // C1, C2, C3, C4, C5
const byte rowsPins[countRows] = {44,45,46,47}; // F1, F2, F3, F4
const byte columnsPins[countColumns] = {39,40,41,42,43}; // C1, C2, C3, C4, C5

/* Buttons info */
String buttons[countRows][countColumns] = {{"cielo1","cielo2","cielo3","azul1","azul2"},
                                         {"verde1","verde2","verde3","verde4","azul3"},
                                         {"amarillo1","amarillo2","amarillo3","amarillo4","azul4"},
                                         {"rojo1","rojo2","rojo3","rojo4","rojo5"}};

String buttonsMainKeyboard[countButtons] = {"VERDE", "AMARILLO","ROJO","BLANCO","NEGRO"};



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




#endif
