/**
 * @file ISR.h
 * @brief Definiciones de las Rutinas de Servicio de Interrupción (ISR)
 *
 * @author Irene Casares Rodríguez
 * @date 07/05/24
 * @version 1.0
 *
 */

#ifndef ISR_H
#define ISR_H

#define DEBOUNCE_TIME 300


// ------------ PINES DE INTERRUPCIÓN ------------
// ---- BARCODE ----
const byte intPinBarcode = 53;
// -----------------------------------------------

// ------------ FLAGS DE INTERRUPCIÓN ------------
// Flag de botón pulsado en Barcode
volatile bool pulsandoBarcode = false;
volatile bool firstInterruptBarcode = true; // Al encender, por fluctuaciones de voltaje se detecta una pulsación fantasma
// -----------------------------------------------



/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/

// Botón de barcode
void ISR_barcode();                       // ISR de botón de barcode

// Avisos de interrupción
bool barcodeButtonInterruptOccurred();    // Devuelve true si se ha activado alguna interrupción en botón Barcode

/******************************************************************************/
/******************************************************************************/




/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/

/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de barcode
 */
/*-----------------------------------------------------------------------------*/
void ISR_barcode()
{ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();

    // Ignorar la primera pulsación (fantasma) era necesario al testear los SMs, pero no creo que sea necesario en
    // el programa final. De hecho, creo que está haciendo que se ignore la primera pulsación real.
    if (firstInterruptBarcode) { firstInterruptBarcode = false; return; } // Descartar la primera interrupción por fluctuaciones de voltaje
    
    if ((interrupt_time - last_interrupt_time) > DEBOUNCE_TIME) pulsandoBarcode = true;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Aviso de interrupción en botón Barcode
 * @return 'true' si ha ocurrido alguna interrupción en botón Barcode
 */
 /*-----------------------------------------------------------------------------*/
 bool barcodeButtonInterruptOccurred(){
    if(pulsandoBarcode) return true;
    else return false;
 }


#endif
