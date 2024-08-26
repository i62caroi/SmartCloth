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

/* --------- CONEXIÓN PLACAS BOTONERAS V1.0 CON ARDUINO DUE (SmartCloth v2.1) -----------
  
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

-----------------------------------------------------------------------------------


/* --------- CONEXIÓN PLACAS BOTONERAS V2.0 CON ARDUINO DUE (SmartCloth v2.2) -----------
  
  BOTONERA GRANDE conectada de la siguiente forma (conexiones hacia derecha
  y de arriba a abajo):
      ----------------------------------------------------------------
      | F4 | F3 | F2 | F1 | C5 | C4 | C3 | C2 | C1 | INT | VCC | GND |
      ----------------------------------------------------------------
      | 47 | 46 | 45 | 44 | 43 | 42 | 41 | 40 | 39 | 38  | 3.3 | GND |       
      ----------------------------------------------------------------


  BOTONERA MAIN conectada de la siguiente forma (conexiones hacia izquierda
  y de arriba a abajo):
      -----------------------------------------------------------------------
      |     B1     |    B2   |    B3    |    B4    |     B5    |  VCC | GND |
      | (cocinado) | (crudo) | (añadir) | (borrar) | (guardar) |      |     |
      -----------------------------------------------------------------------
      |     26     |   27    |    28    |    29    |     30    |  3.3 | GND |       
      -----------------------------------------------------------------------


    BOTON BARCODE conectado de la siguiente forma (indistintamente):
        -----------------
        | GND | Barcode |
        -----------------
        | GND |   53    |       
        -----------------
    ¡IMPORTANTE! El GND junto al D53 en el screw shield no está conectado a nada. Usar alguno
    de los que están junto a 5V o 3.3V.

-----------------------------------------------------------------------------------
*/



#ifndef BUTTONS_H
#define BUTTONS_H

#include "debug.h" // SM_DEBUG --> SerialPC





//  -----   GRANDE  -------------------------------------
/* Estructura que representa la posición de un botón. */
/*struct ButtonPosition {
    byte row;
    byte col;
};*/
byte iRow = 0, iCol = 0;

/* Keyboards variables */
const byte countRows = 4;
const byte countColumns = 5;

/* Buttons */
#ifdef SM_V2_1 // SmartCloth v2.1 (cartón)
    const byte rowsPins[countRows] = {26,28,30,32}; // F1, F2, F3, F4
    const byte columnsPins[countColumns] = {44,46,48,50,52}; // C1, C2, C3, C4, C5
#endif
#ifdef SM_V2_2 // SmartCloth v2.2 (3D)
    const byte rowsPins[countRows] = {44,45,46,47}; // F1, F2, F3, F4
    const byte columnsPins[countColumns] = {39,40,41,42,43}; // C1, C2, C3, C4, C5
#endif

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




/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
// GENERAL
void            checkAllButtons();       // Asignación de eventos según botón pulsado en Grande, Main o Barcode

// BOTONERA GRUPOS ALIMENTOS
//ButtonPosition  readButtonsGrande();     // Polling de botonera grande tras saltar interrupción de pulsación
void            readButtonsGrande();
void            checkGrandeButton();

// BOTONERA PRINCIPAL
void            checkMainButton();

// BOTÓN BARCODE
void            checkBarcodeButton();
/******************************************************************************/
/******************************************************************************/




/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para chequear todos los botones del SmartCloth
 * 
 * Esta función comprueba las interrupciones de pulsaciones de los botones en
 * las botoneras (grande, main y barcode). Dependiendo del botón pulsado, se asigna
 * un evento correspondiente y se agrega al búfer de eventos.
 */
/*-----------------------------------------------------------------------------*/
void checkAllButtons()
{
    // ----- TECLADO MAIN -------
    checkMainButton();
    // --------------------------

    // ----- TECLADO GRANDE -----
    checkGrandeButton();
    // --------------------------

    // ----- BARCODE ------------
    checkBarcodeButton();
    // --------------------------

}



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
/*ButtonPosition readButtonsGrande()
{
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
}*/
 void readButtonsGrande()
 {
    for (byte c = 0; c < countColumns; c++){  
        pinMode(columnsPins[c], INPUT); //Para proteger eléctricamente los puertos de los botones y que no llegue 0 y 1 a la vez
    }
    
    for (byte c = 0; c < countColumns; c++)
    {
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
 * @brief Verifica si se ha pulsado el botón "Grande" y realiza las acciones correspondientes.
 * 
 *  Comprueba si se ha pulsado algún botón de grupos de alimentos y obtiene el grupo escogido:
 *      Si el grupo está entre [7, 9] o [16, 18], se considera TIPO_A.
 *      Si el grupo está entre [1, 6], [10, 15] o [19, 20], se considera TIPO_B.
 * 
 *  Reestablece la flag 'pulsandoGrande' a false hasta la próxima pulsación.
 */
/*-----------------------------------------------------------------------------*/
void checkGrandeButton()
{
    if(grandeButtonInterruptOccurred()) // Se está pulsando un grupo de alimentos
    {
        //ButtonPosition position = readButtonsGrande(); // Posición del botón pulsado
        //buttonGrande = buttons[position.row][position.col];
        readButtonsGrande(); // Qué tecla se está pulsando 
        buttonGrande = buttons[iRow][iCol];

        #if defined(SM_DEBUG)
            SerialPC.print(F("Grupo: ")); SerialPC.println(buttonGrande); 
        #endif 
        
        /* ----- EVENTO ------- */ 
        // Tipo A: [7, 9] o [16, 18]
        /*if (((buttons[position.row][position.col] >= 7) and (buttons[position.row][position.col] <= 9)) or ((buttons[position.row][position.col] >= 16) and 
            (buttons[position.row][position.col] <= 18))){
                    eventoGrande = TIPO_A; // Grupo A (necesita crudo/cocinado)
        }
        // Tipo B: [1, 6], [10, 15] o [19, 20]
        else if(((buttons[position.row][position.col] >= 1) and (buttons[position.row][position.col] <= 6)) or ((buttons[position.row][position.col] >= 10) and 
                (buttons[position.row][position.col] <= 15)) or (buttons[position.row][position.col] >=19)){
                    eventoGrande = TIPO_B;  // Grupo B (no necesita crudo/cocinado pero se permite "escoger" de forma ficticia)  
        }*/
        if (((buttons[iRow][iCol] >= 7) and (buttons[iRow][iCol] <= 9)) or ((buttons[iRow][iCol] >= 16) and 
            (buttons[iRow][iCol] <= 18))){
                    eventoGrande = TIPO_A; // Grupo A (necesita crudo/cocinado)
        }
        else if(((buttons[iRow][iCol] >= 1) and (buttons[iRow][iCol] <= 6)) or ((buttons[iRow][iCol] >= 10) and 
                (buttons[iRow][iCol] <= 15)) or (buttons[iRow][iCol] >=19)){
                    eventoGrande = TIPO_B;  // Grupo B (no necesita crudo/cocinado pero se permite "escoger" de forma ficticia)  
        }
        
        addEventToBuffer(eventoGrande);
        flagEvent = true;

        /*----- Grupo alimentos ---- */
        setGrupoAlimentos(buttonGrande);
        
        pulsandoGrande = false;
    }
        
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que verifica el botón pulsado en la botonera principal y realiza 
 * acciones en consecuencia.
 * 
 *  Si el grupo de alimentos seleccionado es de TIPO_A, necesita diferenciar entre 
 *  crudo o cocinado porque sus valores nutricionales son diferentes según el caso. 
 *  Para los de TIPO_B no importa si los cocinas o no.
 * 
 *  Añade el evento correspondiente al búfer de eventos y reestablece la 
 *  flag 'pulsandoGrande' a false.
 */
/*-----------------------------------------------------------------------------*/
void checkMainButton()
{
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

    if(mainButtonInterruptOccurred()) // Se está pulsando un botón de la botonera principal
    {
        switch (buttonMain) 
        {
            case 1:   eventoMain = COCINADO;        if(eventoGrande == TIPO_A) setGrupoAlimentos(buttonGrande+20);     break;  
            case 2:   eventoMain = CRUDO;           if(eventoGrande == TIPO_A) setGrupoAlimentos(buttonGrande);        break;  // Crudo -> opción predeterminada
            case 3:   eventoMain = ADD_PLATO;       break;  
            case 4:   eventoMain = DELETE_PLATO;    break;  
            case 5:   eventoMain = GUARDAR;         break;  
        }
        
        addEventToBuffer(eventoMain);
        flagEvent = true;

        buttonMain = 0;
    }

}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Verifica si se ha pulsado el botón de código de barras.
 * 
 *      Comprueba si se ha pulsado el botón de código de barras, indica el evento correspondiente.
 *      y desactiva la flag 'pulsandoBarcode' hasta la próxima pulsación.
 */
/*-----------------------------------------------------------------------------*/
void checkBarcodeButton()
{
    if(barcodeButtonInterruptOccurred()) // Se está pulsando el botón de código de barras
    {
        addEventToBuffer(BARCODE);
        flagEvent = true;

        // Se actualiza 'grupoEscogido' si se ha leído barcode y encontrado el producto. 
        // Si se hiciera setGrupoAlimentos(BARCODE_PRODUCT_INDEX) aquí y luego no se encontrara el producto,
        // se mostraría en pantalla "Grupo Actual: " con el nombre vacío porque no se ha obtenido nada. Por eso
        // solo se actualiza si se encuentra producto.
        
        pulsandoBarcode = false; // Reiniciar flag
    }

}





/******************************************************************************/
/******************************************************************************/


#endif
