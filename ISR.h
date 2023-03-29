#ifndef ISR_H
#define ISR_H

//--------------------------------------------------
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
const byte intPinAddPlato = 22;           // Verde
const byte intPinCrudo = 23;              // Amarillo
const byte intPinDeletePlato = 24;        // Rojo
const byte intPinCocinado = 25;           // Blanco
const byte intPinGuardar = 26;            // Negro


/*  -----   GRANDE  ----- */
const byte interruptPinGrande = 29;       // Pin de interrupcion RISING para Grande
volatile bool pulsandoGrande = false;     // Flag de estar pulsando algo en Grande




/*---------------------------------------------------------------------------------------------------------
   ISR_crudo(): Función activada por interrupción RISING de botón CRUDO
----------------------------------------------------------------------------------------------------------*/
void ISR_crudo(){ 
    static unsigned long last_interrupt_time_Crudo = 0;
    unsigned long interrupt_time_Crudo = millis();
    if ((interrupt_time_Crudo - last_interrupt_time_Crudo) > 200) {
        buttonMain = 1;
        flagEvent = true;
    }
    last_interrupt_time_Crudo = interrupt_time_Crudo;
}



/*---------------------------------------------------------------------------------------------------------
   ISR_cocinado(): Función activada por interrupción RISING de botón COCINADO
----------------------------------------------------------------------------------------------------------*/
void ISR_cocinado(){ 
    static unsigned long last_interrupt_time_Cocinado = 0;
    unsigned long interrupt_time_Cocinado = millis();
    if ((interrupt_time_Cocinado - last_interrupt_time_Cocinado) > 200) {
        buttonMain = 2;
        flagEvent = true;
    }
    last_interrupt_time_Cocinado = interrupt_time_Cocinado;
}



/*---------------------------------------------------------------------------------------------------------
   ISR_addPlato(): Función activada por interrupción RISING de botón ADDPLATO
----------------------------------------------------------------------------------------------------------*/
void ISR_addPlato(){ 
    static unsigned long last_interrupt_time_Add = 0;
    unsigned long interrupt_time_Add = millis();
    if ((interrupt_time_Add - last_interrupt_time_Add) > 200) {
        buttonMain = 3;
        flagEvent = true;
    }
    last_interrupt_time_Add = interrupt_time_Add;
}



/*---------------------------------------------------------------------------------------------------------
   ISR_deletePlato(): Función activada por interrupción RISING de botón DELETE
----------------------------------------------------------------------------------------------------------*/
void ISR_deletePlato(){ 
    static unsigned long last_interrupt_time_Delete = 0;
    unsigned long interrupt_time_Delete = millis();
    if ((interrupt_time_Delete - last_interrupt_time_Delete) > 200) {
        buttonMain = 4;
        flagEvent = true;
    }
    last_interrupt_time_Delete = interrupt_time_Delete;
}



/*---------------------------------------------------------------------------------------------------------
   ISR_guardar(): Función activada por interrupción RISING de botón Guardar
----------------------------------------------------------------------------------------------------------*/
void ISR_guardar(){ 
    static unsigned long last_interrupt_time_Guardar = 0;
    unsigned long interrupt_time_Guardar = millis();
    if ((interrupt_time_Guardar - last_interrupt_time_Guardar) > 200) {
        buttonMain = 5;
        flagEvent = true;
    }
    last_interrupt_time_Guardar = interrupt_time_Guardar;
}




/*---------------------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   ISR_pulsandoButtonsGrande(): Función ativada por interrupción RISING de botonera Grande
----------------------------------------------------------------------------------------------------------*/
void ISR_pulsandoButtonsGrande(){
    static unsigned long last_interrupt_time_Grande = 0;
    unsigned long interrupt_time_Grande = millis();
    if ((interrupt_time_Grande - last_interrupt_time_Grande) > 200) {
        pulsandoGrande = true;
        flagEvent = true;
    }
    last_interrupt_time_Grande = interrupt_time_Grande;
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
