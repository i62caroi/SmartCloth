
#ifndef ISR_H
#define ISR_H

#define DEBOUNCE_TIME 300 // Tiempo de debouncing en ms

// ------------ PINES DE INTERRUPCIÓN ------------
// ---- MAIN ----
const byte intPinCocinado     = 26;   
const byte intPinCrudo        = 27;    
const byte intPinAddPlato     = 28; 
const byte intPinGuardar      = 29;  
const byte intPinDeletePlato  = 30;
// ---- GRANDE ----
const byte interruptPinGrande = 38;       // Pin de interrupcion RISING para Grande (botonera A)  
// ---- BARCODE ----
const byte intPinBarcode = 53;
// -----------------------------------------------


// ------------ FLAGS DE INTERRUPCIÓN ------------
// Flag de botón pulsado en Main (botonera B)
volatile byte buttonMain = 0; // No es flag, sino variable que almacena el botón pulsado

// Flag de botón pulsado en Grande (botonera A)
volatile bool pulsandoGrande  = false;    

// Flag de botón pulsado en Barcode
volatile bool pulsandoBarcode = false;
volatile bool firstInterruptBarcode = true; // Al encender, por fluctuaciones de voltaje se detecta una pulsación fantasma
// -----------------------------------------------



//  -----  BÁSCULA --------------------------------------
/*  ----- SCALE ISR ----- */
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include "SAMDUETimerInterrupt.h"
#include "SAMDUE_ISR_Timer.h"

#define HW_TIMER_INTERVAL_MS     10
#define TIMER_INTERVAL_5MS       500L

SAMDUE_ISR_Timer ISR_Timer;

volatile float  weight = 0.0;         ///< Peso real tomado por ISR
volatile bool   pesado = false;       // Flag de haber pesado por ISR
// ------------------------------------------------------



#include "Scale.h" // Debajo de las variables para que estén disponibles en su ámbito


/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/

// Botonera Main (acciones)
void ISR_cocinado();                      // ISR de botón de 'cocinado'
void ISR_crudo();                         // ISR de botón de 'crudo'
void ISR_addPlato();                      // ISR de botón de 'añadir plato'
void ISR_deletePlato();                   // ISR de botón de 'eliminar plato'
void ISR_guardar();                       // ISR de botón de 'guardar comida'

// Botonera Grande (grupos de alimentos)
void ISR_pulsandoButtonsGrande();         // ISR de botonera de grupos de alimentos

// Botón de barcode
void ISR_barcode();                       // ISR de botón de barcode

// Báscula
void TimerHandler();                      // Activar timer de interrupción de la báscula
void ISR_pesarBascula();                  // Activar báscula para pesar cuando salte timer de interrupción
uint16_t attachDueInterrupt(double microseconds, timerCallback callback, const char* TimerName);     // Adjuntar interrupción al timer

// Avisos de interrupción
bool interruptionOccurred();              // Devuelve true si se ha activado alguna interrupción en botoneras o evento en báscula
bool buttonInterruptOccurred();           // Devuelve true si se ha activado alguna interrupción en botoneras 
bool mainButtonInterruptOccurred();       // Devuelve true si se ha activado alguna interrupción en botonera Main
bool grandeButtonInterruptOccurred();     // Devuelve true si se ha activado alguna interrupción en botonera Grande
bool barcodeButtonInterruptOccurred();    // Devuelve true si se ha activado alguna interrupción en botón Barcode
bool hasScaleEventOccurred();             // Devuelve true si se ha activado algún evento en báscula
/******************************************************************************/
/******************************************************************************/




/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'cocinado'
 */
/*-----------------------------------------------------------------------------*/
void ISR_cocinado(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > DEBOUNCE_TIME) buttonMain = 1;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'crudo'
 */
/*-----------------------------------------------------------------------------*/
void ISR_crudo(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > DEBOUNCE_TIME) buttonMain = 2;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'añadir plato'
 */
/*-----------------------------------------------------------------------------*/
void ISR_addPlato(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > DEBOUNCE_TIME) buttonMain = 3;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'eliminar plato'
 */
/*-----------------------------------------------------------------------------*/
void ISR_deletePlato(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > DEBOUNCE_TIME) buttonMain = 4;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'guardar comida'
 */
/*-----------------------------------------------------------------------------*/
void ISR_guardar(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > DEBOUNCE_TIME) buttonMain = 5;
    last_interrupt_time = interrupt_time;
}




/*---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botonera de grupos de alimentos
 * Tiempo de debouncing aumentado a 0.5 seg porque si se pulsa "mucho" rato, al soltar
 * se detecta pulsación en otro botón debido al rebote.
 */
/*-----------------------------------------------------------------------------*/
void ISR_pulsandoButtonsGrande(){
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > DEBOUNCE_TIME) pulsandoGrande = true;
    last_interrupt_time = interrupt_time;
}


/*---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de barcode
 */
/*-----------------------------------------------------------------------------*/
void ISR_barcode(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (firstInterruptBarcode) { firstInterruptBarcode = false; return; } // Descartar la primera interrupción por fluctuaciones de voltaje
    if ((interrupt_time - last_interrupt_time) > DEBOUNCE_TIME) pulsandoBarcode = true;
    last_interrupt_time = interrupt_time;
}




/*---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief Activar timer de interrupción de la báscula
 */
/*-----------------------------------------------------------------------------*/
void TimerHandler() { ISR_Timer.run(); }


/*-----------------------------------------------------------------------------*/
/**
 * @brief Activar báscula para pesar cuando salte timer de interrupción
 */
 /*-----------------------------------------------------------------------------*/
 void ISR_pesarBascula(){  
    weight = weighScale();
    pesado = true; 
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Adjuntar interrupción al timer
 * @param microseconds Intervalo de tiempo en microsegundos
 * @param callback Función de devolución de llamada para la interrupción
 * @param TimerName Nombre del temporizador
 * @return Número del temporizador utilizado
 */
 /*-----------------------------------------------------------------------------*/
 uint16_t attachDueInterrupt(double microseconds, timerCallback callback, const char* TimerName){
    // Primer timer libre 
    DueTimerInterrupt dueTimerInterrupt = DueTimer.getAvailable(); 
    
    // Intervalo de ejecución de la interrupción 
    dueTimerInterrupt.attachInterruptInterval(microseconds, callback); 

    // Número del timer utilizado
    uint16_t timerNumber = dueTimerInterrupt.getTimerNumber();
    #if defined(SM_DEBUG)
        SerialPC.print(TimerName); SerialPC.print(F(" attached to Timer(")); SerialPC.print(timerNumber); SerialPC.println(F(")"));
    #endif 
    return timerNumber;
}




/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/**
 * @brief Aviso de interrupción general
 * @return 'true' si ha ocurrido alguna interrupción en botoneras o evento en báscula
 */
 /*-----------------------------------------------------------------------------*/
 bool interruptionOccurred(){
    if((buttonMain != 0) or (pulsandoGrande)  or (pulsandoBarcode) or (scaleEventOccurred)) return true;
    else return false;
 }


/*-----------------------------------------------------------------------------*/
/**
 * @brief Aviso de interrupción en botoneras 
 * @return 'true' si ha ocurrido alguna interrupción en botoneras 
 */
 /*-----------------------------------------------------------------------------*/
 bool buttonInterruptOccurred(){
    if((buttonMain != 0) or (pulsandoGrande) or (pulsandoBarcode)) return true;
    else return false;
 }


/*-----------------------------------------------------------------------------*/
/**
 * @brief Aviso de interrupción en botonera Main 
 * @return 'true' si ha ocurrido alguna interrupción en botonera Main 
 */
 /*-----------------------------------------------------------------------------*/
 bool mainButtonInterruptOccurred(){
    if(buttonMain != 0) return true;
    else return false;
 }

/*-----------------------------------------------------------------------------*/
/**
 * @brief Aviso de interrupción en botonera Grande 
 * @return 'true' si ha ocurrido alguna interrupción en botonera Grande 
 */
 /*-----------------------------------------------------------------------------*/
 bool grandeButtonInterruptOccurred(){
    if(pulsandoGrande) return true;
    else return false;
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


 /*-----------------------------------------------------------------------------*/
/**
 * @brief Aviso de evento en báscula
 * @return 'true' si ha ocurrido algún evento en báscula
 */
 /*-----------------------------------------------------------------------------*/
 bool hasScaleEventOccurred(){
    if(scaleEventOccurred) return true;
    else return false;
 }




/******************************************************************************/
/******************************************************************************/

#endif
