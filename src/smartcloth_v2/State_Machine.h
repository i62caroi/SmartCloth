#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define MAX_EVENTS 5
#define RULES 93 

#include "Screen.h"   // Incluye Variables.h (Diario.h -> Comida.h -> Plato.h -> Alimento.h -> Valores_Nutricionales.h)
#include "SD_functions.h"



bool doneState;       // Flag para que solo se realicen una vez las actividades del estado cada vez que se entre.


/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- ESTADOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
typedef enum {
              STATE_Empty         =   (1),    // Estado para colocar plato
              STATE_Plato         =   (2),    // Estado para escoger grupo
              STATE_groupA        =   (3),    // grupo A
              STATE_groupB        =   (4),    // grupo B
              STATE_raw           =   (5),    // crudo
              STATE_cooked        =   (6),    // cocinado
              STATE_weighted      =   (7),    // alimento pesado
              STATE_add_check     =   (8),    // Comprobar que se quiere añadir plato
              STATE_added         =   (9),    // plato añadido
              STATE_delete_check  =   (10),   // Comprobar que se quiere eliminar plato
              STATE_deleted       =   (11),   // plato eliminado
              STATE_save_check    =   (12),   // Comprobar que se quiere guardar comida
              STATE_saved         =   (13)    // comida guardada
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
              LIBERAR             =   (12)    // Báscula vacía real
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

static transition_rule rules[RULES] = { // --- Esperando Recipiente ---
                                        {STATE_Empty,STATE_Empty,TARAR},                // Tara inicial
                                        {STATE_Empty,STATE_Plato,INCREMENTO},
                                        {STATE_Empty,STATE_save_check,GUARDAR},         // Guardar comida directamente (comidaActual no está vacía)
                                        //{STATE_Empty,STATE_deleted,DELETE_PLATO},       // Previamente se ha borrado el actual y se ha retirado, volviendo a INI. Ahora se quiere
                                                                                          // seguir borrando los anteriores.
                                        // ----------------------------

                                        // --- Recipiente colocado ---
                                        {STATE_Plato,STATE_Plato,INCREMENTO},           // Cambios por recolocar recipiente
                                        {STATE_Plato,STATE_Plato,DECREMENTO},           // Cambios por recolocar recipiente
                                        {STATE_Plato,STATE_Empty,LIBERAR},              // Se ha retirado el recipiente
                                        {STATE_Plato,STATE_groupA,TIPO_A},                   
                                        {STATE_Plato,STATE_groupB,TIPO_B},                   
                                        // ---------------------------

                                        // --- Alimentos grupo A ---
                                        {STATE_groupA,STATE_Empty,LIBERAR},             // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_groupA,STATE_groupA,TIPO_A},                
                                        {STATE_groupA,STATE_groupA,QUITAR},             // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_groupA,STATE_groupA,TARAR},              // Tarar tras colocar recipiente o alimento   
                                        {STATE_groupA,STATE_groupB,TIPO_B},                
                                        {STATE_groupA,STATE_raw,CRUDO},                         
                                        {STATE_groupA,STATE_cooked,COCINADO},              
                                        {STATE_groupA,STATE_weighted,INCREMENTO},       // Se ha colocado alimento. Aprovechar 'crudo' predeterminado para pesar directamente
                                        //{STATE_groupB,STATE_added,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        //{STATE_groupB,STATE_deleted,DELETE_PLATO}, // Borrar plato actual. 
                                        //{STATE_groupB,STATE_saved,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento.  
                                        {STATE_groupA,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento
                                        {STATE_groupA,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual
                                        {STATE_groupA,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento
                                        // --------------------------

                                        // --- Alimentos grupo B ---
                                        {STATE_groupB,STATE_Empty,LIBERAR},             // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_groupB,STATE_groupB,TIPO_B},                
                                        {STATE_groupB,STATE_groupB,QUITAR},             // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_groupB,STATE_groupB,TARAR},              // Tarar cada vez que se escoja grupo. Podría ser tras colocar recipiente o alimento   
                                        {STATE_groupB,STATE_groupA,TIPO_A},                
                                        {STATE_groupB,STATE_raw,CRUDO},                         
                                        {STATE_groupB,STATE_cooked,COCINADO},              
                                        {STATE_groupB,STATE_weighted,INCREMENTO},       // Se ha colocado alimento. Aprovechar 'crudo' predeterminado para pesar directamente.  
                                        //{STATE_groupB,STATE_added,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        //{STATE_groupB,STATE_deleted,DELETE_PLATO}, // Borrar plato actual. 
                                        //{STATE_groupB,STATE_saved,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento.  
                                        {STATE_groupB,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        {STATE_groupB,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual. 
                                        {STATE_groupB,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento.  
                                        // -------------------------

                                        // --- Alimento crudo ---
                                        {STATE_raw,STATE_Empty,LIBERAR},                // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_raw,STATE_groupA,TIPO_A},                // Cambiar grupo alimentos.  
                                        {STATE_raw,STATE_groupB,TIPO_B},                // Cambiar grupo alimentos.
                                        {STATE_raw,STATE_raw,CRUDO},                    // Para que no dé error si se vuelve a pulsar 'crudo'.
                                        {STATE_raw,STATE_cooked,COCINADO},              // Cambiar procesamiento (crudo => cocinado).
                                        {STATE_raw,STATE_weighted,INCREMENTO},          // Se ha colocado alimento.
                                        //{STATE_raw,STATE_added,ADD_PLATO},              // Nuevo plato, aunque no se haya colocado alimento.
                                        //{STATE_raw,STATE_deleted,DELETE_PLATO},         // Borrar plato actual.
                                        //{STATE_raw,STATE_saved,GUARDAR},                // Guardar comida, aunque no se haya colocado alimento.          
                                        {STATE_raw,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        {STATE_raw,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual. 
                                        {STATE_raw,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento.  
                                        // -----------------------

                                        // --- Alimento cocinado ---
                                        {STATE_cooked,STATE_Empty,LIBERAR},             // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_cooked,STATE_groupA,TIPO_A},             // Cambiar grupo alimentos.  
                                        {STATE_cooked,STATE_groupB,TIPO_B},             // Cambiar grupo alimentos.
                                        {STATE_cooked,STATE_cooked,COCINADO},           // Para que no dé error si se vuelve a pulsar 'cocinado'.
                                        {STATE_cooked,STATE_raw,CRUDO},                 // Cambiar procesamiento (cocinado => crudo).
                                        {STATE_cooked,STATE_weighted,INCREMENTO},       // Se ha colocado alimento.
                                        //{STATE_cooked,STATE_added,ADD_PLATO},           // Nuevo plato, aunque no se haya colocado alimento.
                                        //{STATE_cooked,STATE_deleted,DELETE_PLATO},      // Borrar plato actual.
                                        //{STATE_cooked,STATE_saved,GUARDAR},             // Guardar comida, aunque no se haya colocado alimento.       
                                        {STATE_cooked,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        {STATE_cooked,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual. 
                                        {STATE_cooked,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento.    
                                        // --------------------------

                                        // --- Alimento pesado ---
                                        {STATE_weighted,STATE_Empty,LIBERAR},           // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_weighted,STATE_weighted,INCREMENTO},     // Se coloca más alimento.  
                                        {STATE_weighted,STATE_weighted,DECREMENTO},     // Se retira alimento.   
                                        {STATE_weighted,STATE_weighted,QUITAR},         // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR).

                                                                                        // Estas dos últimas reglas permiten que se coloque algo en la báscula y luego se retire para
                                                                                            // escoger un grupo diferente porque nos hayamos confundido y sin guardar lo que se hubiera 
                                                                                            // colocado, pues al retirarlo su peso sería 0.0 .

                                        {STATE_weighted,STATE_groupA,TIPO_A},           // Escoger nuevo grupo.
                                        {STATE_weighted,STATE_groupB,TIPO_B},           // Escoger nuevo grupo.
                                        //{STATE_weighted,STATE_added,ADD_PLATO},         // Nuevo plato.
                                        //{STATE_weighted,STATE_deleted,DELETE_PLATO},    // Borrar plato actual. Si hay algo en la báscula, se tiene en cuenta para el borrado.
                                        //{STATE_weighted,STATE_saved,GUARDAR},           // Guardar comida. Si hay algo en la báscula, se incluye.
                                        {STATE_weighted,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        {STATE_weighted,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual. 
                                        {STATE_weighted,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento. 
                                        // -----------------------

                                        // --- Check añadir plato ---
                                        {STATE_add_check,STATE_added,ADD_PLATO},         // Sí se quiere añadir plato.
                                        {STATE_add_check,STATE_delete_check,DELETE_PLATO}, // No se quiere añadir plato, se quiere eliminar.
                                        {STATE_add_check,STATE_save_check,GUARDAR},      // No se quiere añadir plato, se quiere guardar comida.
                                        {STATE_add_check,STATE_groupA,TIPO_A},           // No se quiere añadir plato, se quiere escoger otro grupo tipo A.
                                        {STATE_add_check,STATE_groupB,TIPO_B},           // No se quiere añadir plato, se quiere escoger otro grupo tipo B.
                                        {STATE_add_check,STATE_raw,CRUDO},               // No se quiere añadir plato, se quiere escoger procesamiento 'crudo'.
                                        {STATE_add_check,STATE_cooked,COCINADO},         // No se quiere añadir plato, se quiere escoger procesamiento 'cocinado'.
                                        // --------------------------

                                        // --- Plato añadido ---
                                        {STATE_added,STATE_added,TARAR},                 // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_added,STATE_added,QUITAR},                // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR).
                                        {STATE_added,STATE_added,INCREMENTO},            // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_added,STATE_Empty,LIBERAR},               // Se ha retirado el plato completo (+ recipiente).
                                        {STATE_added,STATE_saved,GUARDAR},               // Guardar la comida tras decir que se quiere añadir plato, aunque no se haga. Es posible que 
                                                                                         // el usuario trate el "Añadir plato" como un "Guardar plato".
                                        // ---------------------

                                        // --- Check eliminar plato ---
                                        {STATE_delete_check,STATE_deleted,DELETE_PLATO},         // Sí se quiere eliminar plato.
                                        {STATE_delete_check,STATE_add_check,ADD_PLATO},     // No se quiere eliminar plato, se quiere añadir.
                                        {STATE_delete_check,STATE_save_check,GUARDAR},      // No se quiere eliminar plato, se quiere guardar comida.
                                        {STATE_delete_check,STATE_groupA,TIPO_A},           // No se quiere eliminar plato, se quiere escoger otro grupo tipo A.
                                        {STATE_delete_check,STATE_groupB,TIPO_B},           // No se quiere eliminar plato, se quiere escoger otro grupo tipo B.
                                        {STATE_delete_check,STATE_raw,CRUDO},               // No se quiere eliminar plato, se quiere escoger procesamiento 'crudo'.
                                        {STATE_delete_check,STATE_cooked,COCINADO},         // No se quiere eliminar plato, se quiere escoger procesamiento 'cocinado'.
                                        // --------------------------

                                        // --- Plato eliminado ---
                                        {STATE_deleted,STATE_deleted,TARAR},             // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_deleted,STATE_deleted,QUITAR},            // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR).
                                        {STATE_deleted,STATE_deleted,INCREMENTO},        // Para evitar error de evento cuando, al retirar el plato, puede detectar un ligero incremento.
                                        {STATE_deleted,STATE_Empty,LIBERAR},             // Se ha retirado el plato completo (+ recipiente). 
                                        {STATE_deleted,STATE_saved,GUARDAR},             // Guardar la comida tras borrar el plato actual. Solo se incluirían los platos anteriores.
                                        // -----------------------

                                        // --- Check guardar comida ---
                                        {STATE_save_check,STATE_saved,GUARDAR},           // Sí se quiere guardar comida.
                                        {STATE_save_check,STATE_add_check,ADD_PLATO},     // No se quiere guardar comida, se quiere añadir.
                                        {STATE_save_check,STATE_delete_check,DELETE_PLATO}, // No se quiere guardar comida, se quiere eliminar plato.
                                        {STATE_save_check,STATE_groupA,TIPO_A},           // No se quiere guardar comida, se quiere escoger otro grupo tipo A.
                                        {STATE_save_check,STATE_groupB,TIPO_B},           // No se quiere guardar comida, se quiere escoger otro grupo tipo B.
                                        {STATE_save_check,STATE_raw,CRUDO},               // No se quiere guardar comida, se quiere escoger procesamiento 'crudo'.
                                        {STATE_save_check,STATE_cooked,COCINADO},         // No se quiere guardar comida, se quiere escoger procesamiento 'cocinado'.
                                        // --------------------------

                                        // --- Comida guardada ---
                                        {STATE_saved,STATE_saved,TARAR},                 // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_saved,STATE_saved,QUITAR},                // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR).
                                        {STATE_saved,STATE_saved,INCREMENTO},            // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_saved,STATE_Empty,LIBERAR}                // Se ha retirado el plato completo (+ recipiente).   
                                        // -----------------------
                                      };



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ VARIABLES ESTADOS/EVENTOS -------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

states_t state_actual;      // Estado actual
states_t state_new;         // Nuevo estado al que se va a pasar
states_t state_prev;        // Estado anterior

event_t lastEvent;          // Último evento ocurrido

event_t eventoMain;         // Evento ocurrido en botonera Main
event_t eventoGrande;       // Evento ocurrido en botonera grande
event_t eventoBascula;      // Evento ocurrido en báscula


/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
// --- Reglas transición ---
bool    checkStateConditions();                        // Comprobar reglas de transición de estados

// --- Actividades por estado ---
void    actStateEmpty();                               // Actividades STATE_Empty
void    actStatePlato();                               // Actividades STATE_Plato
void    actGruposAlimentos();                          // Actividades STATE_groupA y STATE_groupB
void    actStateRaw();                                 // Actividades STATE_raw
void    actStateCooked();                              // Actividades STATE_cooked
void    actStateWeighted();                            // Actividades STATE_weighted
void    actStateAddCheck();                            // Actividade STATE_add_check
void    actStateAdded();                               // Actividades STATE_added
void    actStateDeleteCheck();                         // Actividade STATE_delete_check
void    actStateDeleted();                             // Actividades STATE_deleted
void    actStateSaveCheck();                           // Actividade STATE_save_check
void    actStateSaved();                               // Actividades STATE_saved

// --- Actividades estado actual ---
void    doStateActions();                              // Actividades según estado actual

// --- Error de evento ---
void    actEventError();                               // Mensaje de error de evento según el estado actual

// --- Buffer de eventos ---
bool    isBufferEmpty();                               // Comprobar buffer de eventos vacío
bool    isBufferFull();                                // Comprobar buffer de eventos lleno
int     firstGapBuffer();                              // Obtener primer hueco en el buffer
void    shiftLeftEventBuffer();                        // Desplazar buffer a izquierda para incluir nuevo evento
void    addEventToBuffer(event_t evento);              // Añadir evento al buffer
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
        if(state_prev != STATE_Empty){
            Serial.println(F("\nSTATE_Empty...")); 
            
            tareScale();                              // Tara inicial
            
            pesoRecipiente = 0.0;                     // Se inicializa 'pesoRecipiente', que se sumará a 'pesoPlato' para saber el 'pesoARetirar'.
            pesoPlato = 0.0;                          // Se inicializa 'pesoPlato', que se sumará a 'pesoRecipiente' para saber el 'pesoARetirar'.
            pesoLastAlimento = 0.0;                   // Se inicializa 'pesoLastAlimento', que, si hubiera un último alimento que añadir en delete,
                                                      // se sumará a 'pesoPlato' y luego a 'pesoRecipiente' para saber el 'peroARetirar'.
            
            printStateEmpty();                        // Print info estado.
        }

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
        if(state_prev != STATE_Plato){
            Serial.println(F("\nPlato colocado")); 

            if(state_prev == STATE_Empty){   // ==> Si se viene de STATE_Empty, donde se ha tarado.
                tarado = false;                       // Desactivar flag de haber 'tarado' 
            }
                                                      // El peso definitivo del recipiente no se guarda hasta que se escoja grupo de alimentos.
                                                      // Por eso, 'pesoRecipiente' y 'pesoARetirar' son iguales a 0.0 en este estado. Esto no impide que se
                                                      // realice LIBERAR, ya que ese evento ocurre cuando el peso de la báscula es igual a 'pesoARetirar'. 
                                                      // Así, se puede detectar que se ha quitado el recipiente (bascula = 0.0) y se vuelve al estado STATE_Empty.
            
            printStatePlato();                        // Print info del estado.
        }
            
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
        if(state_prev == STATE_Plato){                                      // ==> Si se viene de STATE_Plato porque se acaba de colocar el recipiente.
            pesoRecipiente = pesoBascula;                                   // Se guarda 'pesoRecipiente' para sumarlo a 'pesoPlato' y saber el 'pesoARetirar'.
            //tareScale();                                                    // Tarar para tomar peso real del alimento que se va a colocar.
        }
        //else if((state_prev == STATE_weighted) and (pesoBascula != 0.0)){   // ==> Si se viene del STATE_weighted, porque se ha colocado algo nuevo en la báscula,
                                                                            //     y el pesoBascula marca algo, indicando que lo que se ha colocado no se ha retirado,
                                                                            //     debe incluirse en el plato. 
        else if(pesoBascula != 0.0){ 
                Serial.println(F("Añadiendo alimento al plato..."));
                
                Alimento alimento(grupoAnterior, pesoBascula);                // Cálculo automático de valores nutricionales.
                                                                            // Al escoger un nuevo grupo se guarda el alimento del grupo anterior
                                                                            // colocado en la iteración anterior. Por eso se utiliza 'grupoAnterior' para
                                                                            // crear el alimento, porque 'grupoEscogido' ya ha tomado el valor del
                                                                            // nuevo grupo.
                                                             
                platoActual.addAlimentoPlato(alimento);                               // Alimento ==> Plato
                comidaActual.addAlimentoComida(alimento);                             // Alimento ==> Comida

                pesoPlato = platoActual.getPesoPlato();                     // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                
        }

        

        if((state_prev != STATE_groupA) && (state_prev != STATE_groupB)){ // Si es la primera vez que se escoge grupo, se muestra todo (ejemplos, plato actual y acumulado)
            tareScale();            // Tarar al seleccionar un grupo de alimentos nuevo, a no ser que se estén leyendo ejemplos.
                                    // Esta tara DEBE hacerse antes de printStateGroups() para que muestre el peso a 0.      
            printStateGroups();             
        }
        else{   // Si se está pulsando grupos para ver sus ejemplos, solo se van modificando los grupos y sus ejemplos
            printGrupoyEjemplos();
            tarado = false;        // Desactivar flag de haber 'tarado' 
        }
        
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
        if(state_prev != STATE_raw){
            Serial.println(F("\nAlimento crudo...")); 
            
            procesado = false;
            
            if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
                tarado = false;                                                 // Desactivar flag de haber 'tarado' 
            }
            
            printProcesamiento();                                               // Mostrar imagen de 'crudo'
        }
        
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
        if(state_prev != STATE_cooked){
            Serial.println(F("\nAlimento cocinado...")); 
            
            procesado = true;
            
            if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
                tarado = false;                                                 // Desactivar flag de haber 'tarado' 
            }
            
            printProcesamiento();                                               // Mostrar imagen de 'cocinado'
        }

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
        
        printPlatoActual(false);                                            // 'false' para mostrar valores temporales del Plato actual según el peso del alimento

        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                            // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateAddCheck(): Acciones del STATE_add_check
----------------------------------------------------------------------------------------------------------*/
void actStateAddCheck(){ 
    if(!doneState){
        Serial.println(F("\n¿Seguro que quiere añadir un plato?")); 

        if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
            tarado = false;                                                 // Desactivar flag de haber 'tarado' 
        }
        
        printStateAddCheck();                                               // Mostrar pregunta de confirmación
        
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
                                                                    // actual está vacío.

        if(state_prev != STATE_added){                              // ==> Si no se viene del propio STATE_added, para evitar que se vuelva 
                                                                    //     a guardar el plato en la comida.
          
                                                                    // *****
                                                                    // Primero se actualiza el plato, si es necesario, y después se muestran las actualizaciones.
                                                                    // Si tras comprobar cambios en el plato se ve que sigue vacío, entonces se indica en un mensaje 
                                                                    // que no se ha podido crear otro.
                                                                    // *****
            
            Serial.println(F("Añadiendo plato a la comida..."));


          /*  if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
                tarado = false;                                                 // Desactivar flag de haber 'tarado' 
            }*/


            /* ----- ACTUALIZAR PLATO  ----- */
            //if((state_prev == STATE_weighted) and (pesoBascula != 0.0)){   // ==> Si se viene del STATE_weighted, porque se ha colocado algo nuevo en la báscula,
                                                                           //     y el pesoBascula marca algo, indicando que lo que se ha colocado no se ha retirado,
                                                                           //     debe incluirse en el plato. 
            if(pesoBascula != 0.0){
                                                                    // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                                    // añadir un nuevo plato sin haber colocado un nuevo alimento, no haría falta
                                                                    // actualizar el plato, pues no se habría modificado.
                
                Alimento alimento(grupoEscogido, pesoBascula);        // Cálculo automático de valores nutricionales. 
                                                                    // Usamos 'grupoEscogido' porque no se ha modificado. 
                
                platoActual.addAlimentoPlato(alimento);                       // Alimento ==> Plato
                comidaActual.addAlimentoComida(alimento);                     // Alimento ==> Comida

                /* ----- TARAR  ----- */
                tareScale();                                        // Se debe tarar para que conforme vaya disminuyendo el peso veamos si 
                                                                    // se ha quitado todo el 'pesoARetirar'.
                                                                    // Solo se hace si se viene del STATE_weighted porque en los estados 
                                                                    // previos a ese ya se tara y es en STATE_weighted donde se modifica el peso.
                
            }
            /* ------------------------------ */


            /* ----- GUARDAR PLATO EN COMIDA  ----- */
            if(!platoActual.isPlatoEmpty()){  // PLATO CON COSAS (si había último alimento, se ha incluido) ==> SE GUARDA Y SE CREA OTRO
                errorPlatoWasEmpty = false;
                pesoPlato = platoActual.getPesoPlato();             // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                comidaActual.addPlato(platoActual);                 // Plato ==> Comida (no se vuelven a incluir los alimentos. Solo modifica peso y nPlatos)
                platoActual.restorePlato();                         // "Reiniciar" platoActual para usarlo de nuevo.
            }
            else{   // PLATO VACÍO ==> NO SE CREA OTRO
                errorPlatoWasEmpty = true;
                printEmptyObjectWarning("No se ha creado otro plato porque el actual est\xE1"" vac\xED""o");
                //printEmptyObjectWarning("No se ha guardado el plato porque est\xE1"" vac\xED""o");
            }
            /* --------------------------------------- */

            /* -----  INFORMACIÓN MOSTRADA  ----- */
            if(!errorPlatoWasEmpty){                                    // ==> Si el plato no estaba vacío y se ha guardado/creado otro 
                  printStateAdded();                                   // Print info del estado.
            }
            /* ---------------------------------- */

        }
        else if(tarado){                                                       // ==> Si se viene del propio STATE_added, donde se puede haber tarado.
            tarado = false;                                         // Desactivar flag de haber 'tarado'.          
        }
                                                 
        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                    // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateDeleteCheck(): Acciones del STATE_delete_check
----------------------------------------------------------------------------------------------------------*/
void actStateDeleteCheck(){ 
    if(!doneState){
        Serial.println(F("\n¿Seguro que quiere eliminar el plato?")); 

        if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
            tarado = false;                                                 // Desactivar flag de haber 'tarado' 
        }
        
        printStateDeleteCheck();                                            // Mostrar pregunta de confirmación
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                            // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateDeleted(): Acciones del STATE_deleted
      !!!!!!!!!!!!!!!!!!!!!!!!!!!! IMPORTANTE !!!!!!!!!!!!!!!!!!!!!!!!!

        PARA EL DELETE RETROACTIVO, PRIMERO SE BORRA EL PLATO ACTUAL, SI TIENE ALGO, Y SE DEBE RETIRAR
        PARA PODER SEGUIR BORRANDO PLATOS. HASTA QUE NO SE LIBERE LA BÁSCULA NO SE PERMITIRÁ SEGUIR BORRANDO.

      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
----------------------------------------------------------------------------------------------------------*/
void actStateDeleted(){
    if(!doneState){

        // -------------------- DELETE ACTUAL ------------------------
        static bool errorPlatoWasEmpty;                             // Flag utilizada para saber si se debe mostrar la información "normal"
                                                                    // o un mensaje de aviso indicando no se ha borrado el plato porque el 
                                                                    // actual ya está vacío.
        
        if(state_prev != STATE_deleted){                            // ==> Si no se viene del propio STATE_deleted, para evitar que se vuelva 
                                                                    //     a eliminar el plato de la comida.
          
            
            Serial.println(F("\nEliminando plato..."));

          /*  if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
                tarado = false;                                                 // Desactivar flag de haber 'tarado' 
            }*/

            
            /* ----- PESO ÚLTIMO ALIMENTO  ----- */
            //if((state_prev == STATE_weighted) and (pesoBascula != 0.0)){   // ==> Si se viene del STATE_weighted, porque se ha colocado algo nuevo en la báscula,
                                                                           //     y el pesoBascula marca algo, indicando que lo que se ha colocado no se ha retirado,
                                                                           //     debe incluirse en el plato. 
            if(pesoBascula != 0.0){
                                              // En este caso no se va a guardar el alimento porque se va a borrar el plato. Solamente se guardará el 'pesoLastAlimento'
                                              // para añadirlo a 'pesoPlato' y luego, tras sumarlo al 'pesoRecipiente', saber el 'pesoARetirar'.

                                              // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                              // eliminar el plato actual sin haber colocado un nuevo alimento (pesoBascula == 0.0), 
                                              // no haría falta actualizar el peso del plato, pues no se habría modificado.
                                                         
                pesoLastAlimento = pesoBascula;    // Se guarda el 'pesoLastAlimento' para luego añadirlo a 'pesoPlato' y, tras sumarlo al 'pesoRecipiente', 
                                                   // saber el 'pesoARetirar'.

                /* ----- TARAR  ----- */
                tareScale();                       // Se debe tarar para que conforme vaya disminuyendo el peso veamos si se ha quitado todo el 'pesoARetirar'.
                                                   // Solo se hace si se viene del STATE_weighted porque en los estados previos a ese ya se tara y es en 
                                                   // STATE_weighted donde se modifica el peso.
            }


            /* ----- BORRAR PLATO DE COMIDA  ----- */
            if(!platoActual.isPlatoEmpty()){    // PLATO CON COSAS ==> HAY QUE BORRAR Y RETIRAR (plato guardado o solo último alimento)
                errorPlatoWasEmpty = false;
                if(pesoLastAlimento != 0.0) pesoPlato = pesoLastAlimento; // Incluir último alimento en el 'pesoPlato' para saber 'pesoARetirar'.
                pesoPlato += platoActual.getPesoPlato();                  // 'pesoPlato' puede tener inicialmente el 'pesoLastAlimento' (línea anterior) o estar vacío (0.0). 
                comidaActual.deletePlato(platoActual);                    // Borrar plato actual
                platoActual.restorePlato();                               // "Reiniciar" platoActual para usarlo de nuevo.
            }
            else{   // PLATO VACÍO ==> NO HAY QUE BORRAR, PERO PUEDE QUE SÍ RETIRAR (último alimento)
                if(pesoLastAlimento != 0.0){ // ÚLTIMO ALIMENTO ==> ALGO QUE RETIRAR
                    errorPlatoWasEmpty = false;       // Para mostrar mensaje de plato borrado, aunque no sea cierto porque no se ha guardado el
                                                      // último alimento y el plato está realmente vacío, y pedir retirarlo.
                    pesoPlato = pesoLastAlimento;     // Incluir último alimento en el 'pesoPlato' para saber 'pesoARetirar'.
                }
                else{   // PLATO VACÍO Y NO HAY ÚLTIMO ALIMENTO ==> NADA QUE RETIRAR 
                    errorPlatoWasEmpty = true;
                    //if(state_prev == STATE_Empty){
                    if(pesoARetirar < 1.0){ // STATE_Empty
                              // Si se pulsa 'borrar' estando en INI, el plato seguramente estará vacío y se avisa. Tras unos segundos
                              // para mostrar mensaje de no poder borrar, se debe forzar el regreso a INI para que no se quede aquí (delete) 
                              // esperando una liberación que no llega, porque el último eventoBascula es TARAR, realizado en INI, y necesita
                              // LIBERAR para volver al inicio.
                        printEmptyObjectWarning("No se ha borrado el plato porque est\xE1"" vac\xED""o");
                        Serial.print(F("\nLIBERAR forzada. Regreso a INI..."));
                        delay(3000);                                    
                        eventoBascula = LIBERAR;
                        addEventToBuffer(eventoBascula);
                        flagEvent = true;
                    }
                    else{     // En los otros estados (groupA, groupB, raw, cooked, weighted) habrá recipiente,
                              // por lo que el regreso a STATE_Empty no es automático, sino que se debe liberar la báscula.
                        //printEmptyObjectError("No se puede borrar el plato porque est\xE1"" vac\xED""o"". \n Si ha puesto un recipiente, ret\xED""relo para empezar de nuevo.");
                        printEmptyObjectWarning("No se puede borrar el plato porque est\xE1"" vac\xED""o"". \n Retire el recipiente para empezar de nuevo.");
                    }
                }
            }

            // -----  INFORMACIÓN MOSTRADA  ----- 
            if(!errorPlatoWasEmpty){   // ==> Si la comida no estaba vacía (incluyendo último alimento) y se ha borrado 
                printStateDeleted();     // Print info del estado.
            } // ---------------------------------- 


        }
        else if(tarado){                                            // ==> Si se viene del propio STATE_deleted, donde se puede haber tarado.
            tarado = false;                                         // Desactivar flag de haber 'tarado'.        
            //pesoLastAlimento = 0.0;  
        }
        // ----------- FIN DELETE ACTUAL --------------------------------------------



        // -------------------- DELETE RETROACTIVO ------------------------
        /*
        static bool errorComidaWasEmpty;
        if(state_prev != STATE_deleted){
            Serial.println(F("\nEliminando plato..."));
            if((state_prev == STATE_weighted) and (pesoBascula != 0.0)){
                // Para poder borrar platos retroactivamente, comenzando por el actual, se debe guardar en la comida antes de borrarlo. 
                // Esto es necesario porque se pueden haber ido metiendo alimento del plato directamente en la comida al escoger
                // diferentes grupos de alimentos, por eso hay que terminar de incluir el plato para eliminarlo por completo.
                // -- Se podría tener un ID de plato para que cada alimento esté asignado a ese ID, de forma que no hubiera que
                // -- terminar de incluir el plato para borrarlo, sino que bastaría con borrar la info de los alimentos relacionados
                // -- con el plato del mismo ID.
                Alimento alimento(grupoEscogido, pesoBascula);        // Cálculo automático de valores nutricionales. 
                                                                    // Usamos 'grupoEscogido' porque no se ha modificado. 
                platoActual.addAlimentoPlato(alimento);                       // Alimento ==> Plato
                comidaActual.addAlimentoComida(alimento);                     // Alimento ==> Comida
            }
            if(!platoActual.isPlatoEmpty()){    // Si tiene algo, añadir plato actual a la comida. 
                                                // Se pone aquí por si no ha habido que actualizarlo con último alimento
                  comidaActual.addPlato(platoActual);      // Plato ==> Comida (no se vuelven a incluir los alimentos. Solo modifica nPlatos)
            } 
            // ----------------- BORRAR PLATO CON DELETE RETROACTIVO -------------------------
            // Caso 1 - Si la comida está vacía (ni siquiera el actual tiene algo), se avisa de que no hay nada que borrar.   
            // Caso 2 - Si la comida no está vacía, porque haya platos previos o solamente el actual, temporalmente guardado,
            //           se borra el último.

            if(comidaActual.isComidaEmpty()){ // Caso 1 ==> No queda ningún plato, ni el actual 
                errorComidaWasEmpty = true;

                if(state_prev == STATE_Empty){
                    printEmptyObjectWarning("No hay nada que borrar");
                    

                    // Si se libera la báscula tras addPlato o deletePlato, se vuelve a INI y se quiere seguir borrando,
                    // se quedaría atascado aquí (delete) esperando una liberación que no llega, ya que el último eventoBascula 
                    // es TARAR, realizado en INI. 
                    // Por eso, se debe forzar la liberación (eventoBascula = LIBERAR) tras un delay, para poder
                    // leer los mensajes de haber borrado o no y luego regresar a INI.
                    Serial.print(F("\nLIBERAR forzada. Regreso a INI..."));
                    delay(3000);                                    
                    eventoBascula = LIBERAR;
                    addEventToBuffer(eventoBascula);
                    flagEvent = true;
                }
                else{                                               // En los otros estados (groupA, groupB, raw, cooked, weighted) habrá recipiente,
                                                                    // por lo que el regreso a STATE_Empty no es automático, sino que se debe liberar la báscula.
                    printEmptyObjectWarning("No hay nada que borrar. \n Si ha puesto un recipiente, ret\xED""relo para empezar de nuevo.");
                }
            }
            else{ // Caso 2 ==> Hay algún plato, aunque sea el actual 
                errorComidaWasEmpty = false;
        
                comidaActual.deletePlato();   //Delete último plato, aunque sea el actual (se guarda el actual para poderlo borrar)

                if(!platoActual.isPlatoEmpty()){    // El plato borrado ha sido el actual porque no estaba vacío
                    pesoPlato += platoActual.getPesoPlato();   // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y
                                                                // saber el 'pesoARetirar'.
                    platoActual.restorePlato();    // "Reiniciar" platoActual para usarlo de nuevo.
                }

                // -----  REGRESO A INI, SE HAYA PODIDO BORRAR O NO  -----
                // Si se ha borrado el platoActual, se debe LIBERAR la báscula para salir del estado deletePlato y
                // volver a INI. NO SE PERMITE SEGUIR BORRANDO PLATOS SI NO SE LIBERA ANTES LA BÁSCULA.

                // Si se libera la báscula, se vuelve a INI y se quiere borrar platos previos, se quedaría atascado aquí (deletePlato)
                // esperando una liberación que no llega, ya que el último eventoBascula es TARAR, realizado en INI.
                // Por eso, se debe forzar la liberación (eventoBascula = LIBERAR) tras un delay, para poder leer los
                // mensajes de haber borrado o no y luego regresar a INI.
                if(state_prev == STATE_Empty){
                    Serial.print(F("\nLIBERAR forzada. Regreso a INI..."));
                    delay(2000);  
                    eventoBascula = LIBERAR;
                    addEventToBuffer(eventoBascula);
                    flagEvent = true;
                } // ------------------------------------------------------- 

            } // --------------------------------------------------------------

        }  
        else{                                                       // ==> Si se viene del propio STATE_deleted, donde se puede haber tarado.
            tarado = false;                                         // Desactivar flag de haber 'tarado'.          
        }
        
        // -----  INFORMACIÓN MOSTRADA  ----- 
        if(!errorComidaWasEmpty){                                    // ==> Si la comida no estaba vacía y se ha borrado algún plato
            printStateDeleted();                                 // Print info del estado.
        } // ---------------------------------- 
        
        */
        // --------------------------------------------------------------
        // -------------- FIN DELETE RETROACTIVO ----------------------------------------------
        

        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                    // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateSaveCheck(): Acciones del STATE_save_check
----------------------------------------------------------------------------------------------------------*/
void actStateSaveCheck(){ 
    if(!doneState){
        Serial.println(F("\n¿Seguro que quiere guardar la comida?")); 

        if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){   // ==> Si se viene de STATE_groupA o STATE_groupB, donde se ha tarado.
            tarado = false;                                                 // Desactivar flag de haber 'tarado' 
        }
        
        printStateSaveCheck();                                              // Mostrar pregunta de confirmación
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
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
        
        if(state_prev != STATE_saved){                              // ==> Si no se viene del propio STATE_saved (por TARAR, QUITAR, INCREMENTO), 
                                                                    //     para evitar que se vuelva a guardar la comida en el diario.
            
                                                                    // *****
                                                                    // Primero se actualiza la comida, si es necesario, y después se muestran las actualizaciones.
                                                                    // Si tras comprobar cambios en la comida se ve que sigue vacía, entonces se indica en un mensaje 
                                                                    // que no se ha podido guardar.   
                                                                    // *****                  
          
            Serial.println(F("\nGuardando comida..."));  

            /* ----- ACTUALIZAR PLATO  ----- */
            //if((state_prev == STATE_weighted) and (pesoBascula != 0.0)){   // ==> Si se viene del STATE_weighted, porque se ha colocado algo nuevo en la báscula,
                                                                           //     y el pesoBascula marca algo, indicando que lo que se ha colocado no se ha retirado,
                                                                           //     debe incluirse en el plato. 
            if(pesoBascula != 0.0){ 
                                                                    // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                                    // guardar la comida actual sin haber colocado un nuevo alimento, no haría falta
                                                                    // actualizar la comida, pues no se habría modificado.
            
                Alimento alimento(grupoEscogido, pesoBascula);        // Cálculo automático de valores nutricionales.
                                                                    // Usamos 'grupoEscogido' porque no se ha modificado. 
                
                platoActual.addAlimentoPlato(alimento);                       // Alimento ==> Plato
                comidaActual.addAlimentoComida(alimento);                     // Alimento ==> Comida

                /* ----- TARAR  ----- */
                tareScale();                                        // Se debe tarar para que conforme vaya disminuyendo el peso veamos si 
                                                                    // se ha quitado todo el 'pesoARetirar'.
                                                                    // Solo se hace si se viene del STATE_weighted porque en los estados 
                                                                    // previos a ese ya se tara y es en STATE_weighted donde se modifica el peso.
            }
            /* ------------------------------ */


            /* ----- GUARDAR PLATO EN COMIDA  ----- */
            if(!platoActual.isPlatoEmpty()){   // PLATO CON COSAS ==> INCLUIR EN COMIDA ACTUAL                   

                                               // Si se viene de STATE_added o STATE_deleted, el plato ya estará vacío.

                    pesoPlato = platoActual.getPesoPlato();         // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                    comidaActual.addPlato(platoActual);             // Plato ==> Comida (no se vuelven a incluir los alimentos. Solo modifica peso y nPlatos)
                    platoActual.restorePlato();                     // "Reiniciar" platoActual para usarlo de nuevo.
            }
            /* -------------------------------------- */


            /* ----- GUARDAR COMIDA EN DIARIO  ----- */
            if(comidaActual.isComidaEmpty()){                       // ==> Si la comida está vacía -> no se guarda
                errorComidaWasEmpty = true;
                //if(state_prev == STATE_Empty){
                if(pesoARetirar < 1.0){ // STATE_Empty
                    printEmptyObjectWarning("No se puede guardar la comida porque est\xE1"" vac\xED""a");

                    // Si se libera la báscula tras addPlato o deletePlato, se vuelve a INI y se quiere guardar la comida,
                    // se quedaría atascado aquí (save) esperando una liberación que no llega, ya que el último eventoBascula 
                    // es TARAR, realizado en INI. 
                    // Por eso, se debe forzar la liberación (eventoBascula = LIBERAR) tras un delay, para poder
                    // leer los mensajes de haber guardado o no y luego regresar a INI.
                    Serial.print(F("\nLIBERAR forzada. Regreso a INI..."));
                    delay(3000);                                    
                    eventoBascula = LIBERAR;
                    addEventToBuffer(eventoBascula);
                    flagEvent = true;
                }
                else{                                               // En los otros estados (groupA, groupB, raw, cooked, weighted) habrá recipiente,
                                                                    // por lo que el regreso a STATE_Empty no es automático, sino que se debe liberar la báscula.
                    printEmptyObjectWarning("No se puede guardar la comida porque est\xE1"" vac\xED""a. \n Retire el recipiente para empezar de nuevo.");
                }
            }
            else{                                                   // ==> Si la comida no está vacía -> se guarda en el diario.
                errorComidaWasEmpty = false;

                diaActual.addComida(comidaActual);                  // Comida ==> Diario
                //saveDataSD(comidaActual.getValoresComida(), comidaActual.getPesoComida());        // Comida ==> fichero CSV en SD
                saveComidaSD();                                     // Comida ==> fichero CSV en SD

                comidaActual.restoreComida();                       // "Reiniciar" comidaActual para usarla de nuevo.

            }
            /* --------------------------------------- */


            /* -----  INFORMACIÓN MOSTRADA  ----- */
            if(!errorComidaWasEmpty){                                   // ==> Si la comida no estaba vacía y se ha guardado
                  printStateSaved();                                   // Print info del estado.
                  //if(state_prev == STATE_Empty){
                  if(pesoARetirar < 1.0){ // STATE_Empty
                      // Si se libera la báscula tras addPlato o deletePlato, se vuelve a INI y se quiere guardar la comida,
                      // se quedaría atascado aquí (save) esperando una liberación que no llega, ya que el último eventoBascula 
                      // es TARAR, realizado en INI. 
                      // Por eso, se debe forzar la liberación (eventoBascula = LIBERAR) tras un delay, para poder
                      // leer los mensajes de haber guardado o no y luego regresar a INI.
                      Serial.print(F("\nLIBERAR forzada. Regreso a INI..."));
                      delay(3000);                                    
                      eventoBascula = LIBERAR;
                      addEventToBuffer(eventoBascula);
                      flagEvent = true;
                  }
            }
            /* ---------------------------------- */
        
        }
        else if(tarado){                                            // ==> Si se viene del propio STATE_saved, donde se puede haber tarado.
            tarado = false;                                         // Desactivar flag de haber 'tarado'.          
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
      case 1:   actStateEmpty();        break;
      case 2:   actStatePlato();        break;
      case 3:   actGruposAlimentos();   break;
      case 4:   actGruposAlimentos();   break;
      case 5:   actStateRaw();          break;
      case 6:   actStateCooked();       break;
      case 7:   actStateWeighted();     break;
      case 8:   actStateAddCheck();     break;
      case 9:   actStateAdded();        break;
      case 10:  actStateDeleteCheck();  break;
      case 11:  actStateDeleted();      break;
      case 12:  actStateSaveCheck();    break;
      case 13:  actStateSaved();        break;
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
      case 8:   printEventError("Vuelva a pulsar el bot\xF3""n de a\xF1""adir o escoja otra acci\xF3""n");        break;  // add_check
      case 9:   printEventError("Retire el plato para comenzar uno nuevo");                                       break;  // Added
      case 10:  printEventError("Vuelva a pulsar el bot\xF3""n de eliminar o escoja otra acci\xF3""n");           break;  // delete_check
      case 11:  printEventError("Retire el plato que acaba de eliminar");                                         break;  // Deleted
      case 12:  printEventError("Vuelva a pulsar el bot\xF3""n de guardar o escoja otra acci\xF3""n");            break;  // save_check
      case 13:  printEventError("Retire el plato");                                                               break;  // Saved
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
    Serial.println(F("\n***********************************"));
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
    Serial.println(F("\n***********************************"));
    Serial.println(F("***********************************"));
}





#endif
