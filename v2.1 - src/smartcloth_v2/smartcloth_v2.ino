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

// Error al inicializar la SD
int falloCriticoSD = false;


/*---------------------------------------------------------------------------------------------------------
   \brief Función para inicializar el RTC, la SD, la pantalla, la báscula, las botoneras y las interrupciones
----------------------------------------------------------------------------------------------------------*/
void setup() {
    // ------ COMUNICACIÓN SERIAL --------------
    #if defined(SM_DEBUG)
    // Inicializar comunicación con PC (Serial)
    SerialPC.begin(115200); //115200
    while (!SerialPC); // Eliminar en programa final, cuando el PC no esté conectado
    delay(100);
    #endif // SM_DEBUG

    // Inicializar comunicación con ESP32-CAM (Serial1)
    SerialDueESP32.begin(115200);
    while (!SerialDueESP32);
    delay(100); 
    // -----------------------------------------


    // ------ RTC ------------------------------
    setupRTC(); 
    delay(100); 
    // -----------------------------------------


    // ------ SD card --------------------------
    // Incluye crear fichero .csv, si no existe, y sumar en "Acumulado hoy" las comidas guardadas el día de hoy
    if (!setupSDcard()) falloCriticoSD = true; // Si la SD falla o no se encuentra, no se permitirá usar SM
    //setupSDcard();
    delay(100); 
    // -----------------------------------------


    // ------ SCALE ----------------------------
    setupScale();   
    delay(100); 
    // -----------------------------------------
    
    
    // ------ SCREEN ---------------------------
    setupScreen();  
    delay(100); 
    // -----------------------------------------

    // ------ FICHERO TXT ----------------------
    bool dataToUpload;
    if(!falloCriticoSD){ // Si falló la SD, no se puede chequear el fichero txt
        dataToUpload = checkFileESP32();
        if(dataToUpload) {
            #if defined(SM_DEBUG)
            SerialPC.println(F("Hay data para subir\n"));
            #endif //SM_DEBUG
        }
    }
    // -----------------------------------------
    

    // ------- ESTADO INICIAL ------------------
    // Si falla la SD, se inicia en el estado STATE_CRITIC_FAILURE_SD que no tiene transiciones
    // de entrada ni de salida. Te obliga a reiniciar SM para intentar subsanar el fallo.
    // Si no falla la SD, se inicia en el primer estado funcional.
    if (falloCriticoSD) state_actual = STATE_CRITIC_FAILURE_SD; // Estado sin salida
    else if(dataToUpload) state_actual = STATE_UPLOAD_DATA; // Estado para subir data a database
    else state_actual = STATE_Init; // Estado inicial de la Máquina de Estados
    //state_actual = STATE_Init;
    // -----------------------------------------


    // ---------- BUTTONS PINS ----------------- 
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
    // -----------------------------------------

  
    // --------- INTERRUPTIONS -----------------
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
    // -----------------------------------------


    // ------- BIENVENIDA ----------------------
    // Si no falla la SD, cargar las imágenes y mostrar pantalla de bienvenida.
    // Si falla, directamente se mostrará la pantalla de "FALLO EN MEMORIA"
    if (!falloCriticoSD) welcome();  // Cargar imágenes y mostrar logo de SmartCloth
    //welcome();
    // -----------------------------------------

}




/*---------------------------------------------------------------------------------------------------------
   loop(): Función principal ejecutada continuamente
----------------------------------------------------------------------------------------------------------*/
void loop() {
    
    if (millis() - prevMillis > period){
        prevMillis = millis();
                
        doStateActions();   // Actividades del estado actual. Comienza en STATE_Init (o en STATE_CRITIC_FAILURE_SD si ha fallado la SD)

        // Si no ha ocurrido un fallo al inicializar la SD, se chequean cambios en la Máquina de Estados
        if (!falloCriticoSD){

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
                    SerialPC.print(F("\n\nEstado anterior: "));    printStateName(state_prev);      SerialPC.println();
                    SerialPC.print(F("Nuevo estado: "));           printStateName(state_new);       SerialPC.println();
                    SerialPC.print(F("Último estado válido: "));   printStateName(lastValidState);  SerialPC.println();
                }
                else if((state_actual != STATE_ERROR) and (state_actual != STATE_CANCEL) and (state_actual != STATE_AVISO)){ 
                                                        // Para evitar seguir marcando error durante los 3 segundos que no se cumple
                                                      // ninguna regla de transición porque se está en el estado de error.
                        // ¡¡¡ CHEQUEAR ESTO !!!! ¿HACE FALTA STATE_CANCEL Y STATE_AVISO?
                    //SerialPC.println(F("\nERROR DE EVENTO"));
                    actEventError();       // Mensaje de error por evento erróneo según el estado actual
                }
                flagEvent = false;

          }
          
        }
        
    }
    
}
