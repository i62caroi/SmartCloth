/**
 * @file  smartcloth_v1.ino
 * @brief Programa principal de SmartCloth
 *
 * @author Irene Casares Rodríguez
 * @date 14/06/2023
 * @version 1.0
 * 
 * @note Esta versión reducida limita las acciones para que deban ser en un orden específico.
 * @note Versión con interfaz a partir de diseño wireframe
 * 
 */



#include "Serial_esp32cam.h" //Comunicación con ESP32-CAM

#include "Buttons.h"       /**< Buttons.h 
                                => ISR.h 
                                      => Scale.h
                                          => HX711.h
                                          => State_Machine.h (eventos)
                                => State_Machine.h (eventos)
                                      => SD_functions.h
                                          => RTC.h
                                          => Files.h
                                          => Diario.h 
                                               => Comida.h 
                                                  => Plato.h 
                                                       => Alimento.h
                                                          => Valores_Nutricionales.h
                                                          => Grupos.h
                                      => Screen.h 
                                          => RA8876_v2.h
                                                
                                                                      
                            */


/* Time */
const unsigned long   period = 50;
unsigned long         prevMillis = 0;
unsigned long         tiempoPrevio = 0;


/*---------------------------------------------------------------------------------------------------------
   \brief Función para inicializar el RTC, la SD, la pantalla, la báscula, las botoneras y las interrupciones
----------------------------------------------------------------------------------------------------------*/
void setup() {
    /* ------ COMUNICACIÓN SERIAL ------------ */
    // Inicializar comunicación con PC (Serial)
    Serial.begin(9600); //115200
    while (!Serial);
    delay(100);
    // Inicializar comunicación con ESP32-CAM (Serial1)
    SerialDueESP32.begin(9600);
    while (!SerialDueESP32);
    delay(100); 

    /* ------ RTC ------------ */
    setupRTC(); 
    delay(100); 

    /* ------ SD card -------- */
    setupSDcard(); // Incluye crear fichero .csv, si no existe, y sumar en "Acumulado hoy" las comidas guardadas el día de hoy
    delay(100); 

    /* ------ SCALE --------- */
    setupScale();   
    delay(100); 
    
    /* ------ SCREEN --------- */
    setupScreen();  
    delay(100); 
    

    /*------- ESTADO INICIAL --------- */
    state_actual = STATE_Init;


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
        
        
        doStateActions();   // Actividades del estado actual. Comienza en STATE_Init 


        /*--------------------------------------------------------------*/
        /* ---------------    CHECK INTERRUPCIONES   ------------------ */
        /*--------------------------------------------------------------*/
        checkAllButtons();  // Comprueba interrupción de botoneras y marca evento
        checkBascula();     // Comprueba interrupción de báscula y marca evento
        


        /*------------------------------------------------------------*/
        /* ---------------    MOTOR DE INFERENCIA   ----------------- */
        /*------------------------------------------------------------*/

        if(flagEvent or flagError){     // Para evitar que marque evento para cada interrupción, ya que lo marcaría cada
                                        // medio segundo por la interrupción de la báscula, se utiliza la flag 'flagEvent'.
                                        // Con esta flag solo se da aviso de un evento real (pulsación, incremento o decremento).

                                        // Se incluye 'flagError' en la condición para que también compruebe las reglas
                                        // de transición en el caso de error y pase al STATE_ERROR.
                                        // Esta flag se activa en actEventError() y se desactiva tras los 3 segundos para
                                        // mostrar la pantalla de error en actStateERROR().

                                        // El error es una acción de diferente naturaleza. Es decir, no ocurre un error,
                                        // sino que si ha ocurrido algo que no es evento, se considera error. 

            if(checkStateConditions()){     // Si se ha cumplido alguna regla de transición cuyo estado 
                                            // inicial fuera el actual, se modifica el estado actual por
                                            // el próximo indicado en la regla.
                

                state_prev = state_actual;
                state_actual = state_new;
                
                if(state_prev != lastValidState){
                    switch(state_prev){ // Último estado válido 
                        case STATE_Init: case STATE_Plato: case STATE_groupA: case STATE_groupB: case STATE_raw: case STATE_cooked: case STATE_weighted:
                            lastValidState = state_prev;
                            break;
                        default: break;
                    }
                }
                Serial.print(F("\n\nEstado anterior: "));    printStateName(state_prev);      Serial.println();
                Serial.print(F("Nuevo estado: "));           printStateName(state_new);       Serial.println();
                Serial.print(F("Último estado válido: "));   printStateName(lastValidState);  Serial.println();
            }
            else if((state_actual != STATE_ERROR) and (state_actual != STATE_CANCEL) and (state_actual != STATE_AVISO)){ 
                                                    // Para evitar seguir marcando error durante los 3 segundos que no se cumple
                                                   // ninguna regla de transición porque se está en el estado de error.
                    // ¡¡¡ CHEQUEAR ESTO !!!! ¿HACE FALTA STATE_CANCEL Y STATE_AVISO?
                //Serial.println(F("\nERROR DE EVENTO"));
                actEventError();       // Mensaje de error por evento erróneo según el estado actual
            }
            flagEvent = false;
       }

        
    }
    
}
