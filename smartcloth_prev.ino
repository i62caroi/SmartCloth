#include "BUTTONS.h"
#include "ISR.h" 
#include "SCREEN.h"
#include "SCALE.h"
#include "STATE_MACHINE.h"
#include "aux.h"

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
    attachInterrupt(digitalPinToInterrupt(interruptPinMain), ISR_pulsandoButtonsMain, RISING);
    doIntMainRising = true;
    //attachInterrupt(digitalPinToInterrupt(interruptPinMainFalling), keyReleasedMain, FALLING);
    //doIntMainFalling = true;
    
    //  -----   Grande    -----
    attachInterrupt(digitalPinToInterrupt(interruptPinGrande), ISR_pulsandoButtonsGrande, RISING);
    doIntGrandeRising = true;
    //attachInterrupt(digitalPinToInterrupt(interruptPinGFalling), keyReleasedGrande, FALLING);
    //doIntGrandeFalling = true;*/

    //  -----   Scale   ------
    attachDueInterrupt(HW_TIMER_INTERVAL_MS * 1000, TimerHandler, "ITimer");
    ISR_Timer.setInterval(TIMER_INTERVAL_1S,  ISR_pesarBascula); //timer llama a 'ISR_pesarBascula' cada 1S
    doIntScale = true;


    //  ------ Buffer Eventos  ------
    //clearEventBuffer();
    

    /* ---------- BUTTONS PINS --------------- */
    //  -----   MAIN    -----
    for (byte i = 0; i < countButtons; i++){
        pinMode(buttonsMain[i], INPUT);  
    }
    //  -----   Grande    -----
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT);
        digitalWrite(columnsPins[c], HIGH);
    }
    for (byte r = 0; r < countRows; r++){
        pinMode(rowsPins[r], INPUT);
    }
    
    //Serial.println(F("Pulsa una tecla"));
    
}




/*---------------------------------------------------------------------------------------------------------
   loop(): Función principal ejecutada continuamente
----------------------------------------------------------------------------------------------------------*/
void loop() {
    //printInit();
    
    if (millis() - prevMillis > period){
        prevMillis = millis();

        Serial.print(F("\n\nEstado actual: ")); Serial.println(state_actual);

        /* Actividades del estado actual. Comienza en STATE_INI */
        doStateActions(); //state_machine.h 

        /*--------------------------------------------------------------*/
        /* ---------------    PULSACIONES BOTONERAS   ----------------- */
        /*--------------------------------------------------------------*/
        checkAllButtons(); //buttons.h
        checkBascula();


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
                //recentStateChange = true;
            }
            else{
                Serial.println(F("\nERROR DE EVENTO"));
                //printEventError();
            }
            flagEvent = false;
       }

        
    }
    
}
