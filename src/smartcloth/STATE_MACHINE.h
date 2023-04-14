#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define MAX_EVENTS 5
#define RULES 50 

#include "Screen.h" // Incluye Variables.h
#include "Comida.h"  // Plato.h (Ingrediente.h (Valores_Nutricionales.h))


bool doneState;
bool pesoCopiado;

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
              STATE_saved         =   (10)     // comida guardada
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

/* Buffer de eventos al que se irán añadiendo conforme ocurran */
static event_t event_buffer[MAX_EVENTS];
/* Este buffer no se llega a usar para nada, sino que se comprueba con
   'lastEvent' si se cumple alguna regla de transición.
   Si al final no se usara, se podría/debería eliminar para evitar ocupar
   memoria innecesariamente */



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ REGLAS TRANSICION ---------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
typedef struct{
    states_t state_i;     // estado actual
    states_t state_j;     // estado siguiente
    event_t condition;    // condición cambio estado
}transition_rule;

static transition_rule rules[RULES] = { {STATE_INI,STATE_INI,TARAR},                  // INI --decremento(limpiar balanza)--> INI
                                        {STATE_INI,STATE_Plato,INCREMENTO},
                                        
                                        {STATE_Plato,STATE_Plato,INCREMENTO},        // plato --incremento--> plato   ==> cambios por recolocar plato
                                        {STATE_Plato,STATE_Plato,DECREMENTO},        // plato --decremento--> plato   ==> cambios por recolocar plato
                                        {STATE_Plato,STATE_INI,LIBERAR},           // plato --LIBERAR_bascula--> INI    ==> se ha retirado el plato
                                        {STATE_Plato,STATE_groupA,TIPO_A},                   // INI --tipoA--> grupoA
                                        {STATE_Plato,STATE_groupB,TIPO_B},                   // INI --tipoB--> grupoB
                                        
                                        {STATE_groupA,STATE_INI,LIBERAR},               // grupoA --LIBERAR_bascula--> INI  ==> se ha retirado el plato completo (+ recipiente)
                                        {STATE_groupA,STATE_groupA,TIPO_A},                // grupoA --tipoA--> grupoA
                                        {STATE_groupA,STATE_groupA,QUITAR},               // grupoA --LIBERAR_bascula--> grupoA
                                        {STATE_groupA,STATE_groupA,TARAR},                 // grupoA --TARAR_bascula--> grupoA 
                                        {STATE_groupA,STATE_groupB,TIPO_B},                // grupoA --tipoB--> grupoB  
                                        {STATE_groupA,STATE_raw,CRUDO},                    // grupoA --crudo--> raw      
                                        {STATE_groupA,STATE_cooked,COCINADO},              // grupoA --cocinado--> cooked 
                                        {STATE_groupA,STATE_weighted,INCREMENTO},          // grupoA --incremento--> pesado   ==>  aprovechar 'crudo' predeterminado para pesar directamente
                                        
                                        {STATE_groupB,STATE_INI,LIBERAR},               // grupoB --LIBERAR_bascula--> INI  ==> se ha retirado el plato completo (+ recipiente)
                                        {STATE_groupB,STATE_groupB,TIPO_B},                // grupoB --tipoB--> grupoB
                                        {STATE_groupB,STATE_groupB,QUITAR},               // grupoB --LIBERAR_bascula--> grupoB 
                                        {STATE_groupB,STATE_groupB,TARAR},                 // grupoB --TARAR_bascula--> grupoB 
                                        {STATE_groupB,STATE_groupA,TIPO_A},                // grupoB --tipoA--> grupoA
                                        {STATE_groupB,STATE_raw,CRUDO},                    // grupoB --crudo--> raw      
                                        {STATE_groupB,STATE_cooked,COCINADO},              // grupoB --cocinado--> cooked 
                                        {STATE_groupB,STATE_weighted,INCREMENTO},          // grupoB --incremento--> pesado  
                                        
                                        {STATE_raw,STATE_cooked,COCINADO},                 // raw --cocinado--> cooked
                                        {STATE_raw,STATE_groupA,TIPO_A},                   // raw --tipoA--> grupoA
                                        {STATE_raw,STATE_groupB,TIPO_B},                   // raw --tipoB--> grupoB
                                        {STATE_raw,STATE_weighted,INCREMENTO},             // raw --incremento--> pesado
                                        
                                        {STATE_cooked,STATE_raw,CRUDO},                    // cooked --raw--> crudo
                                        {STATE_cooked,STATE_groupA,TIPO_A},                // cooked --tipoA--> grupoA
                                        {STATE_cooked,STATE_groupB,TIPO_B},                // cooked --tipoB--> grupoB
                                        {STATE_cooked,STATE_weighted,INCREMENTO},          // cooked --incremento--> pesado
                                        
                                        {STATE_weighted,STATE_INI,LIBERAR},           // pesado --LIBERAR_bascula--> INI  ==> se ha retirado el plato completo (+ recipiente)
                                        {STATE_weighted,STATE_weighted,INCREMENTO},        // pesado --incremento--> pesado
                                        {STATE_weighted,STATE_weighted,DECREMENTO},        // pesado --decremento--> pesado
                                        {STATE_weighted,STATE_weighted,QUITAR},        // pesado --quitar--> pesado
                                        {STATE_weighted,STATE_groupA,TIPO_A},              // pesado --tipoA--> grupoA
                                        {STATE_weighted,STATE_groupB,TIPO_B},              // pesado --tipoA--> grupoA 
                                        {STATE_weighted,STATE_added,ADD_PLATO},            // pesado --add--> plato añadido
                                        {STATE_weighted,STATE_deleted,DELETE_PLATO},       // pesado --delete--> plato eliminado
                                        {STATE_weighted,STATE_saved,GUARDAR},              // pesado --save--> comida guardada
                                        
                                        {STATE_added,STATE_added,DECREMENTO},                    
                                        {STATE_added,STATE_added,QUITAR},                    // quitar algo pero no todo el plato. Esperando liberación
                                        {STATE_added,STATE_INI,LIBERAR},                     // añadido --TARAR_bascula--> ini
                                        
                                        {STATE_deleted,STATE_deleted,DECREMENTO},                    
                                        {STATE_deleted,STATE_deleted,QUITAR},                // quitar algo pero no todo el plato. Esperando liberación
                                        {STATE_deleted,STATE_INI,LIBERAR},                   // eliminado --TARAR_bascula--> ini
                                        
                                        {STATE_saved,STATE_saved,DECREMENTO},                    
                                        {STATE_saved,STATE_saved,QUITAR},                    // quitar algo pero no todo el plato. Esperando liberación
                                        {STATE_saved,STATE_INI,LIBERAR}                      // guardar --TARAR_bascula--> ini
                                      };



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ VARIABLES ESTADOS/EVENTOS -------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

states_t state_actual; 
states_t state_new;

event_t lastEvent;

event_t eventoMain;         //Evento ocurrido en botonera Main
event_t eventoGrande;       //Evento ocurrido en botonera grande
event_t eventoBascula;      //Timer


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
                state_new = rules[i].state_j;
                doneState = false;
                return true;
            }
        }
    }
    return false; //Si no se ha cumplido ninguna regla de transición
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
        
        tareScale(); 
        pesoPlatoTotal = 0.0; //Para saber si se ha retirado el plato
        
        printStateInit(); 
        doneState = true;
    }
} 


/*---------------------------------------------------------------------------------------------------------
   actStatePlato(): Acciones del STATE_Plato
----------------------------------------------------------------------------------------------------------*/
void actStatePlato(){ 
    if(!doneState){
        Serial.println(F("\nPlato colocado")); 

        pesoPlatoTotal = pesoBascula; // Añadir peso del recipiente para saber cuándo se ha quitado todo
        tararPlato = true;
        tarado = false;
        
        printStatePlato(); 
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_groupA o STATE_groupB
----------------------------------------------------------------------------------------------------------*/
void actGruposAlimentos(){ 
    if(!doneState){
        //Serial.print(F("Grupo ")); Serial.println(buttonGrande);
        procesamiento = "CRUDO";
        
        if(tararPlato){
            tareScale(); 
            tararPlato = false;
        }
        else{
            /* Comprobamos que haya cambiado el peso antes de añadir el ingrediente para
               evitar que se incluya el mismo varias veces 
               
               Solo se añade el ingrediente del grupo anterior al escoger otro grupo para
               un nuevo ingrediente. 
           */
            static float pesoAnterior;
            static float pesoNuevo;
            pesoAnterior = pesoNuevo;
            pesoNuevo = pesoBascula;
            
            if((pesoNuevo - pesoAnterior) > 1.0){ 
                /* Usamos 'grupoAnterior' porque al entrar a este estado ya se ha actualizado 'grupoEscogido' por el nuevo botón */
                Serial.println(F("Añadiendo ingrediente al plato..."));
                /*  ----- INGREDIENTE ==> PLATO ----- */
                Ingrediente ing(grupoAnterior, pesoBascula); /* Cálculo automático de valores nutricionales */
                platoActual.addIngPlato(ing);
                /*  ----- INGREDIENTE ==> COMIDA  ------ */
                comidaActual.addIngComida(ing);     // Para ir actualizando la comida actual 

                pesoPlatoTotal += pesoBascula; //Añadimos peso del ingrediente al peso total (+ recipiente) para saber si se ha retirado
            }
        }
        
        tareScale();
        printStateABandProcessed();
        doneState = true;
    }
}




/*---------------------------------------------------------------------------------------------------------
   actStateRaw(): Acciones del STATE_raw
----------------------------------------------------------------------------------------------------------*/
void actStateRaw(){ 
    if(!doneState){
        Serial.println(F("\nAlimento crudo...")); 
        procesamiento = "CRUDO";
        tarado = false;
        printStateABandProcessed();
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateCooked(): Acciones del STATE_cooked
----------------------------------------------------------------------------------------------------------*/
void actStateCooked(){ 
    if(!doneState){
        Serial.println(F("\nAlimento cocinado...")); 
        procesamiento = "COCINADO";
        tarado = false;
        printStateABandProcessed();
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateWeighted(): Acciones del STATE_weighted
----------------------------------------------------------------------------------------------------------*/
void actStateWeighted(){ 
    if(!doneState){
        Serial.println(F("\nAlimento pesado...")); 
        tarado = false;
        if(eventoBascula == INCREMENTO) pesoPlatoTotal += pesoBascula; //Actualizamos el peso total (+ recipiente) para saber si se ha retirado
        else pesoPlatoTotal -= diffWeight; //DECREMENTO o QUITAR => Restamos lo añadido anteriormente
        pesoCambiado = true;
        printStateWeighted();
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateAdded(): Acciones del STATE_added
----------------------------------------------------------------------------------------------------------*/
void actStateAdded(){ 
    if(!doneState){
        if(eventoBascula != DECREMENTO){ //Si no se está retirando el plato, se guarda
            Serial.println(F("Añadiendo plato a la comida..."));
            /* Antes de guardar el plato se debe añadir el último ingrediente colocado
               que no se ha guardado aún porque eso se hace al escoger otro grupo de
               alimentos, lo cual no se ha hecho en este caso.
               
               Es posible que alguien escoja un nuevo grupo de alimentos, se guarde
               el ingrediente, pero al final no añada otro ingrediente de ese nuevo
               grupo. Entonces habría que permitir que tras escoger grupo de alimentos
               se guarde el plato sin añadirle nada. 
               
               Usamos 'grupoEscogido' porque no se ha modificado. */
               
            /*  ----- INGREDIENTE ==> PLATO ----- */
            Ingrediente ing(grupoEscogido, pesoBascula); /* Cálculo automático de valores nutricionales */
            platoActual.addIngPlato(ing);
            /*  ----- INGREDIENTE ==> COMIDA  ------ */
            comidaActual.addIngComida(ing);
            /*  ----- PLATO ==> COMIDA  ------ */
            comidaActual.addPlato(platoActual);
    
            if(!pesoCambiado) {
                if(eventoBascula == INCREMENTO) pesoPlatoTotal += pesoBascula; //Actualizamos el peso total (+ recipiente) para saber si se ha retirado
                else pesoPlatoTotal -= diffWeight; //DECREMENTO o QUITAR => Restamos lo añadido anteriormente
                //pesoPlatoTotal += pesoBascula; //Guardamos el peso para saber si se ha retirado
                pesoCambiado = true;
            }
            platoActual.restorePlato();         // "Reiniciar" plato para usarlo de nuevo    
        }
        tarado = false;
        printStateAdded();         
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateDeleted(): Acciones del STATE_deleted
----------------------------------------------------------------------------------------------------------*/
void actStateDeleted(){
    if(!doneState){
        if(eventoBascula != DECREMENTO){ //Si no se está retirando el plato, se guarda
            Serial.println(F("\nPlato eliminado..."));
            /*  ----- INGREDIENTE ==> PLATO ----- */
            Ingrediente ing(grupoEscogido, pesoBascula); /* Cálculo automático de valores nutricionales */
            platoActual.addIngPlato(ing);
            /*  ----- INGREDIENTE ==> COMIDA  ------ */
            comidaActual.addIngComida(ing);
            /*  ----- PLATO !=> COMIDA  ------ */
            comidaActual.deletePlato(platoActual);
            
            if(!pesoCambiado){
              if(eventoBascula == INCREMENTO) pesoPlatoTotal += pesoBascula; //Actualizamos el peso total (+ recipiente) para saber si se ha retirado
              else pesoPlatoTotal -= diffWeight; //DECREMENTO o QUITAR => Restamos lo añadido anteriormente
              //pesoPlatoTotal += pesoBascula; //Guardamos el peso para saber si se ha retirado
              pesoCambiado = true;
            }
            platoActual.restorePlato();         // "Reiniciar" plato para usarlo de nuevo
        }
        tarado = false;
        printStateDeleted();        
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateSaved(): Acciones del STATE_saved
----------------------------------------------------------------------------------------------------------*/
void actStateSaved(){
    if(!doneState){
        if(eventoBascula != DECREMENTO){ //Si no se está retirando el plato, se guarda
            Serial.println(F("\nComida guardada..."));  
            /* Antes de guardar la comida se debe añadir el último ingrediente colocado
               que no se ha guardado aún porque eso se hace al escoger otro grupo de
               alimentos, lo cual no se ha hecho en este caso.
               
               TODO => Es posible que alguien escoja un nuevo grupo de alimentos, se guarde
               el ingrediente, pero al final no añada otro ingrediente de ese nuevo
               grupo. Entonces habría que permitir que tras escoger grupo de alimentos
               se guarde el plato sin añadirle nada. 
               
               Usamos 'grupoEscogido' porque no se ha modificado. */
               
            /*  ----- INGREDIENTE ==> PLATO ----- */
            Ingrediente ing(grupoEscogido, pesoBascula); /* Cálculo automático de valores nutricionales */
            platoActual.addIngPlato(ing);
            /*  ----- INGREDIENTE ==> COMIDA  ------ */
            comidaActual.addIngComida(ing);
            /*  ----- PLATO ==> COMIDA  ------ */
            comidaActual.addPlato(platoActual);
            /*  ----- COMIDA ==> DIARIO ------ */
            diaActual.addComida(comidaActual);
    
            if(!pesoCambiado) {
                if(eventoBascula == INCREMENTO) pesoPlatoTotal += pesoBascula; //Actualizamos el peso total (+ recipiente) para saber si se ha retirado
                else pesoPlatoTotal -= diffWeight; //DECREMENTO o QUITAR => Restamos lo añadido anteriormente
                //pesoPlatoTotal += pesoBascula; //Guardamos el peso para saber si se ha retirado
                pesoCopiado = true;
            }
            platoActual.restorePlato();         // "Reiniciar" plato para usarlo de nuevo
            comidaActual.restoreComida();
        }
        tarado = false;
        printStateSaved();
        doneState = true;
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
      case 1:   printEventError("Coloque un recipiente sobre la b\xE1scula");                                 break; //INIT
      case 2:   printEventError("Escoja un grupo de alimentos");                                              break;
      case 3:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \ngrupo");         break;  //grupoA ==> ¿Y si quieren guardar la comida o poner otro plato? No lo permite
      case 4:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \ngrupo");         break;  //grupoB
      case 5:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \ngrupo");         break;  //Crudo
      case 6:   printEventError("Coloque un alimento, indique crudo/cocinado o escoja otro \ngrupo");         break;  //Cocinado
      case 7:   printEventError("Escoja grupo para otro alimento, a\xF1ada otro plato o \nguarde la comida"); break;  //Pesado
      case 8:   printEventError("Retire el plato para comenzar uno nuevo");                                   break;  //Add
      case 9:   printEventError("Retire el plato que acaba de eliminar");                                     break;  //Delete
      case 10:  printEventError("Retire el plato");                                                           break;  //Save
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
        if (event_buffer[i] == NONE){ //Hay algún hueco
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
        if (event_buffer[i] == NONE){ //Primer hueco
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
            rotateEventBuffer(); //Se rota array a izq y se libera el último hueco
            pos = MAX_EVENTS-1; //Último hueco
        }
        else{
            pos = firstGapBuffer();
        }
    }
    event_buffer[pos] = evento; //Añadir a buffer
    lastEvent = evento;
    Serial.print("Buffer: "); 
    for (int i = 0; i < MAX_EVENTS; i++){
        Serial.print(event_buffer[i]); Serial.print(" ");
    }
    Serial.print(F("Last event: ")); Serial.println(lastEvent);
}





#endif
