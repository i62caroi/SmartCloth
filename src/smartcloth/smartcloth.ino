#include "Buttons.h"       /* Buttons.h => ISR.h y State_Machine.h
                                -  ISR.h => Scale.h
                                -  State_Machine.h => Screen.h y Plato.h
                                     - Screen.h => Variables.h
                                           - Variables.h => Grupos.h
                                     - Plato.h => Ingrediente.h
                                           - Ingrediente.h => Valores_Nutricionales.h
                            */

/* Time */
const unsigned long period = 50;
unsigned long prevMillis = 0;
unsigned long tiempoPrevio = 0;



/*---------------------------------------------------------------------------------------------------------
   setup(): Función para inicializar la pantalla, las botoneras y las interrupciones
----------------------------------------------------------------------------------------------------------*/
void setup() {
    Serial.begin(115200);
    while (!Serial);
    delay(1000);

    /* ------ SCALE --------- */
    setupScale();   //scale.h
    
    /* ------ SCREEN --------- */
    setupScreen();  //screen.h

    /*------- ESTADO INICIAL --------- */
    //initStateMachine();
    state_actual = STATE_INI;

  
    /* --------- INTERRUPTIONS ----------------- */
    //  -----   MAIN    -----
    attachInterrupt(digitalPinToInterrupt(intPinCrudo), ISR_crudo, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinCocinado), ISR_cocinado, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinAddPlato), ISR_addPlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinDeletePlato), ISR_deletePlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinGuardar), ISR_guardar, RISING);
    
    //  -----   Grande    -----
    attachInterrupt(digitalPinToInterrupt(interruptPinGrande), ISR_pulsandoButtonsGrande, RISING);

    //  -----   Scale   ------
    attachDueInterrupt(HW_TIMER_INTERVAL_MS * 1000, TimerHandler, "ITimer");
    ISR_Timer.setInterval(TIMER_INTERVAL_5MS,  ISR_pesarBascula); //timer llama a 'ISR_pesarBascula' cada 100 ms

    

    /* ---------- BUTTONS PINS --------------- */
    //  -----   Grande    -----
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT);
        digitalWrite(columnsPins[c], HIGH);
    }
    for (byte r = 0; r < countRows; r++){
        pinMode(rowsPins[r], INPUT);
    }
    
    
}




/*---------------------------------------------------------------------------------------------------------
   loop(): Función principal ejecutada continuamente
----------------------------------------------------------------------------------------------------------*/
void loop() {
    
    if (millis() - prevMillis > period){
        prevMillis = millis();

        //Serial.print(F("\n\nEstado actual: ")); Serial.println(state_actual);

        /* Actividades del estado actual. Comienza en STATE_INI */
        doStateActions(); //state_machine.h 

        /*--------------------------------------------------------------*/
        /* ---------------    PULSACIONES BOTONERAS   ----------------- */
        /*--------------------------------------------------------------*/
        checkAllButtons(); //buttons.h
        checkBascula(); //scale.h


        /*------------------------------------------------------------*/
        /* ---------------    MOTOR DE INFERENCIA   ----------------- */
        /*------------------------------------------------------------*/

        if(flagEvent){
            /* Si se ha cumplido alguna regla de transición cuyo estado 
              inicial fuera el actual, se modifica el estado actual por
              el próximo indicado en la regla. */
            if(checkStateConditions()){ //state_machine.h 
                Serial.print(F("\nNuevo estado: ")); Serial.println(state_new);
                state_actual = state_new;
            }
            else{
                Serial.println(F("\nERROR DE EVENTO"));
                actEventError();
            }
            flagEvent = false;
       }

        
    }
    
}
