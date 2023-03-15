#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define MAX_EVENTS 5
#define RULES 20 

#include "SCREEN.h"
#include "aux.h"

//bool doneINI, doneGroupA, doneGroupB, doneProcessed, doneWeighted, doneAdded, doneDeleted, doneSaved;
bool doneState;

/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- ESTADOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
typedef enum {
              STATE_INI           =   (1),    // Smartcloth encendido
              STATE_groupA        =   (2),    // grupo A
              STATE_groupB        =   (3),    // grupo B
              STATE_raw           =   (4),    // (grupo A) crudo
              STATE_cooked        =   (5),    // (grupo A) cocinado
              STATE_weighted      =   (6),    // plato pesado
              STATE_added         =   (7),    // plato añadido
              STATE_deleted       =   (8),    // plato eliminado
              STATE_saved         =   (9)     // comida guardada
} states_t;



/*----------------------------------------------------------------------------------------------*/
/*-------------------------------------- EVENTOS -----------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
typedef enum {
              
              NONE            = (0),    
              TIPO_A          = (1),    // Botonera Main (...)
              TIPO_B          = (2),    // Botonera Main (...)
              CRUDO           = (3),    // AMARILLO
              COCINADO        = (4),    // BLANCO
              ADD_PLATO       = (5),    // VERDE
              DELETE_PLATO    = (6),    // ROJO
              GUARDAR         = (7),    // NEGRO 
              INCREMENTO      = (8),    // Báscula
              DECREMENTO      = (9)    // Báscula
} event_t;

/* Buffer de eventos al que se irán añadiendo conforme ocurran */
static event_t event_buffer[MAX_EVENTS];



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ REGLAS TRANSICION ---------------------------------------------*/
/*----------------------------------------------------------------------------------------------*/
typedef struct{
    states_t state_i;     // estado actual
    states_t state_j;     // estado siguiente
    event_t condition;    // condición cambio estado
}transition_rule;

static transition_rule rules[RULES] = { {STATE_INI,STATE_groupA,TIPO_A},                   // INI --tipoA--> grupoA
                                        {STATE_INI,STATE_groupB,TIPO_B},                   // INI --tipoB--> grupoB
                                        {STATE_groupA,STATE_groupA,TIPO_A},                // grupoA --tipoA--> grupoA
                                        {STATE_groupA,STATE_groupB,TIPO_B},                // grupoA --tipoB--> grupoB  
                                        {STATE_groupA,STATE_raw,CRUDO},                    // grupoA --crudo--> procesado      
                                        {STATE_groupA,STATE_cooked,COCINADO},              // grupoA --cocinado--> procesado      
                                        {STATE_groupB,STATE_groupB,TIPO_B},                // grupoB --tipoB--> grupoB
                                        {STATE_groupB,STATE_groupA,TIPO_A},                // grupoB --tipoA--> grupoA
                                        {STATE_groupB,STATE_weighted,INCREMENTO},          // grupoB --incremento--> pesado  
                                        {STATE_raw,STATE_weighted,INCREMENTO},             // crudo --incremento--> pesado
                                        {STATE_cooked,STATE_weighted,INCREMENTO},          // cocinado --incremento--> pesado
                                        {STATE_weighted,STATE_groupA,TIPO_A},              // pesado --tipoA--> grupoA
                                        {STATE_weighted,STATE_groupB,TIPO_B},              // pesado --tipoA--> grupoA 
                                        {STATE_weighted,STATE_added,ADD_PLATO},            // pesado --add--> plato añadido
                                        {STATE_weighted,STATE_deleted,DELETE_PLATO},       // pesado --delete--> plato eliminado
                                        {STATE_weighted,STATE_saved,GUARDAR},              // pesado --save--> comida guardada
                                        {STATE_added,STATE_INI,DECREMENTO},                // añadido --decremento--> INI
                                        {STATE_deleted,STATE_INI,DECREMENTO},              // eliminado --decremento--> INI
                                        {STATE_saved,STATE_INI,DECREMENTO}                 // guardar --decremento--> INI         
                                      };



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ VARIABLES ESTADOS/EVENTOS -------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

states_t state_actual; 
states_t state_new;

event_t lastEvent;          //Ultimo evento ocurrido, válido o no
//event_t lastValidEvent;     //Ultimo evento valido ocurrido
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
    //Serial.println("\nComprobando reglas de transición...");
    //event_t lastEvent = getLastEvent();
    for (int i = 0; i < RULES; i++){
        if(rules[i].state_i == state_actual){
            //Serial.print(F("\nRegla encontrada ")); Serial.println(i);
            /* Inicialmente asumimos que la lista de condiciones es igual al
               buffer de eventos. Si se encuentra un elemento que no lo sea,
               en el mismo orden, se salta a la siguiente regla de transición. */
            //Serial.print(F("Condicion: ")); Serial.print(rules[i].condition);
            if(rules[i].condition == lastEvent){
                state_new = rules[i].state_j;
                doneState = false;
                //Serial.println(F("\nRegla utilizada"));
                //lastValidEvent = lastEvent;
                return true;
            }
        }
    }
    /*if (lastEvent == INCREMENTO){
        Serial.println(F("\nRetire lo que haya colocado"));
        simplePrint("Retire lo que haya colocado en la bascula");
    }
    else if (lastEvent == DECREMENTO){
        Serial.println(F("\nVuelva a colocar lo que haya retirado"));
        simplePrint("Vuelva a colocar lo que haya retirado");
    }*/
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
        printStateInit(); 
        doneState = true;
    }
} 


/*---------------------------------------------------------------------------------------------------------
   actStateGroupA(): Acciones del STATE_groupA
----------------------------------------------------------------------------------------------------------*/
void actStateGroupA(){ 
    if(!doneState){
        Serial.println(F("\nAlimento de tipo A...")); 
        printStateA(); 
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateGroupB(): Acciones del STATE_groupB
----------------------------------------------------------------------------------------------------------*/
void actStateGroupB(){ 
    if(!doneState){
        Serial.println(F("\nAlimento de tipo B...")); 
        printStateB(); 
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateRaw(): Acciones del STATE_raw
----------------------------------------------------------------------------------------------------------*/
void actStateRaw(){ 
    if(!doneState){
        Serial.println(F("\nAlimento crudo...")); 
        printStateRaw(); 
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateCooked(): Acciones del STATE_cooked
----------------------------------------------------------------------------------------------------------*/
void actStateCooked(){ 
    if(!doneState){
        Serial.println(F("\nAlimento cocinado...")); 
        printStateCooked(); 
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateWeighted(): Acciones del STATE_weighted
----------------------------------------------------------------------------------------------------------*/
void actStateWeighted(){ 
    if(!doneState){
        //TODO Valores de alimento grupo X según peso
        Serial.println(F("\nAlimento pesado...")); 
        printStateWeighted(); 
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateAdded(): Acciones del STATE_added
----------------------------------------------------------------------------------------------------------*/
void actStateAdded(){ 
    if(!doneState){
        //TODO añadir plato
        Serial.println(F("\nPlato anadido...")); 
        //printStateAdded(); 
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateDeleted(): Acciones del STATE_deleted
----------------------------------------------------------------------------------------------------------*/
void actStateDeleted(){
    if(!doneState){
        //TODO eliminar plato
        Serial.println(F("\nPlato eliminado..."));
        //printStateDeleted();
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateSaved(): Acciones del STATE_saved
----------------------------------------------------------------------------------------------------------*/
void actStateSaved(){
    if(!doneState){
        //TODO guardar comida
        Serial.println(F("\nComida guardada..."));
        //printStateSaved();
        doneState = true;
    }
}

/*---------------------------------------------------------------------------------------------------------
   doStateActions(): Acciones a realizar según el estado actual
----------------------------------------------------------------------------------------------------------*/
void doStateActions(){
    switch (state_actual){
      case 1:   actStateInit();       break;
      case 2:   actStateGroupA();     break;
      case 3:   actStateGroupB();     break;
      case 4:   actStateRaw();        break;
      case 5:   actStateCooked();     break;
      case 6:   actStateWeighted();   break;
      case 7:   actStateAdded();      break;
      case 8:   actStateDeleted();    break;
      case 9:   actStateSaved();      break;
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


int firstGapBuffer(){
    for (int i = 0; i < MAX_EVENTS; i++){
        if (event_buffer[i] == NONE){ //Primer hueco
            return i;
        }
    }
}


/*---------------------------------------------------------------------------------------------------------
   clearEventBuffer(): Limpiar el buffer de eventos (poner elementos a NONE)
----------------------------------------------------------------------------------------------------------*/
void rotateEventBuffer(){ 
    Serial.println(F("\nRotando buffer de eventos..."));
    //if(!isBufferEmpty()){
        for (int i = 0; i < MAX_EVENTS; i++){
            if(i < (MAX_EVENTS-1)){
                event_buffer[i] = event_buffer[i+1];
            }
            else if (i == (MAX_EVENTS-1)){
                event_buffer[i] = NONE;
            }
        }
    //}
}


/*---------------------------------------------------------------------------------------------------------
   addEventToBuffer(): Añadir el último evento al buffer de eventos
----------------------------------------------------------------------------------------------------------*/
void addEventToBuffer(event_t evento){
    Serial.println(F("\nAnadiendo evento al buffer..."));
    //int nEventsBuffer = sizeof(event_buffer)/sizeof(event_t);
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
            /*for (int i = 0; i < MAX_EVENTS; i++){
                if (event_buffer[i] == NONE){
                    if (!found){
                        pos = i; //Primer espacio vacío (NONE)
                        found = true;
                    }
                }
            }*/
            pos = firstGapBuffer();
        }
    }
    event_buffer[pos] = evento; //Añadir a buffer
    lastEvent = evento;         //Ultimo evento
    Serial.print("Buffer: "); 
    for (int i = 0; i < MAX_EVENTS; i++){
        Serial.print(event_buffer[i]); Serial.print(" ");
    }
    Serial.print("Last event: "); Serial.println(lastEvent);
}





#endif
