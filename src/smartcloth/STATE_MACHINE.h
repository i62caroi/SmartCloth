#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define MAX_EVENTS 5
#define RULES 67 

#include "Screen.h"   // Incluye Variables.h (Diario.h -> Comida.h -> Plato.h -> Ingrediente.h -> Valores_Nutricionales.h)
#include "SD_functions.h"



bool doneState;       // Flag para que solo se realicen una vez las actividades del estado cada vez que se entre.


/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- ESTADOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
typedef enum {
              //STATE_Welcome       =   (1),    // Estado de bienvenida
              STATE_Empty         =   (1),    // Estado para transiciones intermedias con báscula vacía
              STATE_Plato         =   (2),    // Estado para colocar plato
              STATE_groupA        =   (3),    // grupo A
              STATE_groupB        =   (4),    // grupo B
              STATE_raw           =   (5),    // (grupo A) crudo
              STATE_cooked        =   (6),    // (grupo A) cocinado
              STATE_weighted      =   (7),    // plato pesado
              STATE_added         =   (8),    // plato añadido
              STATE_deleted       =   (9),    // plato eliminado
              STATE_saved         =   (10)    // comida guardada
} states_t;



/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- EVENTOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
typedef enum {
              
              NONE                =   (0),    
              TIPO_A              =   (1),    // Botonera Grande (7,8,9,16,17,18)
              TIPO_B              =   (2),    // Botonera Grande (1,2,3,4,5,6,10,11,12,13,14,15,19,20)
              CRUDO               =   (3),    // AMARILLO
              COCINADO            =   (4),    // BLANCO
              ADD_PLATO           =   (5),    // VERDE
              DELETE_PLATO        =   (6),    // ROJO
              GUARDAR             =   (7),    // NEGRO 
              INCREMENTO          =   (8),    // Báscula
              DECREMENTO          =   (9),    // Báscula
              TARAR               =   (10),   // Báscula tarada
              QUITAR              =   (11),   // Quitar de la báscula
              LIBERAR             =   (12)   // Báscula vacía real
} event_t;


static event_t event_buffer[MAX_EVENTS];       // Buffer de eventos al que se irán añadiendo conforme ocurran
/* Este buffer solo se utiliza para debuggear, para ver qué eventos han ido ocurriendo.
   Para comprobar si se cumple alguna regla de transición se utiliza 'lastEvent', que 
   sería el último evento añadido al buffer.
   
   Al final habría que eliminar el buffer para evitar ocupar memoria innecesariamente 
*/



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ REGLAS TRANSICION ---------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
typedef struct{
    states_t state_i;     // estado actual
    states_t state_j;     // estado siguiente
    event_t condition;    // condición cambio estado
}transition_rule;

static transition_rule rules[RULES] = { {STATE_Empty,STATE_Empty,TARAR},                // tara inicial
                                        {STATE_Empty,STATE_Plato,INCREMENTO},
                                        {STATE_Empty,STATE_saved,GUARDAR},              // Guardar comida directamente (comidaActual no está vacía)
                                        
                                        {STATE_Plato,STATE_Plato,INCREMENTO},           // cambios por recolocar plato
                                        {STATE_Plato,STATE_Plato,DECREMENTO},           // cambios por recolocar plato
                                        {STATE_Plato,STATE_Empty,LIBERAR},              // se ha retirado el recipiente
                                        {STATE_Plato,STATE_groupA,TIPO_A},                   
                                        {STATE_Plato,STATE_groupB,TIPO_B},                   
                                        
                                        {STATE_groupA,STATE_Empty,LIBERAR},             // se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_groupA,STATE_groupA,TIPO_A},                
                                        {STATE_groupA,STATE_groupA,QUITAR},             // para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_groupA,STATE_groupA,TARAR},              // tarar tras colocar recipiente o alimento   
                                        {STATE_groupA,STATE_groupB,TIPO_B},                
                                        {STATE_groupA,STATE_raw,CRUDO},                         
                                        {STATE_groupA,STATE_cooked,COCINADO},              
                                        {STATE_groupA,STATE_weighted,INCREMENTO},       // aprovechar 'crudo' predeterminado para pesar directamente
                                        {STATE_groupA,STATE_added,ADD_PLATO},           // Guardar plato aunque no se haya añadido otro alimento
                                        {STATE_groupA,STATE_deleted,DELETE_PLATO},      // Borrar plato actual
                                        {STATE_groupA,STATE_saved,GUARDAR},             // Guardar comida aunque no se haya añadido otro alimento
                                        
                                        {STATE_groupB,STATE_Empty,LIBERAR},             // se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_groupB,STATE_groupB,TIPO_B},                
                                        {STATE_groupB,STATE_groupB,QUITAR},             // para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_groupB,STATE_groupB,TARAR},              // tarar tras colocar recipiente o alimento   
                                        {STATE_groupB,STATE_groupA,TIPO_A},                
                                        {STATE_groupB,STATE_raw,CRUDO},                         
                                        {STATE_groupB,STATE_cooked,COCINADO},              
                                        {STATE_groupB,STATE_weighted,INCREMENTO},       // aprovechar 'crudo' predeterminado para pesar directamente  
                                        {STATE_groupB,STATE_added,ADD_PLATO},           // Guardar plato aunque no se haya añadido otro alimento
                                        {STATE_groupB,STATE_deleted,DELETE_PLATO},      // Borrar plato actual
                                        {STATE_groupB,STATE_saved,GUARDAR},             // Guardar comida aunque no se haya añadido otro alimento  
                                        
                                        {STATE_raw,STATE_cooked,COCINADO},                 
                                        {STATE_raw,STATE_groupA,TIPO_A},                   
                                        {STATE_raw,STATE_groupB,TIPO_B},                   
                                        {STATE_raw,STATE_weighted,INCREMENTO},     
                                        {STATE_raw,STATE_added,ADD_PLATO},              // Guardar plato aunque no se haya añadido otro alimento
                                        {STATE_raw,STATE_deleted,DELETE_PLATO},         // Borrar plato actual
                                        {STATE_raw,STATE_saved,GUARDAR},                // Guardar comida aunque no se haya añadido otro alimento          
                                        
                                        {STATE_cooked,STATE_raw,CRUDO},                    
                                        {STATE_cooked,STATE_groupA,TIPO_A},                
                                        {STATE_cooked,STATE_groupB,TIPO_B},                
                                        {STATE_cooked,STATE_weighted,INCREMENTO},   
                                        {STATE_cooked,STATE_added,ADD_PLATO},           // Guardar plato aunque no se haya añadido otro alimento
                                        {STATE_cooked,STATE_deleted,DELETE_PLATO},      // Borrar plato actual
                                        {STATE_cooked,STATE_saved,GUARDAR},             // Guardar comida aunque no se haya añadido otro alimento         
                                         
                                        
                                        {STATE_weighted,STATE_Empty,LIBERAR},           // se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_weighted,STATE_weighted,INCREMENTO},     // se coloca más alimento  
                                        {STATE_weighted,STATE_weighted,DECREMENTO},     // se retira alimento   
                                        {STATE_weighted,STATE_weighted,QUITAR},         // para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_weighted,STATE_groupA,TIPO_A},              
                                        {STATE_weighted,STATE_groupB,TIPO_B},              
                                        {STATE_weighted,STATE_added,ADD_PLATO},            
                                        {STATE_weighted,STATE_deleted,DELETE_PLATO},       
                                        {STATE_weighted,STATE_saved,GUARDAR},              
                                        
                                        {STATE_added,STATE_added,TARAR},                 // taramos para saber (en negativo) cuánto se va quitando
                                        {STATE_added,STATE_added,QUITAR},                // para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_added,STATE_added,INCREMENTO},            // para evitar error de evento cuando, al retirar el plato, puede detectar un ligero incremento
                                        {STATE_added,STATE_Empty,LIBERAR},               // se ha retirado el plato completo (+ recipiente)  
                                        {STATE_added,STATE_saved,GUARDAR},               // guardar la comida tras decir que se quiere añadir plato, aunque no se haga
                                        
                                        {STATE_deleted,STATE_deleted,TARAR},             // taramos para saber (en negativo) cuánto se va quitando   
                                        {STATE_deleted,STATE_deleted,QUITAR},            // para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_deleted,STATE_deleted,INCREMENTO},        // para evitar error de evento cuando, al retirar el plato, puede detectar un ligero incremento
                                        {STATE_deleted,STATE_Empty,LIBERAR},             // se ha retirado el plato completo (+ recipiente) 
                                        {STATE_deleted,STATE_saved,GUARDAR},             // guardar la comida tras borrar el plato actual  
                                        
                                        {STATE_saved,STATE_saved,TARAR},                 // taramos para saber (en negativo) cuánto se va quitando
                                        {STATE_saved,STATE_saved,QUITAR},                // para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_saved,STATE_saved,INCREMENTO},            // para evitar error de evento cuando, al retirar el plato, puede detectar un ligero incremento
                                        {STATE_saved,STATE_Empty,LIBERAR}                // se ha retirado el plato completo (+ recipiente)   
                                      };



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ VARIABLES ESTADOS/EVENTOS -------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

states_t state_actual;      // Estado actual
states_t state_new;         // Nuevo estado al que se va a pasar
states_t state_prev;        // Estado anterior. Solo utilizado para saber si se debe guardar 
                            // nuevo ing en add/delete/save porque no se venga de STATE_Empty

event_t lastEvent;          // Último evento ocurrido

event_t eventoMain;         // Evento ocurrido en botonera Main
event_t eventoGrande;       // Evento ocurrido en botonera grande
event_t eventoBascula;      // Evento ocurrido en báscula


/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
bool checkStateConditions();                        // Comprobar reglas de transición de estados
void actStateEmpty();                               // Actividades STATE_Empty
void actStatePlato();                               // Actividades STATE_Plato
void actGruposAlimentos();                          // Actividades STATE_groupA y STATE_groupB
void actStateRaw();                                 // Actividades STATE_raw
void actStateCooked();                              // Actividades STATE_cooked
void actStateWeighted();                            // Actividades STATE_weighted
void actStateAdded();                               // Actividades STATE_added
void actStateDeleted();                             // Actividades STATE_deleted
void actStateSaved();                               // Actividades STATE_saved
void doStateActions();                              // Actividades según estado actual
void actEventError();                               // Mensaje de error de evento según el estado actual
bool isBufferEmpty();                               // Comprobar buffer de eventos vacío
bool isBufferFull();                                // Comprobar buffer de eventos lleno
int firstGapBuffer();                               // Obtener primer hueco en el buffer
void shiftLeftEventBuffer();                        // Desplazar buffer a izquierda para incluir nuevo evento
void addEventToBuffer(event_t evento);              // Añadir evento al buffer
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- MOTOR INFERENCIA -----------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/ 

/*---------------------------------------------------------------------------------------------------------
   checkStateConditions(): Comprobar reglas de transición para conocer el próximo estado
----------------------------------------------------------------------------------------------------------*/
bool checkStateConditions(){
    for (int i = 0; i < RULES; i++){
        if(rules[i].state_i == state_actual){
            if(rules[i].condition == lastEvent){    
                state_new = rules[i].state_j;     // Nuevo estado
                doneState = false;                // Desactivar flag de haber hecho las actividades del estado
                return true;                        
            }
        }
    }
    return false;                                 // Si no se ha cumplido ninguna regla de transición ==> ERROR DE EVENTO
}




/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- ACCIONES ESTADOS -----------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/




/*---------------------------------------------------------------------------------------------------------
   actStateEmpty(): Acciones del STATE_Empty
----------------------------------------------------------------------------------------------------------*/
void actStateEmpty(){ 
    if(!doneState){
        Serial.println(F("\nSTATE_Empty...")); 
        
        tareScale();                              // Tara inicial
        
        pesoRecipiente = 0.0;                     // Se inicializa 'pesoRecipiente', que se sumará a 'pesoPlato' para saber el 'pesoARetirar'.
        pesoPlato = 0.0;                          // Se inicializa 'pesoPlato', que se sumará a 'pesoRecipiente' para saber el 'pesoARetirar'.
        
        printStateEmpty();                        // Print info estado.
        
        doneState = true;                         // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                  // cada vez que se entre a esta función debido al loop de Arduino.
                                                  // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                  // las acciones.
    }
} 


/*---------------------------------------------------------------------------------------------------------
   actStatePlato(): Acciones del STATE_Plato
----------------------------------------------------------------------------------------------------------*/
void actStatePlato(){ 
    if(!doneState){
        Serial.println(F("\nPlato colocado")); 
        
        tarado = false;                           // Desactivar flag de haber tarado.

                                                  // El peso definitivo del recipiente no se guarda hasta que se escoja grupo de alimentos.
                                                  // Por eso, 'pesoRecipiente' y 'pesoARetirar' son iguales a 0.0 en este estado. Esto no impide que se
                                                  // realice LIBERAR, ya que ese evento ocurre cuando el peso de la báscula es igual a 'pesoARetirar'. 
                                                  // Así, se puede detectar que se ha quitado el recipiente (bascula = 0.0) y se vuelve al estado STATE_Empty.
        
        printStatePlato();                        // Print info del estado.
        
        doneState = true;                         // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                  // cada vez que se entre a esta función debido al loop de Arduino.
                                                  // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                  // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_groupA o STATE_groupB
----------------------------------------------------------------------------------------------------------*/
void actGruposAlimentos(){ 
    if(!doneState){
        
        procesamiento = "CRUDO";                                            // El procesamiento es 'CRUDO' de forma predeterminada. 
        
        if(state_prev == STATE_Plato){                                      // ==> Si se viene de STATE_Plato porque se acaba de colocar el recipiente.
            pesoRecipiente = pesoBascula;                                   // Se guarda 'pesoRecipiente' para sumarlo a 'pesoPlato' y saber el 'pesoARetirar'.
            tareScale();                                                    // Tarar para tomar peso real del alimento que se va a colocar.
        }
        else if(state_prev == STATE_weighted){                              // ==> Si se viene STATE_weighted porque se ha colocado un ingrediente
                                                                            //     que se debe guardar
                Serial.println(F("Añadiendo ingrediente al plato..."));
                
                Ingrediente ing(grupoAnterior, pesoBascula);                // Cálculo automático de valores nutricionales.
                                                                            // Al escoger un nuevo grupo se guarda el ingrediente del grupo anterior
                                                                            // colocado en la iteración anterior. Por eso se utiliza 'grupoAnterior' para
                                                                            // crear el ingrediente, porque 'grupoEscogido' ya ha tomado el valor del
                                                                            // nuevo grupo.
                                                             
                platoActual.addIngPlato(ing);                               // Ingrediente ==> Plato
                comidaActual.addIngComida(ing);                             // Ingrediente ==> Comida

                pesoPlato = platoActual.getPesoPlato();                     // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.

                tareScale();                                                // Tarar cada vez que se seleccione un grupo de alimentos nuevo.
                
        }
        
        printStateABandProcessed();                                         // Print info del estado.
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                            // las acciones.
    }
}




/*---------------------------------------------------------------------------------------------------------
   actStateRaw(): Acciones del STATE_raw
----------------------------------------------------------------------------------------------------------*/
void actStateRaw(){ 
    if(!doneState){
        Serial.println(F("\nAlimento crudo...")); 
        
        procesamiento = "CRUDO";                                            // Procesamiento del alimento a 'CRUDO'. 
        
        if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
            tarado = false;                                                 // Desactivar flag de haber 'tarado' 
        }
        
        printStateABandProcessed();                                         // Print info del estado.
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                            // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateCooked(): Acciones del STATE_cooked
----------------------------------------------------------------------------------------------------------*/
void actStateCooked(){ 
    if(!doneState){
        Serial.println(F("\nAlimento cocinado...")); 
        
        procesamiento = "COCINADO";                                         // Procesamiento del alimento a 'COCINADO'.  
        
        if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
            tarado = false;                                                 // Desactivar flag de haber 'tarado' 
        }
        
        printStateABandProcessed();                                         // Print info del estado.
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                            // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateWeighted(): Acciones del STATE_weighted
----------------------------------------------------------------------------------------------------------*/
void actStateWeighted(){ 
    if(!doneState){
        Serial.println(F("\nAlimento pesado...")); 

        if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
            tarado = false;                                                 // Desactivar flag de haber 'tarado' 
        }
        
        printStateWeighted();                                               // Print info del estado.
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                            // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateAdded(): Acciones del STATE_added
----------------------------------------------------------------------------------------------------------*/
void actStateAdded(){ 
    if(!doneState){
        static bool errorPlatoWasEmpty;                             // Flag utilizada para saber si se debe mostrar la información "normal"
                                                                    // o un mensaje de aviso indicando no se ha creado otro plato porque el 
                                                                    // actual ya está vacío.

        if(state_prev != STATE_added){                              // ==> Si no se viene del propio STATE_added, para evitar que se vuelva 
                                                                    //     a guardar el plato en la comida.
          
                                                                    // *****
                                                                    // Primero se actualiza el plato, si es necesario, y después se muestran las actualizaciones.
                                                                    // Si tras comprobar cambios en el plato se ve que sigue vacío, entonces se indica en un mensaje 
                                                                    // que no se ha podido crear otro.
                                                                    // *****
            
            Serial.println(F("Añadiendo plato a la comida..."));
                                                                        
            /* ----- ACTUALIZAR PLATO  ----- */
            if((state_prev == STATE_weighted) and (pesoBascula != 0.0)){   // ==> Si se viene del STATE_weighted, porque se ha colocado algo nuevo en la báscula,
                                                                           //     y el pesoBascula marca algo, indicando que lo que se ha colocado no se ha retirado,
                                                                           //     debe incluirse en el plato. 
                                                                           // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                                           // añadir un nuevo plato sin haber colocado un nuevo alimento, no haría falta
                                                                           // actualizar la comida, pues no se habría modificado.
                
                Ingrediente ing(grupoEscogido, pesoBascula);        // Cálculo automático de valores nutricionales. 
                                                                    // Usamos 'grupoEscogido' porque no se ha modificado. 
                
                platoActual.addIngPlato(ing);                       // Ingrediente ==> Plato
                comidaActual.addIngComida(ing);                     // Ingrediente ==> Comida

                /* ----- TARAR  ----- */
                tareScale();                                        // Se debe tarar para que conforme vaya disminuyendo el peso veamos si 
                                                                    // se ha quitado todo el 'pesoARetirar'.
                                                                    // Solo se hace si se viene del STATE_weighted porque en los estados 
                                                                    // previos a ese ya se tara y es en STATE_weighted donde se modifica el peso.
                
            }


            /* ----- GUARDAR PLATO EN COMIDA  ----- */
            if(platoActual.isPlatoEmpty()){                         // ==> Si el plato está vacío -> no se crea otro    
                errorPlatoWasEmpty = true;
                printEmptyObjectError("No se ha creado otro plato porque el actual est\xE1"" vac\xED""o");
                //printEmptyObjectError("No se ha guardado el plato porque est\xE1"" vac\xED""o");
            }
            else{                                                   // ==> Si el plato no está vacío -> se crea otro.
                errorPlatoWasEmpty = false;
                pesoPlato = platoActual.getPesoPlato();             // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                comidaActual.addPlato(platoActual);                 // Plato ==> Comida (no se vuelve a incluir el ingrediente. Solo modifica peso y nPlatos)
                platoActual.restorePlato();                         // "Reiniciar" platoActual para usarlo de nuevo.
            }

        }
        else{                                                       // ==> Si se viene del propio STATE_added, donde se puede haber tarado.
            tarado = false;                                         // Desactivar flag de haber 'tarado'.          
        }
                                                 

        /* -----  INFORMACIÓN MOSTRADA  ----- */
        if(!errorPlatoWasEmpty){                                    // ==> Si el plato no estaba vacío y se ha guardado/creado otro 
               printStateAdded();                                   // Print info del estado.
        }

        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                    // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateDeleted(): Acciones del STATE_deleted
----------------------------------------------------------------------------------------------------------*/
void actStateDeleted(){
    if(!doneState){
        static bool errorPlatoWasEmpty;                             // Flag utilizada para saber si se debe mostrar la información "normal"
                                                                    // o un mensaje de aviso indicando no se ha borrado el plato porque el 
                                                                    // actual ya está vacío.
        
        if(state_prev != STATE_deleted){                            // ==> Si no se viene del propio STATE_deleted, para evitar que se vuelva 
                                                                    //     a eliminar el plato de la comida.
          
                                                                    // *****
                                                                    // Primero se actualiza el plato, si es necesario, y después se muestran las actualizaciones.
                                                                    // Si tras comprobar cambios en el se ve que sigue vacía, entonces se indica en un mensaje 
                                                                    // que no se ha podido guardar. 
                                                                    // *****
            
            Serial.println(F("\nPlato eliminado..."));

            
            /* ----- PESO ÚLTIMO ALIMENTO  ----- */
            if((state_prev == STATE_weighted) and (pesoBascula != 0.0)){   // ==> Si se viene del STATE_weighted, porque se ha colocado algo nuevo en la báscula,
                                                                           //     y el pesoBascula marca algo, indicando que lo que se ha colocado no se ha retirado,
                                                                           //     debe incluirse en el plato. 
                                                                           // En este caso no se va a guardar el alimento porque se va a borrar el plato.
                                                                           // Solamente se añadirá su peso al 'pesoPlato' para, tras sumarlo a 'pesoRecipiente', 
                                                                           // saber el 'pesoARetirar'.
                                                                           // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                                           // eliminar el plato actual sin haber colocado un nuevo alimento, no haría falta
                                                                           // actualizar el peso del plato, pues no se habría modificado.
                                                         
                pesoPlato = pesoBascula;                            // Guardar peso del último alimento colocado

                /* ----- TARAR  ----- */
                tareScale();                                        // Se debe tarar para que conforme vaya disminuyendo el peso veamos si 
                                                                    // se ha quitado todo el 'pesoARetirar'.
                                                                    // Solo se hace si se viene del STATE_weighted porque en los estados 
                                                                    // previos a ese ya se tara y es en STATE_weighted donde se modifica el peso.
            }


            /* ----- BORRAR PLATO DE COMIDA  ----- */
                 // Se debe avisar de que no se puede seguir borrando:
                 // - Si el plato no está vacío, se borra.
                 // - Si el plato está vacío pero la comida no, se borra el último guardado.
                 // - Si el plato está vacío y la comida también, entonces se avisa de que no hay nada que borrar.
            if(platoActual.isPlatoEmpty()){                         // ==> Si el plato está vacío -> no se borra    
                errorPlatoWasEmpty = true;
                printEmptyObjectError("No se ha borrado el plato porque est\xE1"" vac\xED""o");
            }
            else{                                                   // ==> Si el plato no está vacío -> se borra de la comida actual.
                errorPlatoWasEmpty = false;
                pesoPlato += platoActual.getPesoPlato();            // Se actualiza el 'pesoPlato', que podría ya incluir el peso del último alimento
                                                                    // que no se llegó a guardar en el plato, para sumarlo a 'pesoRecipiente' y
                                                                    // saber el 'pesoARetirar'.
                                                             
                comidaActual.deletePlato(platoActual);              // Borrar plato de la comida.
                platoActual.restorePlato();                         // "Reiniciar" platoActual para usarlo de nuevo.
            }

        }
        else{                                                       // ==> Si se viene del propio STATE_deleted, donde se puede haber tarado.
            tarado = false;                                         // Desactivar flag de haber 'tarado'.          
        }


        /* -----  INFORMACIÓN MOSTRADA  ----- */
        if(!errorPlatoWasEmpty){                                    // ==> Si el plato no estaba vacío y se ha borrado
               printStateDeleted();                                 // Print info del estado.
        }
        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                    // las acciones.
    }
}



/*---------------------------------------------------------------------------------------------------------
   actStateSaved(): Acciones del STATE_saved
----------------------------------------------------------------------------------------------------------*/
void actStateSaved(){
    if(!doneState){
        static bool errorComidaWasEmpty;                            // Flag utilizada para saber si se debe mostrar la información "normal"
                                                                    // o un mensaje de aviso indicando no se ha guardado la comida porque 
                                                                    // ya está vacía.
        
        if(state_prev != STATE_saved){                              // ==> Si no se viene del propio STATE_saved, para evitar que se vuelva 
                                                                    //     a guardar la comida en el diario.
            
                                                                    // *****
                                                                    // Primero se actualiza la comida, si es necesario, y después se muestran las actualizaciones.
                                                                    // Si tras comprobar cambios en la comida se ve que sigue vacía, entonces se indica en un mensaje 
                                                                    // que no se ha podido guardar.   
                                                                    // *****                  
          
            Serial.println(F("\nComida guardada..."));  

            /* ----- ACTUALIZAR PLATO  ----- */
            if((state_prev == STATE_weighted) and (pesoBascula != 0.0)){   // ==> Si se viene del STATE_weighted, porque se ha colocado algo nuevo en la báscula,
                                                                           //     y el pesoBascula marca algo, indicando que lo que se ha colocado no se ha retirado,
                                                                           //     debe incluirse en el plato. 
                                                                           // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                                           // guardar la comida actual sin haber colocado un nuevo alimento, no haría falta
                                                                           // actualizar la comida, pues no se habría modificado.
            
                Ingrediente ing(grupoEscogido, pesoBascula);        // Cálculo automático de valores nutricionales.
                                                                    // Usamos 'grupoEscogido' porque no se ha modificado. 
                
                platoActual.addIngPlato(ing);                       // Ingrediente ==> Plato
                comidaActual.addIngComida(ing);                     // Ingrediente ==> Comida

                /* ----- TARAR  ----- */
                tareScale();                                        // Se debe tarar para que conforme vaya disminuyendo el peso veamos si 
                                                                    // se ha quitado todo el 'pesoARetirar'.
                                                                    // Solo se hace si se viene del STATE_weighted porque en los estados 
                                                                    // previos a ese ya se tara y es en STATE_weighted donde se modifica el peso.
            }


            /* ----- GUARDAR PLATO EN COMIDA  ----- */
            if(!platoActual.isPlatoEmpty()){                        // ==> Si el plato no está vacío, se guarda en la comida actual.
                                                                    // Si se viene de STATE_added o STATE_deleted, el plato ya se habrá
                                                                    // guardado en la comida y estará vacío nuevamente.
                    pesoPlato = platoActual.getPesoPlato();         // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                    comidaActual.addPlato(platoActual);             // Plato ==> Comida (no se vuelve a incluir el ingrediente. Solo modifica peso y nPlatos)
                    platoActual.restorePlato();                     // "Reiniciar" platoActual para usarlo de nuevo.
            }


            /* ----- GUARDAR COMIDA EN DIARIO  ----- */
            if(comidaActual.isComidaEmpty()){                       // ==> Si la comida está vacía -> no se guarda
                errorComidaWasEmpty = true;
                if(state_prev == STATE_Empty){
                    printEmptyObjectError("No se puede guardar la comida porque est\xE1"" vac\xED""a");
                    delay(3000);                                    // Tiempo para mostrar mensaje de aviso ya que, en este caso,
                                                                    // el regreso a STATE_Empty es automático porque la báscula está libre (LIBERAR).
                }
                else{                                               // En los otros estados (groupA, groupB, raw, cooked, weighted) habrá recipiente,
                                                                    // por lo que el regreso a STATE_Empty no es automático, sino que se debe liberar la báscula.
                    printEmptyObjectError("No se puede guardar la comida porque est\xE1"" vac\xED""a. \n Si ha puesto un plato, ret\xED""relo para empezar de nuevo.");
                }
            }
            else{                                                   // ==> Si la comida no está vacía -> se guarda en el diario.
                errorComidaWasEmpty = false;
                diaActual.addComida(comidaActual);                  // Comida ==> Diario
                comidaActual.restoreComida();                       // "Reiniciar" comidaActual para usarla de nuevo.
            }
        
        }
        else{                                                       // ==> Si se viene del propio STATE_saved, donde se puede haber tarado.
            tarado = false;                                         // Desactivar flag de haber 'tarado'.          
        }


        /* -----  INFORMACIÓN MOSTRADA  ----- */
        if(!errorComidaWasEmpty){                                   // ==> Si la comida no estaba vacía y se ha guardado
               printStateSaved();                                   // Print info del estado.
        }
        
        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                    // las acciones.
    }
}



/*---------------------------------------------------------------------------------------------------------
   doStateActions(): Acciones a realizar según el estado actual
----------------------------------------------------------------------------------------------------------*/
void doStateActions(){
    switch (state_actual){
      case 1:   actStateEmpty();         break;
      case 2:   actStatePlato();        break;
      case 3:   actGruposAlimentos();   break;
      case 4:   actGruposAlimentos();   break;
      case 5:   actStateRaw();          break;
      case 6:   actStateCooked();       break;
      case 7:   actStateWeighted();     break;
      case 8:   actStateAdded();        break;
      case 9:   actStateDeleted();      break;
      case 10:  actStateSaved();        break;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actEventError(): Información de error según estado cuando ocurre un evento que no correspondía
----------------------------------------------------------------------------------------------------------*/
void actEventError(){
    switch (state_actual){
      case 1:   printEventError("Coloque un recipiente sobre la b\xE1""scula");                                   break;  // Empty
      case 2:   printEventError("Escoja un grupo de alimentos");                                                  break;  // Plato
      case 3:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \n grupo");            break;  // grupoA 
      case 4:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \n grupo");            break;  // grupoB
      case 5:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \n grupo");            break;  // Crudo
      case 6:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \n grupo");            break;  // Cocinado
      case 7:   printEventError("Escoja grupo para otro alimento, a\xF1""ada otro plato o \n  guarde la comida"); break;  // Pesado
      case 8:   printEventError("Retire el plato para comenzar uno nuevo");                                       break;  // Add
      case 9:   printEventError("Retire el plato que acaba de eliminar");                                         break;  // Delete
      case 10:  printEventError("Retire el plato");                                                               break;  // Save
    }
}


/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- BUFFER EVENTOS -------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------
   isBufferEmpty(): Comprobar si el buffer está vacío
----------------------------------------------------------------------------------------------------------*/
bool isBufferEmpty(){
    if(event_buffer[0] == NONE) return true;
    else return false;
}


/*---------------------------------------------------------------------------------------------------------
   isBufferFull(): Comprobar si el buffer está lleno
----------------------------------------------------------------------------------------------------------*/
bool isBufferFull(){
    for (int i = 0; i < MAX_EVENTS; i++){
        if (event_buffer[i] == NONE){           // Hay algún hueco         
            return false;
        }
    }
    return true;
}


/*---------------------------------------------------------------------------------------------------------
   firstGapBuffer(): Primer hueco libre del buffer
----------------------------------------------------------------------------------------------------------*/
int firstGapBuffer(){
    for (int i = 0; i < MAX_EVENTS; i++){
        if (event_buffer[i] == NONE) return i;     // Primer hueco
    }
    return 0; // No debería alcanzar este 'return' porque esta función
              // solo se llama si el buffer no está vacío, con lo que
              // siempre encontrará el hueco. Aun así, se ha incluido
              // esta línea para evitar la posibilidad de terminar la
              // función sin haber devuelto nada.
}


/*---------------------------------------------------------------------------------------------------------
   shiftLeftEventBuffer(): Mover elementos del buffer a la izq para liberar un hueco sin perder 
                           los eventos previos.
----------------------------------------------------------------------------------------------------------*/
void shiftLeftEventBuffer(){ 
    Serial.println(F("\nRotando buffer de eventos..."));
    for (int i = 0; i < MAX_EVENTS; i++){
        if(i < (MAX_EVENTS-1)){
            event_buffer[i] = event_buffer[i+1];
        }
        else if (i == (MAX_EVENTS-1)){
            event_buffer[i] = NONE;
        }
    }
}


/*---------------------------------------------------------------------------------------------------------
   addEventToBuffer(): Añadir el último evento al buffer de eventos
----------------------------------------------------------------------------------------------------------*/
void addEventToBuffer(event_t evento){
    Serial.println(F("\n\n***********************************"));
    Serial.println(F("Añadiendo evento al buffer..."));
    int pos;
    if(isBufferEmpty()){
        pos = 0;
    }
    else{
        if(isBufferFull()){
            shiftLeftEventBuffer();                 // Se rota array a izq y se libera el último hueco
            pos = MAX_EVENTS-1;                  // Último hueco
        }
        else{
            pos = firstGapBuffer();
        }
    }
    event_buffer[pos] = evento;                  // Añadir a buffer
    lastEvent = evento;
    Serial.print("Buffer: "); 
    for (int i = 0; i < MAX_EVENTS; i++){
        Serial.print(event_buffer[i]); Serial.print(" ");
    }
    Serial.print(F("Last event: ")); Serial.println(lastEvent);
}





#endif
