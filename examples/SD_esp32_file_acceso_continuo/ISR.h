/**
 * @file ISR.h
 * @brief Este archivo contiene las definiciones de las Rutinas de Servicio de Interrupción (ISR).
 * 
 * @author Irene Casares Rodríguez
 * @date 01/12/2023
 */

#ifndef ISR_H
#define ISR_H




/*  -----   MAIN  ----- */
volatile int  buttonMain = 0; // Botón pulsado en Main (botonera B)
const byte intPinAddPlato     = 29;  
const byte intPinDeletePlato  = 27;   
const byte intPinGuardar      = 25;   

const byte intPinCocinado     = 33;   // Genera valor aleatorio de peso

const byte intPinCrudo        = 31;   // Da por terminado el pesado de un alimento


/*  -----   GRANDE  ----- */
const byte interruptPinGrande = 37;       // Pin de interrupcion RISING para Grande (botonera A)
volatile bool pulsandoGrande  = false;    // Flag de estar pulsando algo en Grande



/*-----------------------------------------------------------------------------
                            DEFINICIONES
-----------------------------------------------------------------------------*/
void ISR_crudo();                         // ISR de botón de 'crudo'
void ISR_cocinado();                      // ISR de botón de 'cocinado'
void ISR_addPlato();                      // ISR de botón de 'añadir plato'
void ISR_deletePlato();                   // ISR de botón de 'eliminar plato'
void ISR_guardar();                       // ISR de botón de 'guardar comida'

void ISR_pulsandoButtonsGrande();         // ISR de botonera de grupos de alimentos
/*-----------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'crudo'
 */
/*-----------------------------------------------------------------------------*/
void ISR_crudo(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 1;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'cocinado'. Genera un valor aleatorio de peso para no usar la báscula.
 */
/*-----------------------------------------------------------------------------*/
void ISR_cocinado(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 2;
    last_interrupt_time = interrupt_time;
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'añadir plato'
 */
/*-----------------------------------------------------------------------------*/
void ISR_addPlato(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 3;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'eliminar plato'
 */
/*-----------------------------------------------------------------------------*/
void ISR_deletePlato(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 4;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botón de 'guardar comida'
 */
/*-----------------------------------------------------------------------------*/
void ISR_guardar(){ 
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) buttonMain = 5;
    last_interrupt_time = interrupt_time;
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief ISR de botonera de grupos de alimentos
 */
/*-----------------------------------------------------------------------------*/
void ISR_pulsandoButtonsGrande(){
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if ((interrupt_time - last_interrupt_time) > 200) pulsandoGrande = true;
    last_interrupt_time = interrupt_time;
}






#endif
