/** 
 * @file State_Machine.h
 * @brief Máquina de Estados (State Machine) de SmartCloth
 *
 * @author Irene Casares Rodríguez
 * @date 31/01/24
 * @version 1.0
 *
 *  Este archivo contiene las definiciones de los estados y eventos, además de las funciones
 *  realizadas en cada estado y para el funcionamiento de la máquina de estados, incluyendo el
 *  manejo de errores y avisos.
 *  
 */

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H


#include "debug.h" // SM_DEBUG --> SerialPC

/**
 * @def MAX_EVENTS
 * @brief Máximo número de eventos que se puede guardar en el buffer de eventos. 
 */
#define MAX_EVENTS 5

/**
 * @def RULES
 * @brief Máximo número de reglas de transición.
 */
#define RULES 197   // 7 SON DE BORRAR CSV EN PRUEBAS. SI PASA DE 255, HAY QUE CAMBIAR EL BUCLE EN checkStateConditions() byte --> int


#include "SD_functions.h" // Incluye lista_Comida.h y Serial_functions.h
#include "Buttons.h"


// --- MENSAJE MOSTRADO CUANDO ZONA 1 VACÍA ---
#define  NO_MSG             0
#define  MSG_SIN_RECIPIENTE 1
#define  MSG_SIN_GRUPO      2

// --- PEDIR CONFIRMACIÓN ---
#define  ASK_CONFIRMATION_ADD                   1
#define  ASK_CONFIRMATION_DELETE                2
#define  ASK_CONFIRMATION_SAVE                  3
#define  ASK_CONFIRMATION_SAVE_CON_INTERNET     4  
#define  ASK_CONFIRMATION_SAVE_SIN_INTERNET     5  

// --- ACCIÓN CONFIRMADA ---
#define  ADD_EXECUTED                             1  // Se ha añadido un plato
#define  DELETE_EXECUTED                          2  // Se ha borrado el plato
// En SD_functions.h ya están los siguientes:
//      SAVE_EXECUTED_FULL
//      SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP
//      SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT 
//      SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR 
//      SAVE_EXECUTED_ONLY_DATABASE            
//      ERROR_SAVING_DATA                       


// --- MENSAJE DE AVISO ---
#define  WARNING_NOT_ADDED                  1   // Aviso: plato no añadido porque está vacío
#define  WARNING_NOT_DELETED                2   // Aviso: plato no borrado porque está vacío
#define  WARNING_NOT_SAVED                  3   // Aviso: comida no guardada porque está vacía
#define  WARNING_RAW_COOKED_NOT_NEEDED      4   // Aviso: no hace falta crudo/cocinado para producto con código de barras
#define  WARNING_BARCODE_NOT_READ           5   // Aviso: código de barras no leído
#define  WARNING_PRODUCT_NOT_FOUND          6   // Aviso: producto no encontrado
#define  WARNING_MEALS_LEFT                 7   // Aviso: algunas comidas no sincronizadas
#define  WARNING_NO_INTERNET_NO_BARCODE     8   // Aviso: no se puede leer barcode porque no hay conexión a internet


// --- MENSAJE DE CANCELACIÓN ---
#define  ACTION_CANCELLED       1   // Acción cancelada (add/delete/save o leer barcode)
#define  PRODUCT_CANCELLED      2   // Producto cancelado (no es el deseado)


// --- MENSAJE DE ERROR ----
#define  ERROR_GENERAL               0   // Mensaje general de error (barcode_read, barcode_search, barcode_check y barcode)
#define  ERROR_STATE_INIT            1
#define  ERROR_STATE_PLATO           2
#define  ERROR_STATE_GROUP           3  
#define  ERROR_STATE_BARCODE         4
#define  ERROR_STATE_PROCESAMIENTO   5  // raw o cooked
#define  ERROR_STATE_WEIGHTED        6
#define  ERROR_STATE_ADD_CHECK       7
#define  ERROR_STATE_ADDED           8
#define  ERROR_STATE_DELETE_CHECK    9
#define  ERROR_STATE_DELETED         10
#define  ERROR_STATE_SAVE_CHECK      11
#define  ERROR_STATE_SAVED           12
#define  ERROR_STATE_CANCEL          13
#define  ERROR_STATE_AVISO           14





// Declaraciones adelantadas para evitar las inclusiones circulares.
void    tareScale();
void    reiniciarPesos();

void    checkMainButton();
void    checkGrandeButton();
void    checkBarcodeButton();
void    checkAllButtons(); 

bool    buttonInterruptOccurred();
bool    mainButtonInterruptOccurred();
bool    grandeButtonInterruptOccurred();
bool    barcodeButtonInterruptOccurred(); 

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
typedef enum 
{
                STATE_Init              =   (1),    // Estado inicial para colocar plato
                STATE_Plato             =   (2),    // Estado para escoger grupo
                STATE_Grupo             =   (3),    // Grupo alimentos
                STATE_Barcode_read      =   (4),    // Estado para leer barcode 
                STATE_Barcode_search    =   (5),    // Estado para buscar información del producto
                STATE_Barcode_check     =   (6),    // Estado para confirmar el producto encontrado
                STATE_Barcode           =   (7),    // Estado para utilizar el producto encontrado como grupo de alimentos
                STATE_raw               =   (8),    // crudo
                STATE_cooked            =   (9),    // cocinado
                STATE_weighted          =   (10),    // alimento pesado
                STATE_add_check         =   (11),    // Comprobar que se quiere añadir plato
                STATE_added             =   (12),    // plato añadido
                STATE_delete_check      =   (13),   // Comprobar que se quiere eliminar plato
                STATE_deleted           =   (14),   // plato eliminado
                STATE_save_check        =   (15),   // Comprobar que se quiere guardar comida
                STATE_saved             =   (16),   // comida guardada
                STATE_ERROR             =   (17),   // Estado ficticio de error (acción incorrecta)
                STATE_CANCEL            =   (18),   // Estado ficticio de Cancelación
                STATE_AVISO             =   (19),   // Estado ficticio de Aviso

                STATE_DELETE_CSV_CHECK    =   (20),   // COMPROBAR QUE SE QUIERE BORRAR EL CSV. EL USUARIO NO DEBERÍA ACCEDER. SOLO PARA LAS PRUEBAS.
                STATE_DELETED_CSV         =   (21),   // FICHERO CSV BORRADO. EL USUARIO NO DEBRÍA ACCEDER. SOLO PARA LAS PRUEBAS.
                                                      //  --> PARA LIMPIAR EL ACUMULADO DEL DÍA, DEJÁNDOLO LISTO PARA EL SIGUIENTE PACIENTE

                STATE_CRITIC_FAILURE_SD   =   (22),     // La SD ha fallado en el setup o no se encuentra --> no se permite usar SM
                                                        // Este estado no tiene transiciones de entrada ni de salida. Solo sirve para mostrar
                                                        // la pantalla de fallo crítico. Se entra manualmente por código, no por acciones del usuario.

                STATE_UPLOAD_DATA = (23)        // Hay datos en el fichero data-esp.txt y hay que subirlos a la base de datos.
                                                // Solo se pasaría a este estado al iniciar el dispositivo para guardar la info acumulada en el fichero.
                                                // Se entra manualmente por código, no por acciones del usuario, por eso no tiene transiciones de entrada.
                                                // Sí tiene una transición de salida a Init cuando se suben o no los datos.
} state_t;



/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- EVENTOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
/**
 * @enum event_t
 * @brief Enumeración de los diferentes eventos que pueden ocurrir en la Máquina de Estados.
 */
typedef enum 
{
              NONE                              =   (0),    
              TIPO_A                            =   (1),    // Botonera Grande (7,8,9,16,17,18)
              TIPO_B                            =   (2),    // Botonera Grande (1,2,3,4,5,6,10,11,12,13,14,15,19,20)
              BARCODE                           =   (3),    // Botón Barcode
              BARCODE_R                         =   (4),    // Barcode leído
              BARCODE_F                         =   (5),    // Producto encontrado
              CRUDO                             =   (6),    // AMARILLO
              COCINADO                          =   (7),    // BLANCO
              ADD_PLATO                         =   (8),    // VERDE
              DELETE_PLATO                      =   (9),    // ROJO
              GUARDAR                           =   (10),   // NEGRO 
              INCREMENTO                        =   (11),   // Báscula
              DECREMENTO                        =   (12),   // Báscula
              TARAR                             =   (13),   // Báscula tarada
              LIBERAR                           =   (14),   // Báscula vacía real
              ERROR                             =   (15),   // Error (acción incorrecta)
              CANCELAR                          =   (16),   // Cancelar acción de añadir, eliminar o guardar
              AVISO_PLATO_EMPTY_NOT_ADDED       =   (17),   // Aviso de plato vacío, no añadido uno nuevo
              AVISO_PLATO_EMPTY_NOT_DELETED     =   (18),   // Aviso de plato vacío, no borrado
              AVISO_COMIDA_EMPTY_NOT_SAVED      =   (19),   // Aviso de comida vacía, no guardada
              AVISO_NO_WIFI_BARCODE             =   (20),   // Aviso de que no hay conexión a Internet, así que no se podrá buscar el producto
              AVISO_NO_BARCODE                  =   (21),   // Aviso de que no se ha detectado barcode
              AVISO_PRODUCT_NOT_FOUND           =   (22),   // Aviso de que no se ha encontrado el producto en OpenFoodFacts
              GO_TO_INIT                        =   (23),   // Evento ficticio para volver a STATE_Init porque saltó un error, aviso o se canceló una acción (añadir, eliminar o guardar)
              GO_TO_PLATO                       =   (24),   // Evento ficticio para volver a STATE_Plato porque saltó un error 
              GO_TO_GRUPO                       =   (25),   // Evento ficticio para volver a STATE_Grupo porque saltó un error o aviso
              GO_TO_BARCODE_READ                =   (26),   // Evento ficticio para volver a STATE_Barcode_read porque saltó un error o aviso
              GO_TO_BARCODE                     =   (27),   // Evento ficticio para volver a STATE_Barcode porque saltó un error o aviso
              GO_TO_RAW                         =   (28),   // Evento ficticio para volver a STATE_raw porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              GO_TO_COOKED                      =   (29),   // Evento ficticio para volver a STATE_cooked porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              GO_TO_WEIGHTED                    =   (30),   // Evento ficticio para volver a STATE_weighted porque saltó un error o se canceló una acción (añadir, eliminar o guardar)
              GO_TO_ADD_CHECK                   =   (31),   // Evento ficticio para volver a STATE_add_check porque saltó un error 
              GO_TO_ADDED                       =   (32),   // Evento ficticio para volver a STATE_added porque saltó un error 
              GO_TO_DELETE_CHECK                =   (33),   // Evento ficticio para volver a STATE_delete_check porque saltó un error 
              GO_TO_DELETED                     =   (34),   // Evento ficticio para volver a STATE_deleted porque saltó un error
              GO_TO_SAVE_CHECK                  =   (35),   // Evento ficticio para volver a STATE_save_check porque saltó un error 
              GO_TO_SAVED                       =   (36),   // Evento ficticio para volver a STATE_saved porque saltó un error 
              GO_TO_CANCEL                      =   (37),   // Evento ficticio para ir a STATE_CANCEL si se cancela una acción iniciada durante un error
              DELETE_CSV                        =   (38),   // EVENTO PARA BORRAR EL FICHERO CSV. EL USUARIO NO DEBERÍA LLEGAR A ACTIVARLO. SOLO PARA LAS PRUEBAS.
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
typedef struct
{
    state_t state_i;     /**< Estado actual */
    state_t state_j;     /**< Estado siguiente */
    event_t condition;    /**< Condición para transición de estado */
}transition_rule;


/**
 * @brief Array de reglas de transición para la Máquina de Estados.
 */
static transition_rule rules[RULES] =  
{                                       // --- Esperando Recipiente ---
                                        {STATE_Init,STATE_Init,TARAR},                  // Tara inicial
                                        {STATE_Init,STATE_Init,DECREMENTO},             // Por si se inicia SM con un recipiente ya puesto y luego se retira
                                        {STATE_Init,STATE_Init,LIBERAR},                // Por si se inicia SM con recipiente puesto, se retira y resulta que pesaba menos de 20 gramos (el umbral)
                                        {STATE_Init,STATE_Plato,INCREMENTO},            // Colocar recipiente
                                        {STATE_Init,STATE_save_check,GUARDAR},          // Guardar comida directamente (comidaActual no está vacía)
                                        {STATE_Init,STATE_ERROR,ERROR},                 // Acción incorrecta
                                        // ----------------------------

                                        // --- Recipiente colocado ---
                                        {STATE_Plato,STATE_Plato,INCREMENTO},           // Cambios por recolocar recipiente
                                        {STATE_Plato,STATE_Plato,DECREMENTO},           // Cambios por recolocar recipiente
                                        {STATE_Plato,STATE_Init,LIBERAR},               // Se ha retirado el recipiente
                                        {STATE_Plato,STATE_Grupo,TIPO_A},               // Escogido grupo de alimentos de tipo A
                                        {STATE_Plato,STATE_Grupo,TIPO_B},               // Escogido grupo de alimentos de tipo B
                                        {STATE_Plato,STATE_Barcode_read,BARCODE},       // Pulsado botón de barcode para iniciar lectura
                                        {STATE_Plato,STATE_ERROR,ERROR},                // Acción incorrecta             
                                        // ---------------------------
                                        
                                        // --- Grupo de alimentos ---
                                        {STATE_Grupo,STATE_Init,LIBERAR},              // Se ha retirado el plato completo (+ recipiente) ==> Automáticamente se borra el plato, sin preguntar
                                        {STATE_Grupo,STATE_Grupo,DECREMENTO},          // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR)
                                        {STATE_Grupo,STATE_Grupo,TARAR},               // Tarar tras colocar recipiente o alimento   
                                        {STATE_Grupo,STATE_Grupo,TIPO_A},              // Otro grupo de tipo A
                                        {STATE_Grupo,STATE_Grupo,TIPO_B},              // Otro grupo de tipo B
                                        {STATE_Grupo,STATE_Barcode_read,BARCODE},      // Pulsado botón de barcode para iniciar lectura
                                        {STATE_Grupo,STATE_raw,CRUDO},                         
                                        {STATE_Grupo,STATE_cooked,COCINADO},              
                                        {STATE_Grupo,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento
                                        {STATE_Grupo,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual
                                        {STATE_Grupo,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento
                                        {STATE_Grupo,STATE_ERROR,ERROR},               // Acción incorrecta
                                        // --------------------------

                                        // --- Leer Barcode ---------
                                        {STATE_Barcode_read,STATE_Init,LIBERAR},                // Se ha retirado el plato completo (+ recipiente) ==> Automáticamente se borra el plato, sin preguntar
                                        {STATE_Barcode_read,STATE_Barcode_read,DECREMENTO},     // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR)
                                        {STATE_Barcode_read,STATE_Barcode_read,TARAR},          // Tarar tras colocar recipiente o alimento   
                                        {STATE_Barcode_read,STATE_Barcode_search,BARCODE_R},    // Producto detectado y código leído
                                        //{STATE_Barcode_read,STATE_AVISO,AVISO_NO_WIFI_BARCODE}, // Aviso de "No se puede leer barcode porque no hay conexión a Internet"
                                        {STATE_Barcode_read,STATE_AVISO,AVISO_NO_BARCODE},      // Aviso de "Código no detectado" porque no se detecta o porque no responde el ESP32
                                        {STATE_Barcode_read,STATE_CANCEL,TIPO_A},               // Cancelar lectura de barcode pulsando botón de grupo tipoA.
                                        {STATE_Barcode_read,STATE_CANCEL,TIPO_B},               // Cancelar lectura de barcode pulsando botón de grupo tipoB.
                                        {STATE_Barcode_read,STATE_CANCEL,BARCODE},              // Cancelar lectura de barcode pulsando botón de BARCODE
                                        {STATE_Barcode_read,STATE_CANCEL,CRUDO},                // Cancelar lectura de barcode pulsando botón de CRUDO.
                                        {STATE_Barcode_read,STATE_CANCEL,COCINADO},             // Cancelar lectura de barcode pulsando botón de COCINADO.
                                        {STATE_Barcode_read,STATE_CANCEL,ADD_PLATO},            // Cancelar lectura de barcode pulsando botón de AÑADIR PLATO.
                                        {STATE_Barcode_read,STATE_CANCEL,DELETE_PLATO},         // Cancelar lectura de barcode pulsando botón de BORRAR PLATO.
                                        {STATE_Barcode_read,STATE_CANCEL,GUARDAR},              // Cancelar lectura de barcode pulsando botón de GUARDAR.
                                                                                                // No se hace cancelación automática tras 30 segundos porque se asume simplemente que no se ha leído el barcode
                                        {STATE_Barcode_read,STATE_Plato,GO_TO_PLATO},           // Regresar a STATE_Plato si fue lastValidState y no había conexión a Internet, por lo que no se pudo leer barcode.
                                        {STATE_Barcode_read,STATE_Grupo,GO_TO_GRUPO},           // Regresar a STATE_Grupo si fue lastValidState y no había conexión a Internet, por lo que no se pudo leer barcode.
                                        {STATE_Barcode_read,STATE_Barcode,GO_TO_BARCODE},       // Regresar a STATE_Barcode si fue lastValidState y no había conexión a Internet, por lo que no se pudo leer barcode.
                                        {STATE_Barcode_read,STATE_ERROR,ERROR},                 // Acción incorrecta ????? El único error sería poner peso en báscula (Incremento), pero el usuario no debería hacerlo
                                        // --------------------------

                                        // --- Buscar Barcode --------
                                        {STATE_Barcode_search,STATE_Barcode_check,BARCODE_F},       // Producto encontrado
                                        {STATE_Barcode_search,STATE_AVISO,AVISO_PRODUCT_NOT_FOUND}, // Aviso de "Producto no encontrado"
                                        {STATE_Barcode_search,STATE_AVISO,AVISO_NO_WIFI_BARCODE},   // Aviso de "No se puede buscar producto porque no hay conexión a Internet"
                                        //{STATE_Barcode_search,STATE_CANCEL,CANCELAR},             // Cancelar búsqueda de producto por timeout del ESP32
                                        {STATE_Barcode_search,STATE_Plato,GO_TO_PLATO},             // Regresar a STATE_Plato si fue lastValidState y no había conexión a Internet, por lo que no se pudo buscar el producto.
                                        {STATE_Barcode_search,STATE_Grupo,GO_TO_GRUPO},             // Regresar a STATE_Grupo si fue lastValidState y no había conexión a Internet, por lo que no se pudo buscar el producto.
                                        {STATE_Barcode_search,STATE_Barcode,GO_TO_BARCODE},         // Regresar a STATE_Barcode si fue lastValidState y no había conexión a Internet, por lo que no se pudo buscar el producto.
                                        {STATE_Barcode_search,STATE_ERROR,ERROR},                   // Acción incorrecta ????? EL USUARIO NO DEBERÍA HACER NADA MIENTRAS SE BUSCA EL PRODUCTO
                                        // --------------------------

                                        // --- Confirmar producto ---
                                        {STATE_Barcode_check,STATE_Barcode,BARCODE},            // Producto confirmado pulsado botón barcode
                                        {STATE_Barcode_check,STATE_CANCEL,TIPO_A},              // Cancelar producto (no es el desado) pulsando botón de grupo tipoA.
                                        {STATE_Barcode_check,STATE_CANCEL,TIPO_B},              // Cancelar producto (no es el desado) pulsando botón de grupo tipoB.
                                        {STATE_Barcode_check,STATE_CANCEL,CRUDO},               // Cancelar producto (no es el desado) pulsando botón de CRUDO.
                                        {STATE_Barcode_check,STATE_CANCEL,COCINADO},            // Cancelar producto (no es el desado) pulsando botón de COCINADO.
                                        {STATE_Barcode_check,STATE_CANCEL,ADD_PLATO},           // Cancelar producto (no es el desado) pulsando botón de COCINADO.
                                        {STATE_Barcode_check,STATE_CANCEL,DELETE_PLATO},        // Cancelar producto (no es el desado) pulsando botón de BORRAR PLATO.
                                        {STATE_Barcode_check,STATE_CANCEL,GUARDAR},             // Cancelar producto (no es el desado) pulsando botón de GUARDAR.
                                        {STATE_Barcode_check,STATE_CANCEL,CANCELAR},            // Cancelar producto tras 20 segundos de inactividad.
                                        {STATE_Barcode_check,STATE_ERROR,ERROR},                // Acción incorrecta ????? Tocar la báscula (incremento, decremento o liberar) sería error, pero no debería ocurrir
                                        // --------------------------

                                        // --- Grupo Barcode --------
                                        {STATE_Barcode,STATE_Init,LIBERAR},                 // Se ha retirado el plato completo (+ recipiente) ==> Automáticamente se borra el plato, sin preguntar
                                        {STATE_Barcode,STATE_Barcode,DECREMENTO},           // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR)
                                        {STATE_Barcode,STATE_Grupo,TIPO_A},                 // Cambiar grupo alimentos con TIPO_A
                                        {STATE_Barcode,STATE_Grupo,TIPO_B},                 // Cambiar grupo alimentos con TIPO_B
                                        {STATE_Barcode,STATE_Barcode_read,BARCODE},         // Leer otro barcode (vuelve a comenzar)
                                        {STATE_Barcode,STATE_AVISO,CRUDO},                  // Aviso de "No hace falta crudo" porque el producto no necesita crudo
                                        {STATE_Barcode,STATE_AVISO,COCINADO},               // Aviso de "No hace falta cocinado" porque el producto no necesita cocinado
                                        {STATE_Barcode,STATE_weighted,INCREMENTO},          // Se ha colocado alimento del producto leído
                                        {STATE_Barcode,STATE_add_check,ADD_PLATO},          // Nuevo plato, aunque no se haya colocado alimento
                                        {STATE_Barcode,STATE_delete_check,DELETE_PLATO},    // Borrar plato actual
                                        {STATE_Barcode,STATE_save_check,GUARDAR},           // Guardar comida, aunque no se haya colocado alimento
                                        {STATE_Barcode,STATE_ERROR,ERROR},                  // Acción incorrecta ????? Parece que todas las posibles acciones del usuario están contempladas en las reglas de transición
                                        // --------------------------

                                        // --- Alimento crudo ---
                                        {STATE_raw,STATE_Init,LIBERAR},                 // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_raw,STATE_raw,DECREMENTO},               // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR)
                                        {STATE_raw,STATE_raw,TARAR},                    // Tarar tras pesar alimento y querer cambiar procesamiento, por lo que se ha debido de guardar el alimento pesado antes de cambiar procesamiento
                                        {STATE_raw,STATE_Grupo,TIPO_A},                 // Cambiar grupo alimentos
                                        {STATE_raw,STATE_Grupo,TIPO_B},                 // Cambiar grupo alimentos
                                        {STATE_raw,STATE_Barcode_read,BARCODE},         // Pulsado botón de barcode para iniciar lectura
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
                                        {STATE_cooked,STATE_cooked,TARAR},              // Tarar tras pesar alimento y querer cambiar procesamiento, por lo que se ha debido de guardar el alimento pesado antes de cambiar procesamiento
                                        {STATE_cooked,STATE_Grupo,TIPO_A},              // Cambiar grupo alimentos
                                        {STATE_cooked,STATE_Grupo,TIPO_B},              // Cambiar grupo alimentos
                                        {STATE_cooked,STATE_Barcode_read,BARCODE},      // Pulsado botón de barcode para iniciar lectura
                                        {STATE_cooked,STATE_cooked,COCINADO},           // Para que no dé error si se vuelve a pulsar 'cocinado'.
                                        {STATE_cooked,STATE_raw,CRUDO},                 // Cambiar procesamiento (cocinado => crudo).
                                        {STATE_cooked,STATE_weighted,INCREMENTO},       // Se ha colocado alimento.      
                                        {STATE_cooked,STATE_add_check,ADD_PLATO},       // Nuevo plato, aunque no se haya colocado alimento.
                                        {STATE_cooked,STATE_delete_check,DELETE_PLATO}, // Borrar plato actual. 
                                        {STATE_cooked,STATE_save_check,GUARDAR},        // Guardar comida, aunque no se haya colocado alimento.   
                                        {STATE_cooked,STATE_ERROR,ERROR},               // Acción incorrecta 
                                        // --------------------------

                                        // --- Alimento pesado ---
                                        {STATE_weighted,STATE_Init,LIBERAR},                // Se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
                                        {STATE_weighted,STATE_weighted,INCREMENTO},         // Se coloca más alimento.  
                                        {STATE_weighted,STATE_weighted,DECREMENTO},         // Se retira alimento.
                                        {STATE_weighted,STATE_Grupo,TIPO_A},                // Escoger nuevo grupo. Se guardará en STATE_Grupo el peso del alimento en báscula.
                                        {STATE_weighted,STATE_Grupo,TIPO_B},                // Escoger nuevo grupo. Se guardará en STATE_Grupo el peso del alimento en báscula.
                                        {STATE_weighted,STATE_raw,CRUDO},                   // Cambiar procesamiento (pesado => crudo).
                                        {STATE_weighted,STATE_cooked,COCINADO},             // Cambiar procesamiento (pesado => cocinado).
                                        {STATE_weighted,STATE_Barcode_read,BARCODE},        // Pulsado botón de barcode para iniciar lectura. Se guardará en STATE_Barcode el peso del alimento en báscula.
                                        {STATE_weighted,STATE_add_check,ADD_PLATO},         // Nuevo plato, aunque no se haya colocado alimento.
                                        {STATE_weighted,STATE_delete_check,DELETE_PLATO},   // Borrar plato actual. 
                                        {STATE_weighted,STATE_save_check,GUARDAR},          // Guardar comida, aunque no se haya colocado alimento. 
                                        {STATE_weighted,STATE_ERROR,ERROR},                 // Acción incorrecta ????? Parece que todas las posibles acciones del usuario están contempladas en las reglas de transición
                                        // -----------------------

                                        // --- Check añadir plato ---
                                        {STATE_add_check,STATE_added,ADD_PLATO},         // Confirmar añadir plato.
                                        {STATE_add_check,STATE_CANCEL,TIPO_A},           // Cancelar añadir plato pulsando botón de grupo tipoA.
                                        {STATE_add_check,STATE_CANCEL,TIPO_B},           // Cancelar añadir plato pulsando botón de grupo tipoB.
                                        {STATE_add_check,STATE_CANCEL,BARCODE},          // Cancelar añadir plato pulsando botón de BARCODE.
                                        {STATE_add_check,STATE_CANCEL,CRUDO},            // Cancelar añadir plato pulsando botón de CRUDO.
                                        {STATE_add_check,STATE_CANCEL,COCINADO},         // Cancelar añadir plato pulsando botón de COCINADO.
                                        {STATE_add_check,STATE_CANCEL,DELETE_PLATO},     // Cancelar añadir plato pulsando botón de BORRAR PLATO.
                                        {STATE_add_check,STATE_CANCEL,GUARDAR},          // Cancelar añadir plato pulsando botón de GUARDAR.
                                        {STATE_add_check,STATE_CANCEL,CANCELAR},         // Cancelar añadir plato tras 10 segundos de inactividad.
                                        {STATE_add_check,STATE_ERROR,ERROR},             // Acción incorrecta (solo salta si se manipula la báscula).
                                        // --------------------------

                                        // --- Plato añadido ---
                                        {STATE_added,STATE_added,TARAR},                        // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_added,STATE_added,DECREMENTO},                   // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR).
                                        {STATE_added,STATE_added,INCREMENTO},                   // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_added,STATE_Init,LIBERAR},                       // Se ha retirado el plato completo (+ recipiente) tras añadir uno nuevo.
                                        {STATE_added,STATE_ERROR,ERROR},                        // Acción incorrecta
                                        {STATE_added,STATE_AVISO,AVISO_PLATO_EMPTY_NOT_ADDED},  // No se ha añadido un plato porque el actual está vacío
                                        // ---------------------

                                        // --- Check eliminar plato ---
                                        {STATE_delete_check,STATE_deleted,DELETE_PLATO},    // Confirmar eliminar plato.
                                        {STATE_delete_check,STATE_CANCEL,TIPO_A},           // Cancelar eliminar plato pulsando botón de grupo tipoA.
                                        {STATE_delete_check,STATE_CANCEL,TIPO_B},           // Cancelar eliminar plato pulsando botón de grupo tipoB.
                                        {STATE_delete_check,STATE_CANCEL,BARCODE},          // Cancelar eliminar plato pulsando botón de BARCODE.
                                        {STATE_delete_check,STATE_CANCEL,CRUDO},            // Cancelar eliminar plato pulsando botón de CRUDO.
                                        {STATE_delete_check,STATE_CANCEL,COCINADO},         // Cancelar eliminar plato pulsando botón de COCINADO.
                                        {STATE_delete_check,STATE_CANCEL,ADD_PLATO},        // Cancelar eliminar plato pulsando botón de AÑADIR PLATO.
                                        {STATE_delete_check,STATE_CANCEL,GUARDAR},          // Cancelar eliminar plato pulsando botón de GUARDAR.
                                        {STATE_delete_check,STATE_CANCEL,CANCELAR},         // Cancelar eliminar plato tras 10 segundos de inactividad.
                                        {STATE_delete_check,STATE_ERROR,ERROR},             // Acción incorrecta (solo salta si se manipula la báscula).
                                        // --------------------------

                                        // --- Plato eliminado ---
                                        {STATE_deleted,STATE_deleted,TARAR},                        // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_deleted,STATE_deleted,DECREMENTO},                   // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR).
                                        {STATE_deleted,STATE_deleted,INCREMENTO},                   // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_deleted,STATE_Init,LIBERAR},                         // Se ha retirado el plato completo (+ recipiente) tras borrar. 
                                        {STATE_deleted,STATE_ERROR,ERROR},                          // Acción incorrecta
                                        {STATE_deleted,STATE_AVISO,AVISO_PLATO_EMPTY_NOT_DELETED},  // No se ha eliminado el plato porque está vacío
                                        // -----------------------

                                        // --- Check guardar comida ---
                                        {STATE_save_check,STATE_saved,GUARDAR},          // Confirmar guardar comida.
                                        {STATE_save_check,STATE_CANCEL,TIPO_A},          // Cancelar guardar comida pulsando botón de grupo tipoA.
                                        {STATE_save_check,STATE_CANCEL,TIPO_B},          // Cancelar guardar comida pulsando botón de grupo tipoB.
                                        {STATE_save_check,STATE_CANCEL,BARCODE},         // Cancelar guardar comida pulsando botón de BARCODE.
                                        {STATE_save_check,STATE_CANCEL,CRUDO},           // Cancelar guardar comida pulsando botón de CRUDO.
                                        {STATE_save_check,STATE_CANCEL,COCINADO},        // Cancelar guardar comida pulsando botón de COCINADO.
                                        {STATE_save_check,STATE_CANCEL,ADD_PLATO},       // Cancelar guardar comida pulsando botón de AÑADIR PLATO.
                                        {STATE_save_check,STATE_CANCEL,DELETE_PLATO},    // Cancelar guardar comida pulsando botón de BORRAR PLATO.
                                        {STATE_save_check,STATE_CANCEL,CANCELAR},        // Cancelar guardar comida tras 10 segundos de inactividad.
                                        {STATE_save_check,STATE_ERROR,ERROR},            // Acción incorrecta (solo salta si se manipula la báscula).
                                        // --------------------------

                                        // --- Comida guardada ---
                                        {STATE_saved,STATE_saved,TARAR},                        // Taramos para saber (en negativo) cuánto se va quitando al retirar el plato para LIBERAR.
                                        {STATE_saved,STATE_saved,DECREMENTO},                   // Para evitar error de evento cuando pase por condiciones que habilitan DECREMENTO (previo a LIBERAR).
                                        {STATE_saved,STATE_saved,INCREMENTO},                   // Para evitar error de evento cuando, al retirar el plato, pueda detectar un ligero incremento.
                                        {STATE_saved,STATE_Init,LIBERAR},                       // Se ha retirado el plato completo (+ recipiente) tras guardar correctamente.  
                                        {STATE_saved,STATE_Init,GO_TO_INIT},                    // Se regresa automáticamente a STATE_Init tras guardar la comida
                                        {STATE_saved,STATE_ERROR,ERROR},                        // Acción incorrecta
                                        {STATE_saved,STATE_AVISO,AVISO_COMIDA_EMPTY_NOT_SAVED}, // No se ha guardado la comida porque está vacía
                                        // -----------------------


                                        // --- ERROR DE EVENTO ---
                                        {STATE_ERROR,STATE_Init,GO_TO_INIT},                    // Regresar a STATE_Init tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_Plato,GO_TO_PLATO},                  // Regresar a STATE_Plato tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_Grupo,GO_TO_GRUPO},                  // Regresar a STATE_Grupo tras mostrar error cometido allí
                                        {STATE_ERROR,STATE_Barcode_read,GO_TO_BARCODE_READ},    // Regresar a STATE_Barcode_read tras mostrar error cometido allí o en STATE_Grupo pero se pulsa botón de barcode durante error
                                        {STATE_ERROR,STATE_Barcode,GO_TO_BARCODE},              // Regresar a STATE_Barcode tras mostrar error cometido allí
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
                                        {STATE_CANCEL,STATE_Plato,GO_TO_PLATO},         // Regresar a STATE_Plato tras cancelar acción de leer barcode iniciada desde STATE_Plato
                                        {STATE_CANCEL,STATE_Grupo,GO_TO_GRUPO},         // Regresar a STATE_Grupo tras cancelar add/delete/save o leer barcode iniciada desde STATE_Grupo
                                        {STATE_CANCEL,STATE_Barcode,GO_TO_BARCODE},     // Regresar a STATE_Barcode tras cancelar add/delete/save o leer otro barcode iniciada desde STATE_Barcode
                                        {STATE_CANCEL,STATE_raw,GO_TO_RAW},             // Regresar a STATE_raw tras cancelar add/delete/save o leer barcode iniciada desde STATE_raw   
                                        {STATE_CANCEL,STATE_cooked,GO_TO_COOKED},       // Regresar a STATE_cooked tras cancelar add/delete/save o leer barcode iniciada desde STATE_cooked
                                        {STATE_CANCEL,STATE_weighted,GO_TO_WEIGHTED},   // Regresar a STATE_weighted tras cancelar add/delete/save o leer barcode iniciada desde STATE_weighted
                                        {STATE_CANCEL,STATE_ERROR,ERROR},               // Acción incorrecta. No suele ocurrir, pero si ocurre y no se gestiona, se quedaría 
                                                                                        // en bucle marcando error.    

                                        {STATE_CANCEL,STATE_DELETE_CSV_CHECK,DELETE_CSV},  // INICIAR BORRADO DE FICHERO CSV. EL USUARIO NO DEBERÍA ACTIVAR EL EVENTO. SOLO PARA LAS PRUEBAS.
                                        // -----------------------


                                        // --- AVISO -------------
                                        {STATE_AVISO,STATE_Init,GO_TO_INIT},            // Regresar a Init tras 3 segundos de warning
                                        {STATE_AVISO,STATE_Plato,GO_TO_PLATO},          // Regresar a STATE_Plato tras 3 segundos de warning
                                        {STATE_AVISO,STATE_Grupo,GO_TO_GRUPO},          // Regresar a STATE_Grupo tras 3 segundos de warning
                                        {STATE_AVISO,STATE_Barcode,GO_TO_BARCODE},      // Regresar a GO_TO_BARCODE tras 3 segundos de warning por intentar marcar crudo/cocinado
                                        {STATE_AVISO,STATE_ERROR,ERROR},                // Acción incorrecta durante aviso. Cualquier cosa es acción incorrecta. Igual que en Cancelar.
                                        // -----------------------


                                         // --- CHECK DELETE CSV ---
                                        {STATE_DELETE_CSV_CHECK,STATE_DELETE_CSV_CHECK,TIPO_A},   // Entrar de nuevo a DELETE_CSV_CHECK y entonces confirmar o no el borrado según el grupo pulsado
                                        {STATE_DELETE_CSV_CHECK,STATE_DELETE_CSV_CHECK,TIPO_B},
                                        {STATE_DELETE_CSV_CHECK,STATE_DELETED_CSV,DELETE_CSV},    // CONFIRMAR BORRAR CSV
                                        {STATE_DELETE_CSV_CHECK,STATE_Init,GO_TO_INIT},           // Regresar a STATE_Init tras cancelar (5 seg) el BORRAR CSV iniciado desde STATE_Init
                                        {STATE_DELETE_CSV_CHECK,STATE_raw,GO_TO_RAW},             // Regresar a STATE_raw tras cancelar (5 seg) el BORRAR CSV iniciado desde STATE_raw   
                                        {STATE_DELETE_CSV_CHECK,STATE_cooked,GO_TO_COOKED},       // Regresar a STATE_cooked tras cancelar (5 seg) el BORRAR CSV iniciado desde STATE_cooked
                                        {STATE_DELETE_CSV_CHECK,STATE_weighted,GO_TO_WEIGHTED},   // Regresar a STATE_weighted tras cancelar (5 seg) el BORRAR CSV iniciado desde STATE_weighted
                                        {STATE_DELETE_CSV_CHECK,STATE_CANCEL,CANCELAR},           // Cancelar manualmente el BORRAR CSV pulsando cualquier botón distinto del grupo 20
                                        // --------------------------

                                        // --- FICHERO CSV BORRADO ---
                                        {STATE_DELETED_CSV,STATE_Init,GO_TO_INIT},         // Regresar a STATE_Init tras BORRAR CSV (siempre iniciado desde STATE_Init)
                                        // ---------------------

                                        // --- UPLOAD DATA TO DATABASE ---
                                        {STATE_UPLOAD_DATA,STATE_Init,GO_TO_INIT}          // Regresar a STATE_Init tras subir o no la info acumulada
                                        // ---------------------
                                      };





/*----------------------------------------------------------------------------------------------*/
/*------------------------------ VARIABLES ESTADOS/EVENTOS -------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

// ---- VARIABLES DE ESTADOS ---------------------------------------------------------
state_t state_actual;          // Estado actual
state_t state_new;             // Nuevo estado al que se va a pasar
state_t state_prev;            // Estado anterior
state_t lastValidState;        // Último estado válido (Init, Plato, Grupo, raw, cooked, weighted)
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

bool    showingTemporalScreen   = false;    // Flag para saber que se está mostrando una pantalla temporal o auxiliar

bool    flagComidaSaved         = false;    // Flag para saber que se ha guardado la comida
bool    flagFicheroCSVBorrado   = false;    // Flag para saber que se ha borrado el fichero csv
bool    flagRecipienteRetirado  = false;    // Flag para saber que se ha retirado el plato completo
// ------ FIN VARIABLES DE EVENTOS ----------------------------------------------------


// ------ PROCESAMIENTO ---------------------------------------------------------------
/**
 * @enum procesamiento
 * @brief Enumeración de los diferentes valores del procesamiento del alimento.
 */
typedef enum 
{
              ALIMENTO_CRUDO      =   (1),   /**< Alimento crudo  */
              ALIMENTO_COCINADO   =   (2)    /**< Alimento cocinado  */
} procesado_t;

procesado_t procesamiento; // Solo se utiliza para saber qué icono (crudo o cocinado) mostrar en la zona 2 del dashboard 
// ------ FIN PROCESAMIENTO -----------------------------------------------------------

// ------ CODIGO DE BARRAS -------------------------------------------------------------
// Global para que puedan acceder STATE_Grupo, STATE_Barcode, STATE_added y STATE_saved
String barcode;      // Código de barras leído 
String productInfo;  // Información del producto obtenida de la base de datos
// ------ FIN CODIGO DE BARRAS ---------------------------------------------------------



#include "Screen.h"   // Debajo de las variables para que estén disponibles en su ámbito




/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/

/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
#if defined(SM_DEBUG)
void printEventName(event_t event);
void printStateName(state_t state);
#endif
// --- Reglas transición ---
bool    checkStateConditions();                         // Comprobar reglas de transición de estados

// --- Actividades por estado ---
void    actStateInit();                                 // Actividades STATE_Init
void    actStatePlato();                                // Actividades STATE_Plato
void    actGruposAlimentos();                           // Actividades STATE_Grupo
void    actStateBarcodeRead();                          // Actividades STATE_Barcode_read
void    actStateBarcodeSearch();                        // Actividades STATE_Barcode_search
void    actStateBarcodeCheck();                         // Actividades STATE_Barcode_check
void    actStateBarcode();                              // Actividades STATE_Barcode
void    actStateRaw();                                  // Actividades STATE_raw
void    actStateCooked();                               // Actividades STATE_cooked
void    actStateWeighted();                             // Actividades STATE_weighted
void    actStateAddCheck();                             // Actividades STATE_add_check
void    actStateAdded();                                // Actividades STATE_added
void    actStateDeleteCheck();                          // Actividades STATE_delete_check
void    actStateDeleted();                              // Actividades STATE_deleted
void    actStateSaveCheck();                            // Actividades STATE_save_check
void    actStateSaved();                                // Actividades STATE_saved
void    actStateERROR();                                // Actividades STATE_ERROR
void    actStateCANCEL();                               // Actividades STATE_CANCEL
void    actStateAVISO();                                // Actividades STATE_AVISO

void    actState_DELETE_CSV_CHECK();                    // Actividades STATE_DELETE_CSV_CHECK
void    actState_DELETED_CSV();                         // Actividades STATE_DELETED_CSV

void    actState_CRITIC_FAILURE_SD();                   // Actividades STATE_CRITIC_FAILURE_SD
void    actState_UPLOAD_DATA();                         // Actividades STATE_UPLOAD_DATA

// --- Actividades estado actual ---
void    doStateActions();                               // Actividades según estado actual

// --- Error de evento ---
void    actEventError();                                // Mensaje de error de evento según el estado actual

// --- Buffer de eventos ---
bool    isBufferEmpty();                                 // Comprobar buffer de eventos vacío
bool    isBufferFull();                                 // Comprobar buffer de eventos lleno
byte    getFirstGapBuffer();                            // Obtener primer hueco en el buffer
void    shiftLeftEventBuffer();                         // Desplazar buffer a izquierda para incluir nuevo evento
void    addEventToBuffer(event_t evento);               // Añadir evento al buffer
/******************************************************************************/
/******************************************************************************/




/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/



/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------- NOMBRES DE EVENTOS/ESTADOS -----------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/ 

/*---------------------------------------------------------------------------------------------------------
   printEventName(): Imprime el nombre del evento cuyo ID se pasa como argumento
          Parámetros: 
                  event - ID del evento 
----------------------------------------------------------------------------------------------------------*/
#if defined(SM_DEBUG)
void printEventName(event_t event) 
{
    switch (event) 
    {
        case NONE:                              SerialPC.print(F("NONE"));                              break;
        case TIPO_A:                            SerialPC.print(F("TIPO_A"));                            break;
        case TIPO_B:                            SerialPC.print(F("TIPO_B"));                            break;
        case BARCODE:                           SerialPC.print(F("BARCODE"));                           break;
        case CRUDO:                             SerialPC.print(F("CRUDO"));                             break;
        case COCINADO:                          SerialPC.print(F("COCINADO"));                          break;
        case ADD_PLATO:                         SerialPC.print(F("ADD_PLATO"));                         break;
        case DELETE_PLATO:                      SerialPC.print(F("DELETE_PLATO"));                      break;
        case GUARDAR:                           SerialPC.print(F("GUARDAR"));                           break;
        case INCREMENTO:                        SerialPC.print(F("INCREMENTO"));                        break;
        case DECREMENTO:                        SerialPC.print(F("DECREMENTO"));                        break;
        case TARAR:                             SerialPC.print(F("TARAR"));                             break;
        case LIBERAR:                           SerialPC.print(F("LIBERAR"));                           break;
        case ERROR:                             SerialPC.print(F("ERROR"));                             break;
        case CANCELAR:                          SerialPC.print(F("CANCELAR"));                          break;
        case AVISO_PLATO_EMPTY_NOT_ADDED:       SerialPC.print(F("AVISO_PLATO_EMPTY_NOT_ADDED"));       break;
        case AVISO_PLATO_EMPTY_NOT_DELETED:     SerialPC.print(F("AVISO_PLATO_EMPTY_NOT_DELETED"));     break;
        case AVISO_COMIDA_EMPTY_NOT_SAVED:      SerialPC.print(F("AVISO_COMIDA_EMPTY_NOT_SAVED"));      break;
        case AVISO_NO_BARCODE:                  SerialPC.print(F("AVISO_NO_BARCODE"));                  break;
        case AVISO_NO_WIFI_BARCODE:             SerialPC.print(F("AVISO_NO_WIFI_BARCODE"));             break;
        case AVISO_PRODUCT_NOT_FOUND:           SerialPC.print(F("AVISO_PRODUCT_NOT_FOUND"));           break;
        case GO_TO_INIT:                        SerialPC.print(F("GO_TO_INIT"));                        break;
        case GO_TO_PLATO:                       SerialPC.print(F("GO_TO_PLATO"));                       break;
        case GO_TO_GRUPO:                       SerialPC.print(F("GO_TO_GRUPO"));                       break;
        case GO_TO_BARCODE_READ:                SerialPC.print(F("GO_TO_BARCODE_READ"));                break;
        case GO_TO_BARCODE:                     SerialPC.print(F("GO_TO_BARCODE"));                     break;
        case GO_TO_RAW:                         SerialPC.print(F("GO_TO_RAW"));                         break;
        case GO_TO_COOKED:                      SerialPC.print(F("GO_TO_COOKED"));                      break;
        case GO_TO_WEIGHTED:                    SerialPC.print(F("GO_TO_WEIGHTED"));                    break;
        case GO_TO_ADD_CHECK:                   SerialPC.print(F("GO_TO_ADD_CHECK"));                   break;
        case GO_TO_ADDED:                       SerialPC.print(F("GO_TO_ADDED"));                       break;
        case GO_TO_DELETE_CHECK:                SerialPC.print(F("GO_TO_DELETE_CHECK"));                break;
        case GO_TO_DELETED:                     SerialPC.print(F("GO_TO_DELETED"));                     break;
        case GO_TO_SAVE_CHECK:                  SerialPC.print(F("GO_TO_SAVE_CHECK"));                  break;
        case GO_TO_SAVED:                       SerialPC.print(F("GO_TO_SAVED"));                       break;
        case GO_TO_CANCEL:                      SerialPC.print(F("GO_TO_CANCEL"));                      break;
        case DELETE_CSV:                        SerialPC.print(F("DELETE_CSV"));                        break;
        
        default:                                SerialPC.print(F("Evento desconocido"));                break;
    }
}
#endif //SM_DEBUG

/*---------------------------------------------------------------------------------------------------------
   printStateName(): Imprime el nombre del estado cuyo ID se pasa como argumento
          Parámetros: 
                  state - ID del estado 
----------------------------------------------------------------------------------------------------------*/
#if defined(SM_DEBUG)
void printStateName(state_t state) 
{
    switch (state) 
    {
        case STATE_Init:                SerialPC.print(F("STATE_Init"));                 break;
        case STATE_Plato:               SerialPC.print(F("STATE_Plato"));                break;
        case STATE_Grupo:               SerialPC.print(F("STATE_Grupo"));                break;
        case STATE_Barcode_read:        SerialPC.print(F("STATE_Barcode_read"));         break;
        case STATE_Barcode_search:      SerialPC.print(F("STATE_Barcode_search"));       break;
        case STATE_Barcode_check:       SerialPC.print(F("STATE_Barcode_check"));        break;
        case STATE_Barcode:             SerialPC.print(F("STATE_Barcode"));              break;
        case STATE_raw:                 SerialPC.print(F("STATE_raw"));                  break;
        case STATE_cooked:              SerialPC.print(F("STATE_cooked"));               break;
        case STATE_weighted:            SerialPC.print(F("STATE_weighted"));             break;
        case STATE_add_check:           SerialPC.print(F("STATE_add_check"));            break;
        case STATE_added:               SerialPC.print(F("STATE_added"));                break;
        case STATE_delete_check:        SerialPC.print(F("STATE_delete_check"));         break;
        case STATE_deleted:             SerialPC.print(F("STATE_deleted"));              break;
        case STATE_save_check:          SerialPC.print(F("STATE_save_check"));           break;
        case STATE_saved:               SerialPC.print(F("STATE_saved"));                break;
        case STATE_ERROR:               SerialPC.print(F("STATE_ERROR"));                break;
        case STATE_CANCEL:              SerialPC.print(F("STATE_CANCEL"));               break;
        case STATE_AVISO:               SerialPC.print(F("STATE_AVISO"));                break;
        case STATE_DELETE_CSV_CHECK:    SerialPC.print(F("STATE_DELETE_CSV_CHECK"));     break;
        case STATE_DELETED_CSV:         SerialPC.print(F("STATE_DELETED_CSV"));          break;
        case STATE_CRITIC_FAILURE_SD:   SerialPC.print(F("STATE_CRITIC_FAILURE_SD"));    break;
        case STATE_UPLOAD_DATA:         SerialPC.print(F("STATE_UPLOAD_DATA"));          break;
        
        default:                        SerialPC.print(state); SerialPC.print(F(" | Estado desconocido"));         break;
    }
}
#endif //SM_DEBUG





/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- MOTOR INFERENCIA -----------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/ 

/*---------------------------------------------------------------------------------------------------------
   checkStateConditions(): Comprobar reglas de transición para conocer el próximo estado
----------------------------------------------------------------------------------------------------------*/
bool checkStateConditions()
{
    for (byte i = 0; i < RULES; i++) // Si hay más de 255 reglas, cambiar byte --> int        
    {
        if(rules[i].state_i == state_actual)
        {
            if(rules[i].condition == lastEvent)
            {    
                state_new = rules[i].state_j;     // Nuevo estado
                doneState = false;                // Desactivar flag de haber hecho las actividades del estado
                return true;                        
            }
        }
    }
    return false;   // Si no se ha cumplido ninguna regla de transición ==> ERROR DE EVENTO
}




/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- ACCIONES ESTADOS -----------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/




/*---------------------------------------------------------------------------------------------------------
   actStateInit(): Acciones del STATE_Init
----------------------------------------------------------------------------------------------------------*/
void actStateInit()
{ 
    // Tiempos utilizados para alternar entre dashboard y pantalla de pedir recipiente:

    const unsigned long recipienteRetiradoInterval  = 1000; // Intervalo de tiempo para mostrar "Recipiente retirado" (1 segundo)
    unsigned long dashboardInterval = 10000;                // Intervalo de tiempo para mostrar el dashboard (10 segundos) 
    const unsigned long recipienteInterval = 5000;          // Intervalo de tiempo para pedir colocar recipiente (5 segundos)
    

    static unsigned long previousTime;              // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_recipiente_retirado;  
    static bool showing_dash;       
    static bool showing_pedir_recipiente;

    
    if(!doneState)
    {

        // ----- ENCENDER SM CON RECIPIENTE EN BÁSCULA --------------------------------------
        if ((state_prev == STATE_Init) && ((lastEvent == DECREMENTO) || (lastEvent == LIBERAR)))    
        {                                                               // Es posible que se encienda SM con un recipiente ya puesto, por lo que el sistema establecería ese peso
                                                                        // como peso 0 al tarar. Además, se pediría colocar el recipiente y la respuesta más común del usuario
                                                                        // sería retirar el recipiente y volverlo a poner. Por tanto, se ha incluido una regla para volver a 
                                                                        // STATE_Init si se decrementa el peso. También se ha añadido una regla para volver a STATE_Init si se
                                                                        // libera la báscula, evento que se detectaría si el recipiente que hubiera colocado al encender pesara
                                                                        // menos de 20 gramos (UMBRAL_RECIPIENTE_RETIRADO).
                                                                        // Si esto ocurre, se debe volver a tarar la báscula y reiniciar las variables de peso referentes al 
                                                                        // recipiente, al plato (recipiente + alimentos) y al último alimento.

            tareScale();        // Tarar báscula
            reiniciarPesos();   // Poner a 0 los valores de 'pesoRecipiente'. 'pesoPlato' y 'pesoLastAlimento'.
        }
        // ----------------------------------------------------------------------------------

        else if(state_prev != STATE_Init)
        { 
            #if defined(SM_DEBUG)
                SerialPC.println(F("\nSTATE_Init...")); 
            #endif

            // ----- INICIAR COMIDA --------------------------------------
            // Si lo último escrito no es INICIO-COMIDA (la lista está vacía), 
            // se añade a la lista "INICIO-COMIDA"
            listaComidaESP32.iniciarComida();
            #if defined(SM_DEBUG)
                listaComidaESP32.leerLista();
            #endif
            // -----------------------------------------------------------

            // ----- RESETEAR COPIA SI SE HA BORRADO CSV -----------------
            if(flagFicheroCSVBorrado)
            {
                comidaActualCopia.restoreComida();   
                flagFicheroCSVBorrado = false;
                flagComidaSaved = false; // Por si he hecho el borrado justo tras guardar
            } 
            // -----------------------------------------------------------


            tareScale();        // Tarar báscula
            reiniciarPesos();   // Poner a 0 los valores de 'pesoRecipiente'. 'pesoPlato' y 'pesoLastAlimento'.

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
            if(!platoActual.isPlatoEmpty())
            { 
                // ----- BORRAR PLATO --------------------------------------
                // Eliminar info del plato de la lista
                listaComidaESP32.borrarLastPlato();
                #if defined(SM_DEBUG)
                    listaComidaESP32.leerLista();
                #endif
                // -----------------------------------------------------------

                comidaActual.deletePlato(platoActual);    // Borrar plato actual de la comida
                platoActual.restorePlato();               // Restaurar objeto 'platoActual'
            }
            // ----- FIN BORRAR PLATO ---------------------------------


            // ----- INFO INICIAL DE PANTALLA -------------------------
            if(flagRecipienteRetirado)
            {
                recipienteRetirado();                 // Mostrar "Recipiente retirado" tras LIBERAR báscula
                showing_recipiente_retirado = true;   // Se está mostrando "Recipiente retirado"
                showing_dash = false;      
                showing_pedir_recipiente = false;

                flagRecipienteRetirado = false;        // Reiniciar flag de recipiente retirado
            }
            else
            { 
                showDashboardStyle1(MSG_SIN_RECIPIENTE); // Mostrar dashboard al inicio con mensaje de que falta recipiente
                showing_dash = true;                     // Se está mostrando dashboard estilo 1 (Comida | Acumulado)
                showing_pedir_recipiente = false;   
                showing_recipiente_retirado = false;
            }
            // ----- FIN INFO INICIAL DE PANTALLA ---------------------
            
            
        }

        previousTime = millis();            // Inicializar 'previousTime' para la alternancia de pantallas

        doneState = true;                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }



    // ----- ALTERNANCIA PANTALLAS -------------------------
    currentTime = millis();
    if(showing_recipiente_retirado) // Mostrando "Recipiente retirado". Solo aparece tras LIBERAR báscula.
    {
        if (currentTime - previousTime >= recipienteRetiradoInterval)  // Si el "Recipiente retirado" ha estado 1 segundo, se cambia a dashboard estilo 1
        {
            previousTime = currentTime;
            showDashboardStyle1(MSG_SIN_RECIPIENTE);
            showing_recipiente_retirado = false; // Dejar de mostrar "Recipiente retirado". Solo aparece tras LIBERAR báscula.
            showing_dash = true;                  // Mostrando dashboard estilo 1 (Comida | Acumulado)
            showing_pedir_recipiente = false;
        }
    }
    else // Ya no se muestra "Recipiente retirado"
    {
        if(showing_dash) // Se está mostrando dashboard estilo 1 (Comida | Acumulado)
        {
            blinkGrupoyProcesamiento(MSG_SIN_RECIPIENTE);
            if(!flagComidaSaved)  // Si no se acaba de guardar comida, se pide colocar recipiente.
            {                     //    Si se acabara de guarda la comida, se dejaría el dashboard estático para que puedan consultar la 
                                  //    información guardada en "Comida guardada".
                if (currentTime - previousTime >= dashboardInterval)  // Si el dashboard ha estado 10 segundos, se cambia a colocar recipiente
                {
                    previousTime = currentTime;
                    pedirRecipiente();
                    showing_dash = false;  
                    showing_pedir_recipiente = true; 
                }
            }
        }
        else if(showing_pedir_recipiente) // Se está mostrando colocar recipiente
        {
            if (currentTime - previousTime >= recipienteInterval)  // Si el colocar recipiente ha estado 5 segundos, se cambia a dashboard estilo 1
            {
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
void actStatePlato()
{ 
    // Tiempos utilizados para alternar entre recipiente colocado, dashboard y pantalla de escoger grupo:

    const unsigned long recipienteColocadoInterval  = 1000;   // Intervalo de tiempo para mostrar "Recipiente colocado" (1 segundo)
    const unsigned long dashboardInterval = 3000;             // Intervalo de tiempo para mostrar el dashboard (3 segundos)
    const unsigned long grupoInterval = 5000;                 // Intervalo de tiempo para pedir escoger grupo (5 segundos)
                                                              //      En realidad este grupoInterval no afecta mientras se mantenga por debajo de lo que 
                                                              //      tarda en completarse la pantalla de pedirGrupo. Esta pantalla tarda unos 12 seg,
                                                              //      pero al regresar a actStatePlato() ya habrán pasado los 5 seg de este intervalo.

    static unsigned long previousTime;                        // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_recipiente_colocado;  
    static bool showing_dash;      
    static bool showing_escoger_grupo;

    // El peso definitivo del recipiente no se guarda hasta que se escoja grupo de alimentos.
    // Por eso, 'pesoRecipiente' y 'pesoARetirar' son iguales a 0.0 en este estado. Esto no impide que se
    // realice LIBERAR, ya que ese evento ocurre cuando el peso de la báscula es igual a 'pesoARetirar'. 
    // Así, se puede detectar que se ha quitado el recipiente (bascula = 0.0) y se vuelve al estado STATE_Init.   

    if(!doneState)
    {
        if(state_prev != STATE_Plato)  // Solo hacer esta parte la primera vez que se detecta peso en báscula (plato).
        {                              // Si recoloca, provocando INCREMENTO o DECREMENTO, no hace falta repetir esta parte.
            if(lastValidState == STATE_Init) // Si se viene de Init o a través de un error (se colocó el plato durante el error), realmente se acaba de colocar plato. 
            {                               // Así no se inicia plato en la lista cuando se regrese a STATE_Plato tras error o cancelación
                #if defined(SM_DEBUG)
                    SerialPC.println(F("\nPlato colocado")); 
                #endif

                // ----- INICIAR PLATO --------------------------------------
                // Si lo último escrito no es INICIO-PLATO (la comida está vacía o se está empezando otro plato), 
                // se añade a la lista "INICIO-PLATO"
                listaComidaESP32.iniciarPlato();
                #if defined(SM_DEBUG)
                    listaComidaESP32.leerLista();
                #endif 
                // -----------------------------------------------------------

                // ----- REINICIAR COPIA DE COMIDA GUARDADA -------------------------
                // Si se acaba de guardar la comida, ya se ha mostrado la copia de su info en STATE_Init. Entonces, si la comida real
                // está vacía porque se ha reiniciado tras guardarla, al colocar recipiente se entiende que va a comenzar una nueva,
                // por lo que se debe reiniciar también la copia.
                // De esta forma, si se retira el plato y se regresa a Init, ya no vuelve a aparecer la info de la comida guardada antes, 
                // sino que ya se está haciendo la nueva comida, que aún está a 0.
                // 
                if((comidaActual.isComidaEmpty() == true) and (comidaActualCopia.isComidaEmpty() == false)) comidaActualCopia.restoreComida();   

                if(flagComidaSaved) flagComidaSaved = false;  // Si se había guardado comida, se muestra "Comida guardada" en lugar de "Comida actual" en printZona3()
                                                            // en STATE_Init, por eso ahora se resetea la flag.
                // ----- FIN REINICIAR COPIA DE COMIDA GUARDADA ----------------------

                // ----- INFO INICIAL DE PANTALLA -------------------------
                recipienteColocado();                 // Mostrar "Recipiente colocado" una vez al inicio. No volverlo a mostrar si se comete error en STATE_Plato.
                showing_recipiente_colocado = true;   // Se está mostrando "Recipiente colocado"
                showing_dash = false;      
                showing_escoger_grupo = false;
            }
            else // Una vez colocado el recipiente, mostrar dashboard completo
            {
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
    if(showing_recipiente_colocado) // Mostrando "Recipiente colocado". Solo aparece una vez al inicio y si no se viene de error.
    {
        if (currentTime - previousTime >= recipienteColocadoInterval)  // Si el "Recipiente colocado" ha estado 1 segundo, se cambia a dashboard estilo 1
        {
            previousTime = currentTime;
            showDashboardStyle1(MSG_SIN_GRUPO);
            showing_recipiente_colocado = false; // Dejar de mostrar "Recipiente colocado". Solo aparece una vez al inicio.
            showing_dash = true;                  // Mostrando dashboard estilo 1 (Comida | Acumulado)
            showing_escoger_grupo = false;
        }
    }
    else // Ya no se muestra "Recipiente colocado"
    {
        if(showing_dash) // Se está mostrando dashboard estilo 1 (Comida | Acumulado)
        {
            blinkGrupoyProcesamiento(MSG_SIN_GRUPO);
            if (currentTime - previousTime >= dashboardInterval)  // Si el dashboard ha estado 10 segundos, se cambia a escoger grupo
            {
                previousTime = currentTime;
                pedirGrupoAlimentos();
                showing_dash = false;  
                showing_escoger_grupo = true; // Mostrando escoger grupo
            }
        }
        else if(showing_escoger_grupo) // Se está mostrando escoger grupo
        {
            if (currentTime - previousTime >= grupoInterval)  // Si el escoger grupo ha estado 5 segundos, se cambia a dashboard estilo 1
            {
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
   actGruposAlimentos(): Acciones del STATE_Grupo
----------------------------------------------------------------------------------------------------------*/
void actGruposAlimentos()
{ 

    if(!doneState)
    {      
        // Primera vez que se pulsa un grupo de alimentos
        if(state_prev != STATE_Grupo)   // ==> Si no se viene del propio STATE_Grupo, para evitar que se vuelva a guardar el alimento de la iteración anterior
                                        // en la comida y modificar todo el dashboard, sino solo la parte de ejemplos. 
        {                               // Se vendría de STATE_Grupo si al retirar el plato se detectara DECREMENTO antes de LIBERAR, que ya llevaría a STATE_Init.

            #if defined(SM_DEBUG)
                SerialPC.println(F("\nGrupo de alimentos..."));
            #endif
            
            // ----- ACCIONES PREVIAS ----------------------
            if((lastValidState == STATE_Plato) && (pesoBascula != 0.0)) // Si se acaba de colocar el recipiente. Comprobamos que no sea 0.0 el pesoBascula por si acaso
            {
                #if defined(SM_DEBUG)
                    SerialPC.println(F("\nGuardando peso recipiente..."));
                #endif
                pesoRecipiente = pesoBascula;    // Se guarda 'pesoRecipiente' para sumarlo a 'pesoPlato' y saber el 'pesoARetirar'.
                tareScale();                     // Se tara la báscula, preparándola para el primer alimento
            }

            // Se comprueba no venir del propio STATE_Grupo por si se ha retirado el plato, se detecta DECREMENTO (previo a LIBERAR) y se entra 
            // de nuevo en STATE_Grupo, para que no se vuelva a guardar el alimento que ya se guardó en la iteración anterior. Al retirar el plato 
            // se pasa por STATE_Grupo si no se detecta LIBERAR directamente sino que antes se detecta DECREMENTO.
            // Creo que no basta con solo comprobar que pesoBascula != 0.0 porque al retirar el plato el 'pesoBascula' pasará a ser negativo
            // tras haber tarado en la iteración anterior al entrar en STATE_Grupo, por lo que se estaría guardando otra vez el alimento con peso negativo.
            // Lo raro es que hasta ahora (17/09/24) no tenía esta comprobación y parecía funcionar bien, pero por si acaso. 
            //else if ((state_prev != STATE_Grupo) && (pesoBascula != 0.0))
            else if (pesoBascula != 0.0)
            {
                    // ----- AÑADIR ALIMENTO A LISTA -----------------------------
                    if(grupoAnterior.ID_grupo != BARCODE_PRODUCT_INDEX)  // Si el grupo anterior del alimento pesado es de los nuestros, se escribe ALIMENTO,<grupo>,<peso> en la lista, 
                    {                                 // siendo <grupo> el ID de 'grupoAnterior' y <peso> el valor de 'pesoBascula'.
                        listaComidaESP32.addAlimento(grupoAnterior.ID_grupo, pesoBascula);
                    }
                    else  // Si el grupo anterior es un barcode (grupo 50), se escribe ALIMENTO,<grupo>,<peso>,<ean>
                    {
                        listaComidaESP32.addAlimentoBarcode(grupoAnterior.ID_grupo, pesoBascula, barcode);
                    }

                    #if defined(SM_DEBUG)
                        listaComidaESP32.leerLista();
                    #endif 
                    // -----------------------------------------------------------
                    
                    // ----- AÑADIR ALIMENTO A PLATO -----------------------------
                    #if defined(SM_DEBUG)
                        SerialPC.println(F("Añadiendo alimento al plato..."));
                    #endif

                    Alimento alimento(grupoAnterior, pesoBascula);              // Cálculo automático de valores nutricionales.
                                                                                // Al escoger un nuevo grupo se guarda el alimento del grupo anterior
                                                                                // colocado en la báscula en la iteración anterior. Por eso se utiliza 'grupoAnterior' para
                                                                                // crear el alimento, porque 'grupoActual' ya ha tomado el valor del nuevo grupo al pulsar el nuevo botón.
                                                                                // Si el 'grupoAnterior' hubiera sido un barcode, se habrían modificado los valores nutricionales 
                                                                                // del grupo en State_Barcode, por lo que se habrían guardado en 'grupoAnterior' y se usarían aquí.
                                                                
                    platoActual.addAlimentoPlato(alimento);                     // Alimento ==> Plato
                    comidaActual.addAlimentoComida(alimento);                   // Alimento ==> Comida

                    pesoPlato = platoActual.getPesoPlato();                     // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.

                    tareScale();                                                // Tras guardar la información del último alimento colocado, se tara la báscula
                                                                                // para pesar el siguiente alimento
                    // -----------------------------------------------------------
            }
            // ----- FIN ACCIONES --------------------------


            // ----- INFO PANTALLA -------------------------
            // Si es la primera vez que se escoge grupo (no se viene de STATE_Grupo), se forma medio Dashboard (ejemplos, parpadeo zona 2 y pedir cr/co)  
            if(showSemiDashboard_PedirProcesamiento()) return;  // Mostrar semi dashboard completo al inicio
                                                                // Si ocurre alguna interrupción mientras se forma el semi dashboard, se sale de la función.
            // ----- FIN INFO DE PANTALLA ------------------


        }// FIN if(state_prev != STATE_Grupo)

        // Se están pulsando grupos para ver sus ejemplos (se viene del mismo STATE_Grupo)
        else 
        {
            // ----- INFO PANTALLA -------------------------
            printGrupoyEjemplos(); // Solo se modifica la zona 1 con los ejemplos del nuevo grupo escogido
            // ----- FIN INFO DE PANTALLA ------------------
        } 

        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.

    }


    //
    // 1 - La primera vez que se escoge un grupo de alimentos, se forma el semi dashboard completo --> showSemiDashboard_PedirProcesamiento()
    // 2 - Las siguientes veces que se escoge un grupo para ver sus ejemplos, sin pasar por un estado diferente a STATE_Grupo, no se vuelve
    //      a formar el semi dashboard al completo, sino que solo se modifica la zona 1 con el nuevo grupo seleccionado --> printGrupoyEjemplos()
    //
    // En ambos casos anteriores, las zonas 2 y 3-4 presentan movimiento constante. Es decir, la zona 2 parpadea para indicar que no se ha escogido
    // procesamiento (crudo o cocinado) y sobre las zonas 3-4 se muestra una pantalla que pide escoger el procesamiento alternando las imágenes
    // de los botones correspondientes.
    //
    // Por tanto, la creación del semi dashboard completo solo se hace al entrar a STATE_Grupo viniendo de algún estado diferente a ese y la 
    // modificación de la zona 1 con los ejemplos se hace si se viene de STATE_Grupo.
    //
    // Sin embargo, para que los movimientos en las zonas 2 y 3-4 sean fluidos y continuos, las funciones que los realizan se deben llamar en
    // cada ciclo del loop. Estas funciones no realizan esperas bloqueantes (delay) para los parpadeos y alternancias, sino que usan esperas 
    // no bloqueantes y variables 'static' para saber, al volver a entrar en la función, qué fue lo último que se hizo y continuar desde ahí.
    //
    // ----- PANTALLAS CON MOVIMIENTO -------------------------
    blinkGrupoyProcesamiento(NO_MSG);               // Zona 2 - Parpadea (procesamiento sin escoger)
    if(alternateButtonsProcesamiento()) return;     // Zonas 3 y 4 - Alternar botones de crudo y cocinado. Las formas, colores y texto ya están (formGraphicsPedirProcesamiento())
                                                    // Si hay interrupción mientras se alternan botones, se sale de la función --> en loop() se chequea la interrupción.
    // --------------------------------------------------------



}



/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_Barcode_read
----------------------------------------------------------------------------------------------------------*/
void actStateBarcodeRead()
{
    if(!doneState)
    {
        if(state_prev != STATE_Barcode_read)    // ==> Si no se viene del propio STATE_Barcode_read, para evitar que se vuelva a iniciar el proceso de lectura.
        {                                       // Se vendría de STATE_Barcode_read si al retirar el plato se detectara DECREMENTO antes de LIBERAR,
                                                // que ya llevaría a STATE_Init

            #if defined(SM_DEBUG)
                SerialPC.println(F("\nLeyendo barcode...")); 
            #endif

            // ----- ACCIONES PREVIAS --------------------------------------
            if((lastValidState == STATE_Plato) && (pesoBascula != 0.0)) // Si se acaba de colocar el recipiente. Comprobamos que no sea 0.0 el pesoBascula por si acaso
            {
                #if defined(SM_DEBUG)
                    SerialPC.println(F("\nGuardando peso recipiente..."));
                #endif
                pesoRecipiente = pesoBascula;    // Se guarda 'pesoRecipiente' para sumarlo a 'pesoPlato' y saber el 'pesoARetirar'.
                tareScale();                     // Se tara la báscula, preparándola para el primer alimento
            }

            // Se comprueba no venir del propio STATE_Barcode_read por si se ha retirado el plato, se detecta DECREMENTO (previo a LIBERAR) y se entra de nuevo 
            // en STATE_Barcode_read, para que no se vuelva a guardar el alimento que ya se guardó en la iteración anterior. Al retirar el plato, es posible que
            // se detecte DECREMENTO antes que LIBERAR, por lo que se volvería a entrar a STATE_Barcode_read.
            // Creo que no basta con solo comprobar que pesoBascula != 0.0 porque al retirar el plato el 'pesoBascula' pasará a ser negativo,
            // tras haber tarado en la iteración anterior al entrar en STATE_Barcode_read, por lo que se estaría guardando otra vez el alimento con peso negativo.
            // Lo raro es que hasta ahora (17/09/24) no tenía esta comprobación y parecía funcionar bien, pero por si acaso. 
            //else if ((state_prev != STATE_Barcode_read) && (pesoBascula != 0.0))
            else if (pesoBascula != 0.0)
            {
                    // ----- AÑADIR ALIMENTO A LISTA -----------------------------
                    // En el caso de STATE_group, al escoger nuevo grupo se actualiza automáticamente 'grupoActual' con el nuevo grupo al pulsar el botón correspondiente, por eso 
                    // el alimento pesado antes de escoger grupo se guarda, en ese caso, con 'grupoAnterior'. Sin embargo, en el caso de STATE_Barcode, hasta que no se confirme 
                    // el producto no se actualiza el 'grupoActual', por lo que el alimento pesado antes se puede guardar con 'grupoActual', pues sigue siendo válido.
                    
                    // Usamos 'grupoActual' porque aún no se ha actualizado
                    if(grupoActual.ID_grupo != BARCODE_PRODUCT_INDEX) // Si el grupo anterior del alimento pesado es de los nuestros, se escribe ALIMENTO,<grupo>,<peso> en la lista, 
                    {                                                 // siendo <grupo> el ID de 'grupoActual' y <peso> el valor de 'pesoBascula'.
                        listaComidaESP32.addAlimento(grupoActual.ID_grupo, pesoBascula);
                    }
                    else  // Si el grupo anterior es un barcode (grupo 50), se escribe ALIMENTO,<grupo>,<peso>,<ean>
                    {
                        listaComidaESP32.addAlimentoBarcode(grupoActual.ID_grupo, pesoBascula, barcode);
                    }

                    #if defined(SM_DEBUG)
                        listaComidaESP32.leerLista();
                    #endif 
                    // -----------------------------------------------------------
                    
                    // ----- AÑADIR ALIMENTO A PLATO -----------------------------
                    #if defined(SM_DEBUG)
                        SerialPC.println(F("Añadiendo alimento al plato..."));
                    #endif

                    Alimento alimento(grupoActual, pesoBascula);              // Cálculo automático de valores nutricionales.
                                                                            // Usamos 'grupoActual' porque aún no se ha actualizado
                                                                
                    platoActual.addAlimentoPlato(alimento);                     // Alimento ==> Plato
                    comidaActual.addAlimentoComida(alimento);                   // Alimento ==> Comida

                    pesoPlato = platoActual.getPesoPlato();                     // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.

                    tareScale();                                                // Tras guardar la información del último alimento colocado, se tara la báscula
                                                                                // para pesar el siguiente alimento
                    // -----------------------------------------------------------
            }
            // ----- FIN ACCIONES PREVIAS -----------------------------




            // ----- ACCIONES PRINCIPALES Y PANTALLAS -----------------
            // ----- INFO DE PANTALLA -------------------------
            showScanningBarcode();                                   // Mostrar "Escaneando código de barras..."
            // ----- FIN INFO DE PANTALLA ---------------------

            // ----- LEER BARCODE -----------------------------
            byte resultFromReadingBarcode = askForBarcode(barcode);  // Pedir código de barras. Se va a quedar aquí hasta 10.5 segundos esperando respuesta del ESP32.

            switch(resultFromReadingBarcode)
            {
                // --- BARCODE LEÍDO -----------
                case BARCODE_READ: // Si se ha leído el barcode, pasar a STATE_Barcode_search (evento BARCODE_R) para buscar su información
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nBarcode leido. Pasando a STATE_Barcode_search..."));
                                    #endif
                                    addEventToBuffer(BARCODE_R);    break; 
                // -----------------------------

                // -- AVISO: BARCODE NO LEÍDO --
                case BARCODE_NOT_READ: // Si no se ha leído el barcode, STATE_AVISO para mostrar el mensaje "Código de barras no detectado"
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nNo se ha detectado un barcode. Pasando a STATE_AVISO..."));
                                    #endif
                                    addEventToBuffer(AVISO_NO_BARCODE);        break;
                // -----------------------------

                // -- INTERRUPCIÓN (CANCELAR) --
                case INTERRUPTION: // Cancelación de la lectura del barcode por parte del usuario
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nInterrupción. Cancelando lectura de barcode..."));
                                    #endif
                                    // En las reglas de transición ya se pasa a STATE_CANCEL al detectar algún evento de usuario (botoneras) en este estado
                                    // No hace falta marcar aquí el evento manualmente.
                                                                    break;
                // ----------------------------

                // -- TIMEOUT O DESCONOCIDO ---
                default: // TIMEOUT o UNKNOWN_ERROR
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nTimeout o Desconocido, asumimos barcode no leido. Pasando a STATE_AVISO..."));
                                    #endif
                                    addEventToBuffer(AVISO_NO_BARCODE);        break;
                // ----------------------------
            }

            flagEvent = true; // Marcar flag de evento para que se compruebe en loop() y se realice la transición
            
            // ----- FIN LEER BARCODE -------------------------
            // --------------------------------------------------------


        } // FIN if(state_prev != STATE_Barcode_read)

        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }


}


/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_Barcode_search
----------------------------------------------------------------------------------------------------------*/
void actStateBarcodeSearch()
{
    if(!doneState)
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nBuscando producto...")); 
        #endif


        // ----- ACCIONES PRINCIPALES Y PANTALLAS -----------------
        // --- COMPROBAR SI HAY CONEXIÓN A INTERNET -----
        // Se vuelve a comprobar la conexión, aunque se haya hecho en STATE_Barcode_read, por si se ha perdido, para que no
        // se intente buscar el producto en la base de datos de OpenFoodFacts si no se tiene conexión a Internet.

        // --- HAY INTERNET ---
        if(checkWifiConnection()) // Hay WiFi
        {
            // ----- INFO DE PANTALLA -------------------------
            showSearchingProductInfo();                              // Mostrar "Buscando información del producto..."
            delay(500);                                             // Esperar 0.5 segundos para que se muestre el mensaje
            // ----- FIN INFO DE PANTALLA ---------------------

            // ----- LEER BARCODE -----------------------------
            byte resultFromGettingProductInfo = getProductInfo(barcode, productInfo); // Buscar info del producto en OpenFoodFacts en guardarla en 'productInfo'. 
                                                                                      // Se va a quedar aquí hasta 10.5 segundos esperando respuesta del ESP32.
            switch(resultFromGettingProductInfo)
            {
                // --- PRODUCTO ENCONTRADO --------------
                case PRODUCT_FOUND: // Si se ha encontrado el producto, pasar a STATE_Barcode_check (evento BARCODE_F) para confirmar el producto
                                    #if defined(SM_DEBUG)
                                        SerialPC.print(F("\nProducto encontrado. Pasando a STATE_Barcode_check..."));
                                    #endif
                                    addEventToBuffer(BARCODE_F);    break; 
                // --------------------------------------

                // --- AVISO: PRODUCTO NO ENCONTRADO ----
                case PRODUCT_NOT_FOUND: // Si no se ha encontrado el producto, pasar a STATE_AVISO para mostrar el mensaje "Producto no encontrado"
                                    #if defined(SM_DEBUG)
                                        SerialPC.print(F("\nProducto no encontrado. Pasando a STATE_AVISO..."));
                                    #endif
                                    addEventToBuffer(AVISO_PRODUCT_NOT_FOUND);        break;
                // --------------------------------------

                // -- AVISO: TIMEOUT O DESCONOCIDO ------
                default: // TIMEOUT, PRODUCT_TIMEOUT, HTTP_ERROR o UNKNOWN_ERROR
                                    #if defined(SM_DEBUG)
                                        SerialPC.print(F("\nTimeout o Desconocido, asumimos producto no encontrado. Pasando a STATE_AVISO..."));
                                    #endif
                                    addEventToBuffer(AVISO_PRODUCT_NOT_FOUND);        break;
                // -------------------------------------
            }

            flagEvent = true; // Marcar flag de evento para que se compruebe en loop() y se realice la transición
            
            // ----- FIN LEER BARCODE -------------------------

        }
        // --- FIN HAY INTERNET --

        // --- NO HAY INTERNET ---
        else // Si el ESP32 está desconectado, no hay WiFi o TIMEOUT, se vuelve a STATE_Plato, STATE_Grupo o STATE_Barcode
        {

            #if defined(SM_DEBUG)
                SerialPC.println(F("No se procede a buscar producto porque ESP32 esta desconectado o no tiene WiFi. Pasando a STATE_AVISO..."));
            #endif
            addEventToBuffer(AVISO_NO_WIFI_BARCODE);   
            flagEvent = true; // Marcar flag de evento para que se compruebe en loop() y se realice la transición    
        }
        // --- FIN NO HAY INTERNET ---
        // ---- FIN CHEQUEO INTERNET --------------------


        // --------------------------------------------------------


        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

}


/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_Barcode_check
----------------------------------------------------------------------------------------------------------*/
void actStateBarcodeCheck()
{
    static unsigned long previousTimeCancelProduct;      // Tiempo usado para cancelar el producto tras 20 segundos de inactividad
    const unsigned long confirmProductInterval = 20000;  // Intervalo de tiempo para confirmar el producto buscado

    if(!doneState)
    {
        previousTimeCancelProduct = millis(); 

        #if defined(SM_DEBUG)
            SerialPC.println(F("\n¿Es el producto que desea?")); 
        #endif
        
        pedirConfirmacionProducto(productInfo);                             // Mostrar pregunta de confirmación con código y nombre del producto 
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

    // --- CANCELACIÓN AUTOMÁTICA ---
    if ((millis() - previousTimeCancelProduct) > confirmProductInterval)     // Tras 20 segundos de inactividad, se cancela automáticamente el producto buscado
    {
        #if defined(SM_DEBUG)
            SerialPC.print(F("\nTIME-OUT. Cancelando producto encontrado..."));
        #endif
        addEventToBuffer(CANCELAR);   // Se transiciona al STATE_CANCEL, que regresa automáticamente al estado desde donde se inició la lectura de barcode
        flagEvent = true;
    }
    // --- FIN CANCELACIÓN AUTOMÁTICA ---
}


/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_Barcode
----------------------------------------------------------------------------------------------------------*/
void actStateBarcode()
{
    // Tiempos utilizados para alternar entre dashboard y pantalla de colocar alimento:
    const unsigned long dashboardInterval = 10000;  // Intervalo de tiempo para mostrar el dashboard (10 segundos) -> se deja más tiempo para leer ejemplos
    const unsigned long alimentoInterval = 5000;    // Intervalo de tiempo para pedir colocar alimento (5 segundos)

    static unsigned long previousTime;              // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_dash;       
    static bool showing_colocar_alimento;

    if(!doneState)
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nProducto confirmado...")); 
        #endif

        // ----- ACCIONES --------------------------------
        // ----- ACTUALIZAR grupoActual ----------
        // Solo se actualiza 'grupoActual' si se viene directo de STATE_Barcode_check, para evitar que se modifique 'grupoActual' si en realidad se ha cancelado el producto pero al querer
        // regresar al último estado válido se llegó a STATE_Barcode porque se había iniciado una segunda lectura (la del producto cancelado) tras previamente haber confirmado un producto.
        if(state_prev == STATE_Barcode_check) 
            updateGrupoActualFromBarcode(productInfo); // Actualizar información del grupoActual con la info del úlimo producto leído si se acaba de confirmar
        // ---------------------------------------
        // ----- FIN ACCIONES ----------------------------


        // ----- INFO DE PANTALLA -------------------------
        // Solo se modifica la pantalla completa si se estaba mostrando una pantalla temporal/transitoria. 
        // En realidad, todas las pantallas que se puedan mostrar antes de entrar a STATE_Barcode serán transitorias, osea que no haría falta comprobarlo.
        //if(showingTemporalScreen) 
            showDashboardStyle2_Barcode(); // Mostrar dashboard estilo 2 (Alimento | Comida) con Zona 1 (nombre producto) cubriendo la Zona 2 (no necesita procesamiento) para el grupo barcode

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
    if(showing_dash) // Se está mostrando dashboard estilo 2 (Alimento | Comida)
    {
        if (currentTime - previousTime >= dashboardInterval)  // Si el dashboard ha estado 10 segundos, se cambia a pedir alimento
        {
            previousTime = currentTime;
            pedirAlimento();
            showing_dash = false;  
            showing_colocar_alimento = true;   // Mostrando pedir alimento
        }
    }
    else if(showing_colocar_alimento) // Se está mostrando pedir alimento
    {
        if (currentTime - previousTime >= alimentoInterval)  // Si el pedir alimento ha estado 5 segundos, se cambia a dashboard estilo 2 para barcode (sin ejemplos ni procesamiento)
        {
            previousTime = currentTime;
            showDashboardStyle2_Barcode();
            showing_dash = true;  // Mostrando dashboard estilo 2 (Alimento | Comida)
            showing_colocar_alimento = false;   
        }
    }
    // ----- FIN ALTERNANCIA PANTALLAS ---------------------
}



/*---------------------------------------------------------------------------------------------------------
   actStateRaw(): Acciones del STATE_raw
----------------------------------------------------------------------------------------------------------*/
void actStateRaw()
{ 
    // Tiempos utilizados para alternar entre dashboard y pantalla de colocar alimento:
    const unsigned long dashboardInterval = 10000;  // Intervalo de tiempo para mostrar el dashboard (10 segundos) -> se deja más tiempo para leer ejemplos
    const unsigned long alimentoInterval = 5000;    // Intervalo de tiempo para pedir colocar alimento (5 segundos)

    static unsigned long previousTime;              // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_dash;       
    static bool showing_colocar_alimento;

    bool lastAlimentoSaved = false; // Para saber si se ha guardado el último alimento pesado (si se viene de STATE_weighted) antes de cambiar el procesamiento

    if(!doneState)
    {
        // ----- ACCIONES PREVIAS ----------------------
        // Se comprueba que el último estado válido sea STATE_weighted por si, tras pesar un alimento, se ha decidido cambiar el procesamiento sin cambiar de grupo.
        // Entonces, si había algo pesándose, se debe guardar en la lista de alimentos y en el plato antes de cambiar el procesamiento para pesar otro alimento del 
        // mismo grupo actualmente escogido pero con otro procesamiento.
        if((lastValidState == STATE_weighted) && (pesoBascula != 0.0))
        {       
            // Al pulsar el botón de CRUDO se han actualiza 'grupoActual' y 'grupoAnterior', pero aún no se ha guardado el alimento pesado, por lo que se guarda 
            // con 'grupoAnterior' porque representa al grupo del alimento pesado, que aunque fuera del mismo grupo oficial, se ha pesado antes de cambiar el procesamiento.
            // ----- AÑADIR ALIMENTO A LISTA -----------------------------
            if(grupoAnterior.ID_grupo != BARCODE_PRODUCT_INDEX)  // Si el grupo anterior del alimento pesado es de los nuestros, se escribe ALIMENTO,<grupo>,<peso> en la lista, 
            {                                                  //   siendo <grupo> el ID de 'grupoActual' y <peso> el valor de 'pesoBascula'.
                listaComidaESP32.addAlimento(grupoAnterior.ID_grupo, pesoBascula);
            }
            else  // Si el grupo anterior es un barcode (grupo 50), se escribe ALIMENTO,<grupo>,<peso>,<ean>
            {
                listaComidaESP32.addAlimentoBarcode(grupoAnterior.ID_grupo, pesoBascula, barcode);
            }

            #if defined(SM_DEBUG)
                listaComidaESP32.leerLista();
            #endif 
            // -----------------------------------------------------------
                
            // ----- AÑADIR ALIMENTO A PLATO -----------------------------
            #if defined(SM_DEBUG)
                SerialPC.println(F("Añadiendo alimento al plato..."));
            #endif

            Alimento alimento(grupoAnterior, pesoBascula);              // Cálculo automático de valores nutricionales.
                                                            
            platoActual.addAlimentoPlato(alimento);                     // Alimento ==> Plato
            comidaActual.addAlimentoComida(alimento);                   // Alimento ==> Comida

            pesoPlato = platoActual.getPesoPlato();                     // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.

            tareScale();                                                // Tras guardar la información del último alimento colocado, se tara la báscula
                                                                        // para pesar el siguiente alimento
            // -----------------------------------------------------------

            lastAlimentoSaved = true; // Se ha guardado el último alimento pesado
        }
        // ----- FIN ACCIONES PREVIAS --------------------

        // ----- ACCIONES ------------------------------
        // Se comprueba no venir del propio STATE_raw por si se ha retirado el plato, se detecta DECREMENTO (previo a LIBERAR) y se entra 
        // de nuevo en STATE_raw, para que no se vuelva a modificar el procesamiento, aunque no pasaría nada. 
        if(state_prev != STATE_raw)
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("\nAlimento crudo...")); 
            #endif
            procesamiento = ALIMENTO_CRUDO; // Aunque al pulsar el botón de "CRUDO" ya se ha actualizado 'grupoActual', se necesita actualizar 'procesamiento'
                                            // para saber qué icono mostrar en la zona 2 del dashboard.
        }
        // ----- FIN ACCIONES --------------------------

        // ----- INFO DE PANTALLA -------------------------
        // El 'pesoBascula' se ha puesto a 0 al tarar tras guardar el alimento pesado y en showDashboardStyle2() se muestra "Alimento actual" 
        // creando un alimento auxiliar con 'pesoBascula' y 'grupoActual', pero no lo veremos si no se modifica la zona 3, además de la zona 2 
        // con el procesamiento. Por eso se modifica la pantalla completa en lugar de solo la zona 2.

        if(lastAlimentoSaved) // Si se ha guardado el último alimento pesado porque se viene de STATE_weighted, se modifica el dashboard completo 
        {                     //    para ver cambios en la zona 2 (crudo) y en la zona 3 (alimento actual a 0)
            lastAlimentoSaved = false;  // No haría falta porque no es static, pero por claridad
            showDashboardStyle2();      // Mostrar dashboard estilo 2 con el procesamiento "crudo" y el alimento actual a 0
        }
        else // Si no se ha guardado el último alimento pesado porque se viene de STATE_Grupo o STATE_cooked:
        {    //     Si se estaba mostrando una pantalla transitoria (semi dashboard pidiendo procesamiento), se modifica el dashboard completo. 
             //     Si se estaba mostrando el dashboard, solo se modifica la zona 2.
            if(showingTemporalScreen) showDashboardStyle2();  // Mostrar dashboard estilo 2 con el procesamiento "crudo"
            else printProcesamiento();                        // Modificar solamente zona 2 con el procesamiento "crudo"
        }

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
    if(showing_dash) // Se está mostrando dashboard estilo 2 (Alimento | Comida)
    {
        if (currentTime - previousTime >= dashboardInterval)  // Si el dashboard ha estado 10 segundos, se cambia a pedir alimento
        {
            previousTime = currentTime;
            pedirAlimento();
            showing_dash = false;  
            showing_colocar_alimento = true;   // Mostrando pedir alimento
        }
    }
    else if(showing_colocar_alimento) // Se está mostrando pedir alimento
    {
        if (currentTime - previousTime >= alimentoInterval)  // Si el pedir alimento ha estado 5 segundos, se cambia a dashboard estilo 2
        {
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
void actStateCooked()
{ 
    // Tiempos utilizados para alternar entre dashboard y pantalla de colocar alimento:
    const unsigned long dashboardInterval = 10000;  // Intervalo de tiempo para mostrar el dashboard (10 segundos) -> se deja más tiempo para leer ejemplos
    const unsigned long alimentoInterval = 5000;    // Intervalo de tiempo para pedir colocar alimento (5 segundos)

    static unsigned long previousTime;              // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_dash;       
    static bool showing_colocar_alimento;

    bool lastAlimentoSaved = false; // Para saber si se ha guardado el último alimento pesado (si se viene de STATE_weighted) antes de cambiar el procesamiento

    if(!doneState)
    {
        // ----- ACCIONES PREVIAS ----------------------
        // Se comprueba que el último estado válido sea STATE_weighted por si, tras pesar un alimento, se ha decidido cambiar el procesamiento sin cambiar de grupo.
        // Entonces, si había algo pesándose, se debe guardar en la lista de alimentos y en el plato antes de cambiar el procesamiento para pesar otro alimento del 
        // mismo grupo actualmente escogido pero con otro procesamiento.
        if((lastValidState == STATE_weighted) && (pesoBascula != 0.0))
        {       
            // Al pulsar el botón de COCINADO se han actualiza 'grupoActual' y 'grupoAnterior', pero aún no se ha guardado el alimento pesado, por lo que se guarda 
            // con 'grupoAnterior' porque representa al grupo del alimento pesado, que aunque fuera del mismo grupo oficial, se ha pesado antes de cambiar el procesamiento.
            // ----- AÑADIR ALIMENTO A LISTA -----------------------------
            if(grupoAnterior.ID_grupo != BARCODE_PRODUCT_INDEX)  // Si el grupo anterior del alimento pesado es de los nuestros, se escribe ALIMENTO,<grupo>,<peso> en la lista, 
            {                                                  //   siendo <grupo> el ID de 'grupoActual' y <peso> el valor de 'pesoBascula'.
                listaComidaESP32.addAlimento(grupoAnterior.ID_grupo, pesoBascula);
            }
            else  // Si el grupo anterior es un barcode (grupo 50), se escribe ALIMENTO,<grupo>,<peso>,<ean>
            {
                listaComidaESP32.addAlimentoBarcode(grupoAnterior.ID_grupo, pesoBascula, barcode);
            }

            #if defined(SM_DEBUG)
                listaComidaESP32.leerLista();
            #endif 
            // -----------------------------------------------------------
            
            // ----- AÑADIR ALIMENTO A PLATO -----------------------------
            #if defined(SM_DEBUG)
                SerialPC.println(F("Añadiendo alimento al plato..."));
            #endif

            Alimento alimento(grupoAnterior, pesoBascula);              // Cálculo automático de valores nutricionales.
                                                            
            platoActual.addAlimentoPlato(alimento);                     // Alimento ==> Plato
            comidaActual.addAlimentoComida(alimento);                   // Alimento ==> Comida

            pesoPlato = platoActual.getPesoPlato();                     // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.

            tareScale();                                                // Tras guardar la información del último alimento colocado, se tara la báscula
                                                                        // para pesar el siguiente alimento
            // -----------------------------------------------------------

            lastAlimentoSaved = true; // Se ha guardado el último alimento pesado
        }
        // ----- FIN ACCIONES PREVIAS --------------------

        // ----- ACCIONES ------------------------------
        // Se comprueba no venir del propio STATE_cooked por si se ha retirado el plato, se detecta DECREMENTO (previo a LIBERAR) y se entra 
        // de nuevo en STATE_cooked, para que no se vuelva a modificar el procesamiento, aunque no pasaría nada. 
        if(state_prev != STATE_cooked)
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("\nAlimento cocinado..."));   
            #endif     
            procesamiento = ALIMENTO_COCINADO; // Aunque al pulsar el botón de "COCINADO" ya se ha actualizado 'grupoActual', se necesita actualizar 'procesamiento'
                                               // para saber qué icono mostrar en la zona 2 del dashboard.
        }
        // ----- FIN ACCIONES --------------------------

        // ----- INFO DE PANTALLA -------------------------
        // El 'pesoBascula' se ha puesto a 0 al tarar tras guardar el alimento pesado y en showDashboardStyle2() se muestra "Alimento actual" 
        // creando un alimento auxiliar con 'pesoBascula' y 'grupoActual', pero no lo veremos si no se modifica la zona 3, además de la zona 2 
        // con el procesamiento. Por eso se modifica la pantalla completa en lugar de solo la zona 2.

        if(lastAlimentoSaved) // Si se ha guardado el último alimento pesado porque se viene de STATE_weighted, se modifica el dashboard completo 
        {                     //    para ver cambios en la zona 2 (cocinado) y en la zona 3 (alimento actual a 0)
            lastAlimentoSaved = false;  // No haría falta porque no es static, pero por claridad
            showDashboardStyle2();      // Mostrar dashboard estilo 2 con el procesamiento "cocinado" y el alimento actual a 0
        }
        else // Si no se ha guardado el último alimento pesado porque se viene de STATE_Grupo o STATE_raw:
        {    //     Si se estaba mostrando una pantalla transitoria (semi dashboard pidiendo procesamiento), se modifica el dashboard completo. 
             //     Si se estaba mostrando el dashboard, solo se modifica la zona 2.
            if(showingTemporalScreen) showDashboardStyle2();  // Mostrar dashboard estilo 2 con el procesamiento "cocinado"
            else printProcesamiento();                        // Modificar solamente zona 2 con el procesamiento "cocinado"
        }

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
    if(showing_dash) // Se está mostrando dashboard estilo 2 (Alimento | Comida)
    {
        if (currentTime - previousTime >= dashboardInterval)  // Si el dashboard ha estado 10 segundos, se cambia a pedir alimento
        {
            previousTime = currentTime;
            pedirAlimento();
            showing_dash = false;  
            showing_colocar_alimento = true;   // Mostrando pedir alimento
        }
    }
    else if(showing_colocar_alimento) // Se está mostrando pedir alimento
    {
        if (currentTime - previousTime >= alimentoInterval)  // Si el pedir alimento ha estado 5 segundos, se cambia a dashboard estilo 2
        {
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
void actStateWeighted()
{ 
    // Tiempos utilizados para alternar entre dashboard y pantalla de sugerencia de acciones:
    const unsigned long dashboardInterval = 30000;      // Intervalo de tiempo para mostrar el dashboard (30 segundos)
    const unsigned long sugerenciasInterval = 15000;    // Intervalo de tiempo para sugerir acciones (10 segundos)

    static unsigned long previousTime;              // Variable estática para almacenar el tiempo anterior
    unsigned long currentTime;

    static bool showing_dash;       
    static bool showing_sugerir_acciones;

    if(!doneState)
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nAlimento pesado...")); 
        #endif

        // ----- INFO DE PANTALLA -------------------------
        // Solo se modifica la pantalla completa si se estaba mostrando una pantalla temporal/transitoria (sugerencia acciones). 
        // Si se estaba mostrando el dashboard, solo se modifican las zonas 3 y 4 con los valores correspondientes.
        //if(showingTemporalScreen) showDashboardStyle2(); 
        
        // Si el grupo actual es de barcode (ID = 50), el dashboard es algo distinto porque no se muestra la Zona 2 de procesamiento
        if(showingTemporalScreen)
        {
            if(grupoActual.ID_grupo == BARCODE_PRODUCT_INDEX)
                showDashboardStyle2_Barcode(); // Mostrar dashboard estilo 2 (Alimento | Comida) con Zona 1 (nombre producto) cubriendo la Zona 2 (no necesita procesamiento) para el grupo barcode
            else 
                showDashboardStyle2(); // Mostrar dashboard estilo 2 (Alimento | Comida) con Zona 1 (grupo y ejemplos) y Zona 2 (procesamiento) para los grupos normales
        }
        else
        {
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
    if(showing_dash) // Se está mostrando dashboard estilo 2 (Alimento | Comida)
    {
        if (currentTime - previousTime >= dashboardInterval)  // Si el dashboard ha estado 30 segundos, se cambia a sugerir acciones
        {
            previousTime = currentTime;
            sugerirAccion();
            showing_dash = false;  
            showing_sugerir_acciones = true;   // Mostrando sugerir acciones
        }
    }
    else if(showing_sugerir_acciones) // Se está mostrando sugerir acciones
    {
        if (currentTime - previousTime >= sugerenciasInterval)  // Si el sugerir acciones ha estado 10 segundos, se cambia a dashboard estilo 2
        {
            previousTime = currentTime;
            if(grupoActual.ID_grupo == BARCODE_PRODUCT_INDEX) showDashboardStyle2_Barcode(); // Mostrar dashboard estilo 2 para el grupo barcode
            else showDashboardStyle2(); // Mostrar dashboard estilo 2 para los grupos normales
            showing_dash = true;  // Mostrando dashboard estilo 2 (Alimento | Comida)
            showing_sugerir_acciones = false;   
        }
    }
    // ----- FIN ALTERNANCIA PANTALLAS ---------------------
}


/*---------------------------------------------------------------------------------------------------------
   actStateAddCheck(): Acciones del STATE_add_check
----------------------------------------------------------------------------------------------------------*/
void actStateAddCheck()
{ 
    static unsigned long previousTimeCancel;      // Tiempo usado para cancelar la acción tras 10 segundos de inactividad

    if(!doneState)
    {
        previousTimeCancel = millis(); 

        #if defined(SM_DEBUG)
            SerialPC.println(F("\n¿Seguro que quiere añadir un plato?")); 
        #endif
        
        pedirConfirmacion(ASK_CONFIRMATION_ADD);                            // Mostrar pregunta de confirmación para añadir plato
        
        doneState = true;                                                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

    // --- CANCELACIÓN AUTOMÁTICA ---
    if ((millis() - previousTimeCancel) > 15000)     // Tras 15 segundos de inactividad, se cancela automáticamente la acción añadir plato
    {
        #if defined(SM_DEBUG)
            SerialPC.print(F("\nTIME-OUT. Cancelando añadir plato..."));
        #endif
        addEventToBuffer(CANCELAR);   // Se transiciona al STATE_CANCEL, que regresa automáticamente al estado desde donde se inició añadir plato
        flagEvent = true;
    }
    // --- FIN CANCELACIÓN AUTOMÁTICA ---
}

/*---------------------------------------------------------------------------------------------------------
   actStateAdded(): Acciones del STATE_added
----------------------------------------------------------------------------------------------------------*/
void actStateAdded()
{ 
    static bool avisoPlatoWasEmpty;        // Flag utilizada para saber si se debe mostrar la información "normal"
                                           // o un mensaje de aviso indicando no se ha creado otro plato porque el 
                                           // actual está vacío.

    if(!doneState)
    {

        if(state_prev != STATE_added)      // ==> Si no se viene del propio STATE_added, para evitar que se vuelva a guardar el plato en la comida.
        {                                   // Se vendría de STATE_added si al retirar el plato se detectara INCREMENTO o DECREMENTO antes de LIBERAR,
                                            // que ya llevaría a STATE_Init

                                            // *****
                                            // Primero se actualiza el plato, si es necesario, y después se muestran las actualizaciones.
                                            // Si tras comprobar cambios en el plato se ve que sigue vacío, entonces se indica en un mensaje 
                                            // que no se ha podido crear otro.
                                            // *****
            
            #if defined(SM_DEBUG)
                SerialPC.println(F("\nAñadiendo plato a la comida..."));
            #endif

            /* ----- ACTUALIZAR PLATO  ----- */
            if(pesoBascula != 0.0)   // ==> Si se ha colocado algo nuevo en la báscula y no se ha retirado, debe incluirse en el plato. 
            {
                                                                    // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                                    // añadir un nuevo plato sin haber colocado un nuevo alimento, no haría falta
                                                                    // actualizar el plato, pues no se habría modificado.

                // ----- AÑADIR ALIMENTO A LISTA -----------------------------
                // Usamos 'grupoActual' porque no se ha modificado (no se ha pulsado otro grupo).
                if(grupoActual.ID_grupo != BARCODE_PRODUCT_INDEX)  // Si el grupo anterior del alimento pesado es de los nuestros, se escribe ALIMENTO,<grupo>,<peso> en la lista, 
                {                                 // siendo <grupo> el ID de 'grupoActual' y <peso> el valor de 'pesoBascula'.
                    listaComidaESP32.addAlimento(grupoActual.ID_grupo, pesoBascula);
                }
                else  // Si el grupo anterior es un barcode (grupo 50), se escribe ALIMENTO,<grupo>,<peso>,<ean>
                {
                    listaComidaESP32.addAlimentoBarcode(grupoActual.ID_grupo, pesoBascula, barcode);
                }

                #if defined(SM_DEBUG)
                    listaComidaESP32.leerLista();
                #endif 
                // -----------------------------------------------------------
                

                // ----- AÑADIR ALIMENTO A PLATO -----------------------------
                Alimento alimento(grupoActual, pesoBascula);      // Cálculo automático de valores nutricionales. 
                                                                    // Usamos 'grupoActual' porque no se ha modificado (no se ha pulsado otro grupo).
                
                platoActual.addAlimentoPlato(alimento);             // Alimento ==> Plato
                comidaActual.addAlimentoComida(alimento);           // Alimento ==> Comida

                /* ----- TARAR  ----- */
                tareScale();                                        // Se debe tarar para que conforme vaya disminuyendo el peso veamos si 
                                                                    // se ha quitado todo el 'pesoARetirar'.
                                                                    // Solo se hace si se viene del STATE_weighted porque en los estados 
                                                                    // previos a ese ya se tara y es en STATE_weighted donde se modifica el peso.
                // -----------------------------------------------------------
            }
            /* ---- FIN ACTUALIZAR PLATO --- */


            /* ----- GUARDAR PLATO EN COMIDA  ----- */
            // ----- GUARDAR PLATO -------
            if(!platoActual.isPlatoEmpty())  // PLATO CON COSAS (si había último alimento, se ha incluido) ==> SE GUARDA Y SE CREA OTRO
            {
                avisoPlatoWasEmpty = false;
                pesoPlato = platoActual.getPesoPlato();             // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                comidaActual.addPlato(platoActual);                 // Plato ==> Comida (no se vuelven a incluir los alimentos. Solo modifica peso y nPlatos)
                platoActual.restorePlato();                         // "Reiniciar" platoActual para usarlo de nuevo.
                comidaActualCopia.copyComida(comidaActual); // Copiar nº platos, peso y valores de la comida actual a la copia. Este objeto Comida solo sirve para mostrar
                                                            // en el dashboard estilo 1 (STATE_Init y STATE_Plato) la comida actual junto con el acumulado.
                                                            // Se muestra esta copia en lugar de la original 'comidaActual' porque esa se borra tras guardarla.
            }
            // ----- FIN GUARDAR PLATO ---

            // ----- AVISO: PLATO VACÍO ---------
            else if(state_prev != STATE_ERROR)   // PLATO VACÍO y no venir de error ==> NO SE CREA OTRO 
            {
                    // Para evitar marcar un aviso "falso", se chequea no venir de error:
                    //      Si se ha añadido un plato y, en lugar de retirar el plato anterior, se comete un error (pulsando botón), al regresar a este
                    //      estado STATE_added el 'platoActual' ya estará vacío porque se reinició anteriormente (tras guardarlo en la comida), por lo que 
                    //      daría aviso y regresaría al estado desde donde se inició el añadido. Todo eso aún con el plato anterior puesto en la báscula, 
                    //      por lo que se podrían ir encadenando una serie de errores al retirarlo desde ese estado previo. Por eso solo se marca aviso si es la 
                    //      primera vez que se entra al estado STATE_added, pero no si se entró a STATE_added, se cometió error y se regresó a STATE_added, 
                    //      pues ya no podría salir de ahí.
                
                #if defined(SM_DEBUG)
                    SerialPC.println(F("No se ha creado otro plato porque el actual está vacío. Pasando a STATE_AVISO..."));  
                #endif
                
                avisoPlatoWasEmpty = true; 
                addEventToBuffer(AVISO_PLATO_EMPTY_NOT_ADDED);   // Se transiciona al STATE_AVISO, que regresa automáticamente al estado desde donde se inició añadir plato
                flagEvent = true;
            }
            // ----- FIN AVISO: PLATO VACÍO -----
            /* --------------------------------------- */


            /* -----  INFORMACIÓN MOSTRADA  ---------------------------------- */
            if(!avisoPlatoWasEmpty) 
                showAccionRealizada(ADD_EXECUTED); // Se muestra "Plato añadido" si no ha habido aviso de plato vacío.
                                                   // Si hubiera estado vacío, se pasaría al STATE_AVISO y luego se regresaría a grupos
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
void actStateDeleteCheck()
{ 
    static unsigned long previousTimeCancel;      // Tiempo usado para cancelar la acción tras 10 segundos de inactividad

    if(!doneState)
    {
        previousTimeCancel = millis(); 

        #if defined(SM_DEBUG)
            SerialPC.println(F("\n¿Seguro que quiere eliminar el plato?")); 
        #endif
        
        pedirConfirmacion(ASK_CONFIRMATION_DELETE);     // Mostrar pregunta de confirmación para eliminar plato
        
        doneState = true;                               // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                        // cada vez que se entre a esta función debido al loop de Arduino.
                                                        // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

    // --- CANCELACIÓN AUTOMÁTICA ---
    if ((millis() - previousTimeCancel) > 15000)     // Tras 15 segundos de inactividad, se cancela automáticamente la acción eliminar plato
    {
        #if defined(SM_DEBUG)
            SerialPC.print(F("\nTIME-OUT. Cancelando eliminar plato..."));
        #endif
        addEventToBuffer(CANCELAR);   // Se transiciona al STATE_CANCEL, que regresa automáticamente al estado desde donde se inició eliminar plato
        flagEvent = true;
    }
    // --- FIN CANCELACIÓN AUTOMÁTICA ---
}


/*---------------------------------------------------------------------------------------------------------
   actStateDeleted(): Acciones del STATE_deleted
----------------------------------------------------------------------------------------------------------*/
void actStateDeleted()
{
    static bool avisoPlatoWasEmpty;        // Flag utilizada para saber si se debe mostrar la información "normal"
                                           // o un mensaje de aviso indicando no se ha borrado el plato porque el 
                                           // actual ya está vacío.

    if(!doneState)
    {

        if(state_prev != STATE_deleted)    // ==> Si no se viene del propio STATE_deleted, para evitar que se vuelva a eliminar el plato de la comida (saltaría aviso).
        {                                   // Se vendría de STATE_deleted si al retirar el plato se detectara INCREMENTO o DECREMENTO antes de LIBERAR,
                                            // que ya llevaría a STATE_Init

                                            // *****
                                            // Primero se actualiza el plato, si es necesario, y después se muestran las actualizaciones.
                                            // Si tras comprobar cambios en el plato se ve que sigue vacío, entonces se indica en un mensaje 
                                            // que no se ha eliminado porque ya está vacío.
                                            // *****
          

            // ----- BORRAR PLATO DE LISTA--------------------------------
            // Borrar desde la última aparición de "INICIO-PLATO" hasta el final de la lista
            listaComidaESP32.borrarLastPlato();
            #if defined(SM_DEBUG)
                listaComidaESP32.leerLista();
            #endif
            // -----------------------------------------------------------
            

            // ----- BORRAR PLATO DE COMIDA ------------------------------
            #if defined(SM_DEBUG)
                SerialPC.println(F("\nEliminando plato..."));
            #endif

            /* ----- PESO ÚLTIMO ALIMENTO ----- */
            if(pesoBascula != 0.0)         // ==> Si se ha colocado algo nuevo en la báscula (pesoBascula marca algo) y no se ha retirado,
            {                               //     debe incluirse en el plato. 

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
            /* ----- FIN ÚLTIMO ALIMENTO ------ */


            /* ----- BORRAR PLATO DE COMIDA -------- */
            // ----- BORRAR PLATO -------
            if(!platoActual.isPlatoEmpty())    // PLATO CON COSAS ==> HAY QUE BORRAR Y RETIRAR (plato guardado o solo último alimento)
            {
                avisoPlatoWasEmpty = false;
                pesoPlato = platoActual.getPesoPlato();                 
                if(pesoLastAlimento != 0.0) pesoPlato += pesoLastAlimento;  // Incluir último alimento en el 'pesoPlato' para saber 'pesoARetirar'.
                comidaActual.deletePlato(platoActual);                      // Borrar plato actual
                platoActual.restorePlato();                                 // "Reiniciar" platoActual para usarlo de nuevo.
                comidaActualCopia.copyComida(comidaActual); // Copiar nº platos, peso y valores de la comida actual a la copia. Este objeto Comida solo sirve para mostrar
                                                            // en el dashboard estilo 1 (STATE_Init y STATE_Plato) la comida actual junto con el acumulado.
                                                            // Se muestra esta copia en lugar de la original 'comidaActual' porque esa se borra tras guardarla.
            }
            // ----- FIN BORRAR PLATO ---

            // ----- AVISO: PLATO VACÍO --------
            else   // PLATO VACÍO ==> NO HAY QUE BORRAR, PERO PUEDE QUE SÍ RETIRAR (último alimento colocado en la báscula)
            {
                if(pesoLastAlimento != 0.0) // ÚLTIMO ALIMENTO ==> ALGO QUE RETIRAR DE LA BÁSCULA
                {
                    avisoPlatoWasEmpty = false;       // Para mostrar mensaje de plato borrado y pedir retirarlo, aunque no sea cierto funcionalmente porque no se ha guardado el
                                                      // último alimento (solo su peso) y el objeto Plato está realmente vacío.

                    pesoPlato = pesoLastAlimento;     // Incluir último alimento en el 'pesoPlato' para saber 'pesoARetirar'.
                }
                else if(state_prev != STATE_ERROR)   // PLATO VACÍO Y NO HAY ÚLTIMO ALIMENTO ==> NADA QUE BORRAR NI RETIRAR 
                {
                    // Para evitar marcar un aviso "falso", se chequea no venir de error:
                    //      Si se ha borrado el plato y, en lugar de retirar el plato, se comete un error (pulsando botón), al regresar a este
                    //      estado STATE_deleted el 'platoActual' ya estará vacío porque se borró anteriormente, por lo que daría aviso y 
                    //      regresaría al estado desde donde se inició el borrado. Todo eso aún con el plato puesto en la báscula, por lo que se 
                    //      podrían ir encadenando una serie de errores al retirarlo desde ese estado previo. Por eso solo se marca aviso si es la 
                    //      primera vez que se entra al estado STATE_deleted, pero no si se entró a STATE_deleted, se cometió error y se regresó a STATE_deleted, 
                    //      pues ya no podría salir de ahí.
                    
                    #if defined(SM_DEBUG)
                        SerialPC.println(F("No se ha borrado el plato porque está vacío. Pasando a STATE_AVISO..."));             
                    #endif 
                    
                    avisoPlatoWasEmpty = true; 
                    addEventToBuffer(AVISO_PLATO_EMPTY_NOT_DELETED);   // Se transiciona al STATE_AVISO, que regresa automáticamente al estado desde donde se inició borrar plato
                    flagEvent = true;
                }
            }
            // ----- FIN AVISO: PLATO VACÍO ----
            /* ----- FIN BORRAR PLATO DE COMIDA ---- */
            // -----------------------------------------------------------



            /* -----  INFORMACIÓN MOSTRADA  ---------------------------------- */
            if(!avisoPlatoWasEmpty) 
                showAccionRealizada(DELETE_EXECUTED); // Se muestra "Plato eliminado" si no ha habido aviso de plato vacío.
                                                      // Si hubiera estado vacío, se pasaría al STATE_AVISO y luego se regresaría a grupos
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
// Lo primero que hacía era preguntar al ESP32 por WiFi, antes siquiera de poner la pantalla de confirmar. Entonces, si el ESP32 no contestaba (se esperan 3 segundos),
// daba la sensación de que no se había detectado la primera pulsación de GUARDAR, lo que provoca que el usuario volviera a pulsar y, sin saberlo, directamente confirmara
// la subida. Por eso, creo que es mejor preguntar por WiFi después de confirmar la acción de guardar comida y, entonces, informar de si se guardará en la web o 
// solo en el SmartCloth. La pantalla de confirmar guardado ya no mostrará "Conectado a Internet" ni "Sin Internet", pero bueno, al guardarlo se le informará.
void actStateSaveCheck()
{ 
    static unsigned long previousTimeCancel;      // Tiempo usado para cancelar la acción tras 10 segundos de inactividad

    if(!doneState)
    {
        previousTimeCancel = millis(); 

        #if defined(SM_DEBUG)
            SerialPC.println(F("\n¿Seguro que quiere guardar la comida?")); 
        #endif
        
        pedirConfirmacion(ASK_CONFIRMATION_SAVE);       // Mostrar pregunta de confirmación para guardar comida.
                                                        // No se indica si tiene conexión a internet o no para que no se pare a preguntar, por si el ESP32 no responde.   
        
        doneState = true;                               // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                        // cada vez que se entre a esta función debido al loop de Arduino.
                                                        // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }


    // --- CANCELACIÓN AUTOMÁTICA ---
    if ((millis() - previousTimeCancel) > 15000)       // Tras 15 segundos de inactividad, se cancela automáticamente la acción guardar comida
    {
        #if defined(SM_DEBUG)
            SerialPC.print(F("\nTIME-OUT. Cancelando guardar comida..."));
        #endif
        addEventToBuffer(CANCELAR);   // Se transiciona al STATE_CANCEL, que regresa automáticamente al estado desde donde se inició guardar comida
        flagEvent = true;
    }
    // --- FIN CANCELACIÓN AUTOMÁTICA ---
}




/*---------------------------------------------------------------------------------------------------------
   actStateSaved(): Acciones del STATE_saved
----------------------------------------------------------------------------------------------------------*/
void actStateSaved()
{
    static unsigned long previousTimeComidaSaved;   // Para regresar a STATE_Init si se ha querido guardar desde ahí, tras añadir o eliminar. 
                                                    // No se usa si la comida no se guarda porque estuviera vacía, eso sería AVISO.
        
    unsigned long currentTime;                      // Tiempo actual            
    
    static bool avisoComidaWasEmpty;                // Flag utilizada para saber si se debe mostrar la información "normal"
                                                    // o un mensaje de aviso indicando no se ha guardado la comida porque está vacía.

    bool hayConexionInternet;                       // Flag para saber si hay conexión a internet o no

    byte typeOfSavingDone;  // Tipo de guardado que se ha podido hacer:
                            //  - Completo (local y database)
                            //  - Solo local, error HTTP
                            //  - Solo local, sin WiFi
                            //  - Solo local, timeout en respuesta del esp32
                            //  - Solo local, error desconocido

    if(!doneState)
    {
      
        if(state_prev != STATE_saved)       // ==> Si no se viene del propio STATE_saved, para evitar que se vuelva a guardar la comida en el diario.
        {                                   // Se vendría de STATE_saved si al retirar el plato se detectara INCREMENTO o DECREMENTO antes de LIBERAR,
                                            // que ya llevaría a STATE_Init

                                            // *****
                                            // Primero se actualiza la comida, si es necesario, y después se muestran las actualizaciones.
                                            // Si tras comprobar cambios en la comida se ve que sigue vacía, entonces se indica en un mensaje 
                                            // que no se ha podido guardar.   
                                            // *****                  
          
            #if defined(SM_DEBUG)
                SerialPC.println(F("\nGuardando comida..."));  
            #endif

            /* ----- ACTUALIZAR PLATO  ----------------------------------- */
            if(pesoBascula != 0.0)               // ==> Si se ha colocado algo nuevo en la báscula y no se ha retirado,
            {                                    //     debe incluirse en el plato. 
                                                                    // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                                    // guardar la comida actual sin haber colocado un nuevo alimento, no haría falta
                                                                    // actualizar la comida, pues no se habría modificado.


                // ----- AÑADIR ALIMENTO A LISTA -----------------------------
                // Usamos 'grupoActual' porque no se ha modificado (no se ha pulsado otro grupo).
                if(grupoActual.ID_grupo != BARCODE_PRODUCT_INDEX)  // Si el grupo anterior del alimento pesado es de los nuestros, se escribe ALIMENTO,<grupo>,<peso> en la lista, 
                {                                 // siendo <grupo> el ID de 'grupoActual' y <peso> el valor de 'pesoBascula'.
                    listaComidaESP32.addAlimento(grupoActual.ID_grupo, pesoBascula);
                }
                else  // Si el grupo anterior es un barcode (grupo 50), se escribe ALIMENTO,<grupo>,<peso>,<ean>
                {
                    listaComidaESP32.addAlimentoBarcode(grupoActual.ID_grupo, pesoBascula, barcode);
                }

                #if defined(SM_DEBUG)
                    listaComidaESP32.leerLista();
                #endif 
                // -----------------------------------------------------------
                

                // ----- AÑADIR ALIMENTO A PLATO -----------------------------
                Alimento alimento(grupoActual, pesoBascula);      // Cálculo automático de valores nutricionales.
                                                                    // Usamos 'grupoActual' porque no se ha modificado (no se ha pulsado otro grupo).
                
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
            if(!platoActual.isPlatoEmpty())   // PLATO CON COSAS ==> INCLUIR EN COMIDA ACTUAL     
            {              

                                               // Si se viene de STATE_added o STATE_deleted, el plato ya estará vacío.

                pesoPlato = platoActual.getPesoPlato();         // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                comidaActual.addPlato(platoActual);             // Plato ==> Comida (no se vuelven a incluir los alimentos. Solo modifica peso y nPlatos)
                platoActual.restorePlato();                     // "Reiniciar" platoActual para usarlo de nuevo.
            }
            /* ----- FIN GUARDAR PLATO EN COMIDA -------------------------- */



            // ----- AVISO: COMIDA VACÍA -------------
            if(comidaActual.isComidaEmpty())
            { 
                if(state_prev != STATE_ERROR)   // COMIDA VACÍA ==> NO SE GUARDA
                {
                    // Saltaría aviso si se entra a este estado STATE_saved (pulsar dos veces el botón de guardar) y la comida está vacía.

                    // Para evitar marcar un aviso "falso", se chequea no venir de error:
                    //      Si se ha guardado la comida y, en lugar de retirar el plato, se comete un error (pulsando botón), al regresar a este
                    //      estado STATE_saved la comidaActual ya estará vacía porque se guardó y reinició anteriormente, por lo que daría aviso y 
                    //      regresaría al estado desde donde se inició el guardado. Todo eso aún con el plato puesto en la báscula, por lo que se 
                    //      podrían ir encadenando una serie de errores al retirarlo desde ese estado previo. Por eso solo se marca aviso si es la 
                    //      primera vez que se entra al estado STATE_saved, pero no si se entró a STATE_saved, se cometió error y se regresó a STATE_saved, 
                    //      pues ya no podría salir de ahí.
                
                    #if defined(SM_DEBUG)
                        SerialPC.println(F("No se ha guardado la comida porque está vacía. Pasando a STATE_AVISO..."));           
                    #endif 
                    
                    avisoComidaWasEmpty = true;
                    addEventToBuffer(AVISO_COMIDA_EMPTY_NOT_SAVED);   // Se transiciona al STATE_AVISO, que regresa automáticamente al estado desde donde se inició guardar comida
                    flagEvent = true;
                }
            }
            // ----- FIN AVISO: COMIDA VACÍA ---------  

            /* ----- GUARDAR COMIDA EN DIARIO (Y WEB) ----------------------------- */
            // Guardar comida localmente y enviarla a la base de datos, si hay conexión a internet
            else    // COMIDA CON PLATOS ==> HAY QUE GUARDAR
            {
                avisoComidaWasEmpty = false;                   // Para mostrar mensaje de comida guardada y no comida vacía

                // -----  INFORMACIÓN INICIAL MOSTRADA  -------------------------
                // 1. Elementos básicos de pantalla (texto "Guardando comida..." y comentarios)
                showSavingMeal_baseScreen();   

                // --- CONEXIÓN A INTERNET  -------------
                // Antes (previo al 20/09/24) se preguntaba por conexion a internet en saveComida(), pero mejor preguntarlo antes para poderlo indicar
                // en pantalla y luego pasarle si 'hayConexionInternet' a saveComida() para que guarde directamente solo en CSV y en TXT o intente subir a la database.
                hayConexionInternet = checkWifiConnection(); // Espera hasta 3 segundos a recibir respuesta "WIFI-OK" o "NO-WIFI"
                // -------------------------------------

                // 2. Completar pantalla con un mensaje e icono de si hay o no conexión a internet
                showSavingMeal_connectionState(hayConexionInternet);   
                delay(500); // Para que dé tiempo a leer el mensaje de conexión a internet o no
                // ----- FIN INFORMACIÓN INICIAL MOSTRADA -----------------------


                // --- COMIDA A DIARIO Y FICHEROS ---
                // 1. Añadir 'comidaActual' a 'diaActual' para actualizar el "Acumulado Hoy" en el dashboard estilo 1 (Comida Guardada | Acumulado Hoy)
                diaActual.addComida(comidaActual);          // Comida ==> Diario
                
                // 2. Guardar comida en CSV y database si hay conexión a internet o TXT si no la hay.
                typeOfSavingDone = saveComida(hayConexionInternet);     // Comida ==> fichero CSV y fichero TXT en SD o directamente a database 
                                                                        //  Puede haberse guardado en local y en la database (SAVE_EXECUTED_FULL) o solo en local con diferentes
                                                                        //  fallos (error HTTP, no WiFi, timeout o desconocido).
                                                                        //
                                                                        //  Si falla la conexión a la database, se guarda en local y se intentará más tarde, pero si falla el guardado local (CSV),
                                                                        //  se debe arreglar porque no se actualizará el acumulado!!!!!
                                                                        //
                                                                        //  Necesitamos saber qué tipo de guardado ha podido haber para mostrar un mensaje en pantalla

                if(typeOfSavingDone == SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI)
                    delay(1000); // Si no se ha intentado subir a database porque no hay wifi, se espera un poco para que dé tiempo a mostrar el mensaje de "Guardando comida..."
                                 //     Si no se hiciera este delay, no daría tiempo al usuario a entender que se está guardando la comida porque, por lo general, el guardado en 
                                 //     el CSV es instantáneo.
                                 // Se podría crear un estado intermedio STATE_saving entre STATE_save_check y STATE_saved, para dividir la funcionalidad de STATE_saved entre
                                 // las acciones de estar guardando y las de haber guardado, pero no es necesario. Posible futura mejor.
                                                            
                #if defined(SM_DEBUG)
                    if(typeOfSavingDone != SAVE_EXECUTED_FULL) // Si no se ha guardado en la database, mostrar por terminal el fichero con la comida por guardar
                        readFileTXT();                          
                #endif
                // -----------------------------------

                // --- ACTUALIZAR COPIA COMIDA Y REINICIAR COMIDA ACTUAL -------
                // 1. Copiar 'comidaActual' a 'comidaActualCopia' para mostrarla en el dashboard estilo 1 (Comida Guardada | Acumulado Hoy)
                comidaActualCopia.copyComida(comidaActual); // Copiar nº platos, peso y valores de la comida actual a la copia. Este objeto 'comidaActualCopia' de tipo 'Comida' 
                                                            // solo sirve para mostrar en el dashboard estilo 1 la comida guardada junto con el acumulado (Comida Guardada | Acumulado Hoy), 
                                                            // pues tras guardarla se limpia (restoreComida()). Por eso, se copia antes para poderla ver al regresar a STATE_Init.

                // 2. Reiniciar 'comidaActual' para poder usarla de nuevo
                comidaActual.restoreComida();               // "Reiniciar" comidaActual para usarla de nuevo.
                // ------------------------------------------------------------

                flagComidaSaved = true;                     // Se indica que se ha guardado la comida para que el dashboard estilo 1 en STATE_Init se muestre "Comida guardada" en lugar de "Comida actual". 
                                                            // De esta forma se entiende que los valores que se muestran son de la comida que se acaba de guardar. En cuanto se coloque recipiente, se entenderá
                                                            // que se ha comenzado otra comida, pasando ya a "Comida actual".
            
            
                /* -----  INFORMACIÓN FINAL MOSTRADA  ------------------------------- */             
                // Si no hubo aviso, se guardó la comida (en local y en la database si había conexión a internet)
                showAccionRealizada(typeOfSavingDone); // Si el guardado fue completo (local y database), se indica "Subido a web" en la esquina inferior izquierda.
                                                    // Si el guardado no fue completo, se indica por qué en la esquina inferior derecha (sin internet o error).

                if(lastValidState == STATE_Init) 
                    previousTimeComidaSaved = millis(); // Si se guardó la comida desde Init, entonces no habrá plato en la báscula, por lo que se obtiene el tiempo
                                                        //   para regresar a Init automáticamente tras unos segundos mostrando el mensaje de confirmación.
                /* ----- FIN INFO FINAL MOSTRADA ------------------------------------ */
            
            }             
            /* ----- FIN GUARDAR COMIDA EN DIARIO (Y WEB) ------------------------- */


            
        

        } // FIN if(state_prev != STATE_saved)

        
        doneState = true;                                           // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                                                    // cada vez que se entre a esta función debido al loop de Arduino.
                                                                    // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }
 


    // ----- TIEMPO DE ESPERA ---------------------------------------------
    // --- REGRESO A INIT TRAS GUARDAR EXITOSO ----------------
    // Si se inició el guardado desde Init (y no saltó aviso), no habrá plato en báscula, así que se regresa automáticamente a Init tras unos segundos.
    if(!avisoComidaWasEmpty)  // COMIDA CON COSAS --> se guardó
    {
        // Si se inició el guardado desde Init, no habrá plato en báscula, así que se regresa automáticamente a Init tras unos segundos.
        if(lastValidState == STATE_Init) // Si se viene directo de Init (pasando por save_check, claro) o a través de un error (como pulsar un botón en Init y durante el error pulsar GUARDAR).
        {
            unsigned long timeout;                   
            if(typeOfSavingDone == SAVE_EXECUTED_ONLY_DATABASE || typeOfSavingDone == ERROR_SAVING_DATA) timeout = 5000;  // Si ha fallado el guardado local (ERROR CRITICO), se deja más tiempo para el mensaje.
            else timeout = 3000; // Si no ha fallado el guardado local, se deja menos tiempo para el mensaje. Solo tienen que mirar la esquina inferior izquierda (éxito) o derecha (error en web).

            // Si se ha guardado desde cualquier otro estado distinto a Init, con algo sobre la báscula, la pantalla de confirmación de acción pedirá retirar el plato.
            // Este chequeo de tiempo solo es para regresar a STATE_Init si se ha guardado la comida desde STATE_Init.
            currentTime = millis();
            if ((currentTime - previousTimeComidaSaved) > timeout)     // Tras 'timeout' segundos mostrando confirmación de haber guardado...
            {
                #if defined(SM_DEBUG)
                    SerialPC.print(F("\nGO_TO_INIT forzada. Regreso a INI..."));
                #endif
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
void actStateERROR()
{ 
    static unsigned long previousTimeError;   // Tiempo usado para regresar al estado desde el que se cometió el error tras mostrar
                                              // una pantalla de error durante 3 segundos.
    unsigned long currentTime;                // Tiempo actual  

    // -----  INFORMACIÓN MOSTRADA  -------------------------------
    if(!doneState)
    {
        previousTimeError = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de error, regresar al estado anterior.

        #if defined(SM_DEBUG)
            SerialPC.println(F("\nMensaje de error")); 
        #endif

        // Mensaje de error según estado en el que se ha cometido el error (state_prev)
        switch (state_prev)
        {
            case STATE_Init:            showError(ERROR_STATE_INIT);           break;  // Init
            case STATE_Plato:           showError(ERROR_STATE_PLATO);           break;  // Plato
                    // Los errores que se pueden cometer en STATE_PLATO son pulsar crudo, cocinado, añadir, borrar o guardar.
                    // ¿¿Modificar el mensaje para que indique "No puede realizar esa acción. Seleccione grupo y después crudo/cocinado"??
                    // Puede que el mensaje actual no sea lo suficiente claro porque no indica específicamente qué se ha hecho mal.

            case STATE_Grupo: 
                    if(hasScaleEventOccurred() and (eventoBascula == INCREMENTO)){ // El error ha sido colocar alimento antes de escoger crudo/cocinado
                        keepErrorScreen = true;     // Para mantener la pantalla de error hasta que se escoja crudo/cocinado o se retire el plato para comenzar de nuevo.
                                                    // Este error no se trata como los demás (mostrando solo durante 3 seg) porque no es simplemente
                                                    // que no se pueda hacer, como pulsar un botón, sino que se debe rectificar o completar la acción. 
                    } 
                                        showError(ERROR_STATE_GROUP);          break;  

            case STATE_raw: case STATE_cooked:  // Desde STATE_raw y STATE_cooked se puede hacer cualquier cosa (báscula y botones), así que no debería saltar error,
                                                // pero se gestiona igualmente por si acaso.
                                        showError(ERROR_STATE_PROCESAMIENTO);   break;  // Crudo o Cocinado

            case STATE_weighted:        showError(ERROR_STATE_WEIGHTED);        break;  // Pesado

            // En los siguientes casos no se chequea si el error ha sido colocar alimento porque se entiende que es improbable que eso ocurra. Sin embargo, si ocurriera,
            // se marcaría error pero no se gestionaría esperando corregir la acción (quitar el peso), como en el caso de los grupos ==> POSIBLE MEJORA
            //
            // Solo se gestiona el fallo de colocar peso directamente desde STATE_Grupo porque es bastante probable 
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


            default:   showError(ERROR_GENERAL);    break;  // Solo muestra "¡Acción incorrecta!" (STATE_Barcode_read, STATE_Barcode_search, STATE_Barcode_check, STATE_Barcode)
        }
        
        doneState = true;                   // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                            // cada vez que se entre a esta función debido al loop de Arduino.
                                            // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }
    // ----- FIN INFO MOSTRADA ------------------------------------ 


    // -----  TRANSICIONES TRAS ERROR  ----------------------------
    if(!keepErrorScreen) // Transiciones automáticas tras error
    {
        // ----- TIEMPO DE ESPERA -------------------------------------
        // Solo para regresar si el error cometido ha sido pulsar un botón.
        // Si el error ha sido colocar algo en la báscula cuando no tocaba (p. ej. desde Grupos),
        // se mantiene la pantalla de error (keepErrorScreen = true) hasta que se retire. 
        currentTime = millis();
        if ((currentTime - previousTimeError) > 3000)     // Tras 3 segundos mostrando error...
        {
            // Regresar al estado anterior desde donde se cometió el error
            switch (state_prev)
            { 
                case STATE_Init:          
                                            #if defined(SM_DEBUG) 
                                                SerialPC.println(F("\nRegreso a Init tras ERROR...")); 
                                            #endif         
                                            addEventToBuffer(GO_TO_INIT);            break;  // Init

                case STATE_Plato:         
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a Plato tras ERROR..."));        
                                            #endif 
                                            addEventToBuffer(GO_TO_PLATO);            break;  // Plato

                case STATE_Grupo:        
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a Grupo tras ERROR..."));        
                                            #endif
                                            addEventToBuffer(GO_TO_GRUPO);            break;  // Grupo

                case STATE_Barcode_read:
                case STATE_Barcode_search:
                case STATE_Barcode_check:   // No se regresa a STATE_Barcode_read, STATE_Barcode_search o STATE_Barcode_check, sino al último estado válido desde donde se inició la lectura del barcode antes del error.
                    switch (lastValidState)
                    {
                        case STATE_Init:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Init tras ERROR durante lectura de barcode, busqueda del producto o confirmacion del producto..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_INIT);       break;  // Init

                        case STATE_Plato:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Plato tras ERROR durante lectura de barcode, busqueda del producto o confirmacion del producto..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_PLATO);       break;  // Plato

                        case STATE_Grupo:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Grupo tras ERROR durante lectura de barcode, busqueda del producto o confirmacion del producto..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_GRUPO);       break;  // Grupo

                        case STATE_Barcode:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Barcode tras ERROR durante lectura de barcode, busqueda del producto o confirmacion del producto..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_BARCODE);       break;  // Barcode

                        case STATE_raw:       
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a raw tras ERROR durante lectura de barcode, busqueda del producto o confirmacion del producto..."));         
                                                #endif 
                                                addEventToBuffer(GO_TO_RAW);         break;  // Crudo

                        case STATE_cooked:    
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a cooked tras ERROR durante lectura de barcode, busqueda del producto o confirmacion del producto..."));      
                                                #endif 
                                                addEventToBuffer(GO_TO_COOKED);      break;  // Cocinado

                        case STATE_weighted:    
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a weighted tras ERROR durante lectura de barcode, busqueda del producto o confirmacion del producto..."));    
                                                #endif 
                                                addEventToBuffer(GO_TO_WEIGHTED);    break;  // Pesado

                        default:  break;  
                    }
                    break;

                case STATE_Barcode:       
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a Barcode tras ERROR..."));      
                                            #endif 
                                            addEventToBuffer(GO_TO_BARCODE);          break;  // Barcode

                case STATE_raw:             // Desde STATE_raw se puede hacer cualquier cosa (báscula y botones), así que no debería saltar error,
                                            // pero se gestiona igualmente por si acaso.
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a raw tras ERROR..."));           
                                            #endif 
                                            addEventToBuffer(GO_TO_RAW);              break;  // Crudo

                case STATE_cooked:          // Desde STATE_cooked se puede hacer cualquier cosa (báscula y botones), así que no debería saltar error,
                                            // pero se gestiona igualmente por si acaso.
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a cooked tras ERROR..."));        
                                            #endif 
                                            addEventToBuffer(GO_TO_COOKED);           break;  // Cocinado

                case STATE_weighted:      
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a weighted tras ERROR..."));      
                                            #endif 
                                            addEventToBuffer(GO_TO_WEIGHTED);         break;  // Pesado

                case STATE_add_check:     
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a add_check tras ERROR..."));     
                                            #endif 
                                            addEventToBuffer(GO_TO_ADD_CHECK);        break;  // add_check

                case STATE_added:         
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a added tras ERROR..."));         
                                            #endif 
                                            addEventToBuffer(GO_TO_ADDED);            break;  // Added

                case STATE_delete_check:  
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a delete_check tras ERROR..."));  
                                            #endif 
                                            addEventToBuffer(GO_TO_DELETE_CHECK);     break;  // delete_check

                case STATE_deleted:       
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a deleted tras ERROR..."));       
                                            #endif 
                                            addEventToBuffer(GO_TO_DELETED);          break;  // Deleted

                case STATE_save_check:    
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a save_check tras ERROR..."));    
                                            #endif 
                                            addEventToBuffer(GO_TO_SAVE_CHECK);       break;  // save_check

                case STATE_saved:         
                                            #if defined(SM_DEBUG)
                                                SerialPC.println(F("\nRegreso a saved tras ERROR..."));         
                                            #endif 
                                            addEventToBuffer(GO_TO_SAVED);            break;  // Saved

              
                case STATE_CANCEL:   // No se regresa a STATE_CANCEL, sino al último estado válido desde donde se inició la acción que se ha cancelado antes del error.
                                     // Esto se hace para gestionar un posible error ocurrido durante el segundo que se está en el STATE_CANCEL, por improbable que sea.
                    switch (lastValidState)
                    {
                        case STATE_Init:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Init tras ERROR durante CANCELACION..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_INIT);       break;  // Init

                        case STATE_Plato:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Plato tras ERROR durante CANCELACION..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_PLATO);       break;  // Plato

                        case STATE_Grupo:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Grupo tras ERROR durante CANCELACION..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_GRUPO);       break;  // Grupo

                        case STATE_Barcode:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Barcode tras ERROR durante CANCELACION..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_BARCODE);       break;  // Barcode

                        case STATE_raw:       
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a raw tras ERROR durante CANCELACION..."));         
                                                #endif 
                                                addEventToBuffer(GO_TO_RAW);         break;  // Crudo

                        case STATE_cooked:    
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a cooked tras ERROR durante CANCELACION..."));      
                                                #endif 
                                                addEventToBuffer(GO_TO_COOKED);      break;  // Cocinado

                        case STATE_weighted:    
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a weighted tras ERROR durante CANCELACION..."));    
                                                #endif 
                                                addEventToBuffer(GO_TO_WEIGHTED);    break;  // Pesado

                        default:  break;  
                    }
                    break;

                case STATE_AVISO:   // No se regresa a STATE_AVISO, sino al último estado válido desde donde se inició la acción que ha llevado a un aviso antes del error.
                                    // Esto se hace para gestionar un posible error ocurrido durante los 3 segundos que se está en el STATE_AVISO, por improbable que sea.
                    switch (lastValidState)
                    {
                        case STATE_Init:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Init tras ERROR durante AVISO..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_INIT);       break;  // Init

                        case STATE_Plato:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Plato tras ERROR durante AVISO..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_PLATO);       break;  // Plato

                        case STATE_Grupo:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Grupo tras ERROR durante AVISO..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_GRUPO);       break;  // Grupo

                        case STATE_Barcode:     
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a Barcode tras ERROR durante AVISO..."));       
                                                #endif 
                                                addEventToBuffer(GO_TO_BARCODE);       break;  // Barcode

                        case STATE_raw:       
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a raw tras ERROR durante AVISO..."));         
                                                #endif 
                                                addEventToBuffer(GO_TO_RAW);         break;  // Crudo

                        case STATE_cooked:      
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a cooked tras ERROR durante AVISO..."));      
                                                #endif 
                                                addEventToBuffer(GO_TO_COOKED);      break;  // Cocinado

                        case STATE_weighted:  
                                                #if defined(SM_DEBUG)
                                                    SerialPC.println(F("\nRegreso a weighted tras ERROR durante AVISO..."));    
                                                #endif 
                                                addEventToBuffer(GO_TO_WEIGHTED);    break;  // Pesado

                        default:  break;  
                    }
                    break;

                default:   break;  
            }

            flagEvent = true;  // En todos los casos anteriores se marcará evento de transición para regresar al estado donde se cometió.  
            flagError = false; // Reiniciar flag de error hasta que se vuelva a cometer                    
        }
        // ----- FIN TIEMPO DE ESPERA -------------------------------------

        //
        // ----- EVENTOS OCURRIDOS DURANTE ERROR -------------------------------------
        //
        // Si no han pasado los 3 segundos y, mientras estamos en estado de error, se hace algo (pulsar botón o modificar peso), 
        // se hace lo que marque el evento ocurrido, si no es otro error (muy improbable).

        else if(eventOccurred()) // Evento de interrupción (botonera o báscula) durante pantalla de error. Hace checkBascula()
        {   
            // Realizar lo que pide el evento ocurrido durante error, según el estado desde donde se cometió el error.
            switch (state_prev)
            { 
                case STATE_Init:              
                    if(hasScaleEventOccurred()) // Ha habido evento en báscula
                    {
                        if(eventoBascula == INCREMENTO) // Se ha colocado el recipiente mientras se estaba en error
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nRecipiente colocado durante ERROR en STATE_Init..."));            
                                #endif 
                                addEventToBuffer(GO_TO_PLATO);        flagEvent = true;     break;
                        }
                    }
                    if(mainButtonInterruptOccurred()) // Ha habido pulsación en Main
                    {
                        checkMainButton();  // Qué botón del Main se ha pulsado
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

                        if(eventoMain == GUARDAR) // Se ha pulsado "Guardar comida" mientras se estaba en error. Se pasa a STATE_save_check para confirmar acción.
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nGuardar comida durante ERROR en STATE_Init..."));                  
                                #endif 
                                addEventToBuffer(GO_TO_SAVE_CHECK);   flagEvent = true;     break;
                        }
                    }
                    
                    break;  


                case STATE_Plato:  
                    if(hasScaleEventOccurred()) // Ha habido evento en báscula
                    {
                        if((eventoBascula == INCREMENTO) or (eventoBascula == DECREMENTO))
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nRecolocando recipiente durante ERROR en STATE_Plato..."));          
                                #endif 
                                addEventToBuffer(GO_TO_PLATO);        flagEvent = true;     break;
                        }
                        else if(eventoBascula == LIBERAR) // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nRecipiente retirado durante ERROR en STATE_Plato..."));             
                                #endif 
                                addEventToBuffer(GO_TO_INIT);         flagEvent = true;      break;
                        }
                    }
                    if(buttonInterruptOccurred()) // Ha habido pulsación en alguna botonera
                    {
                        if(grandeButtonInterruptOccurred()) // Ha habido pulsación en botonera Grande
                        {
                            checkGrandeButton(); // Qué botón de Grande se ha pulsado, necesario para saber grupo
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nGrupo de alimentos escogido durante ERROR en STATE_Plato..."));           
                            #endif 
                            addEventToBuffer(GO_TO_GRUPO);            flagEvent = true;      break;
                        }
                        else if(barcodeButtonInterruptOccurred()){ // Ha habido pulsación en botón Barcode
                            checkBarcodeButton();   // Marcar grupo Barcode
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nPidiendo barcode durante ERROR en STATE_Plato..."));           
                            #endif 
                            addEventToBuffer(GO_TO_BARCODE_READ);      flagEvent = true;  break;
                        }
                    }
                    break;  

                case STATE_Grupo:   
                    if(hasScaleEventOccurred()) // Ha habido evento en báscula
                    {
                        if(eventoBascula == DECREMENTO)
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nRetirando alimento durante ERROR en STATE_Grupo..."));             
                                #endif 
                                addEventToBuffer(GO_TO_GRUPO);      flagEvent = true;  break;
                        }
                        else if(eventoBascula == LIBERAR) // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nPlato retirado durante ERROR en STATE_Grupo..."));                 
                                #endif 
                                addEventToBuffer(GO_TO_INIT);        flagEvent = true;  break;
                        }
                    }
                    if(buttonInterruptOccurred()) // Ha habido pulsación en alguna botonera
                    {
                        if(mainButtonInterruptOccurred())  // Ha habido pulsación en Main
                        {
                            checkMainButton();  // Qué botón de Main se ha pulsado
                            if(eventoMain == CRUDO)
                            {
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nCRUDO escogido durante ERROR en STATE_Grupo..."));                 
                                    #endif 
                                    addEventToBuffer(GO_TO_RAW);          flagEvent = true;  break;
                            }
                            else if(eventoMain == COCINADO)
                            { 
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nCOCINADO escogido durante ERROR en STATE_Grupo..."));              
                                    #endif 
                                    addEventToBuffer(GO_TO_COOKED);       flagEvent = true;  break;
                            }
                        }
                        else if(grandeButtonInterruptOccurred()) // Ha habido pulsación en Grande
                        {
                            checkGrandeButton();    // Qué botón de Grande se ha pulsado
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nGrupo de alimentos escogido durante ERROR en STATE_Grupo..."));          
                            #endif 
                            addEventToBuffer(GO_TO_GRUPO);      flagEvent = true;  break;
                        }
                        else if(barcodeButtonInterruptOccurred()){ // Ha habido pulsación en botón Barcode
                            checkBarcodeButton();   // Marcar grupo Barcode
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nPidiendo barcode durante ERROR en STATE_Grupo..."));           
                            #endif 
                            addEventToBuffer(GO_TO_BARCODE_READ);      flagEvent = true;  break;
                        } 
                    }
                    break;  

                case STATE_Barcode_check:  
                    // No se chequea báscula porque sería provocar un error dentro de un error. Es improbable
                    if(buttonInterruptOccurred()) // Ha habido pulsación en alguna botonera
                    {
                        if(barcodeButtonInterruptOccurred())
                        {
                            #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nConfirmando producto durante ERROR en STATE_Barcode_check..."));    
                                #endif 
                            addEventToBuffer(GO_TO_BARCODE);  flagEvent = true;  break;
                        }
                        else // Ha habido pulsación en cualquiera de las otras botoneras
                        {
                            // Cancelar producto
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nCancelando producto durante ERROR en STATE_Barcode_check..."));     
                            #endif 
                            addEventToBuffer(GO_TO_CANCEL);  flagEvent = true;  break;
                        }
                        
                    }
                    break;

                case STATE_raw:  case STATE_cooked:   // STATE_raw o STATE_cooked
                    if(hasScaleEventOccurred()) // Ha habido evento en báscula
                    {
                        if(eventoBascula == INCREMENTO)
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nColocando alimento durante ERROR en STATE_raw o STATE_cooked..."));       
                                #endif 
                                addEventToBuffer(GO_TO_WEIGHTED);     flagEvent = true;  break;
                        }
                        else if(eventoBascula == LIBERAR) // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nPlato retirado durante ERROR en STATE_raw o STATE_cooked..."));           
                                #endif 
                                addEventToBuffer(GO_TO_INIT);        flagEvent = true;  break;
                        }
                    }
                    if(buttonInterruptOccurred()) // Ha habido pulsación en alguna botonera
                    {
                        if(mainButtonInterruptOccurred())  // Ha habido pulsación en Main
                        {
                            checkMainButton();  // Qué botón de Main se ha pulsado
                            switch (eventoMain)
                            {
                                case CRUDO:     
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nCRUDO escogido durante ERROR en STATE_raw o STATE_cooked..."));                 
                                                    #endif 
                                                    addEventToBuffer(GO_TO_RAW);          flagEvent = true;  break;

                                case COCINADO:       
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nCOCINADO escogido durante ERROR en STATE_raw o STATE_cooked..."));              
                                                    #endif 
                                                    addEventToBuffer(GO_TO_COOKED);       flagEvent = true;  break;

                                case ADD_PLATO:      
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nAñadir plato durante ERROR en STATE_raw o STATE_cooked..."));             
                                                    #endif 
                                                    addEventToBuffer(GO_TO_ADD_CHECK);    flagEvent = true;  break;

                                case DELETE_PLATO:  
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nBorrar plato durante ERROR en STATE_raw o STATE_cooked..."));             
                                                    #endif 
                                                    addEventToBuffer(GO_TO_DELETE_CHECK); flagEvent = true;  break;
                                
                                case GUARDAR:  
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nGuardar comida durante ERROR en STATE_raw o STATE_cooked..."));           
                                                    #endif 
                                                    addEventToBuffer(GO_TO_SAVE_CHECK);   flagEvent = true;  break;

                                default:  break;  
                            }
                        }
                        else if(grandeButtonInterruptOccurred()) // Ha habido pulsación en Grande
                        {
                            checkGrandeButton();    // Qué botón de Grande se ha pulsado
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nGrupo de alimentos escogido durante ERROR en STATE_raw o STATE_cooked..."));          
                            #endif 
                            addEventToBuffer(GO_TO_GRUPO);      flagEvent = true;  break;
                        }
                        else if(barcodeButtonInterruptOccurred()){ // Ha habido pulsación en botón Barcode
                            checkBarcodeButton();   // Marcar grupo Barcode
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nPidiendo barcode durante ERROR en STATE_raw o STATE_cooked..."));           
                            #endif 
                            addEventToBuffer(GO_TO_BARCODE_READ);      flagEvent = true;  break;
                        } 
                    }
                    break;


                case STATE_weighted:   
                    if(hasScaleEventOccurred()) // Ha habido evento en báscula
                    {
                        if((eventoBascula == INCREMENTO) or (eventoBascula == DECREMENTO))
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nColocando/retirando alimento durante ERROR en STATE_weighted...")); 
                                #endif 
                                addEventToBuffer(GO_TO_WEIGHTED);     flagEvent = true;  break;
                        }
                        else if(eventoBascula == LIBERAR) // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nPlato retirado durante ERROR en STATE_weighted..."));               
                                #endif 
                                addEventToBuffer(GO_TO_INIT);        flagEvent = true;  break;
                        }
                    }
                    if(buttonInterruptOccurred()) // Ha habido pulsación en alguna botonera
                    {
                        if(mainButtonInterruptOccurred()) // Ha habido pulsación en Main
                        {
                            checkMainButton();  // Qué botón de Main se ha pulsado
                            switch (eventoMain)
                            {
                                case CRUDO:     
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nCRUDO escogido durante ERROR en STATE_weighted..."));                 
                                                    #endif 
                                                    addEventToBuffer(GO_TO_RAW);          flagEvent = true;  break;

                                case COCINADO:       
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nCOCINADO escogido durante ERROR en STATE_weighted..."));              
                                                    #endif 
                                                    addEventToBuffer(GO_TO_COOKED);       flagEvent = true;  break;

                                case ADD_PLATO:      
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nAñadir plato durante ERROR en STATE_weighted..."));             
                                                    #endif 
                                                    addEventToBuffer(GO_TO_ADD_CHECK);    flagEvent = true;  break;

                                case DELETE_PLATO:  
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nBorrar plato durante ERROR en STATE_weighted..."));             
                                                    #endif 
                                                    addEventToBuffer(GO_TO_DELETE_CHECK); flagEvent = true;  break;
                                
                                case GUARDAR:  
                                                    #if defined(SM_DEBUG)
                                                        SerialPC.println(F("\nGuardar comida durante ERROR en STATE_weighted..."));           
                                                    #endif 
                                                    addEventToBuffer(GO_TO_SAVE_CHECK);   flagEvent = true;  break;

                                default:  break;  
                            }
                        }
                        else if(grandeButtonInterruptOccurred()) // Ha habido pulsación en Grande
                        {
                            checkGrandeButton();    // Qué botón de Grande se ha pulsado
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nGrupo de alimentos escogido durante ERROR en STATE_weighted..."));          
                            #endif 
                            addEventToBuffer(GO_TO_GRUPO);      flagEvent = true;  break;
                        }
                        else if(barcodeButtonInterruptOccurred()){ // Ha habido pulsación en botón Barcode
                            checkBarcodeButton();   // Marcar grupo Barcode
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nPidiendo barcode durante ERROR en STATE_weighted..."));           
                            #endif 
                            addEventToBuffer(GO_TO_BARCODE_READ);      flagEvent = true;  break;
                        } 
                    }
                    break;


                case STATE_add_check:  
                    // No se chequea báscula porque sería provocar un error dentro de un error. Es improbable
                    if(buttonInterruptOccurred()) // Ha habido pulsación en alguna botonera
                    {
                        if(mainButtonInterruptOccurred()) // Ha habido pulsación en Main
                        {
                            checkMainButton();  // Qué botón de Main se ha pulsado
                            if(eventoMain == ADD_PLATO) // Confirmando acción de añadir
                            {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nConfirmando añadir plato durante ERROR en STATE_add_check..."));    
                                #endif 
                                addEventToBuffer(GO_TO_ADDED);  flagEvent = true;  break;
                            }
                            else // Cualquier otro botón del Main cancela la acción
                            {
                                // No se incluye la cancelación por time-out porque eso solo ocurre si no hay actividad en STATE_add_check. Nunca va a ocurrir en STATE_ERROR, 
                                // aunque se cometiera el error desde add_check, porque tras 3 segundos se regresaría a STATE_add_check y allí ya se comprobaría el time-out.
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nCancelando añadir plato durante ERROR en STATE_add_check..."));     
                                #endif 
                                addEventToBuffer(GO_TO_CANCEL);  flagEvent = true;  break;
                            }
                        }
                        if(grandeButtonInterruptOccurred() or barcodeButtonInterruptOccurred()) // Ha habido pulsación en cualquiera de las otras botoneras
                        {
                            // Cancelar acción de añadir
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nCancelando añadir plato durante ERROR en STATE_add_check..."));     
                            #endif 
                            addEventToBuffer(GO_TO_CANCEL);  flagEvent = true;  break;
                        }
                    }
                    break;

              
                case STATE_added:   
                    if(hasScaleEventOccurred()) // Ha habido evento en báscula
                    {
                        if(eventoBascula == INCREMENTO)
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nRetirando alimento durante ERROR en STATE_added..."));              
                                #endif 
                                addEventToBuffer(GO_TO_ADDED);   flagEvent = true;  break;
                        }
                        else if(eventoBascula == LIBERAR) // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nPlato retirado durante ERROR en STATE_added..."));                  
                                #endif 
                                addEventToBuffer(GO_TO_INIT);   flagEvent = true;  break;
                        }
                    }
                    // No se chequean botones porque si se pulsan, se deben ignorar
                    break;
              
                case STATE_delete_check:  
                    // No se chequea báscula porque sería provocar un error dentro de un error. Es improbable
                    if(buttonInterruptOccurred()) // Ha habido pulsación en alguna botonera
                    {
                        if(mainButtonInterruptOccurred()) // Ha habido pulsación en Main
                        {
                            checkMainButton();  // Qué botón de Main se ha pulsado
                            if(eventoMain == DELETE_PLATO)
                            { 
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nConfirmando borrar plato durante ERROR en STATE_delete_check...")); 
                                #endif 
                                addEventToBuffer(GO_TO_DELETED);  flagEvent = true;  break;
                            }
                            else // Cualquier otro botón del Main cancela la acción
                            {
                                // No se incluye la cancelación por time-out porque eso solo ocurre si no hay actividad en STATE_delete_check. Nunca va a ocurrir en STATE_ERROR, 
                                // aunque se cometiera el error desde delete_check, porque tras 3 segundos se regresaría a STATE_delete_check y allí ya se comprobaría el time-out.
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nCancelando borrar plato durante ERROR en STATE_delete_check..."));  
                                #endif 
                                addEventToBuffer(GO_TO_CANCEL);   flagEvent = true;  break;
                            }
                        }
                        if(grandeButtonInterruptOccurred() or barcodeButtonInterruptOccurred()) // Ha habido pulsación en cualquiera de las otras botoneras
                        {
                            // Cancelar acción de eliminar
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nCancelando borrar plato durante ERROR en STATE_delete_check..."));  
                            #endif 
                            addEventToBuffer(GO_TO_CANCEL);   flagEvent = true;  break;
                        }
                    }
                    break;

              
                case STATE_deleted:  
                    if(hasScaleEventOccurred()) // Ha habido evento en báscula
                    {
                        if(eventoBascula == INCREMENTO)
                        {
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nRetirando alimento durante ERROR en STATE_deleted..."));            
                                #endif 
                                addEventToBuffer(GO_TO_DELETED);  flagEvent = true;  break;
                        }
                        else if(eventoBascula == LIBERAR) // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                        {
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nPlato retirado durante ERROR en STATE_deleted..."));                
                            #endif 
                            addEventToBuffer(GO_TO_INIT);    flagEvent = true;  break;
                        }
                    }
                    // No se chequean botones porque si se pulsan, se deben ignorar
                    break;
              
              case STATE_save_check: 
                  // No se chequea báscula porque sería provocar un error dentro de un error. Es improbable
                  if(buttonInterruptOccurred()) // Ha habido pulsación en alguna botonera
                  {
                        if(mainButtonInterruptOccurred()) // Ha habido pulsación en Main
                        {
                            checkMainButton();  // Qué botón de Main se ha pulsado
                            if(eventoMain == GUARDAR)
                            { 
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nConfirmando guardar comida durante ERROR en STATE_save_check...")); 
                                #endif 
                                addEventToBuffer(GO_TO_SAVED);    flagEvent = true;  break;
                            }
                            else // Cualquier otro botón del Main cancela la acción
                            {
                                // No se incluye la cancelación por time-out porque eso solo ocurre si no hay actividad en STATE_save_check. Nunca va a ocurrir en STATE_ERROR, 
                                // aunque se cometiera el error desde save_check, porque tras 3 segundos se regresaría a STATE_save_check y allí ya se comprobaría el time-out.
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nCancelando guardar comida durante ERROR en STATE_save_check...")); 
                                #endif 
                                addEventToBuffer(GO_TO_CANCEL);    flagEvent = true;  break;
                            }
                        }
                        if(grandeButtonInterruptOccurred() or barcodeButtonInterruptOccurred()) // Ha habido pulsación en cualquiera de las otras botoneras
                        {
                            // Cancelar acción de guardar
                            #if defined(SM_DEBUG)
                                SerialPC.println(F("\nCancelando guardar comida durante ERROR en STATE_save_check...")); 
                            #endif 
                            addEventToBuffer(GO_TO_CANCEL);    flagEvent = true;  break;
                        }
                    }
                  break;

              
              case STATE_saved:  
                  if(hasScaleEventOccurred()) // Ha habido evento en báscula
                  {
                      if(eventoBascula == INCREMENTO)
                      {
                          #if defined(SM_DEBUG)
                            SerialPC.println(F("\nRetirando alimento durante ERROR en STATE_saved..."));             
                          #endif 
                          addEventToBuffer(GO_TO_SAVED);     flagEvent = true;  break;
                      }
                      else if(eventoBascula == LIBERAR) // Esto es menos probable porque se tendría que detectar la retirada completa en una sola medida
                      {
                          #if defined(SM_DEBUG)
                            SerialPC.println(F("\nPlato retirado durante ERROR en STATE_saved..."));                 
                          #endif 
                          addEventToBuffer(GO_TO_INIT);     flagEvent = true;  break;
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
    else    // keepErrorScreen = true --> Se está manteniendo la pantalla de error para obligar a rectificar la acción de haber colocado alimento
    {       // cuando no tocaba (en Grupos).
            
            // -----
            // EN ESTA PARTE SE ATIENDE A INTERRUPCIONES QUE RECTIFICAN EL ERROR COMETIDO Y DEL QUE NO SE PUEDE SALIR AUTOMÁTICAMENTE,
            // SINO QUE EL USUARIO DEBE HACER ALGO PARA RESOLVERLO.
            // -----

            // Si se libera la báscula, se regresa a Init. 
            // Esta acción se gestiona de esta forma (marcando un GO_TO_INIT) por hacerlo de la misma forma que los casos generales
            // de eventos durante estado de error. Además, así se puede deshabilitar la 'flagError', para que no continúe marcando error.
            // Si se gestionara esta liberación con una simple regla de transición a Init con LIBERAR, no se podría deshabilitar
            // la 'flagError', lo que provocaría un falso mensaje de error al regresar a Init.
          
            // La ISR de la Scale salta cada 0.5 seg, entonces se puede no hacer checkBascula() y continuar con la ejecución para que se
            // haga el chequeo de eventos de báscula en el loop().
            //checkBascula();     // Comprueba interrupción de báscula. Lo necesito para ver si hace falta marcar evento

            // ------------------------------------------------------------------------------
            // ------- ERROR: Colocar alimento antes de escoger crudo o cocinado ------------
            // ------------------------------------------------------------------------------

            // ----- SOLUCIÓN 1: Retirar plato completo -------------------------------------
            if(hasScaleEventOccurred() and (eventoBascula == LIBERAR))
            {
                addEventToBuffer(GO_TO_INIT);        
                flagEvent = true;  // Marcar evento
                flagError = false; // Reiniciar flag de error hasta que se vuelva a cometer
            }
            // ------------------------------------------------------------------------------

            // ----- SOLUCION 2: Escoger crudo o cocinado, aunque ya esté el alimento --------
            // Esta solución es la más usada por los usuarios, la más clara
            if(mainButtonInterruptOccurred()) // Ha habido pulsación en botonera Main
            {
                checkMainButton(); // Qué botón de Main se ha pulsado
                if(eventoMain == CRUDO)
                {
                        #if defined(SM_DEBUG)
                            SerialPC.println(F("\nCRUDO escogido durante ERROR por PESO en STATE_Grupo")); 
                            SerialPC.println(F("\nAlimento crudo..."));       
                        #endif 
                        procesamiento = ALIMENTO_CRUDO;         
                }
                else if(eventoMain == COCINADO)
                { 
                        #if defined(SM_DEBUG)
                            SerialPC.println(F("\nCOCINADO escogido durante ERROR por PESO en STATE_Grupo")); 
                            SerialPC.println(F("\nAlimento cocinado..."));        
                        #endif
                        procesamiento = ALIMENTO_COCINADO;  
                }
                addEventToBuffer(GO_TO_WEIGHTED);   // Pasar directamente a STATE_weighted para mostrar la info de lo pesado según la cocción (crudo o cocinado)
                                                    // Si se pasara a STATE_raw o STATE_cooked, el sistema esperaría un incremento de peso para pasar a STATE_weighted,
                                                    // pero ese incremento ya ha ocurrido porque se ha colocado alimento antes, aunque haya resultado en un error, pero es
                                                    // un error ya subsanado.
                flagEvent = true;  // Marcar evento
                flagError = false; // Reiniciar flag de error hasta que se vuelva a cometer
            }
            // ------------------------------------------------------------------------------
    }

    // -----  FIN TRANSICIONES TRAS ERROR  ------------------------
}


/*---------------------------------------------------------------------------------------------------------
   actStateCANCEL(): Acciones del STATE_CANCEL
----------------------------------------------------------------------------------------------------------*/
void actStateCANCEL()
{ 
    static unsigned long previousTimeCancel;  // Para regresar al estado necesario
    unsigned long currentTime;               // Tiempo actual  

    // -----  INFORMACIÓN MOSTRADA  ------------------------
    if(!doneState)
    { 
        previousTimeCancel = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de acción cancelada, regresar al estado anterior.

        if(state_prev == STATE_Barcode_check)
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Producto cancelado")); 
            #endif

            //showProductoCancelado();        // Mostrar info de producto cancelado
            showCancel(PRODUCT_CANCELLED);  // Mostrar info de producto cancelado
        }
        else
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("Acción cancelada")); 
            #endif

            //showAccionCancelada();        // Mostrar info de acción cancelada
            showCancel(ACTION_CANCELLED);   // Mostrar info de acción cancelada
        }

        doneState = true;             // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                      // cada vez que se entre a esta función debido al loop de Arduino.
                                      // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.

    }
    // ----- FIN INFORMACIÓN MOSTRADA -----------------------


    // ----- TIEMPO DE ESPERA -------------------------------
    currentTime = millis();
    if ((currentTime - previousTimeCancel) > 1000)     // Tras 1 segundo mostrando acción cancelada...
    {
        // Regreso al estado desde donde se inició la acción ahora cancelada.

        // En lugar de mirar el último estado válido se comprueba el última estado por si se viene de STATE_DELETE_CSV_CHECK,
        // que no es válido, pero para regresar a Init si se cancela el borrado del CSV
        if(state_prev == STATE_DELETE_CSV_CHECK)
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("\nRegreso a STATE_Init tras CANCELACION de BORRAR CSV..."));      
            #endif 
            addEventToBuffer(GO_TO_INIT);
        }
        else
        {
            // Ultimo estado válido puede ser Init, Grupos, Barcode, raw, cooked o weighted. Es decir, cualquiera de los cuales desde donde se puede intentar un acción
            // cancelable (add/delete/save o leer barcode).
            switch (lastValidState)
            {
                case STATE_Init: // La única acción cancelable que se puede iniciar en STATE_Init es guardar la comida     
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a Init tras CANCELACION de guardar comida..."));       
                                    #endif 
                                    addEventToBuffer(GO_TO_INIT);       break;  // Init

                case STATE_Plato: // La única acción cancelable que se puede iniciar en STATE_Plato es leer barcode     
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a Plato tras CANCELACION de leer barcode..."));       
                                    #endif 
                                    addEventToBuffer(GO_TO_PLATO);       break;  // Plato

                case STATE_Grupo:  // Las acciones cancelables que se pueden iniciar en STATE_Grupo son add/delete/save o leer barcode
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a Grupos tras CANCELACION de add/delete/save o lectura de barcode..."));       
                                    #endif 
                                    addEventToBuffer(GO_TO_GRUPO);       break;  // Grupo

                case STATE_Barcode:  // Las acciones cancelables que se pueden iniciar en STATE_Barcode son add/delete/save o leer otro barcode
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a Barcode tras CANCELACION de add/delete/save o lectura de otro barcode..."));       
                                    #endif 
                                    addEventToBuffer(GO_TO_BARCODE);       break;  // Grupo

                case STATE_raw:   // Las acciones cancelables que se pueden iniciar en STATE_raw son add/delete/save o leer barcode    
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a raw tras CANCELACION de add/delete/save o lectura de barcode..."));         
                                    #endif 
                                    addEventToBuffer(GO_TO_RAW);         break;  // Crudo

                case STATE_cooked: // Las acciones cancelables que se pueden iniciar en STATE_cooked son add/delete/save o leer barcode   
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a cooked tras CANCELACION de add/delete/save o lectura de barcode..."));      
                                    #endif 
                                    addEventToBuffer(GO_TO_COOKED);      break;  // Cocinado

                case STATE_weighted: // Las acciones cancelables que se pueden iniciar en STATE_weighted son add/delete/save o leer barcode 
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a weighted tras CANCELACION de add/delete/save o lectura de barcode..."));    
                                    #endif 
                                    addEventToBuffer(GO_TO_WEIGHTED);    break;  // Pesado

                default:  break;  
            }
            
        }

        flagEvent = true; 
                  
    }
    // ----- FIN TIEMPO DE ESPERA ---------------------------
    

    // ----- EVENTOS OCURRIDOS DURANTE CANCELACIÓN ----------
    // La pantalla solo dura 1 segundo, no debería darle tiempo a los usuarios hacer nada. Además, no deben hacer nada.
    // Solo se va a gestionar el pulsar grupo1 para acceder al menú escondido de borrar el fichero csv.

    else if(grandeButtonInterruptOccurred()) // Evento de interrupción por botonera Grande durante pantalla de cancelación. 
    {
        checkGrandeButton(); // Qué botón de Grande se ha pulsado
        if(buttonGrande == 1) // Se ha pulsado grupo1
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("\ngrupo1 pulsado durante CANCELAR. Entrando a menú de BORRAR CSV..."));   
            #endif
            addEventToBuffer(DELETE_CSV);      
            flagEvent = true;  
        }
    }
    // ----- FIN EVENTOS OCURRIDOS DURANTE CANCELACIÓN ------
}


/*---------------------------------------------------------------------------------------------------------
   actStateAVISO(): Acciones del STATE_AVISO
----------------------------------------------------------------------------------------------------------*/
// Versión mirando el evento anterior
/*void actStateAVISO()
{ 
    static unsigned long previousTimeWarning;  // Para regresar al estado necesario
    unsigned long currentTime;                 // Tiempo actual  

    // -----  INFORMACIÓN MOSTRADA  ------------------------
    if(!doneState)
    { 
        previousTimeWarning = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de aviso, regresar al estado anterior.

        // Mostrar información según el estado en el que se realizó la acción que ha generado aviso (added/deleted/saved o barcode_not_read o product_not_found)
        switch(state_prev)
        { 
            case STATE_Barcode_read:
                                    showWarning(WARNING_BARCODE_NOT_READ);
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("No se ha detectado un barcode"));  
                                    #endif 
                                    break;  // Barcode_read

            case STATE_Barcode_search:
                                    showWarning(WARNING_PRODUCT_NOT_FOUND);
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("No se ha encontrado el producto"));  
                                    #endif 
                                    break;  // Barcode_search

            case STATE_Barcode:
                                    showWarning(WARNING_RAW_COOKED_NOT_NEEDED);
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("No hace falta escoger crudo o cocinado para el producto barcode"));  
                                    #endif 
                                    break;  // Barcode

            case STATE_added:       showWarning(WARNING_NOT_ADDED);    
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("No se ha creado otro plato porque el actual está vacío"));  
                                    #endif 
                                    break;  // added     

            case STATE_deleted:     showWarning(WARNING_NOT_DELETED);  
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("No se ha borrado el plato porque está vacío"));             
                                    #endif 
                                    break;  // deleted

            case STATE_saved:       showWarning(WARNING_NOT_SAVED);    
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("No se ha guardado la comida porque está vacía"));           
                                    #endif 
                                    break;  // saved

            default:  break;  
        }

        doneState = true;             // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                      // cada vez que se entre a esta función debido al loop de Arduino.
                                      // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.

    }
    // ----- FIN INFORMACIÓN MOSTRADA -----------------------


    // ----- TIEMPO DE ESPERA -------------------------------
    currentTime = millis();
    if ((currentTime - previousTimeWarning) > 3000) // Tras 3 segundos mostrando warning...
    {
        // Ultimo estado válido puede ser Init, Plato, Grupos, Barcode, raw, cooked o weighted. Es decir, cualquiera de los cuales desde donde 
        // se puede intentar un acción que pueda marcar aviso (add/delete/save o leer barcode).
        switch (lastValidState)
        {
            case STATE_Init:    
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nGO_TO_INIT forzada. Regreso a STATE_Init tras AVISO de save en STATE_Init..."));    
                                #endif 
                                addEventToBuffer(GO_TO_INIT);       break;  // Init

            case STATE_Plato:
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nGO_TO_PLATO forzado. Regreso a State_Plato tras AVISO de \"Barcode no leido\" o \"Producto no encontrado\" en State_Plato..."));
                                #endif
                                addEventToBuffer(GO_TO_PLATO);      break;  // Plato

            case STATE_Grupo: case STATE_raw: case STATE_cooked:  
                // Si el aviso ha sido de plato o comida vacía, se regresa a STATE_Grupo para escoger crudo/cocinado y conformar el plato.
                // Si el aviso ha sido de no leer barcode, se regresa a STATE_Grupo con el 'grupoActual' del último grupo que se escogió. 
                //      También se podría separar esta opción y que regrese a STATE_Plato para indicar que no se tiene grupo ni barcode escogido.
                                // Se mantiene peso del recipiente.
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nGO_TO_GRUPO forzado. Regreso a State_Grupo tras AVISO de add/delete/save o \"Barcode no leido\" o \"Producto no encontrado\" en State_Grupo, STATE_raw o STATE_cooked..."));
                                #endif
                                addEventToBuffer(GO_TO_GRUPO);      break;  // Grupo

            case STATE_weighted:  
                // Si el aviso ha sido de plato o comida vacía, se regresa a STATE_Plato para escoger grupo o barcode y formar el plato.
                // Si el aviso ha sido de no leer barcode, se regresa a STATE_Plato para indicar que no se tiene escogido un grupo ni barcode.
                //      Si hubiera algo en la báscula al intentar leer barcode, se habría guardado el alimento en el plato y la comida al entrar
                //      en STATE_Barcode_read para intentar leer el barcode, por lo que no se perdería nada.   
                                // Se mantiene peso del recipiente.
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nGO_TO_PLATO forzado. Regreso a State_Plato tras AVISO de add/delete/save o \"Barcode no leido\" o \"Producto no encontrado\" en STATE_weighted..."));
                                #endif
                                addEventToBuffer(GO_TO_PLATO);      break;  // Plato

            case STATE_Barcode:
                // Si el aviso ha sido de plato o comida vacía, se regresa a STATE_Barcode y se mantiene el 'grupoActual' con el barcode leído antes de intentar add/delete/save.
                // Si el aviso ha sido que no se pudo leer otro barcode (tras ya haber confirmado uno al entrar a STATE_Barcode), se regresa a STATE_Barcode y se mantiene el
                //      'grupoActual' con el barcode leído antes de intentar leer otro barcode.
                // Si el aviso ha sido de no encontrar el producto, se regresa a STATE_Barcode y se mantiene el 'grupoActual' con el barcode leído antes de intentar leer otro barcode.
                // Si el aviso ha sido de no necesitar crudo/cocinado, se regresa a STATE_Barcode y se mantiene el 'grupoActual' con el barcode leído antes de intentar raw/cooked.
                                // Se mantiene peso del recipiente.
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nBARCODE forzado. Regreso a State_Barcode tras AVISO de add/delete/save o \"Barcode no leido\" o \"Producto no encontrado\"  o \"No hace falta crudo/cocinado\" en State_Barcode..."));
                                #endif
                                addEventToBuffer(GO_TO_BARCODE);      break;  // Barcode
            
            default:  break;  
        }
        flagEvent = true;
    }
    // ----- FIN TIEMPO DE ESPERA ---------------------------

}*/

// En lugar de mirar el estado anterior en el que se marcó el evento AVISO, se comprueba el último evento,
// que ahora se ha dividido en:
// AVISO_PLATO_EMPTY_NOT_ADDED      --> STATE_added
// AVISO_PLATO_EMPTY_NOT_DELETED    --> STATE_deleted
// AVISO_COMIDA_EMPTY               --> STATE_saved
// AVISO_NO_WIFI_BARCODE            --> STATE_Barcode_read
// AVISO_NO_BARCODE                 --> STATE_Barcode_read
// AVISO_PRODUCT_NOT_FOUND          --> STATE_Barcode_search

// Si el aviso ha sido escoger crudo o cocinado para un producto barcode, no se marca un evento especial de aviso, sino que se
// utiliza el propio evento CRUDO o COCINADO para pasar de STATE_Barcode a STATE_AVISO.
void actStateAVISO()
{ 
    static unsigned long previousTimeWarning;  // Para regresar al estado necesario
    unsigned long currentTime;                 // Tiempo actual  

    // -----  INFORMACIÓN MOSTRADA  ------------------------
    if(!doneState)
    { 
        previousTimeWarning = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de aviso, regresar al estado anterior.

        // Mostrar información según el evento que ha llevado al aviso (AVISO_PLATO_EMPTY_NOT_ADDED, AVISO_PLATO_EMPTY_NOT_DELETED, AVISO_COMIDA_EMPTY, AVISO_NO_WIFI_BARCODE, AVISO_NO_BARCODE, AVISO_PRODUCT_NOT_FOUND)
        switch(lastEvent) 
        { 
            case AVISO_PLATO_EMPTY_NOT_ADDED:       showWarning(WARNING_NOT_ADDED);                 break;  // No se ha añadido plato en STATE_added porque el actual está vacío     
            case AVISO_PLATO_EMPTY_NOT_DELETED:     showWarning(WARNING_NOT_DELETED);               break;  // No se ha borrado el plato en STATE_deleted porque está vacío 
            case AVISO_COMIDA_EMPTY_NOT_SAVED:      showWarning(WARNING_NOT_SAVED);                 break;  // No se ha guardado la comida en STATE_saved porque está vacía
            //case AVISO_NO_WIFI_BARCODE:             showWarning(WARNING_NO_INTERNET_NO_BARCODE);    break;  // No había conexión a Internet, así que no se puede leer barcode en STATE_Barcode_read
            case AVISO_NO_WIFI_BARCODE:             showWarning(WARNING_NO_INTERNET_NO_BARCODE);    break;  // No había conexión a Internet, así que no se puede buscar el producto en STATE_Barcode_search
            case AVISO_NO_BARCODE:                  showWarning(WARNING_BARCODE_NOT_READ);          break;  // No se ha detectado un barcode en 10 segundos en STATE_Barcode_read
            case AVISO_PRODUCT_NOT_FOUND:           showWarning(WARNING_PRODUCT_NOT_FOUND);         break;  // No se ha encontrado el producto en OpenFoodFacts en STATE_Barcode_search

            // Se ha pulsado crudo/cocinado en STATE_Barcode, pero un producto barcode no necesita diferenciar entre crudo o cocinado
            case CRUDO: 
            case COCINADO:
                                    showWarning(WARNING_RAW_COOKED_NOT_NEEDED);
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("No hace falta escoger crudo o cocinado para el producto barcode"));  
                                    #endif 
                                    break;  // Barcode


            default:  break;  
        }

        doneState = true;             // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                      // cada vez que se entre a esta función debido al loop de Arduino.
                                      // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.

    }
    // ----- FIN INFORMACIÓN MOSTRADA -----------------------


    // ----- TIEMPO DE ESPERA -------------------------------
    currentTime = millis();
    if ((currentTime - previousTimeWarning) > 3000) // Tras 3 segundos mostrando warning...
    {
        // Ultimo estado válido puede ser Init, Plato, Grupos, Barcode, raw, cooked o weighted. Es decir, cualquiera de los cuales desde donde 
        // se puede intentar un acción que pueda marcar aviso (added/deleted/saved o no_wifi_barcode o barcode_not_read o product_not_found).
        switch (lastValidState)
        {
            case STATE_Init:    
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nGO_TO_INIT forzada. Regreso a STATE_Init tras AVISO de save en STATE_Init..."));    
                                #endif 
                                addEventToBuffer(GO_TO_INIT);       break;  // Init

            case STATE_Plato:
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nGO_TO_PLATO forzado. Regreso a State_Plato tras AVISO de \"Sin Internet para barcode\", \"Barcode no leido\" o \"Producto no encontrado\" en State_Plato..."));
                                #endif
                                addEventToBuffer(GO_TO_PLATO);      break;  // Plato

            case STATE_Grupo: case STATE_raw: case STATE_cooked: case STATE_weighted: 
                // - Si el aviso ha sido de plato o comida vacía, se regresa a STATE_Plato para escoger grupo o barcode y formar el plato.
                // - Si el aviso ha sido de no leer barcode o no encontrar producto, se regresa a STATE_Plato para indicar que no se tiene escogido un grupo ni barcode.
                //      Si hubiera algo en la báscula al intentar leer barcode, si hubiera sido el caso, se habría guardado el alimento en el plato 
                //      y la comida al entrar en STATE_Barcode_read para intentar leer el barcode, por lo que no se perdería nada.   
                                // Se mantiene peso del recipiente.
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nGO_TO_PLATO forzado. Regreso a State_Plato tras AVISO de add/delete/save o \"Sin Internet para barcode\" o \"Barcode no leido\" o \"Producto no encontrado\" en STATE_Grupo, STATE_raw, STATE_cooked o STATE_weighted..."));
                                #endif
                                addEventToBuffer(GO_TO_PLATO);      break;  // Plato

            case STATE_Barcode:
                // - Si el aviso ha sido de plato o comida vacía, se regresa a STATE_Barcode y se mantiene el 'grupoActual' con el barcode leído antes de intentar add/delete/save.
                // - Si el aviso ha sido que no se pudo leer otro barcode (por no detectado o no wifi), tras ya haber confirmado uno al entrar a STATE_Barcode, se regresa a STATE_Barcode 
                //      y se mantiene el 'grupoActual' con el barcode leído antes de intentar leer otro barcode.
                // - Si el aviso ha sido de no encontrar el producto buscado tras ya haber confirmado uno antes al entrar a STATE_Barcode, se regresa a STATE_Barcode y se mantiene el 'grupoActual' 
                //      con el barcode leído antes de intentar leer otro barcode.
                // - Si el aviso ha sido de no necesitar crudo/cocinado, se regresa a STATE_Barcode y se mantiene el 'grupoActual' con el barcode leído antes de intentar raw/cooked.
                                // Se mantiene peso del recipiente.
                                #if defined(SM_DEBUG)
                                    SerialPC.println(F("\nBARCODE forzado. Regreso a State_Barcode tras AVISO de add/delete/save o \"Barcode no leido\" o \"Producto no encontrado\"  o \"No hace falta crudo/cocinado\" en State_Barcode..."));
                                #endif
                                addEventToBuffer(GO_TO_BARCODE);      break;  // Barcode
            
            default:  break;  
        }
        flagEvent = true;
    }
    // ----- FIN TIEMPO DE ESPERA ---------------------------

}



/*---------------------------------------------------------------------------------------------------------
   actState_DELETE_CSV_CHECK(): Acciones del STATE_DELETE_CSV_CHECK
----------------------------------------------------------------------------------------------------------*/
void actState_DELETE_CSV_CHECK()
{ 

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

    if(!doneState)
    {
        previousTimeCancelDeleteCSV = millis(); 

        #if defined(SM_DEBUG)
            SerialPC.println(F("\n¿SEGURO QUE QUIERE BORRAR EL FICHERO CSV?")); 
        #endif
        
        pedirConfirmacion_DELETE_CSV();       // Mostrar pregunta de confirmación para BORRAR CSV
                                              //    No se ha incluido esta pantalla en pedirConfirmacion(option) porque interesa
                                              //    tenerlo separado para eliminarlo tras las pruebas


        doneState = true;               // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                        // cada vez que se entre a esta función debido al loop de Arduino.
                                        // Así, debe ocurrir un nuevo evento que lleve a este estado para que se "repitan" las acciones.
    }

    
  

    // ----- CONFIRMACIÓN DE ACCIÓN ----------
    if(buttonInterruptOccurred()) // Pulsación de botón
    {
        checkAllButtons(); // Qué botón se ha pulsado
        //if(grandeButtonInterruptOccurred() && (buttonGrande == 20)) // Si se pulsa el grupo 20, se confirma el borrado.
        if(buttonGrande == 20)
        {                                                       
            #if defined(SM_DEBUG)
                SerialPC.println(F("\ngrupo20 pulsado durante DELETE_CSV_CHECK. Iniciando borrado..."));   
            #endif
            addEventToBuffer(DELETE_CSV);      
            flagEvent = true; 
        }
        /*else // Cualquier otro botón (grupo o acción), cancela el borrado
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("\nCANCELAR borrado de fichero CSV."));   
            #endif
            addEventToBuffer(CANCELAR);      
            flagEvent = true; 
        }*/
    }
    // ----- FIN CONFIRMACIÓN DE ACCIÓN ------


    // ----- CANCELACIÓN AUTOMÁTICA -------------------------------
    else
    {
        currentTime = millis(); // Tomar tiempo para saber si han pasado los 5 seg para cancelar la acción
        if ((currentTime - previousTimeCancelDeleteCSV) > 5000)     // Tras 5 segundos de inactividad, se cancela automáticamente la acción BORRAR CSV
        {
            // Regreso al estado desde donde se inició la acción cancelada previo a entrar a este menú.

            // Ultimo estado válido puede ser Init, raw, cooked o weighted. Es decir, cualquiera de los cuales desde donde se puede intentar un acción
            // cancelable (add/delete/save), tras cuya cancelación se habría entrado a este menú.
            switch (lastValidState)
            {
              case STATE_Init:     
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a Init tras cancelar automáticamente BORRAR CSV..."));       
                                    #endif 
                                    addEventToBuffer(GO_TO_INIT);       break;  // Init

              case STATE_raw:       
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a raw tras cancelar automáticamente BORRAR CSV..."));         
                                    #endif 
                                    addEventToBuffer(GO_TO_RAW);         break;  // Crudo

              case STATE_cooked:        
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a cooked tras cancelar automáticamente BORRAR CSV..."));      
                                    #endif 
                                    addEventToBuffer(GO_TO_COOKED);      break;  // Cocinado

              case STATE_weighted:  
                                    #if defined(SM_DEBUG)
                                        SerialPC.println(F("\nRegreso a weighted tras cancelar automáticamente BORRAR CSV..."));    
                                    #endif 
                                    addEventToBuffer(GO_TO_WEIGHTED);    break;  // Pesado

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
void actState_DELETED_CSV()
{ 

    // ------------ FUNCIONAMIENTO BORRADO FICHERO CSV ----------------------------------------------------
    //
    // ESTO SOLO SE VA A REALIZAR EN LAS PRUEBAS, PARA AGILIZAR EL RESETEO DEL FICHERO ENTRE UN PACIENTE Y OTRO.
    // 
    // Ya se ha confirmado la acción en STATE_DELETE_CSV_CHECK. Ahora se realiza el borrado.
    //
    // -----------------------------------------------------------------------------------------------------

    static unsigned long previousTimeDeleted;  // Para regresar a Init tras borrado (o no)
    unsigned long currentTime;                 // Tiempo actual  

    if(!doneState)
    {
        previousTimeDeleted = millis();   // Reiniciar "temporizador" de 3 segundos para, tras mostrar pantalla de borrado, regresar a Init

        #if defined(SM_DEBUG)
            SerialPC.println(F("\nBORRANDO CONTENIDO DEL FICHERO CSV Y FICHERO ESP32. LIMPIANDO ACUMULADO...")); 
        #endif

        if(deleteFileCSV() && deleteFileTXT()) showAcumuladoBorrado(true); // true = éxito en el borrado
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
    if ((currentTime - previousTimeDeleted) > 2000)     // Tras 3 segundos mostrando warning...
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nRegreso a Init tras BORRAR CSV..."));     
        #endif  
        addEventToBuffer(GO_TO_INIT);
        flagEvent = true; 
    }
    // ----- FIN REGRESO A INIT TRAS BORRADO --
}


/*---------------------------------------------------------------------------------------------------------
   actState_CRITIC_FAILURE_SD(): Acciones del STATE_CRITIC_FAILURE_SD
----------------------------------------------------------------------------------------------------------*/
void actState_CRITIC_FAILURE_SD()
{ 

    // -----  INFORMACIÓN MOSTRADA  ------------------------
    if(!doneState)
    { 
        #if defined(SM_DEBUG)
            SerialPC.println(F("FALLO CRÍTICO EN LA SD")); 
        #endif

        showCriticFailureSD();        // Mostrar info de fallo crítico en SD

        doneState = true;             // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                      // cada vez que se entre a esta función debido al loop de Arduino.
    }
    // ----- FIN INFORMACIÓN MOSTRADA -----------------------

    // NO SE ATIENDE A NINGÚN EVENTO PORQUE SE IMPIDE USAR SMARTCLOTH
}


/*---------------------------------------------------------------------------------------------------------
   actState_UPLOAD_DATA(): Acciones del STATE_UPLOAD_DATA
   Se pasa a este estado si hay algo que subir, osea que esa comprobación ya se ha hecho antes 
   de entrar aquí.
----------------------------------------------------------------------------------------------------------*/
void actState_UPLOAD_DATA()
{ 
    // En este caso sí podemos usar delay (espera bloqueante) porque no atenderemos a las acciones del usuario,
    // por lo que no hay que estar pendiente de interrupciones. 

    bool showingScreen = false; // Se muestra pantalla en caso de que haya algo que subir y se pueda subir (WiFi)

    if(!doneState) // Creo que en este caso no hace falta, pero por si acaso
    {
        // -----  INFORMACIÓN MOSTRADA  ------------------------
        #if defined(SM_DEBUG)
            SerialPC.println("\n--------------------------------------------------");
            SerialPC.println("\n++++++++++++++++++++++++++++++++++++++++++++++++++");
            SerialPC.println(F("\nDATA EN EL TXT")); 
        #endif

        if(checkWifiConnection()) // Hay WiFi 
        {
            // -----  INFORMACIÓN MOSTRADA  -------------------------
            showSyncState(UPLOADING_DATA); // Sincronizando data del SM con web
            // ----- FIN INFORMACIÓN MOSTRADA -----------------------

            // -----  ENVIAR INFORMACION AL ESP32  ------------------
            // Avisar al ESP32 de que le va a enviar info. El ESP32 debe autenticarse en database y responder
            byte responseFromESP32ToSavePrompt = prepareSaving(); 

            // ----- ESP32 EN ESPERA ----------------------------------
            if(responseFromESP32ToSavePrompt == WAITING_FOR_DATA) // El ESP32 se ha autenticado correctamente en la database
            {
                // ----- ENVIAR TXT A ESP32 Y ESPERAR RESPUESTA --
                // Enviar el fichero TXT línea a línea y esperar respuesta de la subida
                byte uploadResult = sendTXTFileToESP32ToUpdateWeb(); 

                // ----- MOSTRAR PANTALLA SEGÚN RESPUESTA --------
                // Mostrar en pantalla el resultado de la subida de datos
                if(uploadResult == MEALS_LEFT) showWarning(WARNING_MEALS_LEFT);  // MEALS_LEFT. No se han subido todas las comidas
                else showSyncState(uploadResult); // ALL_MEALS_UPLOADED, ERROR_READING_TXT
            }
            // ------- FIN ESP32 EN ESPERA ----------------------------
            // ------ ESP32 NO RESPONDE O FALLA AUTENTICACIÓN ---------
            else // Al avisar del guardado, ha fallado la comunicación con el ESP32 o la autenticación
            {
                // -- MOSTRAR PANTALLA DE ERROR AL INDICAR GUARDADO --
                // Mostrar en pantalla el fallo al indicar que se va a guardar
                showSyncState(responseFromESP32ToSavePrompt); // NO_INTERNET_CONNECTION, HTTP_ERROR (al autenticarse en web), TIMEOUT, UNKNOWN_ERROR
            }
            // --------------------------------------------------------
    
            // El delay y el GO_TO_INIT se hace al final para todos los casos, ya que se ha mostrado pantalla de sincronización

            showingScreen = true; // Se está mostrando pantalla. Esperar 5 segundos antes de pasar a Init
            // ------------------------------------------------------
        }

        // Pasar a Init para todos los casos:
        // - No había WiFi (NO_INTERNET_CONNECTION)
        // - Había WiFi y se subió OK (ALL_MEALS_UPLOADED)
        // - Había WiFi pero falló la subida (HTTP_ERROR). Esto se mira en handleResponseFromESP32AfterUpload()
        if(showingScreen) delay(5000); // Si se estaba mostrando pantalla de sincronización, se esperan 3 segundos con "SmartCloth sincronizado" en pantalla
        addEventToBuffer(GO_TO_INIT);
        flagEvent = true;
        

        doneState = true;               // Solo realizar una vez las actividades del estado por cada vez que se active y no
                                        // cada vez que se entre a esta función debido al loop de Arduino.
    }
    
}




/*---------------------------------------------------------------------------------------------------------
   doStateActions(): Acciones a realizar según el estado actual
----------------------------------------------------------------------------------------------------------*/
void doStateActions()
{
    switch (state_actual)
    {
        case STATE_Init:                actStateInit();             break;  // Init
        case STATE_Plato:               actStatePlato();            break;  // Plato
        case STATE_Grupo:               actGruposAlimentos();       break;  // Grupo
        case STATE_Barcode_read:        actStateBarcodeRead();      break;  // Barcode_read
        case STATE_Barcode_search:      actStateBarcodeSearch();    break;  // Barcode_search
        case STATE_Barcode_check:       actStateBarcodeCheck();     break;  // Barcode_check
        case STATE_Barcode:             actStateBarcode();          break;  // Barcode
        case STATE_raw:                 actStateRaw();              break;  // raw
        case STATE_cooked:              actStateCooked();           break;  // cooked
        case STATE_weighted:            actStateWeighted();         break;  // weighted
        case STATE_add_check:           actStateAddCheck();         break;  // add_check
        case STATE_added:               actStateAdded();            break;  // added
        case STATE_delete_check:        actStateDeleteCheck();      break;  // delete_check
        case STATE_deleted:             actStateDeleted();          break;  // deleted
        case STATE_save_check:          actStateSaveCheck();        break;  // save_check
        case STATE_saved:               actStateSaved();            break;  // saved
        case STATE_ERROR:               actStateERROR();            break;  // ERROR
        case STATE_CANCEL:              actStateCANCEL();           break;  // CANCEL
        case STATE_AVISO:               actStateAVISO();            break;  // AVISO

        case STATE_DELETE_CSV_CHECK:  actState_DELETE_CSV_CHECK();   break; // delete_csv_check
        case STATE_DELETED_CSV:       actState_DELETED_CSV();        break; // deleted_csv

        case STATE_CRITIC_FAILURE_SD: actState_CRITIC_FAILURE_SD();  break; // Fallo crítico en SD

        case STATE_UPLOAD_DATA:       actState_UPLOAD_DATA();         break; // Hay data en el TXT por subir a database

      default: break;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actEventError(): Marca evento de ERROR por no cumplirse ninguna regla de transición
----------------------------------------------------------------------------------------------------------*/
void actEventError()
{
    // Independientemente del estado actual, se marca el evento ERROR para pasar al STATE_ERROR, donde 
    // se muestra una pantalla de error según el estado previo desde donde se cometió el error. 
    // Tras 3 segundos con la pantalla de error, se marca el evento GO_TO_<estado> correspondiente para 
    // regresar al estado previo.

    #if defined(SM_DEBUG)
        SerialPC.println(F("\nERROR..."));
    #endif
    addEventToBuffer(ERROR);    
    flagError = true;
}


/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- BUFFER EVENTOS -------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------
   isBufferEmpty(): Comprobar si el buffer está vacío, es decir, si el primer hueco lo ocupa un evento NONE.
----------------------------------------------------------------------------------------------------------*/
bool isBufferEmpty()
{
    if(event_buffer[0] == NONE) return true;
    else return false;
}


/*---------------------------------------------------------------------------------------------------------
   isBufferFull(): Comprobar si el buffer está lleno, es decir, si no hay ningún hueco (evento NONE).
----------------------------------------------------------------------------------------------------------*/
bool isBufferFull()
{
    for (byte i = 0; i < MAX_EVENTS; i++){
        if (event_buffer[i] == NONE){           // Hay algún hueco         
            return false;
        }
    }     
    return true;
}


/*---------------------------------------------------------------------------------------------------------
   getFirstGapBuffer(): Primer hueco libre del buffer
----------------------------------------------------------------------------------------------------------*/
byte getFirstGapBuffer()
{
    for (byte i = 0; i < MAX_EVENTS; i++){
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
void shiftLeftEventBuffer()
{ 
    //SerialPC.println(F("\nRotando buffer de eventos..."));
    for (byte i = 0; i < MAX_EVENTS; i++){
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
void addEventToBuffer(event_t evento)
{
    #if defined SM_DEBUG
        SerialPC.println("\n\n--------------------------------------------------");
        SerialPC.println("--------------------------------------------------");
        SerialPC.println("      +++++++++++++++++++++++++++++++++++++++     ");
        SerialPC.println("      EVENTO --> ACTIVANDO MÁQUINA DE ESTADOS      ");
        SerialPC.println("      +++++++++++++++++++++++++++++++++++++++     ");
    #endif

    byte pos;
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

    #if defined(SM_DEBUG)
        SerialPC.print("\nBuffer: "); 
        for (byte i = 0; i < MAX_EVENTS; i++){
            //SerialPC.print(event_buffer[i]); SerialPC.print(" ");
            printEventName(event_buffer[i]); SerialPC.print(" | ");
        }
        SerialPC.println();
        SerialPC.print("Last event: "); printEventName(lastEvent);
        SerialPC.println();
    #endif
}






/******************************************************************************/
/******************************************************************************/


#endif
