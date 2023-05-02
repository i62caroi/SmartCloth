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
const byte intPinCrudo = 22;              // Amarillo 
const byte intPinCocinado = 23;           // Azul 
const byte intPinAddPlato = 24;           // Verde 
const byte intPinDeletePlato = 25;        // Rojo 
const byte intPinGuardar = 26;            // Negro 


/*  -----   GRANDE  ----- */
const byte interruptPinGrande = 29;       // Pin de interrupcion RISING para Grande
volatile bool pulsandoGrande = false;     // Flag de estar pulsando algo en Grande


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



/*---------------------------------------------------------------------------------------------------------
   ISR_crudo(): Función activada por interrupción RISING de botón CRUDO
----------------------------------------------------------------------------------------------------------*/
void ISR_crudo(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) {
        buttonMain = 1;
        flagEvent = true;
    }
    last_interrupt_time = interrupt_time;
}



/*---------------------------------------------------------------------------------------------------------
   ISR_cocinado(): Función activada por interrupción RISING de botón COCINADO
----------------------------------------------------------------------------------------------------------*/
void ISR_cocinado(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) {
        buttonMain = 2;
        flagEvent = true;
    }
    last_interrupt_time = interrupt_time;
}



/*---------------------------------------------------------------------------------------------------------
   ISR_addPlato(): Función activada por interrupción RISING de botón ADDPLATO
----------------------------------------------------------------------------------------------------------*/
void ISR_addPlato(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) {
        buttonMain = 3;
        flagEvent = true;
    }
    last_interrupt_time = interrupt_time;
}



/*---------------------------------------------------------------------------------------------------------
   ISR_deletePlato(): Función activada por interrupción RISING de botón DELETE
----------------------------------------------------------------------------------------------------------*/
void ISR_deletePlato(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) {
        buttonMain = 4;
        flagEvent = true;
    }
    last_interrupt_time = interrupt_time;
}



/*---------------------------------------------------------------------------------------------------------
   ISR_guardar(): Función activada por interrupción RISING de botón Guardar
----------------------------------------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------------------------------------
   ISR_pulsandoButtonsGrande(): Función ativada por interrupción RISING de botonera Grande
----------------------------------------------------------------------------------------------------------*/
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

/*---------------------------------------------------------------------------------------------------------
   TimerHandler(): Función para activar el timer de interrupción
----------------------------------------------------------------------------------------------------------*/
void TimerHandler() { ISR_Timer.run(); }

/*---------------------------------------------------------------------------------------------------------
   ISR_pesarBascula(): Función para pesar por interrupción
----------------------------------------------------------------------------------------------------------*/
void ISR_pesarBascula(){  
    weight = weighScale();
    pesado = true; 
}

/*---------------------------------------------------------------------------------------------------------
   attachDueInterrupt(): Función para adjuntar interrupción al timer
----------------------------------------------------------------------------------------------------------*/
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
