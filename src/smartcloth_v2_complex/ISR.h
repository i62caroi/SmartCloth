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

//--------------------------------------------------
/*  ----- Bienvenida  ----- */



/*  ----- SCALE ISR ----- */
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include "SAMDUETimerInterrupt.h"
#include "SAMDUE_ISR_Timer.h"

#include "Scale.h"


#define HW_TIMER_INTERVAL_MS     10
#define TIMER_INTERVAL_5MS       500L

SAMDUE_ISR_Timer ISR_Timer;



/*  -----   MAIN  ----- */
const byte intPinCocinado     = 33;   // Naranja 
const byte intPinCrudo        = 31;   // Amarillo 
const byte intPinAddPlato     = 29;   // Verde 
const byte intPinDeletePlato  = 27;   // Azul 
const byte intPinGuardar      = 25;   // Morado 


/*  -----   GRANDE  ----- */
const byte interruptPinGrande = 37;       // Pin de interrupcion RISING para Grande
volatile bool pulsandoGrande  = false;    // Flag de estar pulsando algo en Grande


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void ISR_crudo();                         // ISR de botón de 'crudo'
void ISR_cocinado();                      // ISR de botón de 'cocinado'
void ISR_addPlato();                      // ISR de botón de 'añadir plato'
void ISR_deletePlato();                   // ISR de botón de 'eliminar plato'
void ISR_guardar();                       // ISR de botón de 'guardar comida'
void ISR_pulsandoButtonsGrande();         // ISR de botonera de grupos de alimentos
void TimerHandler();                      // Activar timer de interrupción de la báscula
void ISR_pesarBascula();                  // Activar báscula para pesar cuando salte timer de interrupción
uint16_t attachDueInterrupt(double microseconds, timerCallback callback, const char* TimerName);     // Adjuntar interrupción al timer
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
    if ((interrupt_time - last_interrupt_time) > 200) {
        buttonMain = 1;
        flagEvent = true;
    }
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
    if ((interrupt_time - last_interrupt_time) > 200) {
        buttonMain = 2;
        flagEvent = true;
    }
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
    if ((interrupt_time - last_interrupt_time) > 200) {
        buttonMain = 3;
        flagEvent = true;
    }
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
    if ((interrupt_time - last_interrupt_time) > 200) {
        buttonMain = 4;
        flagEvent = true;
    }
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
    if ((interrupt_time - last_interrupt_time) > 200) {
        buttonMain = 5;
        flagEvent = true;
    }
    last_interrupt_time = interrupt_time;
}




/*---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botonera de grupos de alimentos
 */
/*-----------------------------------------------------------------------------*/
void ISR_pulsandoButtonsGrande(){
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) {
        pulsandoGrande = true;
        flagEvent = true;
    }
    last_interrupt_time = interrupt_time;
}




//--------------------------------------------------

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
    Serial.print(TimerName); Serial.print(F(" attached to Timer(")); Serial.print(timerNumber); Serial.println(F(")"));
    return timerNumber;
}


#endif
