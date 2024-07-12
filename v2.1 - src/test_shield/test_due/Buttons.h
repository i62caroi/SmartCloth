

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





//  -----   GRANDE  -------------------------------------
byte iRow = 0, iCol = 0;

/* Keyboards variables */
const byte countRows = 4;
const byte countColumns = 5;

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

#define SerialPC Serial


/******************************************************************************/
// GENERAL
void            checkAllButtons();       // Asignación de eventos según botón pulsado en Grande, Main o Barcode

// BOTONERA GRUPOS ALIMENTOS
void            readButtonsGrande();
void            checkGrandeButton();

// BOTONERA PRINCIPAL
void            checkMainButton();

// BOTÓN BARCODE
void            checkBarcodeButton();
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
        readButtonsGrande(); // Qué tecla se está pulsando 
        buttonGrande = buttons[iRow][iCol];
        SerialPC.print(F("Boton pulsado: Grupo ")); SerialPC.println(buttonGrande); 

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
    if(mainButtonInterruptOccurred()) // Se está pulsando un botón de la botonera principal
    {
        switch (buttonMain) 
        {
            case 1:   SerialPC.println("Boton pulsado: COCINADO");         break;  
            case 2:   SerialPC.println("Boton pulsado: CRUDO");            break;  
            case 3:   SerialPC.println("Boton pulsado: ANADIR PLATO");     break;  
            case 4:   SerialPC.println("Boton pulsado: BORRAR PLATO");     break;  
            case 5:   SerialPC.println("Boton pulsado: GUARDAR");          break;  
        }
        
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
        SerialPC.println("Boton pulsado: BARCODE"); 
        
        pulsandoBarcode = false; // Reiniciar flag
    }

}





/******************************************************************************/
/******************************************************************************/


#endif
