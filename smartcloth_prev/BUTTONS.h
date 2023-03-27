#ifndef BUTTONS_H
#define BUTTONS_H

#include "ISR.h" 
#include "STATE_MACHINE.h"
#include "aux.h"
#include "grupos.h"

/* Keyboards variables */
byte iRow = 0, iCol = 0;//, keyMain = 0;
const byte countRows = 4;
const byte countColumns = 5;
//const byte countButtons = 5; /* Main keyboard */

/* Buttons */
//const byte buttonsMain[countButtons] = {24,25,22,23,26}; //23,25,22,24,26
const byte rowsPins[countRows] = {32,33,30,31};
const byte columnsPins[countColumns] = {49,50,51,52,53};

/* Buttons info */
/*String buttons[countRows][countColumns] = {{"gris1","gris2","gris3","azul1","azul2"},
                                         {"verde1","verde2","verde3","verde4","azul3"},
                                         {"amarillo1","amarillo2","amarillo3","amarillo4","azul4"},
                                         {"rojo1","rojo2","rojo3","rojo4","rojo5"}};*/
int buttons[countRows][countColumns] = {{1,2,3,4,5},
                                        {6,7,8,9,10},
                                        {11,12,13,14,15},
                                        {16,17,18,19,20}};

//String buttonsMainKeyboard[countButtons] = {"AMARILLO", "BLANCO","VERDE","ROJO","NEGRO"};
//int buttonsMainKeyboard[countButtons] = {1,2,3,4,5};


/*---------------------------------------------------------------------------------------------------------
   readButtonsMain(): Función para encontrar botón pulsado en botonera Main
----------------------------------------------------------------------------------------------------------*/
/*void readButtonsMain(){
    for (byte i = 0; i < countButtons; i++){
          if (digitalRead(buttonsMain[i])){ // Activo en HIGH 
                keyMain = i;
          }
    }
}
*/

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
        //Serial.println("\nCHEQUEANDO BOTONES");
        /*--------------------------------------------------------------*/
        /* ---------------    PULSACIONES BOTONERAS   ----------------- */
        /*--------------------------------------------------------------*/

        /* ---------------    TECLADO MAIN   ----------------- */
        
        /*if (pulsandoMain){ // Se está pulsando una tecla       
            readButtonsMain(); // Qué tecla se está pulsando 
            //Serial.print(F("TECLA PULSADA (MAIN): "));
            //Serial.println(buttonsMainKeyboard[keyMain]);
            buttonMain = buttonsMainKeyboard[keyMain];
            
            // ----- EVENTO ------- 
            switch (keyMain) {
                case 0:   eventoMain = CRUDO;           break;  // AMARILLO       Crudo -> opción predeterminada
                case 1:   eventoMain = COCINADO;        if(eventoGrande == TIPO_A) getGrupoAlimentos(buttonGrande+20);     break;  // BLANCO 
                case 2:   eventoMain = ADD_PLATO;       break;  // VERDE 
                case 3:   eventoMain = DELETE_PLATO;    break;  // ROJO 
                case 4:   eventoMain = GUARDAR;         break;  // NEGRO 
            }
            addEventToBuffer(eventoMain);
            
            pulsandoMain = false;
        }*/
        if(buttonMain != 0){
          Serial.println("\nMAIN");
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
          //Serial.println("\nGrande");
            readButtonsGrande(); // Qué tecla se está pulsando 
            //Serial.print(F("tecla pulsada (teclado grande): ")); Serial.println(buttons[iRow][iCol]);
            buttonGrande = buttons[iRow][iCol];
            
            /* ----- EVENTO ------- */ 
            /*if ((buttons[iRow][iCol] == "verde2") or (buttons[iRow][iCol] == "verde3") or (buttons[iRow][iCol] == "verde4") or 
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
            }*/
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
