#ifndef ISR_H
#define ISR_H

//--------------------------------------------------
/*  ----- SCALE ISR ----- */
#define TIMER_INTERRUPT_DEBUG         0
#define _TIMERINTERRUPT_LOGLEVEL_     0

#include "SAMDUETimerInterrupt.h"
#include "SAMDUE_ISR_Timer.h"

#include "SCALE.h"
#include "aux.h"

#define HW_TIMER_INTERVAL_MS     10
#define TIMER_INTERVAL_1S        1000L

SAMDUE_ISR_Timer ISR_Timer;


//volatile float weight;
//volatile bool pesado = false;
//byte doIntScale;
//--------------------------------------------------


//--------------------------------------------------
/* Interrupciones botoneras */
/*  -----   MAIN  ----- */
const byte interruptPinMain = 28;                      // Pin de interrupcion RISING para Main
volatile bool pulsandoMain = false;                    // Flag de estar pulsando algo en Main   
///byte doIntMainRising; 

/*  -----   GRANDE  ----- */
const byte interruptPinGrande = 29;                    // Pin de interrupcion RISING para Grande
volatile bool pulsandoGrande = false;                  // Flag de estar pulsando algo en Grande
//byte doIntGrandeRising;
//--------------------------------------------------


//volatile bool flagEvento = false;



/*---------------------------------------------------------------------------------------------------------
   ISR_pulsandoButtonsMain(): Función ativada por interrupción RISING de botonera Main
----------------------------------------------------------------------------------------------------------*/
void ISR_pulsandoButtonsMain(){
   // if(doIntMainRising){
        static unsigned long last_interrupt_time_Main = 0;
        unsigned long interrupt_time_Main = millis();
        // REBOTE
        if ((interrupt_time_Main - last_interrupt_time_Main) > 200) {
            pulsandoMain = true;
            flagEvent = true;
        }
        last_interrupt_time_Main = interrupt_time_Main;
    //}
}

/*---------------------------------------------------------------------------------------------------------
   ISR_pulsandoButtonsGrande(): Función ativada por interrupción RISING de botonera Grande
----------------------------------------------------------------------------------------------------------*/
void ISR_pulsandoButtonsGrande(){
    //if(doIntGrandeRising){
        static unsigned long last_interrupt_time_Grande = 0;
        unsigned long interrupt_time_Grande = millis();
        // REBOTE
        if ((interrupt_time_Grande - last_interrupt_time_Grande) > 200) {
            pulsandoGrande = true;
            flagEvent = true;
        }
        last_interrupt_time_Grande = interrupt_time_Grande;
    //}
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
    //if(doIntScale){
        weight = weighScale();
        pesado = true; 
    //}
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
