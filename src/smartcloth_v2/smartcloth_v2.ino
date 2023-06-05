/*
File Name : smartcloth_v1.ino                              
Author    : Irene Casares Rodríguez                          
Edit Date : 31/05/2023
Version   : v1.0
Coments   : Version con interfaz sencilla. No incluye comprobaciones para añadir, borrar y guardar
       
*/




#include "Buttons.h"       /* Buttons.h 
                                => ISR.h 
                                      => Scale.h
                                          => HX711.h
                                          => State_Machine.h
                                => State_Machine.h
                                      => SD_functions.h
                                          => RTC.h
                                      => Screen.h 
                                          => RA8876_v2.h
                                          => icons.h
                                          => State_Machine.h
                                          => Variables.h 
                                                => Scale.h
                                                => Diario.h 
                                                      => Comida.h 
                                                            => Plato.h 
                                                                  => Alimento.h
                                                                        => Valores_Nutricionales.h
                                                                        => Grupos.h
                                                                      
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

    /* ------ RTC ------------ */
    setupRTC(); 

    /* ------ SD card -------- */
    setupSDcard(); // Incluye sumar en "Acumulado hoy" las comidas guardadas el día de hoy

    /* ------ SCALE --------- */
    setupScale();   
    
    /* ------ SCREEN --------- */
    setupScreen();  
    

    /*------- ESTADO INICIAL --------- */
    state_actual = STATE_Empty;


    /* ---------- BUTTONS PINS --------------- */
    //  -----   Grande    -----
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT);
        digitalWrite(columnsPins[c], HIGH);
    }
    for (byte r = 0; r < countRows; r++){
        pinMode(rowsPins[r], INPUT);
    }

    //  -----   MAIN    -----
    pinMode(intPinCrudo, INPUT);
    pinMode(intPinCocinado, INPUT);
    pinMode(intPinAddPlato, INPUT);
    pinMode(intPinDeletePlato, INPUT);
    pinMode(intPinGuardar, INPUT);

  
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


    /* ------- BIENVENIDA ------------------------ */
    Welcome();  // Cargar imágenes y mostrar logo de SmartCloth

}




/*---------------------------------------------------------------------------------------------------------
   loop(): Función principal ejecutada continuamente
----------------------------------------------------------------------------------------------------------*/
void loop() {
    
    if (millis() - prevMillis > period){
        prevMillis = millis();
        
        doStateActions();   // Actividades del estado actual. Comienza en STATE_Empty 

        /*--------------------------------------------------------------*/
        /* ---------------    CHECK INTERRUPCIONES   ------------------ */
        /*--------------------------------------------------------------*/
        checkAllButtons();  // Comprueba interrupción de botoneras
        checkBascula();     // Comprueba interrupción de báscula


        /*------------------------------------------------------------*/
        /* ---------------    MOTOR DE INFERENCIA   ----------------- */
        /*------------------------------------------------------------*/

        if(flagEvent){    // Para evitar que marque evento para cada interrupción, ya que
                          // lo marcaría cada medio segundo por la interrupción de la báscula.
                          // Con esta flag 'flagEvent' solo se da aviso de un evento real 
                          // (pulsación, incremento o decremento).
            
            if(checkStateConditions()){     // Si se ha cumplido alguna regla de transición cuyo estado 
                                            // inicial fuera el actual, se modifica el estado actual por
                                            // el próximo indicado en la regla.
                state_prev_prev = state_prev;
                state_prev = state_actual;
                state_actual = state_new;
                Serial.print(F("\nEstado anterior al anterior: ")); Serial.println(state_prev_prev);
                Serial.print(F("\nEstado anterior: ")); Serial.println(state_prev);
                Serial.print(F("\nNuevo estado: "));    Serial.println(state_new);
            }
            else{
                Serial.println(F("\nERROR DE EVENTO"));
                actEventError();            // Mensaje de error por evento erróneo según el estado actual
            }
            flagEvent = false;
       }

        
    }
    
}
