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


    BOTON BARCODE conectada de la siguiente forma (con el botón derecho, botones 
    hacia abajo):
        -----------------
        | GND | Barcode |
        -----------------
        | GND |   51    |       
        -----------------

------------------------------------------------------------------
*/



#ifndef BUTTONS_H
#define BUTTONS_H

#include "debug.h" // SM_DEBUG --> SerialPC


//  -----   MAIN  --------------------------------------
// SmartCloth v2.1
/*const byte intPinCocinado     = 33;   
const byte intPinCrudo        = 31;    
const byte intPinAddPlato     = 29;   
const byte intPinDeletePlato  = 27;   
const byte intPinGuardar      = 25;   
*/
// SmartCloth v2.2
const byte intPinCocinado     = 26;   
const byte intPinCrudo        = 27;    
const byte intPinAddPlato     = 28; 
const byte intPinGuardar      = 29;  
const byte intPinDeletePlato  = 30;   
// ------------------------------------------------------


//  -----   GRANDE  -------------------------------------
/* Estructura que representa la posición de un botón. */
struct ButtonPosition {
    byte row;
    byte col;
};

/* Keyboards variables */
const byte countRows = 4;
const byte countColumns = 5;

/* Buttons */
// SmartCloth v2.1
//const byte rowsPins[countRows] = {26,28,30,32}; // F1, F2, F3, F4
//const byte columnsPins[countColumns] = {44,46,48,50,52}; // C1, C2, C3, C4, C5
// SmartCloth v2.2
const byte rowsPins[countRows] = {44,45,46,47}; // F1, F2, F3, F4
const byte columnsPins[countColumns] = {39,40,41,42,43}; // C1, C2, C3, C4, C5


/* Buttons info => IDs de grupo (crudo) */
const byte buttons[countRows][countColumns] = {{1,2,3,4,5},
                                              {6,7,8,9,10},
                                              {11,12,13,14,15},
                                              {16,17,18,19,20}};


byte  buttonGrande = 0;                  //Botón pulsado en la botonera grande (checkAllButtons())
// ------------------------------------------------------


// Debajo de la declaración de buttonGrande para que esté en el ámbito de State_Machine.h 
#include "ISR.h" 
#include "State_Machine.h"  




/*-----------------------------------------------------------------------------
                          DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
ButtonPosition  readButtonsGrande();     // Polling de botonera grande tras saltar interrupción de pulsación
void            checkMainButton();
void            checkGrandeButton();
void            checkBarcodeButton();
void            checkAllButtons();       // Asignación de eventos según botón pulsado en Grande o Main
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para leer el botón pulsado en la botonera grande
 * 
 * Esta función realiza la lectura del botón pulsado en la botonera grande
 * utilizando el método de polling. Se busca el botón activo en cada fila y columna
 * y se devuelve la posición del botón pulsado.
 * 
 * @return ButtonPosition - Posición del botón pulsado en el teclado matricial
 */
/*-----------------------------------------------------------------------------*/
ButtonPosition readButtonsGrande(){
    ButtonPosition position;

    for (byte c = 0; c < countColumns; c++){  
        pinMode(columnsPins[c], INPUT); //Para proteger eléctricamente los puertos de los botones y que no llegue 0 y 1 a la vez
    }
    
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT); 
        digitalWrite(columnsPins[c], HIGH);
        for (byte r = 0; r < countRows; r++){
            if (digitalRead(rowsPins[r]) == HIGH){ // Activo en HIGH 
                position.row = r;
                position.col = c;
            }
        }
        pinMode(columnsPins[c], INPUT); 
    }
    
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT);
        digitalWrite(columnsPins[c], HIGH);
    }

    return position;
}



void checkMainButton(){
    //
    // Si el grupo de alimentos seleccionado es de TIPO_A, necesita diferenciar entre crudo o cocinado porque sus valores
    // nutricionales son diferentes según el caso. Para los de TIPO_B no importa si los cocinas o no. 
    //
    // Los IDs de los grupos van del 1 al 20 para los crudos y después van los de cocinado. Por ejemplo, el grupo 7 (Verduras y Hortalizas)
    // es crudo, mientras que su correspondiente cocinado es el grupo 27. Por eso, si es un grupo de TIPO_A y se marca COCINADO, se cocinado se hace (buttonGrande+20),
    // para acceder a los valores de ese grupo pero cocinado.
    //
    // Los grupos de TIPO_A son: 7 (27), 8 (28), 9 (29), 16 (36), 17 (37) y 18 (38)
    // Los grupos de TIPO_B son: 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14, 15, 19 y 20
    //
    // El usuario puede escoger crudo o cocinado independientemente del grupo seleccionado y así se mostrará en pantalla, pero solo afectará
    // a los valores si el grupo es de TIPO_A. Esto se hace porque el usuario no tiene por qué saber para qué grupos cambian los valores según
    // si el alimento está crudo o cocinado.
    //

    if(buttonMain != 0){
        switch (buttonMain) {
            case 1:   eventoMain = CRUDO;           if(eventoGrande == TIPO_A) setGrupoAlimentos(buttonGrande);        break;  // Crudo -> opción predeterminada
            case 2:   eventoMain = COCINADO;        if(eventoGrande == TIPO_A) setGrupoAlimentos(buttonGrande+20);     break;  
            case 3:   eventoMain = ADD_PLATO;       break;  
            case 4:   eventoMain = DELETE_PLATO;    break;  
            case 5:   eventoMain = GUARDAR;         break;  
        }
        
        addEventToBuffer(eventoMain);
        flagEvent = true;

        buttonMain = 0;
    }

}

void checkGrandeButton(){
    if (pulsandoGrande){ // Se está pulsando una tecla
        ButtonPosition position = readButtonsGrande(); // Posición del botón pulsado
        buttonGrande = buttons[position.row][position.col];
        
        #if defined(SM_DEBUG)
        SerialPC.print(F("Grupo: ")); SerialPC.println(buttonGrande); 
        #endif 
        
        /* ----- EVENTO ------- */ 
        if (((buttons[position.row][position.col] >= 7) and (buttons[position.row][position.col] <= 9)) or ((buttons[position.row][position.col] >= 16) and 
            (buttons[position.row][position.col] <= 18))){
                    eventoGrande = TIPO_A; // Grupo A (necesita crudo/cocinado)
        }
        else if(((buttons[position.row][position.col] >= 1) and (buttons[position.row][position.col] <= 6)) or ((buttons[position.row][position.col] >= 10) and 
                (buttons[position.row][position.col] <= 15)) or (buttons[position.row][position.col] >=19)){
                    eventoGrande = TIPO_B;  // Grupo B (no necesita crudo/cocinado pero se permite "escoger" de forma ficticia)  
        }

        addEventToBuffer(eventoGrande);
        flagEvent = true;

        /*----- Grupo alimentos ---- */
        setGrupoAlimentos(buttonGrande);
        
        pulsandoGrande = false;
    }
        
}

void checkBarcodeButton(){
    if (pulsandoBarcode){ // Se está pulsando una tecla
        /*eventoGrande = BARCODE;
        addEventToBuffer(eventoGrande);
        flagEvent = true;

        setGrupoAlimentos(50); // Grupo de barcode siempre
        */
        
        pulsandoBarcode = false;
    }

}


/*void checkAllButtons(){
    checkMainButton();
    checkGrandeButton();
    checkBarcodeButton();
}*/



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
        //
        // Si el grupo de alimentos seleccionado es de TIPO_A, necesita diferenciar entre crudo o cocinado porque sus valores
        // nutricionales son diferentes según el caso. Para los de TIPO_B no importa si los cocinas o no. 
        //
        // Los IDs de los grupos van del 1 al 20 para los crudos y después van los de cocinado. Por ejemplo, el grupo 7 (Verduras y Hortalizas)
        // es crudo, mientras que su correspondiente cocinado es el grupo 27. Por eso, si es un grupo de TIPO_A y se marca COCINADO, se cocinado se hace (buttonGrande+20),
        // para acceder a los valores de ese grupo pero cocinado.
        //
        // Los grupos de TIPO_A son: 7 (27), 8 (28), 9 (29), 16 (36), 17 (37) y 18 (38)
        // Los grupos de TIPO_B son: 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14, 15, 19 y 20
        //
        // El usuario puede escoger crudo o cocinado independientemente del grupo seleccionado y así se mostrará en pantalla, pero solo afectará
        // a los valores si el grupo es de TIPO_A. Esto se hace porque el usuario no tiene por qué saber para qué grupos cambian los valores según
        // si el alimento está crudo o cocinado.
        //

        if(buttonMain != 0){
            switch (buttonMain) {
                case 1:   eventoMain = CRUDO;           if(eventoGrande == TIPO_A) setGrupoAlimentos(buttonGrande);        break;  // Crudo -> opción predeterminada
                case 2:   eventoMain = COCINADO;        if(eventoGrande == TIPO_A) setGrupoAlimentos(buttonGrande+20);     break; 
                case 3:   eventoMain = ADD_PLATO;       break;  
                case 4:   eventoMain = DELETE_PLATO;    break; 
                case 5:   eventoMain = GUARDAR;         break; 
            }
            
            addEventToBuffer(eventoMain);
            flagEvent = true;

            buttonMain = 0;
        }
        // ---------------------------------------------------------


        /* ---------------    TECLADO GRANDE   ----------------- */
        if (pulsandoGrande){ // Se está pulsando una tecla
            ButtonPosition position = readButtonsGrande(); // Posición del botón pulsado
            buttonGrande = buttons[position.row][position.col];
            
            #if defined(SM_DEBUG)
            SerialPC.print(F("\nGrupo: ")); SerialPC.println(buttonGrande); 
            #endif 
            
            /* ----- EVENTO ------- */ 
            if (((buttons[position.row][position.col] >= 7) and (buttons[position.row][position.col] <= 9)) or ((buttons[position.row][position.col] >= 16) and 
                (buttons[position.row][position.col] <= 18))){
                       eventoGrande = TIPO_A; // Grupo A (necesita crudo/cocinado)
            }
            else if(((buttons[position.row][position.col] >= 1) and (buttons[position.row][position.col] <= 6)) or ((buttons[position.row][position.col] >= 10) and 
                   (buttons[position.row][position.col] <= 15)) or (buttons[position.row][position.col] >=19)){
                       eventoGrande = TIPO_B;  // Grupo B (no necesita crudo/cocinado pero se permite "escoger" de forma ficticia)  
            }

            addEventToBuffer(eventoGrande);
            flagEvent = true;

            /*----- Grupo alimentos ---- */
            setGrupoAlimentos(buttonGrande);
            
            pulsandoGrande = false;
        }
        // ---------------------------------------------------------


        /* ---------------    BARCODE   -------------------------- */
        if (pulsandoBarcode){ // Se está pulsando una tecla
            /*eventoGrande = BARCODE;
            addEventToBuffer(eventoGrande);
            flagEvent = true;

            setGrupoAlimentos(50); // Grupo de barcode siempre
            */

            pulsandoBarcode = false;
        }
        // ---------------------------------------------------------



}









#endif
