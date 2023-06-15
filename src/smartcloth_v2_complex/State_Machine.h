/** 
 * @file State_Machine.h
 * @brief Máquina de Estados (State Machine) de SmartCloth
 *
 * @author Irene Casares Rodríguez
 * @date 15/06/23
 * @version 1.0
 *
 *  Este archivo contiene las definiciones de los estados y eventos, además de las funciones
 *  realizadas en cada estado y para el funcionamiento de la máquina de estados, incluyendo el
 *  manejo de errores y avisos.
 *  
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

/**
 * @def MAX_EVENTS
 * @brief Máximo número de eventos que se puede guardar en el buffer de eventos. 
 */
#define MAX_EVENTS 5

/**
 * @def RULES
 * @brief Máximo número de reglas de transición.
 */
#define RULES 125 // 134 con las comentadas


#include "Screen.h"   // Incluye Variables.h (Diario.h -> Comida.h -> Plato.h -> Alimento.h -> Valores_Nutricionales.h)
#include "SD_functions.h"


/**
 * @brief Flag para indicar si se han realizado las actividades del estado.
 */
bool doneState;       // Flag para que solo se realicen una vez las actividades del estado cada vez que se entre.


/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- ESTADOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/**
 * @enum states_t
 * @brief Enumeración de los diferentes estados de la Máquina de Estados.
 */
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
              STATE_saved         =   (13),   // comida guardada
              STATE_ERROR         =   (14),   // Estado ficticio de error (acción incorrecta)
              STATE_CANCEL        =   (15)    // Estado ficticio de Cancelación
} states_t;



/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- EVENTOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/**
 * @enum event_t
 * @brief Enumeración de los diferentes eventos que pueden ocurrir en la Máquina de Estados.
 */
typedef enum {
              
              NONE                  =   (0),    
              TIPO_A                =   (1),    // Botonera Grande (7,8,9,16,17,18)
              TIPO_B                =   (2),    // Botonera Grande (1,2,3,4,5,6,10,11,12,13,14,15,19,20)
              CRUDO                 =   (3),    // AMARILLO
              COCINADO              =   (4),    // BLANCO
              ADD_PLATO             =   (5),    // VERDE
              DELETE_PLATO          =   (6),    // ROJO
              GUARDAR               =   (7),    // NEGRO 
              INCREMENTO            =   (8),    // Báscula
              DECREMENTO            =   (9),    // Báscula
              TARAR                 =   (10),   // Báscula tarada
              QUITAR                =   (11),   // Quitar de la báscula
              LIBERAR               =   (12),   // Báscula vacía real
              ERROR                 =   (13),   // Error (acción incorrecta)
              CANCELAR              =   (14),   // Cancelar acción de añadir, eliminar o guardar
              BACK_TO_EMPTY         =   (15),   // Evento ficticio para volver a STATE_Empty porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              BACK_TO_PLATO         =   (16),   // Evento ficticio para volver a STATE_Plato porque saltó un error 
              BACK_TO_GROUP_A       =   (17),   // Evento ficticio para volver a STATE_groupA porque saltó un error 
              BACK_TO_GROUP_B       =   (18),   // Evento ficticio para volver a STATE_groupB porque saltó un error
              BACK_TO_RAW           =   (19),   // Evento ficticio para volver a STATE_raw porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              BACK_TO_COOKED        =   (20),   // Evento ficticio para volver a STATE_cooked porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              BACK_TO_WEIGHTED      =   (21),   // Evento ficticio para volver a STATE_weighted porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              BACK_TO_ADD_CHECK     =   (22),   // Evento ficticio para volver a STATE_add_check porque saltó un error 
              BACK_TO_ADDED         =   (23),   // Evento ficticio para volver a STATE_added porque saltó un error 
              BACK_TO_DELETE_CHECK  =   (24),   // Evento ficticio para volver a STATE_delete_check porque saltó un error 
              BACK_TO_DELETED       =   (25),   // Evento ficticio para volver a STATE_deleted porque saltó un error
              BACK_TO_SAVE_CHECK    =   (26),   // Evento ficticio para volver a STATE_save_check porque saltó un error 
              BACK_TO_SAVED         =   (27)    // Evento ficticio para volver a STATE_saved porque saltó un error 
} event_t;


/**
 * @brief Buffer para guardar los eventos que ocurren en la Máquina de Estados.
 */
static event_t event_buffer[MAX_EVENTS];       // Buffer de eventos al que se irán añadiendo conforme ocurran
/* Este buffer solo se utiliza para debuggear, para ver qué eventos han ido ocurriendo.
   Para comprobar si se cumple alguna regla de transición se utiliza 'lastEvent', que 
   sería el último evento añadido al buffer.
   
   Al final habría que eliminar el buffer para evitar ocupar memoria innecesariamente 
*/



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ REGLAS TRANSICION ---------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/**
 * @struct transition_rule
 * @brief Estructura que define una regla de transición en la Máquina de Estados.
 */
typedef struct{
    states_t state_i;     /**< Estado actual */
    states_t state_j;     /**< Estado siguiente */
    event_t condition;    /**< Condición para transición de estado */
}transition_rule;


/**
 * @brief Array de reglas de transición para la Máquina de Estados.
 */
static transition_rule rules[RULES] = { // --- Esperando Recipiente ---
                                        {STATE_Empty,STATE_Empty,TARAR},                // Tara inicial
                                        {STATE_Empty,STATE_Plato,INCREMENTO},
                                        {STATE_Empty,STATE_save_check,GUARDAR},         // Guardar comida directamente (comidaActual no está vacía)
                                        {STATE_Empty,STATE_groupA,TIPO_A},              // Escoger grupo (tipo A) sin colocar recipiente
                                        {STATE_Empty,STATE_groupB,TIPO_B},              // Escoger grupo (tipo B) sin colocar recipiente
                                        //{STATE_Empty,STATE_deleted,DELETE_PLATO},       // Previamente se ha borrado el actual y se ha retirado, volviendo a INI. Ahora se quiere
                                                                                          // seguir borrando los anteriores.
                                        {STATE_Empty,STATE_ERROR,ERROR},                // Acción incorrecta
                                        // ----------------------------

                                        // --- Recipiente colocado ---
                                        {STATE_Plato,STATE_Plato,INCREMENTO},           // Cambios por recolocar recipiente
                                        {STATE_Plato,STATE_Plato,DECREMENTO},           // Cambios por recolocar recipiente
                                        {STATE_Plato,STATE_Empty,LIBERAR},              // Se ha retirado el recipiente
                                        {STATE_Plato,STATE_groupA,TIPO_A},              // Escoger alimento tipo A
                                        {STATE_Plato,STATE_groupB,TIPO_B},              // Escoger alimento tipo B
                                        {STATE_Plato,STATE_ERROR,ERROR},                // Acción incorrecta             
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
                                        //{STATE_groupA,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento
                                        //{STATE_groupA,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual
                                        //{STATE_groupA,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento
                                        {STATE_groupA,STATE_ERROR,ERROR},               // Acción incorrecta
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
                                        //{STATE_groupB,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        //{STATE_groupB,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual. 
                                        //{STATE_groupB,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento.  
                                        {STATE_groupB,STATE_ERROR,ERROR},               // Acción incorrecta
                                        // -------------------------

                                        // --- Alimento crudo ---
                                        {STATE_raw,STATE_Empty,LIBERAR},                // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_raw,STATE_groupA,TIPO_A},                // Cambiar grupo alimentos.  
                                        {STATE_raw,STATE_groupB,TIPO_B},                // Cambiar grupo alimentos.
                                        {STATE_raw,STATE_raw,CRUDO},                    // Para que no dé error si se vuelve a pulsar 'crudo'.
                                        {STATE_raw,STATE_cooked,COCINADO},              // Cambiar procesamiento (crudo => cocinado).
                                        {STATE_raw,STATE_weighted,INCREMENTO},          // Se ha colocado alimento.
                                        {STATE_raw,STATE_add_check,ADD_PLATO},          // Nuevo plato, aunque no se haya colocado alimento.
                                        {STATE_raw,STATE_delete_check,DELETE_PLATO},    // Borrar plato actual. 
                                        {STATE_raw,STATE_save_check,GUARDAR},           // Guardar comida, aunque no se haya colocado alimento.  
                                        {STATE_raw,STATE_ERROR,ERROR},                  // Acción incorrecta
                                        // -----------------------

                                        // --- Alimento cocinado ---
                                        {STATE_cooked,STATE_Empty,LIBERAR},             // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_cooked,STATE_groupA,TIPO_A},             // Cambiar grupo alimentos.  
                                        {STATE_cooked,STATE_groupB,TIPO_B},             // Cambiar grupo alimentos.
                                        {STATE_cooked,STATE_cooked,COCINADO},           // Para que no dé error si se vuelve a pulsar 'cocinado'.
                                        {STATE_cooked,STATE_raw,CRUDO},                 // Cambiar procesamiento (cocinado => crudo).
                                        {STATE_cooked,STATE_weighted,INCREMENTO},       // Se ha colocado alimento.      
                                        {STATE_cooked,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        {STATE_cooked,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual. 
                                        {STATE_cooked,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento.   
                                        {STATE_cooked,STATE_ERROR,ERROR},               // Acción incorrecta 
                                        // --------------------------

                                        // --- Alimento pesado ---
                                        {STATE_weighted,STATE_Empty,LIBERAR},           // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_weighted,STATE_weighted,INCREMENTO},     // Se coloca más alimento.  
                                        {STATE_weighted,STATE_weighted,DECREMENTO},     // Se retira alimento.   
                                        {STATE_weighted,STATE_weighted,QUITAR},         // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR).

                                                                                        // Estas dos últimas reglas permiten que se coloque algo en la báscula y luego se retire para
                                                                                            // escoger un grupo diferente porque nos hayamos confundido y sin guardar lo que se hubiera 
                                                                                            // colocado, pues al retirarlo su peso sería 0.0 .

                                        {STATE_weighted,STATE_groupA,TIPO_A},             // Escoger nuevo grupo.
                                        {STATE_weighted,STATE_groupB,TIPO_B},             // Escoger nuevo grupo.
                                        {STATE_weighted,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        {STATE_weighted,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual. 
                                        {STATE_weighted,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento. 
                                        {STATE_weighted,STATE_ERROR,ERROR},               // Acción incorrecta
                                        // -----------------------

                                        // --- Check añadir plato ---
                                        {STATE_add_check,STATE_added,ADD_PLATO},         // Confirmar añadir plato.
                                        {STATE_add_check,STATE_CANCEL,TIPO_A},           // Cancelar añadir plato pulsando botón de grupo tipoA.
                                        {STATE_add_check,STATE_CANCEL,TIPO_B},           // Cancelar añadir plato pulsando botón de grupo tipoB.
                                        {STATE_add_check,STATE_CANCEL,CRUDO},            // Cancelar añadir plato pulsando botón de CRUDO.
                                        {STATE_add_check,STATE_CANCEL,COCINADO},         // Cancelar añadir plato pulsando botón de COCINADO.
                                        {STATE_add_check,STATE_CANCEL,DELETE_PLATO},     // Cancelar añadir plato pulsando botón de BORRAR PLATO.
                                        {STATE_add_check,STATE_CANCEL,GUARDAR},          // Cancelar añadir plato pulsando botón de GUARDAR.
                                        {STATE_add_check,STATE_CANCEL,CANCELAR},         // Cancelar añadir plato tras 10 segundos de inactividad.
                                        {STATE_add_check,STATE_ERROR,ERROR},             // Acción incorrecta.
                                        // --------------------------

                                        // --- Plato añadido ---
                                        {STATE_added,STATE_added,TARAR},                 // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_added,STATE_added,QUITAR},                // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR).
                                        {STATE_added,STATE_added,INCREMENTO},            // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_added,STATE_Empty,LIBERAR},               // Se ha retirado el plato completo (+ recipiente) tras añadir uno nuevo.
                                        {STATE_added,STATE_groupA,TIPO_A},               // No se ha creado otro plato porque estaba vacío. Se fuerza el regreso manteniendo recipiente.
                                        {STATE_added,STATE_groupB,TIPO_B},               // No se ha creado otro plato porque estaba vacío. Se fuerza el regreso manteniendo recipiente.
                                        //{STATE_added,STATE_saved,GUARDAR},               // Guardar la comida tras decir que se quiere añadir plato, aunque no se haga. Es posible que 
                                                                                         // el usuario trate el "Añadir plato" como un "Guardar plato".
                                        {STATE_added,STATE_ERROR,ERROR},                 // Acción incorrecta
                                        // ---------------------

                                        // --- Check eliminar plato ---
                                        {STATE_delete_check,STATE_deleted,DELETE_PLATO},    // Confirmar eliminar plato.
                                        {STATE_delete_check,STATE_CANCEL,TIPO_A},           // Cancelar eliminar plato pulsando botón de grupo tipoA.
                                        {STATE_delete_check,STATE_CANCEL,TIPO_B},           // Cancelar eliminar plato pulsando botón de grupo tipoB.
                                        {STATE_delete_check,STATE_CANCEL,CRUDO},            // Cancelar eliminar plato pulsando botón de CRUDO.
                                        {STATE_delete_check,STATE_CANCEL,COCINADO},         // Cancelar eliminar plato pulsando botón de COCINADO.
                                        {STATE_delete_check,STATE_CANCEL,ADD_PLATO},        // Cancelar eliminar plato pulsando botón de AÑADIR PLATO.
                                        {STATE_delete_check,STATE_CANCEL,GUARDAR},          // Cancelar eliminar plato pulsando botón de GUARDAR.
                                        {STATE_delete_check,STATE_CANCEL,CANCELAR},         // Cancelar eliminar plato tras 10 segundos de inactividad.
                                        {STATE_delete_check,STATE_ERROR,ERROR},             // Acción incorrecta.
                                        // --------------------------

                                        // --- Plato eliminado ---
                                        {STATE_deleted,STATE_deleted,TARAR},             // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_deleted,STATE_deleted,QUITAR},            // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR).
                                        {STATE_deleted,STATE_deleted,INCREMENTO},        // Para evitar error de evento cuando, al retirar el plato, puede detectar un ligero incremento.
                                        {STATE_deleted,STATE_Empty,LIBERAR},             // Se ha retirado el plato completo (+ recipiente) tras borrar. 
                                        {STATE_deleted,STATE_groupA,TIPO_A},             // No se ha eliminado el plato porque estaba vacío. Se fuerza el regreso manteniendo recipiente.
                                        {STATE_deleted,STATE_groupB,TIPO_B},             // No se ha eliminado el plato porque estaba vacío. Se fuerza el regreso manteniendo recipiente.
                                        //{STATE_deleted,STATE_saved,GUARDAR},             // Guardar la comida tras borrar el plato actual. Solo se incluirían los platos anteriores.
                                        {STATE_deleted,STATE_ERROR,ERROR},               // Acción incorrecta
                                        // -----------------------

                                        // --- Check guardar comida ---
                                        {STATE_save_check,STATE_saved,GUARDAR},          // Confirmar guardar comida.
                                        {STATE_save_check,STATE_CANCEL,TIPO_A},          // Cancelar guardar comida pulsando botón de grupo tipoA.
                                        {STATE_save_check,STATE_CANCEL,TIPO_B},          // Cancelar guardar comida pulsando botón de grupo tipoB.
                                        {STATE_save_check,STATE_CANCEL,CRUDO},           // Cancelar guardar comida pulsando botón de CRUDO.
                                        {STATE_save_check,STATE_CANCEL,COCINADO},        // Cancelar guardar comida pulsando botón de COCINADO.
                                        {STATE_save_check,STATE_CANCEL,ADD_PLATO},       // Cancelar guardar comida pulsando botón de AÑADIR PLATO.
                                        {STATE_save_check,STATE_CANCEL,DELETE_PLATO},    // Cancelar guardar comida pulsando botón de BORRAR PLATO.
                                        {STATE_save_check,STATE_CANCEL,CANCELAR},        // Cancelar guardar comida tras 10 segundos de inactividad.
                                        {STATE_save_check,STATE_ERROR,ERROR},            // Acción incorrecta.
                                        // --------------------------

                                        // --- Comida guardada ---
                                        {STATE_saved,STATE_saved,TARAR},                 // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_saved,STATE_saved,QUITAR},                // Para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR).
                                        {STATE_saved,STATE_saved,INCREMENTO},            // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_saved,STATE_Empty,LIBERAR},               // Se ha retirado el plato completo (+ recipiente) o se fuerza el regreso porque se ha guardado desde STATE_Empty.   
                                        {STATE_saved,STATE_groupA,TIPO_A},               // No se ha guardado la comida porque estaba vacía. Se fuerza el regreso manteniendo recipiente.
                                        {STATE_saved,STATE_groupB,TIPO_B},               // No se ha guardado la comida porque estaba vacía. Se fuerza el regreso manteniendo recipiente.
                                        {STATE_saved,STATE_ERROR,ERROR},                 // Acción incorrecta
                                        // -----------------------


                                        // --- ERROR DE EVENTO ---
                                        {STATE_ERROR,STATE_Empty,BACK_TO_EMPTY},                  // Regresar a STATE_Empty tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_Plato,BACK_TO_PLATO},                  // Regresar a STATE_Plato tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_groupA,BACK_TO_GROUP_A},               // Regresar a STATE_groupA tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_groupB,BACK_TO_GROUP_B},               // Regresar a STATE_groupB tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_raw,BACK_TO_RAW},                      // Regresar a STATE_raw tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_cooked,BACK_TO_COOKED},                // Regresar a STATE_cooked tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_weighted,BACK_TO_WEIGHTED},            // Regresar a STATE_weighted tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_add_check,BACK_TO_ADD_CHECK},          // Regresar a STATE_add_check tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_added,BACK_TO_ADDED},                  // Regresar a STATE_added tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_delete_check,BACK_TO_DELETE_CHECK},    // Regresar a STATE_delete_check tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_deleted,BACK_TO_DELETED},              // Regresar a STATE_deleted tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_save_check,BACK_TO_SAVE_CHECK},        // Regresar a STATE_save_check tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_saved,BACK_TO_SAVED},                  // Regresar a STATE_saved tras mostrar error cometido allí
                                        // -----------------------


                                        // --- CANCELAR ----------
                                        {STATE_CANCEL,STATE_Empty,BACK_TO_EMPTY},         // Regresar a STATE_Empty tras cancelar acción de guardar iniciada desde STATE_Empty
                                        {STATE_CANCEL,STATE_raw,BACK_TO_RAW},             // Regresar a STATE_raw tras cancelar add/delete/save iniciada desde STATE_raw   
                                        {STATE_CANCEL,STATE_cooked,BACK_TO_COOKED},       // Regresar a STATE_cooked tras cancelar add/delete/save iniciada desde STATE_cooked
                                        {STATE_CANCEL,STATE_weighted,BACK_TO_WEIGHTED}    // Regresar a STATE_weighted tras cancelar add/delete/save iniciada desde STATE_weighted
                                        // -----------------------
                                      };



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ VARIABLES ESTADOS/EVENTOS -------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

states_t state_actual;      // Estado actual
states_t state_new;         // Nuevo estado al que se va a pasar
states_t state_prev;        // Estado anterior
states_t state_prev_prev;   // Estado anterior al anterior. 
                            // Utilizado para saber a qué estado regresar (Empty, raw, cooked o weighted) tras cancelar una acción (añadir, eliminar o guardar)

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
void    actStateERROR();                               // Actividades STATE_ERROR
void    actStateCANCEL();                              // Actividades STATE_CANCEL

// --- Actividades estado actual ---
void    doStateActions();                              // Actividades según estado actual

// --- Error de evento ---
void    actEventError();                               // Mensaje de error de evento según el estado actual

// --- Buffer de eventos ---
bool    isBufferEmpty();                               // Comprobar buffer de eventos vacío
bool    isBufferFull();                                // Comprobar buffer de eventos lleno
int     getFirstGapBuffer();                           // Obtener primer hueco en el buffer
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
            
            tareScale();                    // Tara inicial
            
            pesoRecipiente = 0.0;           // Se inicializa 'pesoRecipiente', que se sumará a 'pesoPlato' para saber el 'pesoARetirar'.
            pesoPlato = 0.0;                // Se inicializa 'pesoPlato', que se sumará a 'pesoRecipiente' para saber el 'pesoARetirar'.
            pesoLastAlimento = 0.0;         // Se inicializa 'pesoLastAlimento', que, si hubiera un último alimento que añadir en delete,
                                            // se sumará a 'pesoPlato' y luego a 'pesoRecipiente' para saber el 'peroARetirar'.
            
            pedirRecipiente();              // Pedir recipiente
        }

        doneState = true;                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
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

            if(state_prev == STATE_Empty){  // ==> Si se viene de STATE_Empty, donde se ha tarado.
                tarado = false;                  // Desactivar flag de haber 'tarado' 
            }
                                            // El peso definitivo del recipiente no se guarda hasta que se escoja grupo de alimentos.
                                            // Por eso, 'pesoRecipiente' y 'pesoARetirar' son iguales a 0.0 en este estado. Esto no impide que se
                                            // realice LIBERAR, ya que ese evento ocurre cuando el peso de la báscula es igual a 'pesoARetirar'. 
                                            // Así, se puede detectar que se ha quitado el recipiente (bascula = 0.0) y se vuelve al estado STATE_Empty.
            
            pedirGrupoAlimentos();          // Print info del estado.
        }
            
        doneState = true;                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                            // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                            // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_groupA o STATE_groupB
----------------------------------------------------------------------------------------------------------*/
void actGruposAlimentos(){ 
    static unsigned long previousTime;     // Tiempos utilizados para regresar al STATE_Empty (pedir recipiente) si pasan 5 segundos sin
    unsigned long currentTime;             // colocar alimento y previamente no se colocó recipiente, sino que se escogió grupo directamente. 
                                           // El "temporizador" de 5 segundos se reinicia cada vez que se escoge un grupo nuevo.

    if(!doneState){        

        previousTime = millis();           // Reiniciar "temporizador" de 5 segundos para regresar a STATE_Empty y pedir recipiente.
        
        // ----- ACCIONES ------------------------------
        if(state_prev == STATE_Plato){                                     // ==> Si se viene de STATE_Plato porque se acaba de colocar el recipiente.
            pesoRecipiente = pesoBascula;                                   // Se guarda 'pesoRecipiente' para sumarlo a 'pesoPlato' y saber el 'pesoARetirar'.
            //tareScale();                                                    // Tarar para tomar peso real del alimento que se va a colocar.
        }
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
        // ----- FIN ACCIONES --------------------------


        // ----- INFO PANTALLA -------------------------
        if((state_prev != STATE_groupA) && (state_prev != STATE_groupB)){ // ==> Si es la primera vez que se escoge grupo, se muestra todo (ejemplos, plato actual y acumulado)
            tareScale();            // Tarar al seleccionar un grupo de alimentos nuevo, a no ser que se estén leyendo ejemplos.
                                    // Esta tara DEBE hacerse antes de showFullDashboard() para que muestre el peso a 0.      
            showFullDashboard(true);             
        }
        else{   // Si se está pulsando grupos para ver sus ejemplos, solo se van modificando los grupos y sus ejemplos
            printGrupoyEjemplos();
            tarado = false;        // Desactivar flag de haber 'tarado' 
        }
        // ----- FIN INFO PANTALLA ---------------------

            
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                            // las acciones.
    }


    // ----- TIEMPO DE ESPERA ------------------------------
    if((pesoRecipiente == 0.0) && (pesoPlato == 0.0)){                // Si se escogió grupo sin colocar recipiente y aún no se ha colocado alimento
        currentTime = millis();
        if ((currentTime - previousTime) > 5000) {                    // Si han pasado 5 segundos sin colocar alimento, se vuelve a STATE_Empty para pedir recipiente
            Serial.print(F("\nLIBERAR forzada. Regreso a INI..."));
            eventoBascula = LIBERAR;                                  // Forzar regreso a STATE_Empty para volver a pedir recipiente
            addEventToBuffer(eventoBascula);
            flagEvent = true;
        }
    }
    // ----- FIN TIEMPO DE ESPERA --------------------------

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
            
            // Si se ha dado a add/delete/save, pero finalmente se cancela esa acción, entonces se 
            // muestra el dashboard completo y no se modifica únicamente la zona 2 (procesamiento).
            if(state_prev == STATE_CANCEL) showFullDashboard(true);     
            else printProcesamiento();                                      // Mostrar imagen de 'crudo'

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
            
            // Si se ha dado a add/delete/save, pero finalmente se cancela esa acción, entonces se 
            // muestra el dashboard completo y no se modifica únicamente la zona 2 (procesamiento).
            if(state_prev == STATE_CANCEL) showFullDashboard(true);    
            else printProcesamiento();                                      // Mostrar imagen de 'cocinado'

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
        
        // Si se ha dado a add/delete/save, pero finalmente se cancela esa acción, entonces se 
        // muestra el dashboard completo y no se modifica únicamente la zona 2 (procesamiento).
        if(state_prev == STATE_CANCEL) showFullDashboard(false);    // 'false' para mostrar valores temporales del Plato actual según el peso del alimento
        else printPlatoActual(false);               // 'false' para mostrar valores temporales del Plato actual según el peso del alimento

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
        
        pedirConfirmacion(1);                                               // Mostrar pregunta de confirmación para añadir plato
        
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
    static unsigned long previousTimeWarning;     // Tiempos utilizados para regresar a STATE_groupA / B si se ha querido borrar pero el plato 
    unsigned long currentTime;                    // estaba vacío. Se muestra un aviso y pasados 3 segundos se regresa al estado según el último grupo escogido.

    static unsigned long previousTimeCancel = millis();      // Tiempo usado para cancelar la acción tras 10 segundos de inactividad

    static bool errorPlatoWasEmpty;        // Flag utilizada para saber si se debe mostrar la información "normal"
                                           // o un mensaje de aviso indicando no se ha creado otro plato porque el 
                                           // actual está vacío.
    if(!doneState){
        //static bool errorPlatoWasEmpty;                             // Flag utilizada para saber si se debe mostrar la información "normal"
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

            /* ----- ACTUALIZAR PLATO  ----- */
            if(pesoBascula != 0.0){                       // ==> Si se ha colocado algo nuevo en la báscula y no se ha retirado,
                                                          //     debe incluirse en el plato. 
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
            }
            /* --------------------------------------- */


            /* -----  INFORMACIÓN MOSTRADA  ---------------------------------- */
            if(!errorPlatoWasEmpty){           // ==> Si el plato no estaba vacío y se ha guardado/creado otro 
                showAccionConfirmada(1);            // Mostrar mensaje de plato añadido
            }
            else{   // ==> Si el plato está vacío, no se crea otro
                showWarning(1); // Mostrar aviso de plato vacío y no se crea otro
                Serial.println(F("No se ha creado otro plato porque el actual está vacío"));
                previousTimeWarning = millis();   // Reiniciar "temporizador" de 3 segundos para, tras NO borrar, regresar a STATE_groupA / B, según el último grupo escogido.
            }
            /* ----- FIN INFO MOSTRADA ---------------------------------------- */


        } // FIN if(state_prev != STATE_added)
        else if(tarado){           // ==> Si se viene del propio STATE_added, donde se puede haber tarado.
            tarado = false;              // Desactivar flag de haber 'tarado'.          
        }
                                                 
        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                    // las acciones.
    }

    // ----- TIEMPO DE ESPERA ------------------------------
    if(errorPlatoWasEmpty){     // Si no se ha creado otro plato porque el plato actual está vacío 
        currentTime = millis();
        if ((currentTime - previousTimeWarning) > 3000) {    // Tras 3 segundos mostrando warning...
            Serial.print(F("\nTIPO_A o TIPO_B forzada. Regreso a GRUPOS..."));
            addEventToBuffer(eventoGrande);   // Se regresa a STATE_groupA o STATE_groupB, según el último grupo escogido. 
            flagEvent = true;
                                              // Si se había colocado recipiente, se mantiene su peso al volver a STATE_groupA / B y se podrá regresar a STATE_Empty si se retira.
                                              // Si no se había colocado recipiente, al regresar a STATE_groupA / B salta el temporizador de 5 segundos para volver a STATE_Empty a  
                                              // pedir recipiente si no se coloca ningún alimento.
        }
    }

    // --- CANCELACIÓN AUTOMÁTICA ---
    if ((millis() - previousTimeCancel) > 10000) {    // Tras 10 segundos de inactividad, se cancela automáticamente la acción añadir plato
        Serial.print(F("\nTime out. Cancelando añadir plato..."));
        addEventToBuffer(CANCELAR);   // Se transiciona al STATE_CANCEL, que regresa automáticamente al estado desde donde se inició añadir plato
        flagEvent = true;
    }
    // --- FIN CANCELACIÓN AUTOMÁTICA ---
    // ----- FIN TIEMPO DE ESPERA --------------------------
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
        
        pedirConfirmacion(2);                                               // Mostrar pregunta de confirmación para eliminar plato
        
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
    static unsigned long previousTime;     // Tiempos utilizados para regresar a STATE_groupA / B si se ha querido borrar pero el plato 
    unsigned long currentTime;             // estaba vacío. Se muestra un aviso y pasados 3 segundos se regresa al estado según el último grupo escogido.

    static bool errorPlatoWasEmpty;        // Flag utilizada para saber si se debe mostrar la información "normal"
                                           // o un mensaje de aviso indicando no se ha borrado el plato porque el 
                                           // actual ya está vacío.

    if(!doneState){

        // -------------------- DELETE ACTUAL ------------------------        
        if(state_prev != STATE_deleted){                            // ==> Si no se viene del propio STATE_deleted, para evitar que se vuelva 
                                                                    //     a eliminar el plato de la comida.
          
            
            Serial.println(F("\nEliminando plato..."));

            
            /* ----- PESO ÚLTIMO ALIMENTO  ----- */
            if(pesoBascula != 0.0){           // ==> Si se ha colocado algo nuevo en la báscula (pesoBascula marca algo) y no se ha retirado,
                                              //     debe incluirse en el plato. 

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
                    errorPlatoWasEmpty = false;       // Para mostrar mensaje de plato borrado y pedir retirarlo, aunque no sea cierto porque no se ha guardado el
                                                      // último alimento (solo su peso) y el objeto Plato está realmente vacío.
                    pesoPlato = pesoLastAlimento;     // Incluir último alimento en el 'pesoPlato' para saber 'pesoARetirar'.
                }
                else{   // PLATO VACÍO Y NO HAY ÚLTIMO ALIMENTO ==> NADA QUE RETIRAR 
                    errorPlatoWasEmpty = true;
                }
            }


            /* -----  INFORMACIÓN MOSTRADA  ---------------------------------- */
            if(!errorPlatoWasEmpty){   // ==> Si la comida no estaba vacía (incluyendo último alimento) y se ha borrado 
                showAccionConfirmada(2);     // Mostrar confirmacion de haber eliminado el plato
            }
            else{     // ==> Si el plato está vacío, no se borra
                showWarning(2); // Mostrar aviso de plato vacío y no se borra
                Serial.println(F("No se ha borrado el plato porque está vacío"));
                previousTime = millis();   // Reiniciar "temporizador" de 3 segundos para, tras NO borrar, regresar a STATE_groupA / B, según el último grupo escogido.
            }
            /* ----- FIN INFO MOSTRADA ---------------------------------------- */


        } // FIN if(state_prev != STATE_deleted)
        else if(tarado){                                            // ==> Si se viene del propio STATE_deleted, donde se puede haber tarado.
            tarado = false;                                         // Desactivar flag de haber 'tarado'.        
        }
        // ----------- FIN DELETE ACTUAL --------------------------------------------


            doneState = true;                                       // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                    // las acciones.
    }

    // ----- TIEMPO DE ESPERA ------------------------------
    if(errorPlatoWasEmpty){     // Si no se ha borrado nada porque el plato está vacío 
        currentTime = millis();
        if ((currentTime - previousTime) > 3000) {    // Tras 3 segundos mostrando warning...
            Serial.print(F("\nTIPO_A o TIPO_B forzada. Regreso a GRUPOS..."));
            addEventToBuffer(eventoGrande);   // Se regresa a STATE_groupA o STATE_groupB, según el último grupo escogido. 
            flagEvent = true;
                                              // Si se había colocado recipiente, se mantiene su peso al volver a STATE_groupA / B y se podrá regresar a STATE_Empty si se retira.
                                              // Si no se había colocado recipiente, al regresar a STATE_groupA / B salta el temporizador de 5 segundos para volver a STATE_Empty a  
                                              // pedir recipiente si no se coloca ningún alimento.
        }
    }
    // ----- FIN TIEMPO DE ESPERA --------------------------



    /*
    if(!doneState){

        // -------------------- DELETE RETROACTIVO ------------------------
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
            showAccionConfirmada(2);                                 // Mostrar confirmación de haber eliminado el plato
        } // ---------------------------------- 
        
        
        // --------------------------------------------------------------
        // -------------- FIN DELETE RETROACTIVO ----------------------------------------------
        

        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                    // las acciones.
    }*/

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
        
        pedirConfirmacion(3);                                               // Mostrar pregunta de confirmación para guardar comida
        
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
    // Tiempo utilizado para regresar a ciertos estados (tras 3 segundos de warning) si se ha querido guardar pero la comida estaba vacía:
    
    static unsigned long previousTimeEmpty;   // Para regresar a STATE_Empty si se ha querido guardar desde ahí, tras añadir o eliminar. No se usa si la
                                              // comida no se guarda porque estuviera vacía.
    
    static unsigned long previousTimeGroups;  // Para regresar a STATE_groupA / B si se ha querido guardar desde ahí.
    
    unsigned long currentTime;                // Tiempo actual            

    
    static bool errorComidaWasEmpty;       // Flag utilizada para saber si se debe mostrar la información "normal"
                                           // o un mensaje de aviso indicando no se ha guardado la comida porque  
                                           // está vacía.

    if(!doneState){
      
        if(state_prev != STATE_saved){                              // ==> Si no se viene del propio STATE_saved (por TARAR, QUITAR, INCREMENTO), 
                                                                    //     para evitar que se vuelva a guardar la comida en el diario.
            
                                                                    // *****
                                                                    // Primero se actualiza la comida, si es necesario, y después se muestran las actualizaciones.
                                                                    // Si tras comprobar cambios en la comida se ve que sigue vacía, entonces se indica en un mensaje 
                                                                    // que no se ha podido guardar.   
                                                                    // *****                  
          
            Serial.println(F("\nGuardando comida..."));  

            /* ----- ACTUALIZAR PLATO  ----------------------------------- */
            if(pesoBascula != 0.0){               // ==> Si se ha colocado algo nuevo en la báscula y no se ha retirado,
                                                  //     debe incluirse en el plato. 
                                                                    // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                                    // guardar la comida actual sin haber colocado un nuevo alimento, no haría falta
                                                                    // actualizar la comida, pues no se habría modificado.
            
                Alimento alimento(grupoEscogido, pesoBascula);      // Cálculo automático de valores nutricionales.
                                                                    // Usamos 'grupoEscogido' porque no se ha modificado. 
                
                platoActual.addAlimentoPlato(alimento);                       // Alimento ==> Plato
                comidaActual.addAlimentoComida(alimento);                     // Alimento ==> Comida

                /* ----- TARAR  ----- */
                tareScale();                                        // Se debe tarar para que conforme vaya disminuyendo el peso veamos si 
                                                                    // se ha quitado todo el 'pesoARetirar'.
                                                                    // Solo se hace si se viene del STATE_weighted porque en los estados 
                                                                    // previos a ese ya se tara y es en STATE_weighted donde se modifica el peso.
            }
            /* ----- FIN ACTUALIZAR PLATO --------------------------------- */


            /* ----- GUARDAR PLATO EN COMIDA  ----------------------------- */
            if(!platoActual.isPlatoEmpty()){   // PLATO CON COSAS ==> INCLUIR EN COMIDA ACTUAL                   

                                               // Si se viene de STATE_added o STATE_deleted, el plato ya estará vacío.

                    pesoPlato = platoActual.getPesoPlato();         // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                    comidaActual.addPlato(platoActual);             // Plato ==> Comida (no se vuelven a incluir los alimentos. Solo modifica peso y nPlatos)
                    platoActual.restorePlato();                     // "Reiniciar" platoActual para usarlo de nuevo.
            }
            /* ----- FIN GUARDAR PLATO EN COMIDA -------------------------- */


            /* ----- GUARDAR COMIDA EN DIARIO ----------------------------- */
            if(!comidaActual.isComidaEmpty()){    // COMIDA CON PLATOS ==> HAY QUE GUARDAR
                errorComidaWasEmpty = false;
                diaActual.addComida(comidaActual);        // Comida ==> Diario
                saveComidaSD();                           // Comida ==> fichero CSV en SD
                comidaActual.restoreComida();             // "Reiniciar" comidaActual para usarla de nuevo.
            }
            else{   // COMIDA VACÍA ==> NO HAY QUE GUARDAR
                errorComidaWasEmpty = true;
            }
            /* ----- FIN GUARDAR COMIDA EN DIARIO ------------------------- */


            /* -----  INFORMACIÓN MOSTRADA  ------------------------------- */
            if(!errorComidaWasEmpty){     // ==> Si la comida no estaba vacía y se ha guardado
                showAccionConfirmada(3);          //  Mostrar confirmación de haber guardado la comida
                Serial.println(F("COMIDA GUARDADA"));
                // No se pone if(pesoARetirar ...) porque aún no ha dado tiempo a actualizar 'pesoARetirar' y puede ser incorrecto
                if((pesoRecipiente + pesoPlato) == 0.0) previousTimeEmpty = millis();     //  Reiniciar "temporizador" de 3 segundos para, tras guardar, regresar a STATE_Empty.
                // else -> Si hay algo que retirar (pesoARetirar != 0.0), lo dirá el mensaje de confirmación

            }
            else{     // ==> Si la comida estaba vacía y no se ha guardado
                showWarning(3); // Mostrar aviso de comida vacía y no se guarda
                Serial.println(F("No se ha guardado la comida porque está vacía"));
                previousTimeGroups = millis();   // Reiniciar "temporizador" de 3 segundos para, tras NO guardar, regresar a STATE_groupA / B, según el último grupo escogido.
            }
            /* ----- FIN INFO MOSTRADA ------------------------------------ */
        

        } // FIN if(state_prev != STATE_saved)
        else if(tarado){                                            // ==> Si se viene del propio STATE_saved, donde se puede haber tarado.
            tarado = false;                                         // Desactivar flag de haber 'tarado'.          
        }

        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                    // las acciones.
    }


    // ----- TIEMPO DE ESPERA ------------------------------
    if(!errorComidaWasEmpty){     // ==> Si se ha guardado la comida desde el STATE_Empty, tras añadir o eliminar. 
        // Aquí ya sí se puede poner 'pesoARetirar' porque en los 3 segundos de esperar da tiempo a que se actualice.
        // Si no, habría que poner (pesoRecipiente + pesoPlato), igual que antes.
        if(pesoARetirar == 0.0){    // ==> Si no hay nada que retirar (STATE_Empty), se fuerza el regreso. 
                                    //     Se especifica esta condición para que no regrese a STATE_Empty con plato sobre báscula
            currentTime = millis();
            if ((currentTime - previousTimeEmpty) > 3000) {    // Tras 3 segundos mostrando confirmación de haber guardado...
                Serial.print(F("\nLIBERAR forzada. Regreso a INI..."));
                eventoBascula = LIBERAR;
                addEventToBuffer(eventoBascula);  // Se regresa a STATE_Empty
                flagEvent = true;
            }
        }
    }
    else{     // Si no se ha guardado la comida porque está vacía
        currentTime = millis();
        if ((currentTime - previousTimeGroups) > 3000) {    // Tras 3 segundos mostrando warning...
            if((eventoGrande != TIPO_A) and (eventoGrande != TIPO_B)){  
                // Puede que se haya querido dar a guardar nada más encender, en cuyo caso 'eventoGrande' aún no tiene ningún
                // valor como para regresar a alguno de los estados de grupos de alimentos.
                // Ese este caso, se regresa al STATE_Empty, que es en realidad desde donde se ha querido guardar.
                // Este es un caso muy poco probable, pero si surgiera, el programa se quedaría atascado puesto
                // que espera la liberación forzada para regresar.
                Serial.print(F("\nLIBERAR forzada por vacía. Regreso a INI..."));
                eventoBascula = LIBERAR;
                addEventToBuffer(eventoBascula);  // Se regresa a STATE_Empty
                flagEvent = true;
            }
            else{
                // Si se había colocado recipiente, se mantiene su peso al volver a STATE_groupA / B y se podrá regresar a STATE_Empty si se retira.
                // Si no se había colocado recipiente, al regresar a STATE_groupA / B salta el temporizador de 5 segundos para volver a STATE_Empty a  
                // pedir recipiente si no se coloca ningún alimento.
                Serial.print(F("\nTIPO_A o TIPO_B forzada. Regreso a GRUPOS..."));
                addEventToBuffer(eventoGrande);   // Se regresa a STATE_groupA o STATE_groupB, según el último grupo escogido. 
            }
            flagEvent = true;
                                              
        }
    }
    // ----- FIN TIEMPO DE ESPERA --------------------------
}



/*---------------------------------------------------------------------------------------------------------
   actStateERROR(): Acciones del STATE_ERROR
----------------------------------------------------------------------------------------------------------*/
void actStateERROR(){ 
    static unsigned long previousTimeError;  // Para regresar a STATE_groupA / B si se ha querido guardar desde ahí.
    unsigned long currentTime;                // Tiempo actual  

    if(!doneState){
        previousTimeError = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de error, regresar al estado anterior.

        /* -----  INFORMACIÓN MOSTRADA  ------------------------------- */
        Serial.println(F("Mensaje de error")); 
        // Mensaje de error según estado en el que se ha cometido el error (state_prev)
        switch (state_prev){
          case 1:   showError(1);   break;  // Empty
          case 2:   showError(2);   break;  // Plato
          case 3:   showError(3);   break;  // grupoA 
          case 4:   showError(4);   break;  // grupoB
          case 5:   showError(5);   break;  // Crudo
          case 6:   showError(6);   break;  // Cocinado
          case 7:   showError(7);   break;  // Pesado
          case 8:   showError(8);   break;  // add_check
          case 9:   showError(9);   break;  // Added
          case 10:  showError(10);  break;  // delete_check
          case 11:  showError(11);  break;  // Deleted
          case 12:  showError(12);  break;  // save_check
          case 13:  showError(13);  break;  // Saved
          default:  break;  
        }
        /* ----- FIN INFO MOSTRADA ------------------------------------ */
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                                            // las acciones.
    }


    // ----- TIEMPO DE ESPERA ------------------------------
    currentTime = millis();
    if ((currentTime - previousTimeError) > 3000) {    // Tras 3 segundos mostrando error...
        // Regresar al estado anterior desde donde se cometió el error
        switch (state_prev){
          case 1:   Serial.print(F("\nRegreso a Empty tras ERROR..."));         addEventToBuffer(BACK_TO_EMPTY);         flagEvent = true;   break;  // Empty
          case 2:   Serial.print(F("\nRegreso a Plato tras ERROR..."));         addEventToBuffer(BACK_TO_PLATO);         flagEvent = true;   break;  // Plato
          case 3:   Serial.print(F("\nRegreso a groupA tras ERROR..."));        addEventToBuffer(BACK_TO_GROUP_A);       flagEvent = true;   break;  // grupoA 
          case 4:   Serial.print(F("\nRegreso a groupB tras ERROR..."));        addEventToBuffer(BACK_TO_GROUP_B);       flagEvent = true;   break;  // grupoB
          case 5:   Serial.print(F("\nRegreso a raw tras ERROR..."));           addEventToBuffer(BACK_TO_RAW);           flagEvent = true;   break;  // Crudo
          case 6:   Serial.print(F("\nRegreso a cooked tras ERROR..."));        addEventToBuffer(BACK_TO_COOKED);        flagEvent = true;   break;  // Cocinado
          case 7:   Serial.print(F("\nRegreso a weighted tras ERROR..."));      addEventToBuffer(BACK_TO_WEIGHTED);      flagEvent = true;   break;  // Pesado
          case 8:   Serial.print(F("\nRegreso a add_check tras ERROR..."));     addEventToBuffer(BACK_TO_ADD_CHECK);     flagEvent = true;   break;  // add_check
          case 9:   Serial.print(F("\nRegreso a added tras ERROR..."));         addEventToBuffer(BACK_TO_ADDED);         flagEvent = true;   break;  // Added
          case 10:  Serial.print(F("\nRegreso a delete_check tras ERROR..."));  addEventToBuffer(BACK_TO_DELETE_CHECK);  flagEvent = true;   break;  // delete_check
          case 11:  Serial.print(F("\nRegreso a deleted tras ERROR..."));       addEventToBuffer(BACK_TO_DELETED);       flagEvent = true;   break;  // Deleted
          case 12:  Serial.print(F("\nRegreso a save_check tras ERROR..."));    addEventToBuffer(BACK_TO_SAVE_CHECK);    flagEvent = true;   break;  // save_check
          case 13:  Serial.print(F("\nRegreso a saved tras ERROR..."));         addEventToBuffer(BACK_TO_SAVED);         flagEvent = true;   break;  // Saved
          default:  break;  
        }

        flagError = false; // Reiniciar flag de error hasta que se vuelva a cometer
                  
    }
    // ----- FIN TIEMPO DE ESPERA --------------------------
}



/*---------------------------------------------------------------------------------------------------------
   actStateCANCEL(): Acciones del STATE_CANCEL
----------------------------------------------------------------------------------------------------------*/
void actStateCANCEL(){ 
    if(!doneState){ // ¿Hace falta aquí si es inmediato?

        /* -----  INFORMACIÓN MOSTRADA  ------------------------------- */
        Serial.println(F("Acción cancelada")); 

        // Regreso al estado desde donde se inició la acción cancelada.
        // Es previo al último estado (add_check, delete_check o save_check), por eso es 'state_prev_prev'
        switch (state_prev_prev){
          case 1:   Serial.print(F("\nRegreso a Empty tras CANCELACION..."));       addEventToBuffer(BACK_TO_EMPTY);       break;  // Empty
          case 5:   Serial.print(F("\nRegreso a raw tras CANCELACION..."));         addEventToBuffer(BACK_TO_RAW);         break;  // Crudo
          case 6:   Serial.print(F("\nRegreso a cooked tras CANCELACION..."));      addEventToBuffer(BACK_TO_COOKED);      break;  // Cocinado
          case 7:   Serial.print(F("\nRegreso a weighted tras CANCELACION..."));    addEventToBuffer(BACK_TO_WEIGHTED);    break;  // Pesado
          default:  break;  
        }

        flagEvent = true; 
        

        doneState = true;             // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                      // cada vez que se entre a esta función debido al loop de Arduino.
                                      // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                      // las acciones.

                                      // ¿Hace falta aquí si es inmediato? 

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
      case 14:  actStateERROR();        break;
      case 15:  actStateCANCEL();       break;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actEventError(): Información de error según estado cuando ocurre un evento que no correspondía
----------------------------------------------------------------------------------------------------------*/
void actEventError(){
    // Independientemente del estado actual, se marca el evento ERROR para pasar al STATE_ERROR, donde 
    // se muestra una pantalla de error según el estado previo desde donde se cometió el error. 
    // Tras 3 segundos con la pantalla de error, se marca el evento BACK_TO_<estado> correspondiente para 
    // regresar al estado previo.

    // Inicialmente se pensó hacer esta espera de 3 seg reiniciando la variable 'doneState', de forma que simplemente
    // se volvieran a hacer las actividades del estado en que se estuviera. El problema es que algunos estados 
    // no vuelven a hacerlas si vienen del propio estado, para evitar reimprimir pantallas, por ejemplo. 
    // Por eso, un estado ficticio de error parece ser la mejor solución.

    Serial.print(F("\nERROR..."));
    addEventToBuffer(ERROR);    
    flagError = true;
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
   getFirstGapBuffer(): Primer hueco libre del buffer
----------------------------------------------------------------------------------------------------------*/
int getFirstGapBuffer(){
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
            pos = getFirstGapBuffer();
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
