#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#define MAX_EVENTS 5
#define RULES 30 

#include "Screen.h" // Incluye Variables.h
#include "Plato.h"  // Ingrediente.h (Valores_Nutricionales.h)


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
              LIBERAR             =   (10)    // Báscula a 0
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
                                        {STATE_groupA,STATE_groupA,DECREMENTO},            // grupoA --decremento(tara)--> grupoA
                                        {STATE_groupA,STATE_groupB,TIPO_B},                // grupoA --tipoB--> grupoB  
                                        {STATE_groupA,STATE_raw,CRUDO},                    // grupoA --crudo--> raw      
                                        {STATE_groupA,STATE_cooked,COCINADO},              // grupoA --cocinado--> cooked 
                                        {STATE_raw,STATE_cooked,COCINADO},                 // raw --cocinado--> cooked
                                        {STATE_raw,STATE_groupA,TIPO_A},                   // raw --tipoA--> grupoA
                                        {STATE_raw,STATE_groupB,TIPO_B},                   // raw --tipoB--> grupoB
                                        {STATE_raw,STATE_weighted,INCREMENTO},             // raw --incremento--> pesado
                                        {STATE_cooked,STATE_raw,CRUDO},                    // cooked --raw--> crudo
                                        {STATE_cooked,STATE_groupA,TIPO_A},                // cooked --tipoA--> grupoA
                                        {STATE_cooked,STATE_groupB,TIPO_B},                // cooked --tipoB--> grupoB
                                        {STATE_cooked,STATE_weighted,INCREMENTO},          // cooked --incremento--> pesado
                                        {STATE_groupB,STATE_groupB,TIPO_B},                // grupoB --tipoB--> grupoB
                                        {STATE_groupB,STATE_groupA,TIPO_A},                // grupoB --tipoA--> grupoA
                                        {STATE_groupB,STATE_weighted,INCREMENTO},          // grupoB --incremento--> pesado  
                                        {STATE_groupB,STATE_weighted,DECREMENTO},          // grupoB --decremento(tara)--> pesado
                                        {STATE_weighted,STATE_weighted,INCREMENTO},        // pesado --incremento--> pesado
                                        {STATE_weighted,STATE_weighted,DECREMENTO},        // pesado --decremento--> pesado
                                        {STATE_weighted,STATE_weighted,LIBERAR},           // pesado --liberar_bascula--> pesado
                                        {STATE_weighted,STATE_groupA,TIPO_A},              // pesado --tipoA--> grupoA
                                        {STATE_weighted,STATE_groupB,TIPO_B},              // pesado --tipoA--> grupoA 
                                        {STATE_weighted,STATE_added,ADD_PLATO},            // pesado --add--> plato añadido
                                        {STATE_weighted,STATE_deleted,DELETE_PLATO},       // pesado --delete--> plato eliminado
                                        {STATE_weighted,STATE_saved,GUARDAR},              // pesado --save--> comida guardada
                                        {STATE_added,STATE_INI,LIBERAR},                   // añadido --liberar_bascula--> INI
                                        {STATE_deleted,STATE_INI,LIBERAR},                 // eliminado --liberar_bascula--> INI
                                        {STATE_saved,STATE_INI,LIBERAR}                    // guardar --liberar_bascula--> INI         
                                      };



/*----------------------------------------------------------------------------------------------*/
/*------------------------------ VARIABLES ESTADOS/EVENTOS -------------------------------------*/
/*----------------------------------------------------------------------------------------------*/

states_t state_actual; 
states_t state_new;

event_t lastEvent;
//event_t lastValidEvent;

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
            //if((rules[i].condition == lastValidEvent) or (rules[i].condition == lastEvent)){
            if(rules[i].condition == lastEvent){
                //lastValidEvent = lastEvent;
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
        //tareScale(); 
        //Plato actual es global (Variables.h)
        printStateInit(); 
        doneState = true;
    }
} 


/*---------------------------------------------------------------------------------------------------------
   actGruposAlimentos(): Acciones del STATE_groupA o STATE_groupB
----------------------------------------------------------------------------------------------------------*/
void actGruposAlimentos(){ 
    if(!doneState){
        Serial.print(F("Grupo ")); Serial.println(buttonGrande);
        /* Comprobamos que haya cambiado el peso antes de añadir el ingrediente para
           evitar que se incluya el mismo varias veces */
        static float pesoAnterior;
        static float pesoNuevo;
        pesoAnterior = pesoNuevo;
        pesoNuevo = weight;
        if(abs(pesoAnterior - pesoNuevo) > 1.0){ /* Si se colocado algo en la báscula antes de pulsar este botón */
            /* Usamos grupoAnterior porque al entrar a este estado ya se ha actualizado grupoEscogido por el nuevo botón */
            Serial.println(F("Añadiendo ingrediente al plato..."));
            Ingrediente ing(grupoAnterior, weight);
            platoActual.addIngrediente(ing);
            
        }
        //tareScale(); 
        printStateAB();
        doneState = true;
    }
}




/*---------------------------------------------------------------------------------------------------------
   actStateRaw(): Acciones del STATE_raw
----------------------------------------------------------------------------------------------------------*/
void actStateRaw(){ 
    if(!doneState){
        Serial.println(F("\nAlimento crudo...")); 
        printStateRawCooked("CRUDO"); 
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateCooked(): Acciones del STATE_cooked
----------------------------------------------------------------------------------------------------------*/
void actStateCooked(){ 
    if(!doneState){
        Serial.println(F("\nAlimento cocinado...")); 
        printStateRawCooked("COCINADO"); 
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateWeighted(): Acciones del STATE_weighted
----------------------------------------------------------------------------------------------------------*/
void actStateWeighted(){ 
    if(!doneState){
        Serial.println(F("\nAlimento pesado...")); 
        //Al seleccionar otro grupo de alimentos se guardará el ingrediente pesado
        Ingrediente ing(grupoEscogido, weight); //Ingrediente auxiliar usado para mostrar información variable de lo pesado
        printStateWeighted(ing);
        doneState = true;
    }
}


/*---------------------------------------------------------------------------------------------------------
   actStateAdded(): Acciones del STATE_added
----------------------------------------------------------------------------------------------------------*/
void actStateAdded(){ 
    if(!doneState){
        //TODO añadir plato
        Serial.println(F("\nPlato a\xF1""adido...")); 
        //tareScale(); 
        printStateAdded(); 
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
        //tareScale(); 
        printStateDeleted();
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
        //tareScale(); 
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
      case 2:   actGruposAlimentos();   break;
      case 3:   actGruposAlimentos();   break;
      case 4:   actStateRaw();          break;
      case 5:   actStateCooked();       break;
      case 6:   actStateWeighted();     break;
      case 7:   actStateAdded();        break;
      case 8:   actStateDeleted();      break;
      case 9:   actStateSaved();        break;
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
