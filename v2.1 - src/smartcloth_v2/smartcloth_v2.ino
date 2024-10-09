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


// ------ LIBRERÍAS -------------------------
#include "debug.h" // SM_DEBUG --> SerialPC
//#include "Serial_esp32cam.h" //Comunicación con ESP32-CAM

#include "Buttons.h"       
/*
    Estructura de archivos:

    - Buttons.h 
        - ISR.h 
            - Scale.h
                - State_Machine.h (eventos)
                    - Serial_esp32cam.h
                    - SD_functions.h
                        - lista_Comida.h
                        - RTC.h
                        - Files.h
                        - Diario.h 
                            - Comida.h 
                                - Plato.h 
                                    - Alimento.h
                                        - Valores_Nutricionales.h
                                        - Grupos.h
                    - Screen.h 
                        - RA8876_v2.h
                            - COLORS.h
*/
// ------------------------------------------


/* Time */
const unsigned long   period = 50;
unsigned long         prevMillis = 0;
unsigned long         tiempoPrevio = 0;

// Error al inicializar la SD
bool falloCriticoSD = false;


/*---------------------------------------------------------------------------------------------------------
   setup(): Función para inicializar el RTC, la SD, la pantalla, la báscula, las botoneras y las interrupciones
----------------------------------------------------------------------------------------------------------*/
void setup() 
{
    // ------ COMUNICACIÓN SERIAL --------------
    #if defined(SM_DEBUG)
    setupSerialPC();
    #endif
    // -----------------------------------------


    // ------ RTC ------------------------------
    #if defined(SM_DEBUG)
        SerialPC.println("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++");
        SerialPC.println(F("Inicializando RTC..."));
    #endif
    setupRTC(); 
    delay(100); 
    // -----------------------------------------


    // ------ SD card --------------------------
    #if defined(SM_DEBUG)
        SerialPC.println("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++");
        SerialPC.println(F("Inicializando tarjeta SD..."));
    #endif
    // Incluye crear fichero .csv, si no existe, y sumar en "Acumulado hoy" las comidas guardadas el día de hoy
    if (!setupSDcard()) falloCriticoSD = true; // Si la SD falla o no se encuentra, no se permitirá usar SM
    delay(100); 
    // -----------------------------------------


    // ------ SCALE ----------------------------
    #if defined(SM_DEBUG)
        SerialPC.println("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++");
        SerialPC.println(F("Inicializando scale..."));
    #endif
    setupScale();   
    delay(100); 
    // -----------------------------------------
    
    
    // ------ SCREEN ---------------------------
    #if defined(SM_DEBUG)
        SerialPC.println("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++");
        SerialPC.println(F("Inicializando pantalla..."));
    #endif
    setupScreen();  
    delay(100); 
    // -----------------------------------------

    // ------ FICHERO TXT ----------------------
    bool dataToUpload;
    if(!falloCriticoSD){ // Si falló la SD, no se puede chequear el fichero txt
        dataToUpload = !isFileTXTEmpty(); // Si el fichero no está vacío, hay data para subir
        if(dataToUpload) {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Hay data para subir"));
                SerialPC.println("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
            #endif //SM_DEBUG
        }
    }
    // -----------------------------------------
    

    // ------- ESTADO INICIAL ------------------
    // Si falla la SD, se inicia en el estado STATE_CRITIC_FAILURE_SD que no tiene transiciones
    // de entrada ni de salida. Te obliga a reiniciar SM para intentar subsanar el fallo
    if (falloCriticoSD) state_actual = STATE_CRITIC_FAILURE_SD;     // Estado sin salida
    // Si no falla la SD, se inicia en el primer estado funcional
    else if(dataToUpload) state_actual = STATE_UPLOAD_DATA;         // Estado para subir data a database
    else state_actual = STATE_Init;                                 // Estado inicial de la Máquina de Estados
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

    //  ----- BARCODE  -----
    pinMode(intPinBarcode, INPUT_PULLUP); // Entrada con resistencia pull-up interna
    // -----------------------------------------

  
    // --------- INTERRUPTIONS -----------------
    //  -----   MAIN    -----
    // RISING porque tienen resistencias pull-down que permiten activar al alza
    attachInterrupt(digitalPinToInterrupt(intPinCocinado), ISR_cocinado, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinCrudo), ISR_crudo, RISING);     
    attachInterrupt(digitalPinToInterrupt(intPinAddPlato), ISR_addPlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinDeletePlato), ISR_deletePlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinGuardar), ISR_guardar, RISING);
    
    //  -----   Grande    -----
    // RISING porque las filas tienen resistencias pull-down que permiten activar al alza
    attachInterrupt(digitalPinToInterrupt(intPinGrande), ISR_pulsandoButtonsGrande, RISING);

    //  -----   Barcode   -----
    // FALLING porque su resistencia (la interna del Due) está modo pull-up, así que el botón se activa a la baja
    attachInterrupt(digitalPinToInterrupt(intPinBarcode), ISR_barcode, FALLING); // Interrupción en flanco de bajada

    //  -----   Scale   ------
    attachDueInterrupt(HW_TIMER_INTERVAL_MS * 1000, TimerHandler, "ITimer");
    ISR_Timer.setInterval(TIMER_INTERVAL_5MS,  ISR_pesarBascula); //timer llama a 'ISR_pesarBascula' cada 100 ms
    // -----------------------------------------


    // ------- BIENVENIDA ----------------------
    // Si no falla la SD, cargar las imágenes y mostrar pantalla de bienvenida.
    // Si falla, directamente se mostrará la pantalla de "FALLO EN MEMORIA" en el estado STATE_CRITIC_FAILURE_SD
    if (!falloCriticoSD) welcome();  // Cargar imágenes y mostrar logo de SmartCloth
    // -----------------------------------------

    // ------ COMUNICACIÓN SERIAL --------------
    // Establecer el Serial del ESP32 al final para que le dé tiempo al ESP32 a encenderse, porque tarda más que el Due.
    // Si se hace al principio, el Due no puede configurar correctamente la comunicación Serial.
    setupSerialESP32();
    delay(100);
    // -----------------------------------------


}




/*---------------------------------------------------------------------------------------------------------
   loop(): Función principal ejecutada continuamente
----------------------------------------------------------------------------------------------------------*/
void loop() 
{    
    if (millis() - prevMillis > period)
    {
        prevMillis = millis();
                
        doStateActions();   // Actividades del estado actual. Comienza en STATE_Init (o en STATE_CRITIC_FAILURE_SD si ha fallado la SD)

        // Si no ha ocurrido un fallo al inicializar la SD, se chequean cambios en la Máquina de Estados
        if (!falloCriticoSD)
        {

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
                            case STATE_Init: case STATE_Plato: case STATE_Grupo: case STATE_Barcode: case STATE_raw: case STATE_cooked: case STATE_weighted:
                                lastValidState = state_prev;
                                break;
                            default: break;
                        }
                    }
                    #if defined SM_DEBUG
                        SerialPC.print(F("\nEstado anterior: "));       printStateName(state_prev);      SerialPC.println();
                        SerialPC.print(F("Nuevo estado: "));            printStateName(state_new);       SerialPC.println();
                        SerialPC.print(F("Ultimo estado valido: "));    printStateName(lastValidState);  SerialPC.println();
                        SerialPC.println("--------------------------------------------------");
                        SerialPC.println("--------------------------------------------------\n\n");
                    #endif
                }
                else if((state_actual != STATE_ERROR) and (state_actual != STATE_CANCEL) and (state_actual != STATE_AVISO))
                //else if(state_actual != STATE_ERROR) // PROBAR ESTO SOLO
                { 
                        // Se hace esta comprobación para evitar seguir marcando error durante los 3 segundos que no se cumple
                        // ninguna regla de transición porque se está en el estado de error.

                        // ¡¡¡ CHEQUEAR ESTO !!!! ¿HACE FALTA STATE_CANCEL Y STATE_AVISO?
                        // Creo que solo haría falta comprobar no estar en STATE_ERROR, porque en ese estado se marca todo el rato
                        // error porque la flagError está activa, entonces entra en la condición y al revisar las reglas se ve que no 
                        // ha ocurrido ningún evento de los establecidos en las reglas (lo que ha ocurrido es un error), por lo que
                        // entra en un ciclo "infinito" de marcar evento de error durante los 3 segundos que dura el estado (si es transitorio).
                        // La 'flagError' se desactiva en actStateERROR() tras los 3 segundos,
                    actEventError();       // Mensaje de error por evento erróneo según el estado actual
                }
                flagEvent = false;

          }
          
        }
        
    }
    
}
