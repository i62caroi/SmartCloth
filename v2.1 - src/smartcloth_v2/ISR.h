/**
 * @file ISR.h
 * @brief Definiciones de las Rutinas de Servicio de Interrupción (ISR)
 *
 * @author Irene Casares Rodríguez
 * @date 06/06/23
 * @version 1.0
 *
 * Este archivo contiene las definiciones de las funciones utilizadas para la 
 * activación y detección de interrupciones causadas por la pulsación de algún
 * botón en cualquiera de las botoneras (grande y main) y por el timer de interrupción
 * utilizado para la lectura de la báscula cada 0.5 seg.
 *
 * @note Este archivo asume la existencia del siguiente archivo de encabezado:
 *       - "Scale.h" para la definición de las funciones de la báscula
 * 
 * @see Scale.h
 * 
 */

#ifndef ISR_H
#define ISR_H

#include "debug.h" // SM_DEBUG --> SerialPC


/*  ----- SCALE ISR ----- */
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include "SAMDUETimerInterrupt.h"
#include "SAMDUE_ISR_Timer.h"

#define HW_TIMER_INTERVAL_MS     10
#define TIMER_INTERVAL_5MS       500L

SAMDUE_ISR_Timer ISR_Timer;



//  -----   MAIN  --------------------------------------
volatile byte  buttonMain = 0; // Botón pulsado en Main (botonera B)
// ------------------------------------------------------


//  -----   GRANDE  -------------------------------------
// SmartCloth v2.1
//const byte interruptPinGrande = 37;       // Pin de interrupcion RISING para Grande (botonera A)
// SmartCloth v2.2
const byte interruptPinGrande = 38;       // Pin de interrupcion RISING para Grande (botonera A)
volatile bool pulsandoGrande  = false;    // Flag de estar pulsando algo en Grande
// ------------------------------------------------------


// ----- BOTÓN BARCODE ----------------------------------
const byte intPinBarcode = 53;
volatile bool pulsandoBarcode = false;
// ------------------------------------------------------



//  -----  BÁSCULA --------------------------------------
volatile float  weight = 0.0;         ///< Peso real tomado por ISR
volatile bool   pesado = false;       // Flag de haber pesado por ISR
// ------------------------------------------------------



#include "Scale.h" // Debajo de las variables para que estén disponibles en su ámbito


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void ISR_crudo();                         // ISR de botón de 'crudo'
void ISR_cocinado();                      // ISR de botón de 'cocinado'
void ISR_addPlato();                      // ISR de botón de 'añadir plato'
void ISR_deletePlato();                   // ISR de botón de 'eliminar plato'
void ISR_guardar();                       // ISR de botón de 'guardar comida'
void ISR_pulsandoButtonsGrande();         // ISR de botonera de grupos de alimentos
void ISR_barcode();                       // ISR de botón de barcode
void TimerHandler();                      // Activar timer de interrupción de la báscula
void ISR_pesarBascula();                  // Activar báscula para pesar cuando salte timer de interrupción
uint16_t attachDueInterrupt(double microseconds, timerCallback callback, const char* TimerName);     // Adjuntar interrupción al timer

bool interruptionOccurred();              // Devuelve true si se ha activado alguna interrupción en botoneras o evento en báscula
bool buttonInterruptOccurred();           // Devuelve true si se ha activado alguna interrupción en botoneras 
bool mainButtonInterruptOccurred();
bool grandeButtonInterruptOccurred();
bool barcodeButtonInterruptOccurred();
bool hasScaleEventOccurred();             // Devuelve true si se ha activado algún evento en báscula
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/





/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'crudo'
 */
/*-----------------------------------------------------------------------------*/
void ISR_crudo(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 1;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'cocinado'
 */
/*-----------------------------------------------------------------------------*/
void ISR_cocinado(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 2;
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
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 3;
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
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 4;
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
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 5;
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
    if ((interrupt_time - last_interrupt_time) > 500) pulsandoGrande = true;
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
    if ((interrupt_time - last_interrupt_time) > 200) pulsandoBarcode = true;
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


#endif
