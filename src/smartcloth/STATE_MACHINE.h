#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define MAX_EVENTS 5
#define RULES 65 

#include "Screen.h"   // Incluye Variables.h
#include "Comida.h"   // Plato.h (Ingrediente.h (Valores_Nutricionales.h))


bool doneState;       // Flag para que solo se realicen una vez las actividades del estado cada vez que se entre.
bool tararPlato;      // Flag para indicar que se ha colocado un recipiente y se debe tarar.
bool tararRetirar;    // Flag para tarar báscula una vez tras add/delete/save y comprobar que se ha retirado todo el peso (-X = +X).
bool savePlato;       // Flag para guardar la info del plato solo una vez, no cada vez que se entre a add/delete/save


/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- ESTADOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
typedef enum {
              //STATE_Bienvenida    =   (1),    // Estado inicial de bienvenida
              STATE_INI           =   (1),    // Estado para transiciones intermedias
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

static transition_rule rules[RULES] = { {STATE_INI,STATE_INI,TARAR},                    // tara inicial
                                        {STATE_INI,STATE_Plato,INCREMENTO},
                                        {STATE_INI,STATE_saved,GUARDAR},                // Guardar comida directamente (comidaActual no está vacía)
                                        
                                        {STATE_Plato,STATE_Plato,INCREMENTO},           // cambios por recolocar plato
                                        {STATE_Plato,STATE_Plato,DECREMENTO},           // cambios por recolocar plato
                                        //{STATE_Plato,STATE_INI,QUITAR},                // se ha retirado el recipiente
                                        {STATE_Plato,STATE_INI,LIBERAR},                // se ha retirado el recipiente
                                        {STATE_Plato,STATE_groupA,TIPO_A},                   
                                        {STATE_Plato,STATE_groupB,TIPO_B},                   
                                        
                                        {STATE_groupA,STATE_INI,LIBERAR},               // se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
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
                                        
                                        {STATE_groupB,STATE_INI,LIBERAR},               // se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
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
                                        {STATE_raw,STATE_added,ADD_PLATO},           // Guardar plato aunque no se haya añadido otro alimento
                                        {STATE_raw,STATE_deleted,DELETE_PLATO},      // Borrar plato actual
                                        {STATE_raw,STATE_saved,GUARDAR},             // Guardar comida aunque no se haya añadido otro alimento          
                                        
                                        {STATE_cooked,STATE_raw,CRUDO},                    
                                        {STATE_cooked,STATE_groupA,TIPO_A},                
                                        {STATE_cooked,STATE_groupB,TIPO_B},                
                                        {STATE_cooked,STATE_weighted,INCREMENTO},   
                                        {STATE_cooked,STATE_added,ADD_PLATO},           // Guardar plato aunque no se haya añadido otro alimento
                                        {STATE_cooked,STATE_deleted,DELETE_PLATO},      // Borrar plato actual
                                        {STATE_cooked,STATE_saved,GUARDAR},             // Guardar comida aunque no se haya añadido otro alimento         
                                         
                                        
                                        {STATE_weighted,STATE_INI,LIBERAR},             // se ha retirado el plato completo (+ recipiente) ==> ¿Habría que borrar y empezar de nuevo?
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
                                        {STATE_added,STATE_INI,LIBERAR},                 // se ha retirado el plato completo (+ recipiente)  
                                        {STATE_added,STATE_saved,GUARDAR},               // guardar la comida tras decir que se quiere añadir plato, aunque no se haga
                                        
                                        {STATE_deleted,STATE_deleted,TARAR},             // taramos para saber (en negativo) cuánto se va quitando   
                                        {STATE_deleted,STATE_deleted,QUITAR},            // para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_deleted,STATE_INI,LIBERAR},               // se ha retirado el plato completo (+ recipiente) 
                                        {STATE_deleted,STATE_saved,GUARDAR},             // guardar la comida tras borrar el plato actual  
                                        
                                        {STATE_saved,STATE_saved,TARAR},                 // taramos para saber (en negativo) cuánto se va quitando
                                        {STATE_saved,STATE_saved,QUITAR},                // para evitar error de evento cuando pase por condiciones que habilitan QUITAR (previo a LIBERAR)
                                        {STATE_saved,STATE_INI,LIBERAR}                  // se ha retirado el plato completo (+ recipiente)   
                                      };



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ VARIABLES ESTADOS/EVENTOS -------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

states_t state_actual;      // Estado actual
states_t state_new;         // Nuevo estado al que se va a pasar

event_t lastEvent;          // Último evento ocurrido

event_t eventoMain;         // Evento ocurrido en botonera Main
event_t eventoGrande;       // Evento ocurrido en botonera grande
event_t eventoBascula;      // Evento ocurrido en báscula


/*----------------------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------------------------------------------------*/


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
    return false;      // Si no se ha cumplido ninguna regla de transición.
                       //   ==> ERROR DE EVENTO  
}




/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- ACCIONES ESTADOS -----------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/




/*---------------------------------------------------------------------------------------------------------
   actStateInit(): Acciones del STATE_INI
----------------------------------------------------------------------------------------------------------*/
void actStateInit(){ 
    if(!doneState){
        Serial.println(F("\nSTATE_INI...")); 
        
        tareScale();                   // Tara inicial
        
        pesoRecipiente = 0.0;          // Se inicializa 'pesoRecipiente', que se sumará a 'pesoPlato' para saber el 'pesoARetirar'.
        pesoPlato = 0.0;               // Se inicializa 'pesoPlato', que se sumará a 'pesoRecipiente' para saber el 'pesoARetirar'.

        savePlato = true;              // Guardar una sola vez la info del plato en STATE_saved. Solo se guarda si la 'comidaActual'
                                       // no está vacía. En este caso, esta flag solo sirve para deshabilitar la flag de haber 'tarado'
                                       // la segunda vez que se entre a STATE_saved, de forma que los siguientes decrementos no los 
                                       // considere debidos a taras y se pueda comprobar si se ha quitado todo el 'pesoARetirar'.
        
        printStateInit();              // Print info estado.
        
        doneState = true;              // Solo realizar una vez las actividades del estado por cada vez que se active.
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

        pesoRecipiente = pesoBascula;  // Se guarda 'pesoRecipiente' para sumarlo a 'pesoPlato' y saber el 'pesoARetirar'.
        
        tararPlato = true;             // Tarar báscula tras colocar recipiente
        tarado = false;                // Desactivar flag de haber tarado.

                                       // El peso definitivo del recipiente no se guarda hasta que se escoja grupo de alimentos.
                                       // Por eso, 'pesoRecipiente' y 'pesoARetirar' son iguales a 0.0 en este estado. Esto no impide que se
                                       // realice LIBERAR, ya que ese evento ocurre cuando el peso de la báscula es igual a 'pesoARetirar'. 
                                       // Así, se puede detectar que se ha quitado el recipiente (bascula = 0.0) y se vuelve al estado INI.

        savePlato = true;              // Guardar una sola vez la info del plato en STATE_saved. Solo se guarda si la 'comidaActual'
                                       // no está vacía. En este caso, esta flag solo sirve para deshabilitar la flag de haber 'tarado'
                                       // la segunda vez que se entre a STATE_saved, de forma que los siguientes decrementos no los 
                                       // considere debidos a taras y se pueda comprobar si se ha quitado todo el 'pesoARetirar'.
        
        printStatePlato();             // Print info del estado.
        
        doneState = true;              // Solo realizar una vez las actividades del estado por cada vez que se active.
                                       // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                       // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_groupA o STATE_groupB
----------------------------------------------------------------------------------------------------------*/
void actGruposAlimentos(){ 
    if(!doneState){
        //Serial.print(F("Grupo ")); Serial.println(buttonGrande);
        
        procesamiento = "CRUDO";                             // El procesamiento es 'CRUDO' de forma predeterminada. 
        
        if(tararPlato){                                      // Se tara si se acaba de colocar el recipiente.
            tareScale();                                     // Tarar para tomar peso real del alimento que se va a colocar.
            tararPlato = false;                              // Desactivar flag para tarar solo una vez tras colocar el recipiente.
        }
        else{
            static float pesoAnterior;
            static float pesoNuevo;
            pesoAnterior = pesoNuevo;
            pesoNuevo = pesoBascula;
            
            if((pesoNuevo - pesoAnterior) > 1.0){            // Comprobamos que haya cambiado el peso antes de añadir el ingrediente 
                                                             // para evitar que se incluya el mismo varias veces al volver a entrar a 
                                                             // este mismo estado escogiendo diferentes grupos de alimentos.
              
                Serial.println(F("Añadiendo ingrediente al plato..."));
                
                Ingrediente ing(grupoAnterior, pesoBascula); // Cálculo automático de valores nutricionales.
                                                             // Al escoger un nuevo grupo se guarda el ingrediente del grupo anterior
                                                             // colocado en la iteración anterior. Por eso se utiliza 'grupoAnterior' para
                                                             // crear el ingrediente, porque 'grupoEscogido' ya ha tomado el valor del
                                                             // nuevo grupo.
                                                             
                platoActual.addIngPlato(ing);                // Ingrediente ==> Plato
                comidaActual.addIngComida(ing);              // Ingrediente ==> Comida

                pesoPlato = platoActual.getPesoPlato();      // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.

            }

            tareScale();                                     // Tarar cada vez que se seleccione un grupo de alimentos nuevo.
        }

        savePlato = true;                                    // Guardar una sola vez la nueva info del plato en add/delete/save. Solo si se pasa
                                                             // a alguno de esos estados sin colocar nada nuevo en la báscula.
        
        printStateABandProcessed();                          // Print info del estado.
        
        doneState = true;                                    // Solo realizar una vez las actividades del estado por cada vez que se active.
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
        
        procesamiento = "CRUDO";                     // Procesamiento del alimento a 'CRUDO'. 
        tarado = false;                              // Desactivar flag de haber tarado.
        savePlato = true;                            // Guardar una sola vez la nueva info del plato en add/delete/save. Solo si se pasa
                                                     // a alguno de esos estados sin colocar nada nuevo en la báscula.
        
        printStateABandProcessed();                  // Print info del estado.
        
        doneState = true;                            // Solo realizar una vez las actividades del estado por cada vez que se active.
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
        
        procesamiento = "COCINADO";                  // Procesamiento del alimento a 'COCINADO'.  
        tarado = false;                              // Desactivar flag de haber tarado.
        savePlato = true;                            // Guardar una sola vez la nueva info del plato en add/delete/save. Solo si se pasa
                                                     // a alguno de esos estados sin colocar nada nuevo en la báscula.
        
        printStateABandProcessed();                  // Print info del estado.
        
        doneState = true;                            // Solo realizar una vez las actividades del estado por cada vez que se active.
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

        savePlato = true;                            // Guardar una sola vez la nueva info del plato en add/delete/save.
        tarado = false;                              // Desactivar flag de haber tarado.
        tararRetirar = true;                         // Tarar tras guardar la nueva info del plato en add/delete/save.
        
        printStateWeighted();                        // Print info del estado.
        
        doneState = true;                            // Solo realizar una vez las actividades del estado por cada vez que se active.
                                                     // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                     // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateAdded(): Acciones del STATE_added
----------------------------------------------------------------------------------------------------------*/
void actStateAdded(){ 
    if(!doneState){
        if(savePlato){                                   // Si es la primera vez que se entra a este estado, se guarda.
          
            Serial.println(F("Añadiendo plato a la comida..."));
                                                         // Antes de guardar el plato se debe añadir el último ingrediente colocado
                                                         // que no se ha guardado aún porque eso se hace al escoger otro grupo de
                                                         // alimentos, lo cual no se ha hecho en este caso.
               
                                                         // Es posible que alguien escoja un nuevo grupo de alimentos, se guarde
                                                         // el ingrediente, pero al final no añada otro ingrediente de ese nuevo
                                                         // grupo. Entonces habría que permitir que tras escoger grupo de alimentos
                                                         // se guarde el plato sin añadirle nada. 
               
                                                         // Usamos 'grupoEscogido' porque no se ha modificado. 
            // Si hay algo por incluir en el plato
            //if(pesoBascula != 0.0){                      // Solo se guarda alimento si se ha pasado por STATE_weighted porque se haya
                                                         // colocado algo en la báscula tras escoger grupo en STATE_groupA o STATE_groupB.
                                                         // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                         // añadir un nuevo plato sin haber colocado un nuevo alimento, entonces no haría 
                                                         // falta actualizar la comida, pues no se habría modificado.
                                                         
                Ingrediente ing(grupoEscogido, pesoBascula); // Cálculo automático de valores nutricionales. 
                
                platoActual.addIngPlato(ing);                // Ingrediente ==> Plato
                comidaActual.addIngComida(ing);              // Ingrediente ==> Comida
                
            //}

            // Si el plato no está vacío, se guarda en la comida actual
            //if(!platoActual.isPlatoEmpty()){
                pesoPlato = platoActual.getPesoPlato();      // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                comidaActual.addPlato(platoActual);          // Plato ==> Comida (no se vuelve a incluir el ingrediente. Solo modifica peso y nPlatos)
                platoActual.restorePlato();                  // "Reiniciar" platoActual para usarlo de nuevo.
            //}

            if(tararRetirar){                            // Taramos y conforme vaya disminuyendo el peso veremos si -X = +X .
                tareScale();
                tararRetirar = false;
            }

            savePlato = false;                           // Para evitar guardar el plato varias veces al reentrar al estado sin pasar por otro.
        }
        else tarado = false;                             // Para que no siga marcando 'TARAR' al detectar un nuevo decremento en el 
                                                         // proceso de liberar la báscula.
        
        printStateAdded();                               // Print info del estado.
        
        doneState = true;                                // Solo realizar una vez las actividades del estado por cada vez que se active.
                                                         // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                         // las acciones.
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateDeleted(): Acciones del STATE_deleted
----------------------------------------------------------------------------------------------------------*/
void actStateDeleted(){
    if(!doneState){
        if(savePlato){                                   // Si es la primera vez que se entra a este estado, se guarda.
          
            Serial.println(F("\nPlato eliminado..."));

            // Si hay algo por incluir en el plato
            //if(pesoBascula != 0.0){                      // Solo se guarda alimento si se ha pasado por STATE_weighted porque se haya
                                                         // colocado algo en la báscula tras escoger grupo en STATE_groupA o STATE_groupB.
                                                         // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                         // eliminar el plato actual sin haber colocado un nuevo alimento, entonces no haría 
                                                         // falta actualizar la comida, pues no se habría modificado.
                                                         
                Ingrediente ing(grupoEscogido, pesoBascula); // Cálculo automático de valores nutricionales.
                
                platoActual.addIngPlato(ing);                // Ingrediente ==> Plato
                comidaActual.addIngComida(ing);              // Ingrediente ==> Comida (se guarda para poder borrar el plato completo)
            //}

            // Si el plato no está vacío, se incluye en la comida actual para poderlo borrar
            //if(!platoActual.isPlatoEmpty()){
                pesoPlato = platoActual.getPesoPlato();      // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                comidaActual.deletePlato(platoActual);       // Borrar plato de la comida.
                platoActual.restorePlato();                  // "Reiniciar" platoActual para usarlo de nuevo.
            //}

            if(tararRetirar){                            // Taramos y conforme vaya disminuyendo el peso veremos si -X = +X .
                tareScale();
                tararRetirar = false;
            }

            savePlato = false;                           // Para evitar guardar y borrar el plato varias veces al reentrar al estado 
                                                         // sin pasar por otro.
        }
        else tarado = false;                             // Para que no siga marcando 'TARAR' al detectar un nuevo decremento en el
                                                         // proceso de liberar la báscula.
        
        printStateDeleted();                             // Print info del estado.
        
        doneState = true;                                // Solo realizar una vez las actividades del estado por cada vez que se active.
                                                         // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                         // las acciones.
    }
}



/*---------------------------------------------------------------------------------------------------------
   actStateSaved(): Acciones del STATE_saved
----------------------------------------------------------------------------------------------------------*/
void actStateSaved(){
    if(!doneState){
        if(savePlato){                                   // Si es la primera vez que se entra a este estado, se guarda.
          
            Serial.println(F("\nComida guardada..."));  
                                                         // Antes de guardar la comida se debe añadir el último ingrediente colocado
                                                         // que no se ha guardado aún porque eso se hace al escoger otro grupo de
                                                         // alimentos, lo cual no se ha hecho en este caso. 
               
                                                         // Usamos 'grupoEscogido' porque no se ha modificado. 

            // Si hay algo por incluir en el plato
            //if(pesoBascula != 0.0){                      // Solo se guarda alimento si se ha pasado por STATE_weighted porque se haya
                                                         // colocado algo en la báscula tras escoger grupo en STATE_groupA o STATE_groupB.
                                                         // Estando en cualquiera de los estados previos a STATE_weighted, si se decidiera 
                                                         // guardar la comida actual sin haber colocado un nuevo alimento, entonces no haría 
                                                         // falta actualizarla, pues no se habría modificado.
            
                Ingrediente ing(grupoEscogido, pesoBascula); // Cálculo automático de valores nutricionales.
                
                platoActual.addIngPlato(ing);                // Ingrediente ==> Plato
                comidaActual.addIngComida(ing);              // Ingrediente ==> Comida
            //}

            // Si el plato no está vacío, se incluye en la comida actual
            //if(!platoActual.isPlatoEmpty()){
                comidaActual.addPlato(platoActual);          // Plato ==> Comida (no se vuelve a incluir el ingrediente. Solo modifica peso y nPlatos)
                pesoPlato = platoActual.getPesoPlato();      // Se actualiza el 'pesoPlato' para sumarlo a 'pesoRecipiente' y saber el 'pesoARetirar'.
                platoActual.restorePlato();                  // "Reiniciar" platoActual para usarlo de nuevo.
            //}

            // Si la comida actual no está vacía, se incluye en el diario
            //if(!comidaActual.isComidaEmpty()){
                diaActual.addComida(comidaActual);           // Comida ==> Diario
                comidaActual.restoreComida();                // "Reiniciar" comidaActual para usarla de nuevo.
            //}

            //if(pesoRecipiente != 0.0){                   // Si hay un recipiente colocado, además de alimentos. Es decir, no se viene del STATE_INI.
                if(tararRetirar){                            // Taramos y conforme vaya disminuyendo el peso veremos si -X = +X .
                    tareScale();
                    tararRetirar = false;
                }
            //}

            savePlato = false;                           // Para evitar guardar y borrar el plato varias veces al reentrar al estado 
                                                         // sin pasar por otro.
        }
        else tarado = false;                             // Para que no siga marcando 'TARAR' al detectar un nuevo decremento en el 
                                                         // proceso de liberar la báscula.
        
        printStateSaved();                               // Print info del estado.
        
        doneState = true;                                // Solo realizar una vez las actividades del estado por cada vez que se active.
                                                         // Así, debe ocurrir una nueva transición que lleve a este evento para que se "repitan"
                                                         // las acciones.
    }
}



/*---------------------------------------------------------------------------------------------------------
   doStateActions(): Acciones a realizar según el estado actual
----------------------------------------------------------------------------------------------------------*/
void doStateActions(){
    switch (state_actual){
      case 1:   actStateInit();         break;
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
      case 1:   printEventError("Coloque un recipiente sobre la b\xE1scula");                                   break;  // INIT
      case 2:   printEventError("Escoja un grupo de alimentos");                                                break;  // Plato
      case 3:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \n\tgrupo");         break;  // grupoA 
      case 4:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \n\tgrupo");         break;  // grupoB
      case 5:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \n\tgrupo");         break;  // Crudo
      case 6:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \n\tgrupo");         break;  // Cocinado
      case 7:   printEventError("Escoja grupo para otro alimento, a\xF1ada otro plato o \n\tguarde la comida"); break;  // Pesado
      case 8:   printEventError("Retire el plato para comenzar uno nuevo");                                     break;  // Add
      case 9:   printEventError("Retire el plato que acaba de eliminar");                                       break;  // Delete
      case 10:  printEventError("Retire el plato");                                                             break;  // Save
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
        if (event_buffer[i] == NONE){     // Hay algún hueco
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
        if (event_buffer[i] == NONE){     // Primer hueco
            return i;
        }
    }
}


/*---------------------------------------------------------------------------------------------------------
   rotateEventBuffer(): Mover elementos del buffer a la izq para liberar un hueco sin perder 
                        los eventos previos.
----------------------------------------------------------------------------------------------------------*/
void rotateEventBuffer(){ 
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
    bool found = false;
    if(isBufferEmpty()){
        pos = 0;
    }
    else{
        if(isBufferFull()){
            rotateEventBuffer();    // Se rota array a izq y se libera el último hueco
            pos = MAX_EVENTS-1;     // Último hueco
        }
        else{
            pos = firstGapBuffer();
        }
    }
    event_buffer[pos] = evento;     // Añadir a buffer
    lastEvent = evento;
    Serial.print("Buffer: "); 
    for (int i = 0; i < MAX_EVENTS; i++){
        Serial.print(event_buffer[i]); Serial.print(" ");
    }
    Serial.print(F("Last event: ")); Serial.println(lastEvent);
}





#endif
