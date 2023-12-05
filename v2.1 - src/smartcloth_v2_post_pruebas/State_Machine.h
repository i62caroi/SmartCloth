/** 
 * @file State_Machine.h
 * @brief Máquina de Estados (State Machine) de SmartCloth
 *
 * @author Irene Casares Rodríguez
 * @date 17/07/23
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
#define RULES 133   // 7 SON DE BORRAR CSV EN PRUEBAS


#include "SD_functions.h"
#include "Buttons.h"
#include "Serial_esp32cam.h"
#include "lista_Comida.h" 



// --- PEDIR CONFIRMACIÓN ---
#define  ASK_CONFIRMATION_ADD     1
#define  ASK_CONFIRMATION_DELETE  2
#define  ASK_CONFIRMATION_SAVE    3  

// --- ACCIÓN CONFIRMADA ---
#define  ADD_EXECUTED                             1
#define  DELETE_EXECUTED                          2
#define  SAVE_EXECUTED_FULL                       3  
#define  SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP      4
#define  SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI         5
#define  SAVE_ESP32_TIMEOUT                       6
#define  SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR   7

// --- RESPUESTAS AL GUARDAR EN DATABASE ---
#define  SAVED_OK     1
#define  ERROR_HTTP   2
#define  NO_WIFI      3
#define  TIMEOUT      4
#define  UNKNOWN      5

// --- MENSAJE DE AVISO ---
#define  WARNING_NOT_ADDED      1
#define  WARNING_NOT_DELETED    2
#define  WARNING_NOT_SAVED      3

// --- MENSAJE DE ERROR ----
#define  ERROR_STATE_INIT            1
#define  ERROR_STATE_PLATO           2
#define  ERROR_STATE_GROUPS          3  // groupA o groupB
#define  ERROR_STATE_PROCESAMIENTO   4  // raw o cooked
#define  ERROR_STATE_WEIGHTED        5
#define  ERROR_STATE_ADD_CHECK       6
#define  ERROR_STATE_ADDED           7
#define  ERROR_STATE_DELETE_CHECK    8
#define  ERROR_STATE_DELETED         9
#define  ERROR_STATE_SAVE_CHECK      10
#define  ERROR_STATE_SAVED           11
#define  ERROR_STATE_CANCEL          12
#define  ERROR_STATE_AVISO           13

// --- PARPADEO DE ZONAS 1 Y 2 CUANDO NO ESCOGIDO ---
//#define  BLINK_AMBAS_ZONAS true
//#define  BLINK_SOLO_ZONA2  false

// --- MENSAJE MOSTRADO CUANDO ZONA 1 VACÍA ---
#define  NO_MSG             0
#define  MSG_SIN_RECIPIENTE 1
#define  MSG_SIN_GRUPO      2



void    tareScale();
void    checkAllButtons(); 
bool    buttonInterruptOccurred(); 
bool    hasScaleEventOccurred(); 



/**
 * @brief Flag para indicar si se han realizado las actividades del estado.
 */
bool doneState;       // Flag para que solo se realicen una vez las actividades del estado cada vez que se entre al estado, 
                      // pero no en cada iteración del loop de Arduino.


/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- ESTADOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/**
 * @enum state_t
 * @brief Enumeración de los diferentes estados de la Máquina de Estados.
 */
typedef enum {
              STATE_Init          =   (1),    // Estado inicial para colocar plato
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
              STATE_CANCEL        =   (15),   // Estado ficticio de Cancelación
              STATE_AVISO         =   (16),   // Estado ficticio de Aviso

              STATE_DELETE_CSV_CHECK    =   (17),   // COMPROBAR QUE SE QUIERE BORRAR EL CSV. EL USUARIO NO DEBERÍA ACCEDER. SOLO PARA LAS PRUEBAS.
              STATE_DELETED_CSV         =   (18),   // FICHERO CSV BORRADO. EL USUARIO NO DEBRÍA ACCEDER. SOLO PARA LAS PRUEBAS.
                                                    //  --> PARA LIMPIAR EL ACUMULADO DEL DÍA, DEJÁNDOLO LISTO PARA EL SIGUIENTE PACIENTE

              STATE_CRITIC_FAILURE_SD   =   (19)    // La SD ha fallado en el setup o no se encuentra --> no se permite usar SM
                                                    // Este estado no tiene transiciones de entrada ni de salida. Solo sirve para mostrar
                                                    // la pantalla de fallo crítico.
} state_t;



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
              LIBERAR               =   (11),   // Báscula vacía real
              ERROR                 =   (12),   // Error (acción incorrecta)
              CANCELAR              =   (13),   // Cancelar acción de añadir, eliminar o guardar
              AVISO                 =   (14),   // Aviso de plato o comida vacía
              GO_TO_INIT            =   (15),   // Evento ficticio para volver a STATE_Init porque saltó un error, aviso o se canceló una acción (añadir, eliminar o guardar)
              GO_TO_PLATO           =   (16),   // Evento ficticio para volver a STATE_Plato porque saltó un error 
              GO_TO_GROUP_A         =   (17),   // Evento ficticio para volver a STATE_groupA porque saltó un error o aviso
              GO_TO_GROUP_B         =   (18),   // Evento ficticio para volver a STATE_groupB porque saltó un error o aviso
              GO_TO_RAW             =   (19),   // Evento ficticio para volver a STATE_raw porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              GO_TO_COOKED          =   (20),   // Evento ficticio para volver a STATE_cooked porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              GO_TO_WEIGHTED        =   (21),   // Evento ficticio para volver a STATE_weighted porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              GO_TO_ADD_CHECK       =   (22),   // Evento ficticio para volver a STATE_add_check porque saltó un error 
              GO_TO_ADDED           =   (23),   // Evento ficticio para volver a STATE_added porque saltó un error 
              GO_TO_DELETE_CHECK    =   (24),   // Evento ficticio para volver a STATE_delete_check porque saltó un error 
              GO_TO_DELETED         =   (25),   // Evento ficticio para volver a STATE_deleted porque saltó un error
              GO_TO_SAVE_CHECK      =   (26),   // Evento ficticio para volver a STATE_save_check porque saltó un error 
              GO_TO_SAVED           =   (27),   // Evento ficticio para volver a STATE_saved porque saltó un error 
              GO_TO_CANCEL          =   (28),   // Evento ficticio para ir a STATE_CANCEL si se cancela una acción iniciada durante un error
              DELETE_CSV            =   (29)    // EVENTO PARA BORRAR EL FICHERO CSV. EL USUARIO NO DEBERÍA LLEGAR A ACTIVARLO. SOLO PARA LAS PRUEBAS.
} event_t;


/**
 * @brief Buffer para guardar los eventos que ocurren en la Máquina de Estados.
 */
static event_t event_buffer[MAX_EVENTS];       // Buffer de eventos al que se irán añadiendo conforme ocurran
/* Este buffer solo se utiliza para debuggear, para ver qué eventos han ido ocurriendo.
   Para comprobar si se cumple alguna regla de transición se utiliza 'lastEvent', que 
   sería el último evento añadido al buffer.
   
   Se podría eliminar el buffer para evitar ocupar memoria innecesariamente.
*/



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ REGLAS TRANSICION ---------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/**
 * @struct transition_rule
 * @brief Estructura que define una regla de transición en la Máquina de Estados.
 */
typedef struct{
    state_t state_i;     /**< Estado actual */
    state_t state_j;     /**< Estado siguiente */
    event_t condition;    /**< Condición para transición de estado */
}transition_rule;


/**
 * @brief Array de reglas de transición para la Máquina de Estados.
 */
static transition_rule rules[RULES] = { // --- Esperando Recipiente ---
                                        {STATE_Init,STATE_Init,TARAR},                 // Tara inicial
                                        {STATE_Init,STATE_Plato,INCREMENTO},           // Colocar recipiente
                                        {STATE_Init,STATE_save_check,GUARDAR},         // Guardar comida directamente (comidaActual no está vacía)
                                        {STATE_Init,STATE_ERROR,ERROR},                // Acción incorrecta
                                        // ----------------------------

                                        // --- Recipiente colocado ---
                                        {STATE_Plato,STATE_Plato,INCREMENTO},           // Cambios por recolocar recipiente
                                        {STATE_Plato,STATE_Plato,DECREMENTO},           // Cambios por recolocar recipiente
                                        {STATE_Plato,STATE_Init,LIBERAR},               // Se ha retirado el recipiente
                                        {STATE_Plato,STATE_groupA,TIPO_A},              // Escoger alimento tipo A
                                        {STATE_Plato,STATE_groupB,TIPO_B},              // Escoger alimento tipo B
                                        {STATE_Plato,STATE_ERROR,ERROR},                // Acción incorrecta             
                                        // ---------------------------

                                        // --- Alimentos grupo A ---
                                        {STATE_groupA,STATE_Init,LIBERAR},              // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_groupA,STATE_groupA,DECREMENTO},         // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR)
                                        {STATE_groupA,STATE_groupA,TIPO_A},                
                                        {STATE_groupA,STATE_groupA,TARAR},              // Tarar tras colocar recipiente o alimento   
                                        {STATE_groupA,STATE_groupB,TIPO_B},                
                                        {STATE_groupA,STATE_raw,CRUDO},                         
                                        {STATE_groupA,STATE_cooked,COCINADO},              
                                        {STATE_groupA,STATE_ERROR,ERROR},               // Acción incorrecta
                                        // --------------------------

                                        // --- Alimentos grupo B ---
                                        {STATE_groupB,STATE_Init,LIBERAR},              // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_groupB,STATE_groupB,DECREMENTO},         // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR)
                                        {STATE_groupB,STATE_groupB,TIPO_B},                
                                        {STATE_groupB,STATE_groupB,TARAR},              // Tarar cada vez que se escoja grupo. Podría ser tras colocar recipiente o alimento   
                                        {STATE_groupB,STATE_groupA,TIPO_A},                
                                        {STATE_groupB,STATE_raw,CRUDO},                         
                                        {STATE_groupB,STATE_cooked,COCINADO},              
                                        {STATE_groupB,STATE_ERROR,ERROR},               // Acción incorrecta
                                        // -------------------------

                                        // --- Alimento crudo ---
                                        {STATE_raw,STATE_Init,LIBERAR},                 // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_raw,STATE_raw,DECREMENTO},               // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR)
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
                                        {STATE_cooked,STATE_Init,LIBERAR},              // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_cooked,STATE_cooked,DECREMENTO},         // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR)
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
                                        {STATE_weighted,STATE_Init,LIBERAR},              // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_weighted,STATE_weighted,INCREMENTO},       // Se coloca más alimento.  
                                        {STATE_weighted,STATE_weighted,DECREMENTO},       // Se retira alimento.  Esta regla permite que se coloque algo en la báscula y luego se retire para
                                                                                          //                      escoger un grupo diferente porque nos hayamos confundido y sin guardar lo 
                                                                                          //                      que se hubiera colocado, pues al retirarlo su peso sería 0.0 .
                                        {STATE_weighted,STATE_groupA,TIPO_A},             // Escoger nuevo grupo. Se guarda el peso del alimento en báscula.
                                        {STATE_weighted,STATE_groupB,TIPO_B},             // Escoger nuevo grupo. Se guarda el peso del alimento en báscula.
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
                                        {STATE_add_check,STATE_ERROR,ERROR},             // Acción incorrecta (solo salta si se manipula la báscula).
                                        // --------------------------

                                        // --- Plato añadido ---
                                        {STATE_added,STATE_added,TARAR},                 // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_added,STATE_added,DECREMENTO},            // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR).
                                        {STATE_added,STATE_added,INCREMENTO},            // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_added,STATE_Init,LIBERAR},                // Se ha retirado el plato completo (+ recipiente) tras añadir uno nuevo.
                                        {STATE_added,STATE_ERROR,ERROR},                 // Acción incorrecta
                                        {STATE_added,STATE_AVISO,AVISO},                 // No se ha añadido un plato porque el actual está vacío
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
                                        {STATE_delete_check,STATE_ERROR,ERROR},             // Acción incorrecta (solo salta si se manipula la báscula).
                                        // --------------------------

                                        // --- Plato eliminado ---
                                        {STATE_deleted,STATE_deleted,TARAR},             // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_deleted,STATE_deleted,DECREMENTO},        // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR).
                                        {STATE_deleted,STATE_deleted,INCREMENTO},        // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_deleted,STATE_Init,LIBERAR},              // Se ha retirado el plato completo (+ recipiente) tras borrar. 
                                        {STATE_deleted,STATE_ERROR,ERROR},               // Acción incorrecta
                                        {STATE_deleted,STATE_AVISO,AVISO},               // No se ha eliminado el plato porque está vacío
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
                                        {STATE_save_check,STATE_ERROR,ERROR},            // Acción incorrecta (solo salta si se manipula la báscula).
                                        // --------------------------

                                        // --- Comida guardada ---
                                        {STATE_saved,STATE_saved,TARAR},                 // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_saved,STATE_saved,DECREMENTO},            // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR).
                                        {STATE_saved,STATE_saved,INCREMENTO},            // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_saved,STATE_Init,LIBERAR},                // Se ha retirado el plato completo (+ recipiente) tras guardar correctamente.  
                                        {STATE_saved,STATE_Init,GO_TO_INIT},             // Se regresa automáticamente a STATE_Init tras guardar la comida
                                        {STATE_saved,STATE_ERROR,ERROR},                 // Acción incorrecta
                                        {STATE_saved,STATE_AVISO,AVISO},                 // No se ha guardado la comida porque está vacía
                                        // -----------------------


                                        // --- ERROR DE EVENTO ---
                                        {STATE_ERROR,STATE_Init,GO_TO_INIT},                    // Regresar a STATE_Init tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_Plato,GO_TO_PLATO},                  // Regresar a STATE_Plato tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_groupA,GO_TO_GROUP_A},               // Regresar a STATE_groupA tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_groupB,GO_TO_GROUP_B},               // Regresar a STATE_groupB tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_raw,GO_TO_RAW},                      // Regresar a STATE_raw tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_cooked,GO_TO_COOKED},                // Regresar a STATE_cooked tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_weighted,GO_TO_WEIGHTED},            // Regresar a STATE_weighted tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_add_check,GO_TO_ADD_CHECK},          // Regresar a STATE_add_check tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_added,GO_TO_ADDED},                  // Regresar a STATE_added tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_delete_check,GO_TO_DELETE_CHECK},    // Regresar a STATE_delete_check tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_deleted,GO_TO_DELETED},              // Regresar a STATE_deleted tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_save_check,GO_TO_SAVE_CHECK},        // Regresar a STATE_save_check tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_saved,GO_TO_SAVED},                  // Regresar a STATE_saved tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_CANCEL,GO_TO_CANCEL},                // Ir a STATE_CANCEL si se ha cancelado un acción iniciada durante error
                                        // -----------------------


                                        // --- CANCELAR ----------
                                        {STATE_CANCEL,STATE_Init,GO_TO_INIT},           // Regresar a STATE_Init tras cancelar acción de guardar iniciada desde STATE_Init
                                        {STATE_CANCEL,STATE_raw,GO_TO_RAW},             // Regresar a STATE_raw tras cancelar add/delete/save iniciada desde STATE_raw   
                                        {STATE_CANCEL,STATE_cooked,GO_TO_COOKED},       // Regresar a STATE_cooked tras cancelar add/delete/save iniciada desde STATE_cooked
                                        {STATE_CANCEL,STATE_weighted,GO_TO_WEIGHTED},   // Regresar a STATE_weighted tras cancelar add/delete/save iniciada desde STATE_weighted
                                        {STATE_CANCEL,STATE_ERROR,ERROR},               // Acción incorrecta. No suele ocurrir, pero si ocurre y no se gestiona, se quedaría 
                                                                                        // en bucle marcando error.    

                                        {STATE_CANCEL,STATE_DELETE_CSV_CHECK,DELETE_CSV},  // BORRAR FICHERO CSV. EL USUARIO NO DEBERÍA ACTIVAR EL EVENTO. SOLO PARA LAS PRUEBAS.
                                        // -----------------------


                                        // --- AVISO -------------
                                        {STATE_AVISO,STATE_groupA,GO_TO_GROUP_A},     // Regresar a groupA tras 3 segundos de warning
                                        {STATE_AVISO,STATE_groupB,GO_TO_GROUP_B},     // Regresar a groupB tras 3 segundos de warning
                                        {STATE_AVISO,STATE_Init,GO_TO_INIT},          // Regresar a Init tras 3 segundos de warning
                                        {STATE_AVISO,STATE_ERROR,ERROR},              // Acción incorrecta durante aviso. Cualquier cosa es acción incorrecta. Igual que en Cancelar.
                                        // -----------------------


                                         // --- CHECK DELETE CSV ---
                                        {STATE_DELETE_CSV_CHECK,STATE_DELETED_CSV,DELETE_CSV},    // CONFIRMAR BORRAR CSV
                                        {STATE_DELETE_CSV_CHECK,STATE_Init,GO_TO_INIT},           // Regresar a STATE_Init tras cancelar (5 seg) el BORRAR CSV iniciado desde STATE_Init
                                        {STATE_DELETE_CSV_CHECK,STATE_raw,GO_TO_RAW},             // Regresar a STATE_raw tras cancelar (5 seg) el BORRAR CSV iniciado desde STATE_raw   
                                        {STATE_DELETE_CSV_CHECK,STATE_cooked,GO_TO_COOKED},       // Regresar a STATE_cooked tras cancelar (5 seg) el BORRAR CSV iniciado desde STATE_cooked
                                        {STATE_DELETE_CSV_CHECK,STATE_weighted,GO_TO_WEIGHTED},   // Regresar a STATE_weighted tras cancelar (5 seg) el BORRAR CSV iniciado desde STATE_weighted
                                        // --------------------------

                                        // --- FICHERO CSV BORRADO ---
                                        {STATE_DELETED_CSV,STATE_Init,GO_TO_INIT}         // Regresar a STATE_Init tras BORRAR CSV (siempre iniciado desde STATE_Init)
                                        // ---------------------
                                      };





/*----------------------------------------------------------------------------------------------*/
/*------------------------------ VARIABLES ESTADOS/EVENTOS -------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

// ---- VARIABLES DE ESTADOS ---------------------------------------------------------
state_t state_actual;          // Estado actual
state_t state_new;             // Nuevo estado al que se va a pasar
state_t state_prev;            // Estado anterior
state_t lastValidState;         // Último estado válido (Init, Plato, groupA, groupB, raw, cooked, weighted)
// ------ FIN VARIABLES DE ESTADOS ----------------------------------------------------


// ---- VARIABLES DE EVENTOS ----------------------------------------------------------
event_t lastEvent;          // Último evento ocurrido
event_t eventoMain;         // Evento ocurrido en botonera Main
event_t eventoGrande;       // Evento ocurrido en botonera grande
event_t eventoBascula;      // Evento ocurrido en báscula

bool    flagEvent               = false;    // Para evitar que marque evento para cada interrupción, ya que lo marcaría cada medio segundo
                                            // por la interrupción de la báscula.
                                            // Con esta flag solo se da aviso de un evento real (pulsación, incremento o decremento)

bool    flagError               = false;    // El error es una acción de diferente naturaleza. Es decir, no ocurre un error,
                                            // sino que si ha ocurrido algo que no es evento, se considera error. Por eso se utiliza
                                            // otra flag.

bool    keepErrorScreen         = false;    // Mantener pantalla de error cometido por colocar alimento cuando no toca (desde Grupos).
                                            // En este caso no se retira la pantalla tras 3 segundos, sino que se mantiene hasta que
                                            // se retire el plato para comenzar de nuevo.

bool    flagComidaSaved         = false;    // Flag para saber que se ha guardado la comida
bool    flagFicheroCSVBorrado   = false;    // Flag para saber que se ha borrado el fichero csv
bool    flagRecipienteRetirado  = false;    // Flag para saber que se ha retirado el plato completo
// ------ FIN VARIABLES DE EVENTOS ----------------------------------------------------


// ------ PROCESAMIENTO ---------------------------------------------------------------
/**
 * @enum procesamiento
 * @brief Enumeración de los diferentes valores del procesamiento del alimento.
 */
typedef enum {
              ALIMENTO_CRUDO      =   (1),   /**< Alimento crudo  */
              ALIMENTO_COCINADO   =   (2)    /**< Alimento cocinado  */
} procesado_t;

procesado_t procesamiento;
// ------ FIN PROCESAMIENTO ------------------------------------------------------------


// ------ VARIABLES DE PESO ------------------------------------------------------------
float     pesoRecipiente    =   0.0; 
float     pesoPlato         =   0.0;
float     pesoLastAlimento  =   0.0;
// ------ FIN VARIABLES DE PESO --------------------------------------------------------


bool      showingTemporalScreen = false; // Flag para saber que se está mostrando una pantalla temporal o auxiliar




#include "Screen.h"   // Debajo de las variables para que estén disponibles en su ámbito




/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
void printEventName(event_t event);
void printStateName(state_t state);
// --- Reglas transición ---
bool    checkStateConditions();                        // Comprobar reglas de transición de estados

// --- Actividades por estado ---
void    actStateInit();                                // Actividades STATE_Init
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
void    actStateAVISO();                               // Actividades STATE_AVISO

void    actState_DELETE_CSV_CHECK();       // Actividades STATE_DELETE_CSV_CHECK
void    actState_DELETED_CSV();            // Actividades STATE_DELETED_CSV

void    actState_CRITIC_FAILURE_SD();                  // Actividades STATE_CRITIC_FAILURE_SD

// --- Actividades estado actual ---
void    doStateActions();                              // Actividades según estado actual

// --- Error de evento ---
void    actEventError();                               // Mensaje de error de evento según el estado actual

// --- Buffer de eventos ---
bool    isBufferInit();                                // Comprobar buffer de eventos vacío
bool    isBufferFull();                                // Comprobar buffer de eventos lleno
int     getFirstGapBuffer();                           // Obtener primer hueco en el buffer
void    shiftLeftEventBuffer();                        // Desplazar buffer a izquierda para incluir nuevo evento
void    addEventToBuffer(event_t evento);              // Añadir evento al buffer
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/




/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------- NOMBRES DE EVENTOS/ESTADOS -----------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/ 

/*---------------------------------------------------------------------------------------------------------
   printEventName(): Imprime el nombre del evento cuyo ID se pasa como argumento
          Parámetros: 
                  event - ID del evento 
----------------------------------------------------------------------------------------------------------*/
void printEventName(event_t event) {
    switch (event) {
        case NONE:                  Serial.print("NONE");                     break;
        case TIPO_A:                Serial.print("TIPO_A");                   break;
        case TIPO_B:                Serial.print("TIPO_B");                   break;
        case CRUDO:                 Serial.print("CRUDO");                    break;
        case COCINADO:              Serial.print("COCINADO");                 break;
        case ADD_PLATO:             Serial.print("ADD_PLATO");                break;
        case DELETE_PLATO:          Serial.print("DELETE_PLATO");             break;
        case GUARDAR:               Serial.print("GUARDAR");                  break;
        case INCREMENTO:            Serial.print("INCREMENTO");               break;
        case DECREMENTO:            Serial.print("DECREMENTO");               break;
        case TARAR:                 Serial.print("TARAR");                    break;
        case LIBERAR:               Serial.print("LIBERAR");                  break;
        case ERROR:                 Serial.print("ERROR");                    break;
        case CANCELAR:              Serial.print("CANCELAR");                 break;
        case AVISO:                 Serial.print("AVISO");                    break;
        case GO_TO_INIT:            Serial.print("GO_TO_INIT");               break;
        case GO_TO_PLATO:           Serial.print("GO_TO_PLATO");              break;
        case GO_TO_GROUP_A:         Serial.print("GO_TO_GROUP_A");            break;
        case GO_TO_GROUP_B:         Serial.print("GO_TO_GROUP_B");            break;
        case GO_TO_RAW:             Serial.print("GO_TO_RAW");                break;
        case GO_TO_COOKED:          Serial.print("GO_TO_COOKED");             break;
        case GO_TO_WEIGHTED:        Serial.print("GO_TO_WEIGHTED");           break;
        case GO_TO_ADD_CHECK:       Serial.print("GO_TO_ADD_CHECK");          break;
        case GO_TO_ADDED:           Serial.print("GO_TO_ADDED");              break;
        case GO_TO_DELETE_CHECK:    Serial.print("GO_TO_DELETE_CHECK");       break;
        case GO_TO_DELETED:         Serial.print("GO_TO_DELETED");            break;
        case GO_TO_SAVE_CHECK:      Serial.print("GO_TO_SAVE_CHECK");         break;
        case GO_TO_SAVED:           Serial.print("GO_TO_SAVED");              break;
        case GO_TO_CANCEL:          Serial.print("GO_TO_CANCEL");             break;
        case DELETE_CSV:            Serial.print("DELETE_CSV");               break;
        
        default:                    Serial.print("Evento desconocido");       break;
    }
}

/*---------------------------------------------------------------------------------------------------------
   printStateName(): Imprime el nombre del estado cuyo ID se pasa como argumento
          Parámetros: 
                  state - ID del estado 
----------------------------------------------------------------------------------------------------------*/
void printStateName(state_t state) {
    switch (state) {
        case STATE_Init:                Serial.print("STATE_Init");                 break;
        case STATE_Plato:               Serial.print("STATE_Plato");                break;
        case STATE_groupA:              Serial.print("STATE_groupA");               break;
        case STATE_groupB:              Serial.print("STATE_groupB");               break;
        case STATE_raw:                 Serial.print("STATE_raw");                  break;
        case STATE_cooked:              Serial.print("STATE_cooked");               break;
        case STATE_weighted:            Serial.print("STATE_weighted");             break;
        case STATE_add_check:           Serial.print("STATE_add_check");            break;
        case STATE_added:               Serial.print("STATE_added");                break;
        case STATE_delete_check:        Serial.print("STATE_delete_check");         break;
        case STATE_deleted:             Serial.print("STATE_deleted");              break;
        case STATE_save_check:          Serial.print("STATE_save_check");           break;
        case STATE_saved:               Serial.print("STATE_saved");                break;
        case STATE_ERROR:               Serial.print("STATE_ERROR");                break;
        case STATE_CANCEL:              Serial.print("STATE_CANCEL");               break;
        case STATE_AVISO:               Serial.print("STATE_AVISO");                break;
        case STATE_DELETE_CSV_CHECK:    Serial.print("STATE_DELETE_CSV_CHECK");     break;
        case STATE_DELETED_CSV:         Serial.print("STATE_DELETED_CSV");          break;
        case STATE_CRITIC_FAILURE_SD:   Serial.print("STATE_CRITIC_FAILURE_SD");    break;
        
        default:                        Serial.print("Estado desconocido");         break;
    }
}





/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- MOTOR INFERENCIA -----------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/ 

/*---------------------------------------------------------------------------------------------------------
   checkStateConditions(): Comprobar reglas de transición para conocer el próximo estado
----------------------------------------------------------------------------------------------------------*/
bool checkStateConditions(){
    for (int i = 0; i < RULES; i++){
       // Serial.print("state_act = "); Serial.print(rules[i].state_i); Serial.print("  --  state_new = "); Serial.print(rules[i].state_j);
       // Serial.print("  --  condition = "); Serial.println(rules[i].condition);
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
   actStateInit(): Acciones del STATE_Init
----------------------------------------------------------------------------------------------------------*/
void actStateInit(){ 
    // Tiempos utilizados para alternar entre dashboard y pantalla de pedir recipiente:

    const unsigned long recipienteRetiradoInterval  = 1000; // Intervalo de tiempo para mostrar "Recipiente retirado" (1 segundo)
    unsigned long dashboardInterval = 5000;                 // Intervalo de tiempo para mostrar el dashboard (5 segundos) 
    const unsigned long recipienteInterval = 5000;          // Intervalo de tiempo para pedir colocar recipiente (5 segundos)
    

    static unsigned long previousTime;              // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_recipiente_retirado;  
    static bool showing_dash;       
    static bool showing_pedir_recipiente;

    
    if(!doneState){
        if(state_prev != STATE_Init){ 
            Serial.println(F("\nSTATE_Init...")); 

            // ----- INICIAR COMIDA --------------------------------------
            // Si lo último escrito no es INICIO-COMIDA (la lista está vacía), 
            // se añade a la lista "INICIO-COMIDA"
            listaComidaESP32.iniciarComida();
            listaComidaESP32.leerLista();
            // -----------------------------------------------------------

            // ----- RESETEAR COPIA SI SE HA BORRADO CSV -----------------
            if(flagFicheroCSVBorrado){
                comidaActualCopia.restoreComida();   
                flagFicheroCSVBorrado = false;
                flagComidaSaved = false; // Por si he hecho el borrado justo tras guardar
            } 
            // -----------------------------------------------------------


            tareScale();
            
            pesoRecipiente = 0.0;           // Se inicializa 'pesoRecipiente', que se sumará a 'pesoPlato' para saber el 'pesoARetirar'.
            pesoPlato = 0.0;                // Se inicializa 'pesoPlato', que se sumará a 'pesoRecipiente' para saber el 'pesoARetirar'.
            pesoLastAlimento = 0.0;         // Se inicializa 'pesoLastAlimento', que, si hubiera un último alimento que añadir en delete,
                                            // se sumará a 'pesoPlato' y luego a 'pesoRecipiente' para saber el 'peroARetirar'.

            keepErrorScreen = false;

            // ----- BORRAR PLATO ------------------------------------
            // Si se regresa a Init con el platoActual aún con cosas, es porque no se ha borrado con "Eliminar plato" ni
            // se ha restaurado con "Añadir plato" o "Guardar comida", sino que se ha retirado de repente en mitad del proceso.
            // Si ocurre esa liberación tan repentina y cuando no toca (desde Grupos, raw, cooked, weighted...),
            // entonces se borra. Lo entiendo como una eliminación forzada del plato. 
            // Si no se hiciera así, seguiría apareciendo su información en la comida copiada en el dashboard estilo 1, aunque 
            // no se hubiera guardado ni eliminado.
            //
            //  ¡IMPORTANTE! ==> con este borrado se impide por completo el retirar el plato para colocar el alimento fuera
            //                   de SmartCloth y luego colocarlo ya con el alimento.
            //
            // Solo se mira si el plato actual está vacío porque si fuera el 2º plato o sucesivos, los anteriores se
            // habrían "guardado" al hacer "Añadir plato". 
            //
            if(!platoActual.isPlatoEmpty()){ 
                // ----- BORRAR PLATO --------------------------------------
                // Eliminar info del plato de la lista
                listaComidaESP32.borrarLastPlato();
                listaComidaESP32.leerLista();
                // -----------------------------------------------------------

                comidaActual.deletePlato(platoActual);    // Borrar plato actual
                platoActual.restorePlato();               // Restaurar plato
            }
            // ----- FIN BORRAR PLATO ---------------------------------


            // ----- INFO INICIAL DE PANTALLA -------------------------
            if(flagRecipienteRetirado){
                recipienteRetirado();                 // Mostrar "Recipiente retirado" tras LIBERAR báscula
                showing_recipiente_retirado = true;   // Se está mostrando "Recipiente retirado"
                showing_dash = false;      
                showing_pedir_recipiente = false;
            }
            else{ 
                showDashboardStyle1(MSG_SIN_RECIPIENTE); // Mostrar dashboard al inicio con mensaje de que falta recipiente
                showing_dash = true;                // Se está mostrando dashboard estilo 1 (Comida | Acumulado)
                showing_pedir_recipiente = false;   
                showing_recipiente_retirado = false;
            }
            if(flagRecipienteRetirado) flagRecipienteRetirado = false;
            // ----- FIN INFO INICIAL DE PANTALLA ---------------------
            
            
        }

        previousTime = millis();                              // Inicializar 'previousTime' para la alternancia de pantallas

        doneState = true;                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }



    // ----- ALTERNANCIA PANTALLAS -------------------------
    currentTime = millis();
    if(showing_recipiente_retirado){ // Mostrando "Recipiente retirado". Solo aparece tras LIBERAR báscula.
        if (currentTime - previousTime >= recipienteRetiradoInterval) { // Si el "Recipiente retirado" ha estado 1 segundo, se cambia a dashboard estilo 1
            previousTime = currentTime;
            showDashboardStyle1(MSG_SIN_RECIPIENTE);
            showing_recipiente_retirado = false; // Dejar de mostrar "Recipiente retirado". Solo aparece tras LIBERAR báscula.
            showing_dash = true;                  // Mostrando dashboard estilo 1 (Comida | Acumulado)
            showing_pedir_recipiente = false;
        }
    }
    else{ // Ya no se muestra "Recipiente retirado"
        if(showing_dash){ // Se está mostrando dashboard estilo 1 (Comida | Acumulado)
            blinkGrupoyProcesamiento(MSG_SIN_RECIPIENTE);
            if(!flagComidaSaved){ // Si no se acaba de guardar comida, se pide colocar recipiente.
                                  //    Si se acabara de guarda la comida, se dejaría el dashboard estático para que puedan consultar la 
                                  //    información guardada en "Comida guardada".
                if (currentTime - previousTime >= dashboardInterval) { // Si el dashboard ha estado 10 segundos, se cambia a colocar recipiente
                    previousTime = currentTime;
                    pedirRecipiente();
                    showing_dash = false;  
                    showing_pedir_recipiente = true; 
                }
            }
        }
        else if(showing_pedir_recipiente){ // Se está mostrando colocar recipiente
            if (currentTime - previousTime >= recipienteInterval) { // Si el colocar recipiente ha estado 5 segundos, se cambia a dashboard estilo 1
                previousTime = currentTime;
                showDashboardStyle1(MSG_SIN_RECIPIENTE);
                showing_dash = true;  // Mostrando dashboard estilo 1 (Comida | Acumulado)
                showing_pedir_recipiente = false;
            }
        }
    }    
    // ----- FIN ALTERNANCIA PANTALLAS ---------------------

} 



/*---------------------------------------------------------------------------------------------------------
   actStatePlato(): Acciones del STATE_Plato
----------------------------------------------------------------------------------------------------------*/
void actStatePlato(){ 
    // Tiempos utilizados para alternar entre recipiente colocado, dashboard y pantalla de escoger grupo:

    const unsigned long recipienteColocadoInterval  = 1000;   // Intervalo de tiempo para mostrar "Recipiente colocado" (1 segundo)
    const unsigned long dashboardInterval = 3000;             // Intervalo de tiempo para mostrar el dashboard (3 segundos)
    const unsigned long grupoInterval = 5000;                 // Intervalo de tiempo para pedir escoger grupo (5 segundos)
                                                              //      En realidad este grupoInterval no afecta mientras se mantenga por debajo de lo que 
                                                              //      tarda en completarse la pantalla de pedirGrupo. Esta pantalla tarda unos 12 seg,
                                                              //      pero al regresar a actStatePlato() ya habrán pasado los 5 seg de este intervalo.
    //const unsigned long dashboardInterval = 10000;          // Intervalo de tiempo para mostrar el dashboard (10 segundos)

    static unsigned long previousTime;                        // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_recipiente_colocado;  
    static bool showing_dash;      
    static bool showing_escoger_grupo;

    // El peso definitivo del recipiente no se guarda hasta que se escoja grupo de alimentos.
    // Por eso, 'pesoRecipiente' y 'pesoARetirar' son iguales a 0.0 en este estado. Esto no impide que se
    // realice LIBERAR, ya que ese evento ocurre cuando el peso de la báscula es igual a 'pesoARetirar'. 
    // Así, se puede detectar que se ha quitado el recipiente (bascula = 0.0) y se vuelve al estado STATE_Init.   

    if(!doneState){
        if(state_prev != STATE_Plato){
            Serial.println(F("\nPlato colocado")); 

            // ----- INICIAR PLATO --------------------------------------
            // Si lo último escrito no es INICIO-PLATO (la comida está vacía o se está empezando otro plato), 
            // se añade a la lista "INICIO-PLATO"
            listaComidaESP32.iniciarPlato();
            listaComidaESP32.leerLista();
            // -----------------------------------------------------------

            // ----- REINICIAR COPIA DE COMIDA GUARDADA -------------------------
            // Si se acaba de guardar la comida, ya se ha mostrado la copia de su info en STATE_Init. Entonces, si la comida real
            // está vacía porque se ha reiniciado tras guardarla, al colocar recipiente se entiende que va a comenzar una nueva,
            // por lo que se debe reiniciar también la copia.
            // De esta forma, si se retira el plato y se regresa a Init, ya no vuelve a aparecer la info de la comida guardada antes, 
            // sino qye ya se está haciendo la nueva comida, que aún está a 0.
            // 
            if((comidaActual.isComidaEmpty() == true) and (comidaActualCopia.isComidaEmpty() == false)) comidaActualCopia.restoreComida();   

            if(flagComidaSaved) flagComidaSaved = false;  // Si se había guardado comida, se muestra "Comida guardada" en lugar de "Comida actual" en printZona3()
                                                          // en STATE_Init, por eso ahora se resetea la flag.
            // ----- FIN REINICIAR COPIA DE COMIDA GUARDADA ----------------------

            // ----- INFO INICIAL DE PANTALLA -------------------------
            if(lastValidState == STATE_Init){ // Si se viene directo de Init o a través de un error (se colocó el plato durante el error)
                recipienteColocado();                 // Mostrar "Recipiente colocado" una vez al inicio. No volverlo a mostrar si se comete error en STATE_Plato.
                showing_recipiente_colocado = true;   // Se está mostrando "Recipiente colocado"
                showing_dash = false;      
                showing_escoger_grupo = false;
            }
            else{ // Una vez colocado el recipiente, mostrar dashboard completo
                showDashboardStyle1(MSG_SIN_GRUPO);   // Mostrar dashboard con mensaje de sin grupo
                showing_recipiente_colocado = false;                
                showing_dash = true;                  // Se está mostrando dashboard estilo 1 (Comida | Acumulado)
                showing_escoger_grupo = false;
            }
            // ----- FIN INFO INICIAL DE PANTALLA ---------------------
        }


        previousTime = millis();                              // Inicializar 'previousTime' para la alternancia de pantallas


        doneState = true;                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }


    // ----- ALTERNANCIA PANTALLAS -------------------------
    currentTime = millis();
    if(showing_recipiente_colocado){ // Mostrando "Recipiente colocado". Solo aparece una vez al inicio y si no se viene de error.
        if (currentTime - previousTime >= recipienteColocadoInterval) { // Si el "Recipiente colocado" ha estado 1 segundo, se cambia a dashboard estilo 1
            previousTime = currentTime;
            showDashboardStyle1(MSG_SIN_GRUPO);
            showing_recipiente_colocado = false; // Dejar de mostrar "Recipiente colocado". Solo aparece una vez al inicio.
            showing_dash = true;                  // Mostrando dashboard estilo 1 (Comida | Acumulado)
            showing_escoger_grupo = false;
        }
    }
    else{ // Ya no se muestra "Recipiente colocado"
        if(showing_dash){ // Se está mostrando dashboard estilo 1 (Comida | Acumulado)
            blinkGrupoyProcesamiento(MSG_SIN_GRUPO);
            if (currentTime - previousTime >= dashboardInterval) { // Si el dashboard ha estado 10 segundos, se cambia a escoger grupo
                previousTime = currentTime;
                pedirGrupoAlimentos();
                showing_dash = false;  
                showing_escoger_grupo = true; // Mostrando escoger grupo
            }
        }
        else if(showing_escoger_grupo){ // Se está mostrando escoger grupo
            if (currentTime - previousTime >= grupoInterval) { // Si el escoger grupo ha estado 5 segundos, se cambia a dashboard estilo 1
                previousTime = currentTime;
                showDashboardStyle1(MSG_SIN_GRUPO);
                showing_dash = true;  // Mostrando dashboard estilo 1 (Comida | Acumulado)
                showing_escoger_grupo = false;
            }
        }
    }    
    // ----- FIN ALTERNANCIA PANTALLAS ---------------------

}



/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_groupA o STATE_groupB
----------------------------------------------------------------------------------------------------------*/
void actGruposAlimentos(){ 

    if(!doneState){        
        Serial.println(F("Grupo de alimentos..."));
        
        // ----- ACCIONES ------------------------------
        if(lastValidState == STATE_Plato){ // Si se acaba de colocar el recipiente
            pesoRecipiente = pesoBascula;    // Se guarda 'pesoRecipiente' para sumarlo a 'pesoPlato' y saber el 'pesoARetirar'.
            tareScale();                     // Se tara la báscula, preparándola para el primer alimento
        }
        else if((state_prev != STATE_ERROR) and (state_prev != STATE_groupA) and (state_prev != STATE_groupB) and (pesoBascula != 0.0)){ 

                // ----- AÑADIR ALIMENTO A LISTA -----------------------------
                // Escribir ALIMENTO,<grupo>,<peso> a la lista, siendo <grupo> el ID de 'grupoAnterior' y <peso> el valor de 'pesoBascula'.
                listaComidaESP32.addAlimento(grupoAnterior.ID_grupo, pesoBascula);
                listaComidaESP32.leerLista();
                // -----------------------------------------------------------
                
                // ----- AÑADIR ALIMENTO A PLATO -----------------------------
                Serial.println(F("Añadiendo alimento al plato..."));

                Alimento alimento(grupoAnterior, pesoBascula);              // Cálculo automático de valores nutricionales.
                                                                            // Al escoger un nuevo grupo se guarda el alimento del grupo anterior
                                                                            // colocado en la iteración anterior. Por eso se utiliza 'grupoAnterior' para
                                                                            // crear el alimento, porque 'grupoEscogido' ya ha tomado el valor del
                                                                            // nuevo grupo.
                                                             
                platoActual.addAlimentoPlato(alimento);                     // Alimento ==> Plato
                comidaActual.addAlimentoComida(alimento);                   // Alimento ==> Comida

                pesoPlato = platoActual.getPesoPlato();                     // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.

                tareScale();                                                // Tras guardar la información del último alimento colocado, se tara la báscula
                                                                            // para pesar el siguiente alimento
                // -----------------------------------------------------------
        }
        // ----- FIN ACCIONES --------------------------


        // ----- INFO PANTALLA -------------------------
        if((state_prev != STATE_groupA) and (state_prev != STATE_groupB)){ // ==> Si es la primera vez que se escoge grupo, se forma medio Dashboard (ejemplos y parpadeo zona 2)  
            if(showSemiDashboard_PedirProcesamiento()) return;              // Mostrar semi dashboard completo al inicio
                                                                            // Si ocurre alguna interrupción mientras se forma el semi dashboard, se sale de la función.
        }
        else if((state_prev == STATE_groupA) or (state_prev == STATE_groupB)){  // ==> Si se están pulsando grupos para ver sus ejemplos, solo se modifica la zona 1
            printGrupoyEjemplos();                                          // Modificar zona 1 con los ejemplos del nuevo grupo escogido
        }

        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.

    }


    //
    // 1 - La primera vez que se escoge un grupo de alimentos, se forma el semi dashboard completo --> showSemiDashboard_PedirProcesamiento()
    // 2 - Las siguientes veces que se escoge un grupo para ver sus ejemplos, sin pasar por un estado diferente a STATE_groupA/B, no se vuelve
    //      a formar el semi dashboard al completo, sino que solo se modifica la zona 1 con el nuevo grupo seleccionado --> printGrupoyEjemplos()
    //
    // En ambos casos anteriores, las zonas 2 y 3-4 presentan movimiento constante. Es decir, la zona 2 parpadea para indicar que no se ha escogido
    // procesamiento (crudo o cocinado) y sobre las zonas 3-4 se muestra una pantalla que pide escoger el procesamiento alternando las imágenes
    // de los botones correspondientes.
    //
    // Por tanto, la creación del semi dashboard completo solo se hace al entrar a STATE_groupA/B viniendo de algún estado diferente a esos y la 
    // modificación de la zona 1 con los ejemplos se hace si se viene de STATE_groupA/B.
    //
    // Sin embargo, para que los movimientos en las zonas 2 y 3-4 sean fluidos y continuos, las funciones que los realizan se deben llamar en
    // cada ciclo del loop. Estas funciones no realizan esperas bloqueantes (delay) para los parpadeos y alternancias, sino que usan esperas 
    // no bloqueantes y variables 'static' para saber, al volver a entrar en la función, qué fue lo último que se hizo y continuar desde ahí.
    //
    // ----- PANTALLAS CON MOVIMIENTO -------------------------
    blinkGrupoyProcesamiento(NO_MSG);               // Zona 2 - Parpadea (procesamiento sin escoger)
    if(alternateButtonsProcesamiento()) return;     // Zonas 3 y 4 - Alternar botones de crudo y cocinado. Las formas colores y texto ya están (formGraphicsPedirProcesamiento())
                                                    // Si hay interrupción, mientras se alternan botones, se sale de la función --> en loop() se chequea la interrupción.
    // --------------------------------------------------------



}




/*---------------------------------------------------------------------------------------------------------
   actStateRaw(): Acciones del STATE_raw
----------------------------------------------------------------------------------------------------------*/
void actStateRaw(){ 
    // Tiempos utilizados para alternar entre dashboard y pantalla de colocar alimento:
    const unsigned long dashboardInterval = 10000;  // Intervalo de tiempo para mostrar el dashboard (10 segundos) -> se deja más tiempo para leer ejemplos
    const unsigned long alimentoInterval = 5000;    // Intervalo de tiempo para pedir colocar alimento (5 segundos)

    static unsigned long previousTime;              // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_dash;       
    static bool showing_colocar_alimento;

    if(!doneState){
        // ----- ACCIONES ------------------------------
        if(state_prev != STATE_raw){
            Serial.println(F("\nAlimento crudo...")); 
            procesamiento = ALIMENTO_CRUDO;
        }
        // ----- FIN ACCIONES --------------------------

        // ----- INFO DE PANTALLA -------------------------
        // Solo se modifica la pantalla completa si se estaba mostrando una pantalla temporal/transitoria. Si se estaba mostrando el dashboard,
        // ya fuera con "crudo" o "cocinado" escogido, solo se modifica la zona 2 con la imagen de "crudo" activa.
        if(showingTemporalScreen) showDashboardStyle2();  // Mostrar dashboard estilo 2 con el procesamiento "crudo"
        else printProcesamiento();                        // Modificar solamente zona 2 con el procesamiento "crudo"
        showing_dash = true;                // Se está mostrando dashboard estilo 2 (Alimento | Comida)
        showing_colocar_alimento = false;   
        previousTime = millis();            // Inicializar 'previousTime' para la alternancia de pantallas
        // ----- FIN INFO DE PANTALLA ---------------------
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }


    // ----- ALTERNANCIA PANTALLAS -------------------------
    currentTime = millis();
    if(showing_dash){ // Se está mostrando dashboard estilo 2 (Alimento | Comida)
        if (currentTime - previousTime >= dashboardInterval) { // Si el dashboard ha estado 10 segundos, se cambia a pedir alimento
            previousTime = currentTime;
            pedirAlimento();
            showing_dash = false;  
            showing_colocar_alimento = true;   // Mostrando pedir alimento
        }
    }
    else if(showing_colocar_alimento){ // Se está mostrando pedir alimento
        if (currentTime - previousTime >= alimentoInterval) { // Si el pedir alimento ha estado 5 segundos, se cambia a dashboard estilo 2
            previousTime = currentTime;
            showDashboardStyle2();
            showing_dash = true;  // Mostrando dashboard estilo 2 (Alimento | Comida)
            showing_colocar_alimento = false;   
        }
    }
    // ----- FIN ALTERNANCIA PANTALLAS ---------------------
}


/*---------------------------------------------------------------------------------------------------------
   actStateCooked(): Acciones del STATE_cooked
----------------------------------------------------------------------------------------------------------*/
void actStateCooked(){ 
    // Tiempos utilizados para alternar entre dashboard y pantalla de colocar alimento:
    const unsigned long dashboardInterval = 10000;  // Intervalo de tiempo para mostrar el dashboard (10 segundos) -> se deja más tiempo para leer ejemplos
    const unsigned long alimentoInterval = 5000;    // Intervalo de tiempo para pedir colocar alimento (5 segundos)

    static unsigned long previousTime;              // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_dash;       
    static bool showing_colocar_alimento;

    if(!doneState){
        // ----- ACCIONES ------------------------------
        if(state_prev != STATE_cooked){
            Serial.println(F("\nAlimento cocinado..."));        
            procesamiento = ALIMENTO_COCINADO;
        }
        // ----- FIN ACCIONES --------------------------

        // ----- INFO DE PANTALLA -------------------------
        // Solo se modifica la pantalla completa si se estaba mostrando una pantalla temporal/transitoria. Si se estaba mostrando el dashboard,
        // ya fuera con "crudo" o "cocinado" escogido, solo se modifica la zona 2 con la imagen de "cocinado" activa.
        if(showingTemporalScreen) showDashboardStyle2();  // Mostrar dashboard estilo 2 con el procesamiento "cocinado"
        else printProcesamiento();                        // Modificar solamente zona 2 con el procesamiento "cocinado"
        showing_dash = true;                // Se está mostrando dashboard estilo 2 (Alimento | Comida)
        showing_colocar_alimento = false;   
        previousTime = millis();            // Inicializar 'previousTime' para la alternancia de pantallas
        // ----- FIN INFO DE PANTALLA ---------------------

        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }


    // ----- ALTERNANCIA PANTALLAS -------------------------
    currentTime = millis();
    if(showing_dash){ // Se está mostrando dashboard estilo 2 (Alimento | Comida)
        if (currentTime - previousTime >= dashboardInterval) { // Si el dashboard ha estado 10 segundos, se cambia a pedir alimento
            previousTime = currentTime;
            pedirAlimento();
            showing_dash = false;  
            showing_colocar_alimento = true;   // Mostrando pedir alimento
        }
    }
    else if(showing_colocar_alimento){ // Se está mostrando pedir alimento
        if (currentTime - previousTime >= alimentoInterval) { // Si el pedir alimento ha estado 5 segundos, se cambia a dashboard estilo 2
            previousTime = currentTime;
            showDashboardStyle2();
            showing_dash = true;  // Mostrando dashboard estilo 2 (Alimento | Comida)
            showing_colocar_alimento = false;   
        }
    }
    // ----- FIN ALTERNANCIA PANTALLAS ---------------------
}


/*---------------------------------------------------------------------------------------------------------
   actStateWeighted(): Acciones del STATE_weighted
----------------------------------------------------------------------------------------------------------*/
void actStateWeighted(){ 
    // Tiempos utilizados para alternar entre dashboard y pantalla de sugerencia de acciones:
    const unsigned long dashboardInterval = 10000;      // Intervalo de tiempo para mostrar el dashboard (10 segundos)
    const unsigned long sugerenciasInterval = 15000;    // Intervalo de tiempo para sugerir acciones (10 segundos)

    static unsigned long previousTime;              // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_dash;       
    static bool showing_sugerir_acciones;

    if(!doneState){
        Serial.println(F("\nAlimento pesado...")); 

        // ----- INFO DE PANTALLA -------------------------
        // Solo se modifica la pantalla completa si se estaba mostrando una pantalla temporal/transitoria (sugerencia acciones). 
        // Si se estaba mostrando el dashboard, solo se modifican las zonas 3 y 4 con los valores correspondientes.
        if(showingTemporalScreen) showDashboardStyle2();  
        else{
            printZona3(SHOW_ALIMENTO_ACTUAL_ZONA3); // Zona 3 - Valores alimento actual pesado
            printZona4(SHOW_COMIDA_ACTUAL_ZONA4);   // Zona 4 - Valores Comida actual actualizada en tiempo real según el peso del alimento
        }
        showing_dash = true;                        // Se está mostrando dashboard estilo 2 (Alimento | Comida)
        showing_sugerir_acciones = false;   
        previousTime = millis();                    // Inicializar 'previousTime' para la alternancia de pantallas
        // ----- FIN INFO DE PANTALLA ---------------------

        doneState = true;                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                    // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }


    // ----- ALTERNANCIA PANTALLAS -------------------------
    currentTime = millis();
    if(showing_dash){ // Se está mostrando dashboard estilo 2 (Alimento | Comida)
        if (currentTime - previousTime >= dashboardInterval) { // Si el dashboard ha estado 10 segundos, se cambia a sugerir acciones
            previousTime = currentTime;
            sugerirAccion();
            showing_dash = false;  
            showing_sugerir_acciones = true;   // Mostrando sugerir acciones
        }
    }
    else if(showing_sugerir_acciones){ // Se está mostrando sugerir acciones
        if (currentTime - previousTime >= sugerenciasInterval) { // Si el sugerir acciones ha estado 10 segundos, se cambia a dashboard estilo 2
            previousTime = currentTime;
            showDashboardStyle2();
            showing_dash = true;  // Mostrando dashboard estilo 2 (Alimento | Comida)
            showing_sugerir_acciones = false;   
        }
    }
    // ----- FIN ALTERNANCIA PANTALLAS ---------------------
}


/*---------------------------------------------------------------------------------------------------------
   actStateAddCheck(): Acciones del STATE_add_check
----------------------------------------------------------------------------------------------------------*/
void actStateAddCheck(){ 
    static unsigned long previousTimeCancel;      // Tiempo usado para cancelar la acción tras 10 segundos de inactividad

    if(!doneState){
        previousTimeCancel = millis(); 

        Serial.println(F("\n¿Seguro que quiere añadir un plato?")); 
        
        pedirConfirmacion(ASK_CONFIRMATION_ADD);                            // Mostrar pregunta de confirmación para añadir plato
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

    // --- CANCELACIÓN AUTOMÁTICA ---
    if ((millis() - previousTimeCancel) > 10000) {    // Tras 10 segundos de inactividad, se cancela automáticamente la acción añadir plato
        Serial.print(F("\nTIME-OUT. Cancelando añadir plato..."));
        addEventToBuffer(CANCELAR);   // Se transiciona al STATE_CANCEL, que regresa automáticamente al estado desde donde se inició añadir plato
        flagEvent = true;
    }
    // --- FIN CANCELACIÓN AUTOMÁTICA ---
}


/*---------------------------------------------------------------------------------------------------------
   actStateAdded(): Acciones del STATE_added
----------------------------------------------------------------------------------------------------------*/
void actStateAdded(){ 
    static bool errorPlatoWasEmpty;        // Flag utilizada para saber si se debe mostrar la información "normal"
                                           // o un mensaje de aviso indicando no se ha creado otro plato porque el 
                                           // actual está vacío.

    if(!doneState){

        if(state_prev != STATE_added){                              // ==> Si no se viene del propio STATE_added, para evitar que se vuelva 
                                                                    //     a guardar el plato en la comida.
          
                                                                    // *****
                                                                    // Primero se actualiza el plato, si es necesario, y después se muestran las actualizaciones.
                                                                    // Si tras comprobar cambios en el plato se ve que sigue vacío, entonces se indica en un mensaje 
                                                                    // que no se ha podido crear otro.
                                                                    // *****
            
            Serial.println(F("Añadiendo plato a la comida..."));

            /* ----- ACTUALIZAR PLATO  ----- */
            if(pesoBascula != 0.0){   // ==> Si se ha colocado algo nuevo en la báscula y no se ha retirado,
                                                                        //     debe incluirse en el plato. 
                                                                    // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                                    // añadir un nuevo plato sin haber colocado un nuevo alimento, no haría falta
                                                                    // actualizar el plato, pues no se habría modificado.

                // ----- AÑADIR ALIMENTO A LISTA -----------------------------
                // Escribir ALIMENTO,<grupo>,<peso> a la lista, siendo <grupo> el ID de 'grupoEscogido' y <peso> el valor de 'pesoBascula'.
                listaComidaESP32.addAlimento(grupoEscogido.ID_grupo, pesoBascula);
                listaComidaESP32.leerLista();
                // -----------------------------------------------------------
                

                // ----- AÑADIR ALIMENTO A PLATO -----------------------------
                Alimento alimento(grupoEscogido, pesoBascula);        // Cálculo automático de valores nutricionales. 
                                                                    // Usamos 'grupoEscogido' porque no se ha modificado. 
                
                platoActual.addAlimentoPlato(alimento);                       // Alimento ==> Plato
                comidaActual.addAlimentoComida(alimento);                     // Alimento ==> Comida

                /* ----- TARAR  ----- */
                tareScale();                                        // Se debe tarar para que conforme vaya disminuyendo el peso veamos si 
                                                                    // se ha quitado todo el 'pesoARetirar'.
                                                                    // Solo se hace si se viene del STATE_weighted porque en los estados 
                                                                    // previos a ese ya se tara y es en STATE_weighted donde se modifica el peso.
                // -----------------------------------------------------------
            }
            /* ------------------------------ */


            /* ----- GUARDAR PLATO EN COMIDA  ----- */
            if(!platoActual.isPlatoEmpty()){  // PLATO CON COSAS (si había último alimento, se ha incluido) ==> SE GUARDA Y SE CREA OTRO
                errorPlatoWasEmpty = false;
                pesoPlato = platoActual.getPesoPlato();             // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                comidaActual.addPlato(platoActual);                 // Plato ==> Comida (no se vuelven a incluir los alimentos. Solo modifica peso y nPlatos)
                platoActual.restorePlato();                         // "Reiniciar" platoActual para usarlo de nuevo.
                comidaActualCopia.copyComida(comidaActual); // Copiar nº platos, peso y valores de la comida actual a la copia. Este objeto Comida solo sirve para mostrar
                                                            // en el dashboard estilo 1 (STATE_Init y STATE_Plato) la comida actual junto con el acumulado.
                                                            // Se muestra esta copia en lugar de la original 'comidaActual' porque esa se borra tras guardarla.
            }
            else if(state_prev != STATE_ERROR){   // PLATO VACÍO ==> NO SE CREA OTRO 
                                                            // Se chequea no venir de error para no marcar un falso aviso. Un aviso real habría saltado
                                                            // al entrar a este estado STATE_added, antes de cometer error. Si no hubiera saltado, se habría
                                                            // añadido un plato y limpiado el actual, por lo que tras un posible error en este estado,
                                                            // siempre se marcaría aviso de plato vacío, porque se acaba de limpiar. Por eso se comprueba no venir
                                                            // de error, para no marcar aviso.
                errorPlatoWasEmpty = true; 
                 addEventToBuffer(AVISO); 
                 flagEvent = true;
            }
            /* --------------------------------------- */


            /* -----  INFORMACIÓN MOSTRADA  ---------------------------------- */
            if(!errorPlatoWasEmpty) showAccionRealizada(ADD_EXECUTED); // Se muestra si no ha habido aviso.
                // Solo habrá aviso si el plato está vacío y si no se viene de error, por eso no hace falta chequear aquí
                // si se viene de error. En cualquiera de los dos casos que no marcan aviso (se ha añadido plato o se 
                // ha cometido error) se muestra el mensaje de confirmación (accion realizada) para que pida retirar el plato.
                // Si saltara aviso, se pasaría al STATE_AVISO y luego se regresaría a grupos, por lo que no es posible
                // cometer un error en este STATE_added si ha saltado aviso. Solo se puede cometer error si se ha añadido plato
                // y se está esperando retirarlo.
                //
                // ==> Un error en este estado sería pulsar cualquier botón. Lo único que se debe hacer es retirar el plato.
            /* ----- FIN INFO MOSTRADA ---------------------------------------- */


        } // FIN state_prev != STATE_added
                                                 
        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

}


/*---------------------------------------------------------------------------------------------------------
   actStateDeleteCheck(): Acciones del STATE_delete_check
----------------------------------------------------------------------------------------------------------*/
void actStateDeleteCheck(){ 
    static unsigned long previousTimeCancel;      // Tiempo usado para cancelar la acción tras 10 segundos de inactividad

    if(!doneState){
        previousTimeCancel = millis(); 

        Serial.println(F("\n¿Seguro que quiere eliminar el plato?")); 
        
        pedirConfirmacion(ASK_CONFIRMATION_DELETE);                         // Mostrar pregunta de confirmación para eliminar plato
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

    // --- CANCELACIÓN AUTOMÁTICA ---
    if ((millis() - previousTimeCancel) > 10000) {    // Tras 10 segundos de inactividad, se cancela automáticamente la acción eliminar plato
        Serial.print(F("\nTIME-OUT. Cancelando eliminar plato..."));
        addEventToBuffer(CANCELAR);   // Se transiciona al STATE_CANCEL, que regresa automáticamente al estado desde donde se inició eliminar plato
        flagEvent = true;
    }
    // --- FIN CANCELACIÓN AUTOMÁTICA ---
}


/*---------------------------------------------------------------------------------------------------------
   actStateDeleted(): Acciones del STATE_deleted
----------------------------------------------------------------------------------------------------------*/
void actStateDeleted(){
    static bool errorPlatoWasEmpty;        // Flag utilizada para saber si se debe mostrar la información "normal"
                                           // o un mensaje de aviso indicando no se ha borrado el plato porque el 
                                           // actual ya está vacío.

    if(!doneState){

        if(state_prev != STATE_deleted){                            // ==> Si no se viene del propio STATE_deleted, para evitar que se vuelva 
                                                                    //     a eliminar el plato de la comida.
          

            // ----- BORRAR PLATO DE LISTA--------------------------------
            // Borrar desde el final de la lista hasta la última aparición de "INICIO-PLATO".
            listaComidaESP32.borrarLastPlato();
            listaComidaESP32.leerLista();
            // -----------------------------------------------------------
            

            // ----- BORRAR PLATO DE COMIDA ------------------------------
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
                pesoPlato = platoActual.getPesoPlato();                 
                if(pesoLastAlimento != 0.0) pesoPlato += pesoLastAlimento; // Incluir último alimento en el 'pesoPlato' para saber 'pesoARetirar'.
                comidaActual.deletePlato(platoActual);                    // Borrar plato actual
                platoActual.restorePlato();                               // "Reiniciar" platoActual para usarlo de nuevo.
                comidaActualCopia.copyComida(comidaActual); // Copiar nº platos, peso y valores de la comida actual a la copia. Este objeto Comida solo sirve para mostrar
                                                            // en el dashboard estilo 1 (STATE_Init y STATE_Plato) la comida actual junto con el acumulado.
                                                            // Se muestra esta copia en lugar de la original 'comidaActual' porque esa se borra tras guardarla.
            }
            else{   // PLATO VACÍO ==> NO HAY QUE BORRAR, PERO PUEDE QUE SÍ RETIRAR (último alimento)
                if(pesoLastAlimento != 0.0){ // ÚLTIMO ALIMENTO ==> ALGO QUE RETIRAR
                    errorPlatoWasEmpty = false;       // Para mostrar mensaje de plato borrado y pedir retirarlo, aunque no sea cierto porque no se ha guardado el
                                                      // último alimento (solo su peso) y el objeto Plato está realmente vacío.
                    pesoPlato = pesoLastAlimento;     // Incluir último alimento en el 'pesoPlato' para saber 'pesoARetirar'.
                }
                else if(state_prev != STATE_ERROR){   // PLATO VACÍO Y NO HAY ÚLTIMO ALIMENTO ==> NADA QUE BORRAR NI RETIRAR 
                          // Se chequea no venir de error para no marcar un falso aviso. Un aviso real habría saltado
                          // al entrar a este estado STATE_deleted, antes de cometer error. Si no hubiera saltado, se habría
                          // eliminado y limpiado el plato actual, por lo que tras un posible error en este estado,
                          // siempre se marcaría aviso de plato vacío, porque se acaba de limpiar. Por eso se comprueba no venir
                          // de error, para no marcar aviso.
                    errorPlatoWasEmpty = true; 
                    addEventToBuffer(AVISO); 
                    flagEvent = true;
                }
            }
            // -----------------------------------------------------------



            /* -----  INFORMACIÓN MOSTRADA  ---------------------------------- */
             if(!errorPlatoWasEmpty) showAccionRealizada(DELETE_EXECUTED); // Se muestra si no ha habido aviso.
                  // Solo habrá aviso si el plato está vacío y si no se viene de error, por eso no hace falta chequear aquí
                  // si se viene de error. En cualquiera de los dos casos que no marcan aviso (se ha eliminado plato o se 
                  // ha cometido error) se muestra el mensaje de confirmación (acción realizada) para que pida retirar el plato.
                  // Si saltara aviso, se pasaría al STATE_AVISO y luego se regresaría a grupos, por lo que no es posible
                  // cometer un error en este STATE_deleted si ha saltado aviso. Solo se puede cometer error si se ha eliminado el plato
                  // // y se está esperando retirarlo.
                  //
                  // ==> Un error en este estado sería pulsar cualquier botón. Lo único que se debe hacer es retirar el plato.
            /* ----- FIN INFO MOSTRADA ---------------------------------------- */


        } // FIN if(state_prev != STATE_deleted)


            doneState = true;                                       // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

}


/*---------------------------------------------------------------------------------------------------------
   actStateSaveCheck(): Acciones del STATE_save_check
----------------------------------------------------------------------------------------------------------*/
void actStateSaveCheck(){ 
    static unsigned long previousTimeCancel;      // Tiempo usado para cancelar la acción tras 10 segundos de inactividad

    if(!doneState){
        previousTimeCancel = millis(); 

        Serial.println(F("\n¿Seguro que quiere guardar la comida?")); 
        
        pedirConfirmacion(ASK_CONFIRMATION_SAVE);                           // Mostrar pregunta de confirmación para guardar comida
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }


    // --- CANCELACIÓN AUTOMÁTICA ---
    if ((millis() - previousTimeCancel) > 10000) {    // Tras 10 segundos de inactividad, se cancela automáticamente la acción guardar comida
        Serial.print(F("\nTIME-OUT. Cancelando guardar comida..."));
        addEventToBuffer(CANCELAR);   // Se transiciona al STATE_CANCEL, que regresa automáticamente al estado desde donde se inició guardar comida
        flagEvent = true;
    }
    // --- FIN CANCELACIÓN AUTOMÁTICA ---
}



/*---------------------------------------------------------------------------------------------------------
   actStateSaved(): Acciones del STATE_saved
----------------------------------------------------------------------------------------------------------*/
void actStateSaved(){
    static unsigned long previousTimeComidaSaved;   // Para regresar a STATE_Init si se ha querido guardar desde ahí, tras añadir o eliminar. 
                                                    // No se usa si la comida no se guarda porque estuviera vacía, eso sería AVISO.
        
    unsigned long currentTime;                // Tiempo actual            
    
    static bool errorComidaWasEmpty;       // Flag utilizada para saber si se debe mostrar la información "normal"
                                           // o un mensaje de aviso indicando no se ha guardado la comida porque  
                                           // está vacía.

    //int responseFromESP32;


    if(!doneState){
      
        if(state_prev != STATE_saved){        // ==> Si no se viene del propio STATE_saved, para evitar que se vuelva 
                                              //     a guardar la comida en el diario.

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
            
                // ----- AÑADIR ALIMENTO A LISTA -----------------------------
                // Escribir ALIMENTO,<grupo>,<peso> a la lista, siendo <grupo> el ID de 'grupoEscogido' y <peso> el valor de 'pesoBascula'.
                listaComidaESP32.addAlimento(grupoEscogido.ID_grupo, pesoBascula);
                listaComidaESP32.leerLista();
                // -----------------------------------------------------------
                

                // ----- AÑADIR ALIMENTO A PLATO -----------------------------
                Alimento alimento(grupoEscogido, pesoBascula);      // Cálculo automático de valores nutricionales.
                                                                    // Usamos 'grupoEscogido' porque no se ha modificado. 
                
                platoActual.addAlimentoPlato(alimento);             // Alimento ==> Plato
                comidaActual.addAlimentoComida(alimento);           // Alimento ==> Comida

                /* ----- TARAR  ----- */
                tareScale();                                        // Se debe tarar para que conforme vaya disminuyendo el peso veamos si 
                                                                    // se ha quitado todo el 'pesoARetirar'.
                                                                    // Solo se hace si se viene del STATE_weighted porque en los estados 
                                                                    // previos a ese ya se tara y es en STATE_weighted donde se modifica el peso.
                // -----------------------------------------------------------
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

                // --- COMIDA A DIARIO Y FICHEROS ---
                diaActual.addComida(comidaActual);          // Comida ==> Diario
                saveComidaSD();                             // Comida ==> fichero CSV y fichero ESP32 en SD
                readFileESP32();                            // Se lee el fichero en lugar de la lista porque, tras guardar la comida, la lista ya está vacía
                //responseFromESP32 = saveComidaDatabase();   // Comida ==> esp32 ==> base de datos
                // -----------------------------------

                // --- ACTUALIZAR COPIA COMIDA -------
                comidaActualCopia.copyComida(comidaActual); // Copiar nº platos, peso y valores de la comida actual a la copia. Este objeto 'Comida' solo sirve para mostrar
                                                            // en el dashboard estilo 1 (STATE_Init y STATE_Plato) la comida guardada junto con el acumulado, pues tras guardarla
                                                            // se limpia (siguiente línea). Por eso se copia antes, para poderla ver al regresar a STATE_Init.
                comidaActual.restoreComida();               // "Reiniciar" comidaActual para usarla de nuevo.
                // -----------------------------------

                flagComidaSaved = true;                     // Se indica que se ha guardado la comida para que el dashboard estilo 1 en STATE_Init se muestre
                                                            // "Comida guardada" en lugar de "Comida actual". De este forma se entiende que los valores que se
                                                            // muestran son de la comida que se acaba de guardar. En cuanto se coloque recipiente, se entenderá
                                                            // que se ha comenzado otra comida, pasando ya a "Comida actual".
            }   
            else if(state_prev != STATE_ERROR){   // PLATO VACÍO ==> NO SE CREA OTRO 
                    // Si se ha guardado la comida y, en lugar de retirar el plato, se comete un error (pulsando botón), al regresar a este
                    // estado STATE_saved la comidaActual ya estará vacía porque se guardó anteriormente, por lo que daría aviso y regresaría al estado
                    // desde donde se inició el guardado. Todo eso aún con el plato puesto en la báscula, por lo que se podrían ir encadenando
                    // una serie de errores al retirarlo desde ese estado previo.
                    // Para evitar marcar un aviso "falso", se chequea no venir de error. Un aviso real habría saltado al entrar a este estado STATE_saved
                    // por primera vez y que la comida estuviera realmente vacía, no por venir de un error tras guardar la comida y limpiar comidaActual. 
                errorComidaWasEmpty = true;  
                 addEventToBuffer(AVISO); 
                 flagEvent = true;
            }
            /* ----- FIN GUARDAR COMIDA EN DIARIO ------------------------- */


            /* -----  INFORMACIÓN MOSTRADA  ------------------------------- */             
            if(!errorComidaWasEmpty){ 
                // Se muestra si no ha habido aviso. 
                /*switch (responseFromESP32){
                  case SAVED_OK:       showAccionRealizada(SAVE_EXECUTED_FULL);                       break; // COMIDA GUARDADA EN LOCAL Y DATABASE
                  case ERROR_HTTP:     showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP);      break; // COMIDA GUARDADA SOLO LOCAL, FALLO EN SERVIDOR
                  case NO_WIFI:        showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI);         break; // COMIDA GUARDADA SOLO LOCAL, NO HAY WIFI
                  case TIMEOUT:        showAccionRealizada(SAVE_ESP32_TIMEOUT);         break; // COMIDA GUARDADA SOLO LOCAL POR TIMEOUT EN RESPUESTA DE ESP32
                  case UNKNOWN:        showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR);   break; // COMIDA GUARDADA SOLO LOCAL, ERROR DESCONOCIDO
                }*/
                showAccionRealizada(SAVE_EXECUTED_FULL);

                if(lastValidState == STATE_Init) previousTimeComidaSaved = millis(); // Comida guardada desde Init
            }
                // Solo habrá aviso si la comida está vacía y si no se viene de error, por eso no hace falta chequear aquí
                // si se viene de error. En cualquiera de los dos casos que no marcan aviso (se ha guardado la comida o se 
                // ha cometido error) se muestra el mensaje de confirmación (accion realizada) para que pida retirar el plato.
                // Si saltara aviso, se pasaría al STATE_AVISO y luego se regresaría a grupos, por lo que no es posible
                // cometer un error en este STATE_saved si ha saltado aviso. Solo se puede cometer error si se ha guardado la comida
                // y se está esperando retirar el plato.
                //
                // ==> Un error en este estado sería pulsar cualquier botón. Lo único que se debe hacer es retirar el plato.
            /* ----- FIN INFO MOSTRADA ------------------------------------ */
        

        } // FIN if(state_prev != STATE_saved)

        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }
 


    // ----- TIEMPO DE ESPERA ---------------------------------------------
    // --- REGRESO A INIT TRAS GUARDAR EXITOSO ----------------
    // Se regresa automáticamente a Init si se ha guardado desde ahí porque en este caso 
    // no es necesario esperar a que el usuario retire el plato, ya que no hay nada sobre
    // la báscula.
    if(!errorComidaWasEmpty){  // COMIDA CON COSAS --> se guardó
      if(lastValidState == STATE_Init){ // Si se viene directo de Init (pasando por save_check) o a través de un error
                // Si se ha guardado desde cualquier otro estado, con algo sobre la báscula, la pantalla
                // de confirmación de acción pedirá retirar el plato.
                // Este chequeo de tiempo solo es para regresar a STATE_Init si se ha guardado la comida,
                // que no estaba vacía.
            currentTime = millis();
            if ((currentTime - previousTimeComidaSaved) > 3000) {    // Tras 3 segundos mostrando confirmación de haber guardado...
                Serial.print(F("\nGO_TO_INIT forzada. Regreso a INI..."));
                addEventToBuffer(GO_TO_INIT);
                flagEvent = true;
            }
        }
    }
    // --- FIN REGRESO A INIT TRAS GUARDAR EXITOSO ------------
    // ----- FIN TIEMPO DE ESPERA ------------------------------------------
    
}



/*---------------------------------------------------------------------------------------------------------
   actStateERROR(): Acciones del STATE_ERROR según el estado anterior
----------------------------------------------------------------------------------------------------------*/
void actStateERROR(){ 
    static unsigned long previousTimeError;   // Tiempo usado para regresar al estado desde el que se cometió el error tras mostrar
                                              // una pantalla de error durante 3 segundos.
    unsigned long currentTime;                // Tiempo actual  

    // -----  INFORMACIÓN MOSTRADA  -------------------------------
    if(!doneState){
        previousTimeError = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de error, regresar al estado anterior.

        Serial.println(F("Mensaje de error")); 
        // Mensaje de error según estado en el que se ha cometido el error (state_prev)
        switch (state_prev){
          case STATE_Init:          showError(ERROR_STATE_INIT);           break;  // Init
          case STATE_Plato:         showError(ERROR_STATE_PLATO);           break;  // Plato
                    // Los errores que se pueden cometer en STATE_PLATO son pulsar crudo, cocinado, añadir, borrar o guardar.
                    // ¿¿Modificar el mensaje para que indique "No puede realizar esa acción. Seleccione grupo y después crudo/cocinado"??
                    // Puede que el mensaje actual no sea lo suficiente claro porque no indica específicamente qué se ha hecho mal.

          case STATE_groupA: case STATE_groupB:    
                                    if(hasScaleEventOccurred() and (eventoBascula == INCREMENTO)){ // El error ha sido colocar alimento antes de escoger crudo/cocinado
                                      keepErrorScreen = true; // Para mantener la pantalla de error hasta que se retire el plato para comenzar de nuevo.
                                                                    // Este error no se trata como los demás (mostrando solo durante 3 seg) porque no es simplemente
                                                                    // que no se pueda hacer, como pulsar un botón, sino que se debe corregir la acción. Es decir,
                                                                    // retirar lo colocado.
                                    } 
                                    showError(ERROR_STATE_GROUPS);          break;  // grupoA o groupB

          case STATE_raw: case STATE_cooked:  
                                    showError(ERROR_STATE_PROCESAMIENTO);   break;  // Crudo o Cocinado

          case STATE_weighted:      showError(ERROR_STATE_WEIGHTED);        break;  // Pesado

          // En los siguientes casos no se chequea si el error ha sido colocar alimento porque se entiende que es improbable que eso ocurra. Sin embargo, si ocurriera,
          // se marcaría error pero no se gestionaría esperando corregir la acción (quitar el peso), como en el caso de los grupos ==> Posible mejora
          //
          // Solo se gestiona el fallo de colocar peso directamente desde groupA/B porque es bastante probable 
          // que lo hagan los usuarios mientras se acostumbran al funcionamiento.
          //
          case STATE_add_check:     showError(ERROR_STATE_ADD_CHECK);       break;  // add_check
          case STATE_added:         showError(ERROR_STATE_ADDED);           break;  // Added
          case STATE_delete_check:  showError(ERROR_STATE_DELETE_CHECK);    break;  // delete_check
          case STATE_deleted:       showError(ERROR_STATE_DELETED);         break;  // Deleted
          case STATE_save_check:    showError(ERROR_STATE_SAVE_CHECK);      break;  // save_check
          case STATE_saved:         showError(ERROR_STATE_SAVED);           break;  // Saved
          case STATE_CANCEL:        showError(ERROR_STATE_CANCEL);          break;  // Cancelar
          case STATE_AVISO:         showError(ERROR_STATE_AVISO);           break;  // Aviso                          
          default:   break;  
        }
        
        doneState = true;                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }
    // ----- FIN INFO MOSTRADA ------------------------------------ 


    // -----  TRANSICIONES TRAS ERROR  ----------------------------
    if(!keepErrorScreen){
        // ----- TIEMPO DE ESPERA -------------------------------------
        // Solo para regresar si el error cometido ha sido pulsar un botón.
        // Si el error ha sido colocar algo en la báscula cuando no tocaba (p. ej. desde Grupos),
        // se mantiene la pantalla de error (keepErrorScreen = true) hasta que se retire. 
        currentTime = millis();
        if ((currentTime - previousTimeError) > 3000) {    // Tras 3 segundos mostrando error...
            // Regresar al estado anterior desde donde se cometió el error
            switch (state_prev){ 
              case STATE_Init:         Serial.print(F("\nRegreso a Init tras ERROR..."));         addEventToBuffer(GO_TO_INIT);            break;  // Init
              case STATE_Plato:         Serial.print(F("\nRegreso a Plato tras ERROR..."));         addEventToBuffer(GO_TO_PLATO);            break;  // Plato
              case STATE_groupA:        Serial.print(F("\nRegreso a groupA tras ERROR..."));        addEventToBuffer(GO_TO_GROUP_A);          break;  // grupoA 
              case STATE_groupB:        Serial.print(F("\nRegreso a groupB tras ERROR..."));        addEventToBuffer(GO_TO_GROUP_B);          break;  // grupoB
              case STATE_raw:           Serial.print(F("\nRegreso a raw tras ERROR..."));           addEventToBuffer(GO_TO_RAW);              break;  // Crudo
              case STATE_cooked:        Serial.print(F("\nRegreso a cooked tras ERROR..."));        addEventToBuffer(GO_TO_COOKED);           break;  // Cocinado
              case STATE_weighted:      Serial.print(F("\nRegreso a weighted tras ERROR..."));      addEventToBuffer(GO_TO_WEIGHTED);         break;  // Pesado
              case STATE_add_check:     Serial.print(F("\nRegreso a add_check tras ERROR..."));     addEventToBuffer(GO_TO_ADD_CHECK);        break;  // add_check
              case STATE_added:         Serial.print(F("\nRegreso a added tras ERROR..."));         addEventToBuffer(GO_TO_ADDED);            break;  // Added
              case STATE_delete_check:  Serial.print(F("\nRegreso a delete_check tras ERROR..."));  addEventToBuffer(GO_TO_DELETE_CHECK);     break;  // delete_check
              case STATE_deleted:       Serial.print(F("\nRegreso a deleted tras ERROR..."));       addEventToBuffer(GO_TO_DELETED);          break;  // Deleted
              case STATE_save_check:    Serial.print(F("\nRegreso a save_check tras ERROR..."));    addEventToBuffer(GO_TO_SAVE_CHECK);       break;  // save_check
              case STATE_saved:         Serial.print(F("\nRegreso a saved tras ERROR..."));         addEventToBuffer(GO_TO_SAVED);            break;  // Saved
              
              case STATE_CANCEL:   // No se regresa a STATE_CANCEL, sino al último estado válido desde donde se inició la acción que se ha cancelado previo al error.
                  switch (lastValidState){
                    case STATE_Init:     Serial.print(F("\nRegreso a Init tras ERROR durante CANCELACION..."));       addEventToBuffer(GO_TO_INIT);       break;  // Init
                    case STATE_raw:       Serial.print(F("\nRegreso a raw tras ERROR durante CANCELACION..."));         addEventToBuffer(GO_TO_RAW);         break;  // Crudo
                    case STATE_cooked:    Serial.print(F("\nRegreso a cooked tras ERROR durante CANCELACION..."));      addEventToBuffer(GO_TO_COOKED);      break;  // Cocinado
                    case STATE_weighted:  Serial.print(F("\nRegreso a weighted tras ERROR durante CANCELACION..."));    addEventToBuffer(GO_TO_WEIGHTED);    break;  // Pesado
                    default:  break;  
                  }
                  break;

              case STATE_AVISO:   // No se regresa a STATE_AVISO, sino al último estado válido desde donde se inició la acción que se ha cancelado previo al error.
                  switch (lastValidState){
                    case STATE_Init:     Serial.print(F("\nRegreso a Init tras ERROR durante AVISO..."));       addEventToBuffer(GO_TO_INIT);       break;  // Init
                    case STATE_raw:       Serial.print(F("\nRegreso a raw tras ERROR durante AVISO..."));         addEventToBuffer(GO_TO_RAW);         break;  // Crudo
                    case STATE_cooked:    Serial.print(F("\nRegreso a cooked tras ERROR durante AVISO..."));      addEventToBuffer(GO_TO_COOKED);      break;  // Cocinado
                    case STATE_weighted:  Serial.print(F("\nRegreso a weighted tras ERROR durante AVISO..."));    addEventToBuffer(GO_TO_WEIGHTED);    break;  // Pesado
                    default:  break;  
                  }
                  break;

              default:   break;  
            }

            flagEvent = true; // En todos los casos anteriores se marcará evento
            flagError = false; // Reiniciar flag de error hasta que se vuelva a cometer
                      
        }
        // ----- FIN TIEMPO DE ESPERA -------------------------------------

        //
        // ----- EVENTOS OCURRIDOS DURANTE ERROR -------------------------------------
        //
        // Si no han pasado los 3 segundos y, mientras estamos en estado de error, se hace algo (pulsar botón o modificar peso), 
        // se hace lo que marque el evento ocurrido, si no es otro error (muy improbable).

        else if(eventOccurred()){ // Evento de interrupción (botonera o báscula) durante pantalla de error. Hace checkBascula()
            // Realizar lo que pide el evento ocurrido durante error, según el estado desde donde se cometió el error.
            switch (state_prev){ 

              case STATE_Init:              
                  if(hasScaleEventOccurred()){ // Ha habido evento en báscula
                      if(eventoBascula == INCREMENTO){ // Se ha colocado el recipiente mientras se estaba en error
                          Serial.print(F("\nRecipiente colocado durante ERROR en STATE_Init..."));            addEventToBuffer(GO_TO_PLATO);        flagEvent = true;  break;
                      }
                  }
                  if(buttonInterruptOccurred()){ // Ha habido pulsación
                      checkAllButtons();  // Qué botón se ha pulsado
                                          // Comprueba qué botón se ha pulsado. Aunque en checkAllButtons() ya se añada un evento en el buffer con
                                          // el botón pulsado, no lo utilizamos como transición directa desde este estado porque solo queremos que 
                                          // atienda a los eventos si corresponde. Es decir, si se tiene que escoger grupo (eventoGrande = TIPO_A | TIPO_B), 
                                          // pero se escoge procesamiento (eventoMain = CRUDO | COCINADO), no queremos que pase a ese estado, porque
                                          // estando en STATE_Plato (que es donde se espera escoger grupo) no se permite escoger procesamiento.
                                          //
                                          // Solo se debe "forzar" la transición a estados a los que se pueda transicionar desde el estado desde donde
                                          // se cometió el error, por eso se debe chequear qué botón se ha pulsado, para ver si hacer caso o ignorar.
                                          //
                                          // Entonces, aunque checkAllButtons() añada un evento al buffer, el 'lastEvent' con el que se compararán las
                                          // reglas de transición será el último marcado aquí.

                      if(eventoMain == GUARDAR){ // Se ha pulsado "Guardar comida" mientras se estaba en error. Se pasa a STATE_save_check para confirmar acción.
                          Serial.print(F("\nGuardar comida durante ERROR en STATE_Init..."));                  addEventToBuffer(GO_TO_SAVE_CHECK);   flagEvent = true;  break;
                      }
                  }
                
                  break;  


              case STATE_Plato:  
                  if(hasScaleEventOccurred()){ // Ha habido evento en báscula
                      if((eventoBascula == INCREMENTO) or (eventoBascula == DECREMENTO)){
                          Serial.print(F("\nRecolocando recipiente durante ERROR en STATE_Plato..."));          addEventToBuffer(GO_TO_PLATO);        flagEvent = true;  break;
                      }
                      else if(eventoBascula == LIBERAR){ // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                          Serial.print(F("\nRecipiente retirado durante ERROR en STATE_Plato..."));             addEventToBuffer(GO_TO_INIT);        flagEvent = true;  break;
                      }
                  }
                  if(buttonInterruptOccurred()){ // Ha habido pulsación
                      checkAllButtons(); // Qué botón se ha pulsado
                      if(eventoGrande == TIPO_A){ 
                          Serial.print(F("\nGrupo tipo A escogido durante ERROR en STATE_Plato..."));           addEventToBuffer(GO_TO_GROUP_A);      flagEvent = true;  break;
                      }
                      else if(eventoGrande == TIPO_B){ 
                          Serial.print(F("\nGrupo tipo B escogido durante ERROR en STATE_Plato..."));           addEventToBuffer(GO_TO_GROUP_B);      flagEvent = true;  break;
                      }
                  }
                  break;  


              case STATE_groupA:   
                  if(hasScaleEventOccurred()){ // Ha habido evento en báscula
                      if(eventoBascula == DECREMENTO){
                          Serial.print(F("\nRetirando alimento durante ERROR en STATE_grupoA..."));             addEventToBuffer(GO_TO_GROUP_A);      flagEvent = true;  break;
                      }
                      else if(eventoBascula == LIBERAR){ // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                          Serial.print(F("\nPlato retirado durante ERROR en STATE_grupoA..."));                 addEventToBuffer(GO_TO_INIT);        flagEvent = true;  break;
                      }
                  }
                  if(buttonInterruptOccurred()){ // Ha habido pulsación
                      checkAllButtons(); // Qué botón se ha pulsado
                      if(eventoGrande == TIPO_A){
                          Serial.print(F("\nGrupo tipo A escogido durante ERROR en STATE_grupoA..."));          addEventToBuffer(GO_TO_GROUP_A);      flagEvent = true;  break;
                      }
                      else if(eventoGrande == TIPO_B){ 
                          Serial.print(F("\nGrupo tipo B escogido durante ERROR en STATE_grupoA..."));          addEventToBuffer(GO_TO_GROUP_B);      flagEvent = true;  break;
                      }
                      else if(eventoMain == CRUDO){
                          Serial.print(F("\nCRUDO escogido durante ERROR en STATE_grupoA..."));                 addEventToBuffer(GO_TO_RAW);          flagEvent = true;  break;
                      }
                      else if(eventoMain == COCINADO){ 
                          Serial.print(F("\nCOCINADO escogido durante ERROR en STATE_grupoA..."));              addEventToBuffer(GO_TO_COOKED);       flagEvent = true;  break;
                      }
                  }
                  break;  
            
            
              case STATE_groupB:  
                  if(hasScaleEventOccurred()){ // Ha habido evento en báscula
                      if(eventoBascula == DECREMENTO){
                          Serial.print(F("\nRetirando alimento durante ERROR en STATE_grupoB..."));             addEventToBuffer(GO_TO_GROUP_B);      flagEvent = true;  break;
                      }
                      else if(eventoBascula == LIBERAR){ // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                          Serial.print(F("\nPlato retirado durante ERROR en STATE_grupoB..."));                 addEventToBuffer(GO_TO_INIT);        flagEvent = true;  break;
                      }
                  }
                  if(buttonInterruptOccurred()){ // Ha habido pulsación
                      checkAllButtons(); // Qué botón se ha pulsado => deshabilita flag de interrupcion
                      if(eventoGrande == TIPO_A){ 
                          Serial.print(F("\nGrupo tipo A escogido durante ERROR en STATE_grupoB..."));          addEventToBuffer(GO_TO_GROUP_A);      flagEvent = true;  break;
                      }
                      else if(eventoGrande == TIPO_B){ 
                          Serial.print(F("\nGrupo tipo B escogido durante ERROR en STATE_grupoB..."));          addEventToBuffer(GO_TO_GROUP_B);      flagEvent = true;  break;
                      }
                      else if(eventoMain == CRUDO){ 
                          Serial.print(F("\nCRUDO escogido durante ERROR en STATE_grupoB..."));                 addEventToBuffer(GO_TO_RAW);          flagEvent = true;  break;
                      }
                      else if(eventoMain == COCINADO){ 
                          Serial.print(F("\nCOCINADO escogido durante ERROR en STATE_grupoB..."));              addEventToBuffer(GO_TO_COOKED);       flagEvent = true;  break;
                      }
                  }
                  break;


              case STATE_raw:  case STATE_cooked:   // STATE_raw o STATE_cooked
                  if(hasScaleEventOccurred()){ // Ha habido evento en báscula
                      if(eventoBascula == INCREMENTO){
                          Serial.print(F("\nColocando alimento durante ERROR en STATE_raw o STATE_cooked..."));       addEventToBuffer(GO_TO_WEIGHTED);     flagEvent = true;  break;
                      }
                      else if(eventoBascula == LIBERAR){ // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                          Serial.print(F("\nPlato retirado durante ERROR en STATE_raw o STATE_cooked..."));           addEventToBuffer(GO_TO_INIT);        flagEvent = true;  break;
                      }
                  }
                  if(buttonInterruptOccurred()){ // Ha habido pulsación
                      checkAllButtons(); // Qué botón se ha pulsado
                      if(eventoGrande == TIPO_A){ 
                          Serial.print(F("\nGrupo tipo A escogido durante ERROR en STATE_raw o STATE_cooked..."));    addEventToBuffer(GO_TO_GROUP_A);      flagEvent = true;  break;
                      }
                      else if(eventoGrande == TIPO_B){ 
                          Serial.print(F("\nGrupo tipo B escogido durante ERROR en STATE_raw o STATE_cooked..."));    addEventToBuffer(GO_TO_GROUP_B);      flagEvent = true;  break;
                      }
                      else if(eventoMain == CRUDO){ 
                          Serial.print(F("\nCRUDO escogido durante ERROR en STATE_raw o STATE_cooked..."));           addEventToBuffer(GO_TO_RAW);          flagEvent = true;  break;
                      }
                      else if(eventoMain == COCINADO){ 
                          Serial.print(F("\nCOCINADO escogido durante ERROR en STATE_raw o STATE_cooked..."));        addEventToBuffer(GO_TO_COOKED);       flagEvent = true;  break;
                      }
                      else if(eventoMain == ADD_PLATO){ 
                          Serial.print(F("\nAñadir plato durante ERROR en STATE_raw o STATE_cooked..."));             addEventToBuffer(GO_TO_ADD_CHECK);    flagEvent = true;  break;
                      }
                      else if(eventoMain == DELETE_PLATO){
                          Serial.print(F("\nBorrar plato durante ERROR en STATE_raw o STATE_cooked..."));             addEventToBuffer(GO_TO_DELETE_CHECK); flagEvent = true;  break;
                      }
                      else if(eventoMain == GUARDAR){
                          Serial.print(F("\nGuardar comida durante ERROR en STATE_raw o STATE_cooked..."));           addEventToBuffer(GO_TO_SAVE_CHECK);   flagEvent = true;  break;
                      }
                  }
                  break;


              case STATE_weighted:   
                  if(hasScaleEventOccurred()){ // Ha habido evento en báscula
                      if((eventoBascula == INCREMENTO) or (eventoBascula == DECREMENTO)){
                          Serial.print(F("\nColocando/retirando alimento durante ERROR en STATE_weighted...")); addEventToBuffer(GO_TO_WEIGHTED);     flagEvent = true;  break;
                      }
                      else if(eventoBascula == LIBERAR){ // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                          Serial.print(F("\nPlato retirado durante ERROR en STATE_weighted..."));               addEventToBuffer(GO_TO_INIT);        flagEvent = true;  break;
                      }
                  }
                  if(buttonInterruptOccurred()){ // Ha habido pulsación
                      checkAllButtons(); // Qué botón se ha pulsado
                      if(eventoGrande == TIPO_A){ 
                          Serial.print(F("\nGrupo tipo A escogido durante ERROR en STATE_weighted..."));        addEventToBuffer(GO_TO_GROUP_A);      flagEvent = true;  break;
                      }
                      else if(eventoGrande == TIPO_B){ 
                          Serial.print(F("\nGrupo tipo B escogido durante ERROR en STATE_weighted..."));        addEventToBuffer(GO_TO_GROUP_B);      flagEvent = true;  break;
                      }
                      else if(eventoMain == CRUDO){ 
                          Serial.print(F("\nCRUDO escogido durante ERROR en STATE_weighted..."));               addEventToBuffer(GO_TO_RAW);          flagEvent = true;  break;
                      }
                      else if(eventoMain == COCINADO){ 
                          Serial.print(F("\nCOCINADO escogido durante ERROR en STATE_weighted..."));            addEventToBuffer(GO_TO_COOKED);       flagEvent = true;  break;
                      }
                      else if(eventoMain == ADD_PLATO){ 
                          Serial.print(F("\nAñadir plato durante ERROR en STATE_weighted..."));                 addEventToBuffer(GO_TO_ADD_CHECK);    flagEvent = true;  break;
                      }
                      else if(eventoMain == DELETE_PLATO){
                          Serial.print(F("\nBorrar plato durante ERROR en STATE_weighted..."));                 addEventToBuffer(GO_TO_DELETE_CHECK); flagEvent = true;  break;
                      }
                      else if(eventoMain == GUARDAR){
                          Serial.print(F("\nGuardar comida durante ERROR en STATE_weighted..."));               addEventToBuffer(GO_TO_SAVE_CHECK);   flagEvent = true;  break;
                      }
                  }
                  break;


              case STATE_add_check:  
                  // No se chequea báscula porque sería provocar un error dentro de un error. Es improbable
                  if(buttonInterruptOccurred()){ // Ha habido pulsación
                      checkAllButtons(); // Qué botón se ha pulsado 
                      if(eventoMain == ADD_PLATO){ 
                          Serial.print(F("\nConfirmando añadir plato durante ERROR en STATE_add_check..."));    addEventToBuffer(GO_TO_ADDED);  flagEvent = true;  break;
                      }
                      else if((eventoGrande == TIPO_A) or (eventoGrande == TIPO_B) or (eventoMain == CRUDO) or (eventoMain == COCINADO) or (eventoMain == DELETE_PLATO) or(eventoMain == GUARDAR)){ 
                          // No se incluye la cancelación por time-out porque eso solo ocurre si no hay actividad en STATE_add_check. Nunca va a ocurrir en STATE_ERROR, 
                          // aunque se cometiera el error desde add_check, porque tras 3 segundos se regresaría a STATE_add_check y allí ya se comprobaría el time-out.
                          Serial.print(F("\nCancelando añadir plato durante ERROR en STATE_add_check..."));     addEventToBuffer(GO_TO_CANCEL);  flagEvent = true;  break;
                      }
                  }
                  break;

              
              case STATE_added:   
                  if(hasScaleEventOccurred()){ // Ha habido evento en báscula
                      if(eventoBascula == INCREMENTO){
                          Serial.print(F("\nRetirando alimento durante ERROR en STATE_added..."));              addEventToBuffer(GO_TO_ADDED);   flagEvent = true;  break;
                      }
                      else if(eventoBascula == LIBERAR){ // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                          Serial.print(F("\nPlato retirado durante ERROR en STATE_added..."));                  addEventToBuffer(GO_TO_INIT);   flagEvent = true;  break;
                      }
                  }
                  // No se chequean botones porque si se pulsan, se deben ignorar
                  break;
              
              case STATE_delete_check:  
                  // No se chequea báscula porque sería provocar un error dentro de un error. Es improbable
                  if(buttonInterruptOccurred()){ // Ha habido pulsación
                      checkAllButtons(); // Qué botón se ha pulsado
                      if(eventoMain == DELETE_PLATO){ 
                          Serial.print(F("\nConfirmando borrar plato durante ERROR en STATE_delete_check...")); addEventToBuffer(GO_TO_DELETED);  flagEvent = true;  break;
                      }
                      else if((eventoGrande == TIPO_A) or (eventoGrande == TIPO_B) or (eventoMain == CRUDO) or (eventoMain == COCINADO) or (eventoMain == ADD_PLATO) or(eventoMain == GUARDAR)){ 
                          // No se incluye la cancelación por time-out porque eso solo ocurre si no hay actividad en STATE_delete_check. Nunca va a ocurrir en STATE_ERROR, 
                          // aunque se cometiera el error desde delete_check, porque tras 3 segundos se regresaría a STATE_delete_check y allí ya se comprobaría el time-out.
                          Serial.print(F("\nCancelando borrar plato durante ERROR en STATE_delete_check..."));  addEventToBuffer(GO_TO_CANCEL);   flagEvent = true;  break;
                      }
                  }
                  break;

              
              case STATE_deleted:  
                  if(hasScaleEventOccurred()){ // Ha habido evento en báscula
                      if(eventoBascula == INCREMENTO){
                          Serial.print(F("\nRetirando alimento durante ERROR en STATE_deleted..."));            addEventToBuffer(GO_TO_DELETED);  flagEvent = true;  break;
                      }
                      else if(eventoBascula == LIBERAR){ // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                          Serial.print(F("\nPlato retirado durante ERROR en STATE_deleted..."));                addEventToBuffer(GO_TO_INIT);    flagEvent = true;  break;
                      }
                  }
                  // No se chequean botones porque si se pulsan, se deben ignorar
                  break;
              
              case STATE_save_check: 
                  // No se chequea báscula porque sería provocar un error dentro de un error. Es improbable
                  if(buttonInterruptOccurred()){ // Ha habido pulsación
                      checkAllButtons(); // Qué botón se ha pulsado
                      if(eventoMain == GUARDAR){ 
                          Serial.print(F("\nConfirmando guardar comida durante ERROR en STATE_save_check...")); addEventToBuffer(GO_TO_SAVED);    flagEvent = true;  break;
                      }
                      else if((eventoGrande == TIPO_A) or (eventoGrande == TIPO_B) or (eventoMain == CRUDO) or (eventoMain == COCINADO) or (eventoMain == ADD_PLATO) or(eventoMain == DELETE_PLATO)){ 
                          // No se incluye la cancelación por time-out porque eso solo ocurre si no hay actividad en STATE_save_check. Nunca va a ocurrir en STATE_ERROR, 
                          // aunque se cometiera el error desde save_check, porque tras 3 segundos se regresaría a STATE_save_check y allí ya se comprobaría el time-out.
                          Serial.print(F("\nCancelando guardar comida durante ERROR en STATE_save_check...")); addEventToBuffer(GO_TO_CANCEL);    flagEvent = true;  break;
                      }
                  }
                  break;

              
              case STATE_saved:  
                  if(hasScaleEventOccurred()){ // Ha habido evento en báscula
                      if(eventoBascula == INCREMENTO){
                          Serial.print(F("\nRetirando alimento durante ERROR en STATE_saved..."));             addEventToBuffer(GO_TO_SAVED);     flagEvent = true;  break;
                      }
                      else if(eventoBascula == LIBERAR){ // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                          Serial.print(F("\nPlato retirado durante ERROR en STATE_saved..."));                 addEventToBuffer(GO_TO_INIT);     flagEvent = true;  break;
                      }
                  }
                  // No se chequean botones porque si se pulsan, se deben ignorar
                  break;

              // No se están chequeando eventos durante ERROR ocurrido en CANCEL o AVISO porque en esos estados no se debería hacer nada, de hecho,
              // todos los eventos que parten de esos estados son artificiales, no provocados por el usuario. Además, es muy improbable que el 
              // usuario haga algo mientras está la pantalla de error ocurrido durante CANCEL o AVISO.

              default:  break;  
            }

            flagError = false; // Reiniciar flag de error hasta que se vuelva a cometer
        }
        // ----- FIN EVENTOS OCURRIDOS DURANTE ERROR -------------------------------------
    }
    else{ // keepErrorScreen = true --> Se está manteniendo la pantalla de error para obligar a rectificar la acción de haber colocado alimento
          // cuando no tocaba (en Grupos).

          // Si se libera la báscula, se regresa a Init. 
          // Esta acción se gestiona de esta forma (marcando un GO_TO_INIT) por hacerlo de la misma forma que los casos generales
          // de eventos durante estado de error. Además, así se puede deshabilitar la 'flagError', para que no continúe marcando error.
          // Si se gestionara esta liberación con una simple regla de transición a Init con LIBERAR, no se podría deshabilitar
          // la 'flagError', lo que provocaría un falso mensaje de error al regresar a Init.
          
          // La ISR de la Scale salta cada 0.5 seg, entonces se puede no hacer checkBascula() y continuar con la ejecución para que se
          // haga el chequeo de eventos de báscula en el loop().
          //checkBascula();     // Comprueba interrupción de báscula. Lo necesito para ver si hace falta marcar evento

          if(hasScaleEventOccurred() and (eventoBascula == LIBERAR)){
              addEventToBuffer(GO_TO_INIT);        
              flagEvent = true;  
              flagError = false; // Reiniciar flag de error hasta que se vuelva a cometer
          }

          if(buttonInterruptOccurred()){ // Ha habido pulsación
              checkAllButtons(); // Qué botón se ha pulsado
              if((eventoMain == CRUDO) or (eventoMain == COCINADO)){
                  if(eventoMain == CRUDO){
                      Serial.print(F("\nCRUDO escogido durante ERROR por PESO en ")); printStateName(state_prev); Serial.print(F("...")); // STATE_groupA o STATE_groupB
                      Serial.println(F("\nAlimento crudo..."));        
                      procesamiento = ALIMENTO_CRUDO;         
                  }
                  else if(eventoMain == COCINADO){ 
                      Serial.print(F("\nCOCINADO escogido durante ERROR por PESO en ")); printStateName(state_prev); Serial.print(F("...")); // STATE_groupA o STATE_groupB  
                      Serial.println(F("\nAlimento cocinado..."));        
                      procesamiento = ALIMENTO_COCINADO;  
                  }
                  addEventToBuffer(GO_TO_WEIGHTED);   // Pasar directamente a STATE_weighted para mostrar la info de lo pesado según la cocción (crudo o cocinado)
                                                      // Si se pasara a STATE_raw o STATE_cooked, el sistema esperaría un incremento de peso para pasar a STATE_weighted,
                                                      // pero ese incremento ya ha ocurrido porque se ha colocado alimento, aunque haya resultado en un error, pero es
                                                      // un error subsanable de esta forma: pulsando CRUDO o COCINADO tras marcarse el error.
                  flagEvent = true;  
                  flagError = false; // Reiniciar flag de error hasta que se vuelva a cometer
              }
          }
    }

    // -----  FIN TRANSICIONES TRAS ERROR  ------------------------
}



/*---------------------------------------------------------------------------------------------------------
   actStateCANCEL(): Acciones del STATE_CANCEL
----------------------------------------------------------------------------------------------------------*/
void actStateCANCEL(){ 
    static unsigned long previousTimeCancel;  // Para regresar al estado necesario
    unsigned long currentTime;               // Tiempo actual  

    // -----  INFORMACIÓN MOSTRADA  ------------------------
    if(!doneState){ 
        previousTimeCancel = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de acción cancelada, regresar al estado anterior.

        Serial.println(F("Acción cancelada")); 

        showAccionCancelada();        // Mostrar info de acción cancelada

        doneState = true;             // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                      // cada vez que se entre a esta función debido al loop de Arduino.
                                      // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.

    }
    // ----- FIN INFORMACIÓN MOSTRADA -----------------------


    // ----- TIEMPO DE ESPERA -------------------------------
    currentTime = millis();
    if ((currentTime - previousTimeCancel) > 1000) {    // Tras 1 segundo mostrando acción cancelada...
        // Regreso al estado desde donde se inició la acción ahora cancelada.

        // Ultimo estado válido puede ser Init, raw, cooked o weighted. Es decir, cualquiera de los cuales desde donde se puede intentar un acción
        // cancelable (add/delete/save).
        switch (lastValidState){
          case STATE_Init:      Serial.print(F("\nRegreso a Init tras CANCELACION..."));       addEventToBuffer(GO_TO_INIT);       break;  // Init
          case STATE_raw:       Serial.print(F("\nRegreso a raw tras CANCELACION..."));         addEventToBuffer(GO_TO_RAW);         break;  // Crudo
          case STATE_cooked:    Serial.print(F("\nRegreso a cooked tras CANCELACION..."));      addEventToBuffer(GO_TO_COOKED);      break;  // Cocinado
          case STATE_weighted:  Serial.print(F("\nRegreso a weighted tras CANCELACION..."));    addEventToBuffer(GO_TO_WEIGHTED);    break;  // Pesado
          default:  break;  
        }

        flagEvent = true; 
                  
    }
    // ----- FIN TIEMPO DE ESPERA ---------------------------
    

    // ----- EVENTOS OCURRIDOS DURANTE CANCELACIÓN ----------
    // La pantalla solo dura 1 segundo, no debería darle tiempo a los usuarios hacer nada. Además, no deben hacer nada.
    // Solo se va a gestionar el pulsar grupo1 para acceder al menú escondido de borrar el fichero csv.

    else if(buttonInterruptOccurred()){ // Evento de interrupción por botonera durante pantalla de cancelación. 
        checkAllButtons(); // Qué botón se ha pulsado
        if(buttonGrande == 1){ // Se ha pulsado grupo1
            Serial.print(F("\ngrupo1 pulsado durante CANCELAR. Entrando a menú de BORRAR CSV..."));   
            addEventToBuffer(DELETE_CSV);      
            flagEvent = true;  
        }
    }
    // ----- FIN EVENTOS OCURRIDOS DURANTE CANCELACIÓN ------
}


/*---------------------------------------------------------------------------------------------------------
   actStateAVISO(): Acciones del STATE_AVISO
----------------------------------------------------------------------------------------------------------*/
void actStateAVISO(){ 
    static unsigned long previousTimeWarning;  // Para regresar al estado necesario
    unsigned long currentTime;                 // Tiempo actual  

    // -----  INFORMACIÓN MOSTRADA  ------------------------
    if(!doneState){ 
        previousTimeWarning = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de aviso, regresar al estado anterior.

        switch(state_prev){ // En este caso sí es el estado previo, pues solo se puede pasar a STATE_AVISO desde added/deleted/saved.
            case STATE_added:     showWarning(WARNING_NOT_ADDED);    Serial.println(F("No se ha creado otro plato porque el actual está vacío"));  break;  // added                    
            case STATE_deleted:   showWarning(WARNING_NOT_DELETED);  Serial.println(F("No se ha borrado el plato porque está vacío"));             break;  // deleted
            case STATE_saved:     showWarning(WARNING_NOT_SAVED);    Serial.println(F("No se ha guardado la comida porque está vacía"));           break;  // saved
            default:  break;  
        }

        doneState = true;             // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                      // cada vez que se entre a esta función debido al loop de Arduino.
                                      // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.

    }
    // ----- FIN INFORMACIÓN MOSTRADA -----------------------


    // ----- TIEMPO DE ESPERA -------------------------------
    currentTime = millis();
    if ((currentTime - previousTimeWarning) > 3000) {    // Tras 3 segundos mostrando warning...
        // Ultimo estado válido puede ser Init, groupA o groupB. Es decir, cualquiera de los cuales desde donde se puede intentar un acción
        // que pueda marcar aviso (add/delete/save).
        switch (lastValidState){
          case STATE_Init:    Serial.print(F("\nGO_TO_INIT forzada. Regreso a INI..."));    addEventToBuffer(GO_TO_INIT);       break;  // Init

          case STATE_raw: case STATE_cooked:   
              // Se regresa a STATE_groupA o STATE_groupB, según el último grupo escogido. 
              //  Allí se resetea procesamiento. Se mantiene peso del recipiente.
              if(eventoGrande == TIPO_A) {
                  addEventToBuffer(GO_TO_GROUP_A);  Serial.print(F("\nTIPO_A forzada. Regreso a GRUPOS..."));
              }
              else if(eventoGrande == TIPO_B){
                  addEventToBuffer(GO_TO_GROUP_B);  Serial.print(F("\nTIPO_A forzada. Regreso a GRUPOS..."));      
              }
              break;  // groupB

          default:  break;  
        }
        flagEvent = true;
    }
    // ----- FIN TIEMPO DE ESPERA ---------------------------

}


/*---------------------------------------------------------------------------------------------------------
   actState_DELETE_CSV_CHECK(): Acciones del STATE_DELETE_CSV_CHECK
----------------------------------------------------------------------------------------------------------*/
void actState_DELETE_CSV_CHECK(){ 

    // ------------ FUNCIONAMIENTO BORRADO FICHERO CSV ----------------------------------------------------
    //
    // Durante la pantalla de "Acción cancelada", que solo se muestra durante 1 segundo, los usuarios no deberían hacer nada.
    // Aprovechando esto, se va a introducir una función escondida para borrar el contenido del fichero csv que guarda la info
    // de las comidas. Así, el "Acumulado hoy" quedará vacío para el próximo paciente. 
    //
    // ESTO SOLO SE VA A REALIZAR EN LAS PRUEBAS, PARA AGILIZAR EL RESETEO DEL FICHERO ENTRE UN PACIENTE Y OTRO.
    // 
    // La idea es, durante la pantalla de cancelación, pulsar el botón de grupo1, accediendo a STATE_DELETE_CSV_CHECK donde se
    // pide confirmar la acción pulsando grupo20. Si no se confirma en 5 segundos, se regresa a donde se estuviera.
    // De esta forma, si algún usuario entrara sin querer a este menú, solo tendría que dejar pasar esos 5 seg para continuar.
    //
    // No se incluye el CANCELAR la acción pulsando cualquier otro botón por simplificar.
    //
    // -----------------------------------------------------------------------------------------------------


    static unsigned long previousTimeCancelDeleteCSV;      // Tiempo usado para cancelar la acción tras 5 segundos de inactividad
    unsigned long currentTime;                             // Tiempo actual  

    if(!doneState){
        previousTimeCancelDeleteCSV = millis(); 

        Serial.println(F("\n¿SEGURO QUE QUIERE BORRAR EL FICHERO CSV?")); 
        
        pedirConfirmacion_DELETE_CSV();       // Mostrar pregunta de confirmación para BORRAR CSV
                                              //    No se ha incluido esta pantalla en pedirConfirmacion(option) porque interesa
                                              //    tenerlo separado para eliminarlo tras las pruebas


        doneState = true;               // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                        // cada vez que se entre a esta función debido al loop de Arduino.
                                        // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

    
  

    // ----- CONFIRMACIÓN DE ACCIÓN ----------
    if(buttonInterruptOccurred()){ // Pulsación de botón
        checkAllButtons(); // Qué botón se ha pulsado
        if(buttonGrande == 20){ // Se ha pulsado grupo20
            Serial.print(F("\ngrupo20 pulsado durante DELETE_CSV_CHECK. Iniciando borrado..."));   
            addEventToBuffer(DELETE_CSV);      
            flagEvent = true; 
        }
    }
    // ----- FIN CONFIRMACIÓN DE ACCIÓN ------


    // ----- CANCELACIÓN AUTOMÁTICA -------------------------------
    else{
        currentTime = millis(); // Tomar tiempo para saber si han pasado los 5 seg para cancelar la acción
        if ((currentTime - previousTimeCancelDeleteCSV) > 5000) {    // Tras 5 segundos de inactividad, se cancela automáticamente la acción BORRAR CSV
            // Regreso al estado desde donde se inició la acción cancelada previo a entrar a este menú.

            // Ultimo estado válido puede ser Init, raw, cooked o weighted. Es decir, cualquiera de los cuales desde donde se puede intentar un acción
            // cancelable (add/delete/save), tras cuya cancelación se habría entrado a este menú.
            switch (lastValidState){
              case STATE_Init:     Serial.print(F("\nRegreso a Init tras cancelar BORRAR CSV..."));       addEventToBuffer(GO_TO_INIT);       break;  // Init
              case STATE_raw:       Serial.print(F("\nRegreso a raw tras cancelar BORRAR CSV..."));         addEventToBuffer(GO_TO_RAW);         break;  // Crudo
              case STATE_cooked:    Serial.print(F("\nRegreso a cooked tras cancelar BORRAR CSV..."));      addEventToBuffer(GO_TO_COOKED);      break;  // Cocinado
              case STATE_weighted:  Serial.print(F("\nRegreso a weighted tras cancelar BORRAR CSV..."));    addEventToBuffer(GO_TO_WEIGHTED);    break;  // Pesado
              default:  break;  
            }

            flagEvent = true; 
        }
                  
    }
    // ----- FIN CANCELACIÓN AUTOMÁTICA ---------------------------

    

}


/*---------------------------------------------------------------------------------------------------------
   actState_DELETED_CSV(): Acciones del STATE_DELETED_CSV
----------------------------------------------------------------------------------------------------------*/
void actState_DELETED_CSV(){ 

    // ------------ FUNCIONAMIENTO BORRADO FICHERO CSV ----------------------------------------------------
    //
    // ESTO SOLO SE VA A REALIZAR EN LAS PRUEBAS, PARA AGILIZAR EL RESETEO DEL FICHERO ENTRE UN PACIENTE Y OTRO.
    // 
    // Ya se ha confirmado la acción en STATE_DELETE_CSV_CHECK. Ahora se realiza el borrado.
    //
    // -----------------------------------------------------------------------------------------------------

    static unsigned long previousTimeDeleted;  // Para regresar a Init tras borrado (o no)
    unsigned long currentTime;                 // Tiempo actual  

    if(!doneState){
        previousTimeDeleted = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de borrado, regresar a Init

        Serial.println(F("\nBORRANDO CONTENIDO DEL FICHERO CSV Y FICHERO ESP32. LIMPIANDO ACUMULADO...")); 

        if(borrarFicheroCSV() && borrarFicheroESP32()) showAcumuladoBorrado(true); // true = éxito en el borrado
        else showAcumuladoBorrado(false); // false = error en el borrado

        flagFicheroCSVBorrado = true;

        doneState = true;               // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                        // cada vez que se entre a esta función debido al loop de Arduino.
                                        // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

    // ----- REGRESO A INIT TRAS BORRADO -----
    // Se ha gestionado la posibilidad de que los usuarios accedan a este menú desde cualquier estado desde donde se pueda
    // iniciar una acción cancelable (add/delete/save), pero no deberían llevar a cabo el borrado del csv, sino dejar
    // pasar los 5 seg para su cancelación.
    //
    // En las pruebas, nosotros (yo) iniciaremos el borrado únicamente desde STATE_Init (nada en la báscula), por lo que
    // hacemos el regreso directamente a Init.
    currentTime = millis();
    if ((currentTime - previousTimeDeleted) > 2000) {    // Tras 3 segundos mostrando warning...
        Serial.print(F("\nRegreso a Init tras BORRAR CSV..."));       
        addEventToBuffer(GO_TO_INIT);
        flagEvent = true; 
    }
    // ----- FIN REGRESO A INIT TRAS BORRADO --
}


/*---------------------------------------------------------------------------------------------------------
   actState_CRITIC_FAILURE_SD(): Acciones del STATE_CRITIC_FAILURE_SD
----------------------------------------------------------------------------------------------------------*/
void actState_CRITIC_FAILURE_SD(){ 

    // -----  INFORMACIÓN MOSTRADA  ------------------------
    if(!doneState){ 
        Serial.println(F("FALLO CRÍTICO EN LA SD")); 

        showCriticFailureSD();        // Mostrar info de fallo crítico en SD

        doneState = true;             // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                      // cada vez que se entre a esta función debido al loop de Arduino.
    }
    // ----- FIN INFORMACIÓN MOSTRADA -----------------------

    // NO SE ATIENDE A NINGÚN EVENTO PORQUE SE IMPIDE USAR SMARTCLOTH
}




/*---------------------------------------------------------------------------------------------------------
   doStateActions(): Acciones a realizar según el estado actual
----------------------------------------------------------------------------------------------------------*/
void doStateActions(){
    switch (state_actual){
      case STATE_Init:          actStateInit();         break;  // Init
      case STATE_Plato:         actStatePlato();        break;  // Plato
      case STATE_groupA:        // Misma función para groupA y groupB
      case STATE_groupB:        actGruposAlimentos();   break;  // groupB
      case STATE_raw:           actStateRaw();          break;  // raw
      case STATE_cooked:        actStateCooked();       break;  // cooked
      case STATE_weighted:      actStateWeighted();     break;  // weighted
      case STATE_add_check:     actStateAddCheck();     break;  // add_check
      case STATE_added:         actStateAdded();        break;  // added
      case STATE_delete_check:  actStateDeleteCheck();  break;  // delete_check
      case STATE_deleted:       actStateDeleted();      break;  // deleted
      case STATE_save_check:    actStateSaveCheck();    break;  // save_check
      case STATE_saved:         actStateSaved();        break;  // saved
      case STATE_ERROR:         actStateERROR();        break;  // ERROR
      case STATE_CANCEL:        actStateCANCEL();       break;  // CANCEL
      case STATE_AVISO:         actStateAVISO();        break; // AVISO

      case STATE_DELETE_CSV_CHECK:  actState_DELETE_CSV_CHECK();   break; // delete_csv_check
      case STATE_DELETED_CSV:       actState_DELETED_CSV();        break; // deleted_csv

      case STATE_CRITIC_FAILURE_SD: actState_CRITIC_FAILURE_SD();  break; // Fallo crítico en SD

      default: break;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actEventError(): Marca evento de ERROR por no cumplirse ninguna regla de transición
----------------------------------------------------------------------------------------------------------*/
void actEventError(){
    // Independientemente del estado actual, se marca el evento ERROR para pasar al STATE_ERROR, donde 
    // se muestra una pantalla de error según el estado previo desde donde se cometió el error. 
    // Tras 3 segundos con la pantalla de error, se marca el evento GO_TO_<estado> correspondiente para 
    // regresar al estado previo.

    Serial.print(F("\nERROR..."));
    addEventToBuffer(ERROR);    
    flagError = true;
}


/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- BUFFER EVENTOS -------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------
   isBufferInit(): Comprobar si el buffer está vacío, es decir, si el primer hueco lo ocupa un evento NONE.
----------------------------------------------------------------------------------------------------------*/
bool isBufferInit(){
    if(event_buffer[0] == NONE) return true;
    else return false;
}


/*---------------------------------------------------------------------------------------------------------
   isBufferFull(): Comprobar si el buffer está lleno, es decir, si no hay ningún hueco (evento NONE).
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
   shiftLeftEventBuffer(): Traslada los elementos del buffer a la izq para liberar un hueco sin perder 
                           los eventos previos.
----------------------------------------------------------------------------------------------------------*/
void shiftLeftEventBuffer(){ 
    //Serial.println(F("\nRotando buffer de eventos..."));
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
   addEventToBuffer(): Añade el último evento ocurrido al buffer de eventos
----------------------------------------------------------------------------------------------------------*/
void addEventToBuffer(event_t evento){
    //Serial.println(F("\n***********************************"));
    //Serial.println(F("Añadiendo evento al buffer..."));
    int pos;
    if(isBufferInit()){
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
    Serial.print("\nBuffer: "); 
    for (int i = 0; i < MAX_EVENTS; i++){
        //Serial.print(event_buffer[i]); Serial.print(" ");
        printEventName(event_buffer[i]); Serial.print(" | ");
    }
    //Serial.print(F("Last event: ")); Serial.println(lastEvent);
    Serial.println();
    printEventName(lastEvent);
    Serial.println();
}





#endif
