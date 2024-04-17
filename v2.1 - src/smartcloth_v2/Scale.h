/** 
 * @file Scale.h
 * @brief Funciones de la célula de carga (báscula)
 *
 * @author Irene Casares Rodríguez
 * @date 21/06/23
 * @version 1.0
 */

 /*
  --------- CONEXIÓN CÉLULA DE CARGA CON ARDUINO DUE -----------
  
  Célula conectada de la siguiente forma:
      ------------
      |  Célula  |
      ------------------------------------------------
      |   SCK    |   pin 3 (digital PWM Arduino Due) |   
      |   DOUT   |   pin 2 (digital PWM Arduino Due) |
      |   VCC    |   3.3 V                           |
      |   GND    |   GND                             |
      ------------------------------------------------
  
  ------------------------------------------------------------------
 
 */


#ifndef SCALE_H
#define SCALE_H

#include "ISR.h" 
#include "HX711.h"

#include "debug.h" // SM_DEBUG --> SerialPC

HX711 scale;


// ------ HX711 circuit wiring -----------
// SmartCloth v2.1
//const byte LOADCELL_DOUT_PIN = 3;
//const byte LOADCELL_SCK_PIN = 2;
// SmartCloth v2.2
const byte LOADCELL_DOUT_PIN = 2;
const byte LOADCELL_SCK_PIN = 3;
// ---------------------------------------


bool      scaleEventOccurred = false;
bool      tarado = false;

// ------ VARIABLES DE PESO ------------------------------------------------------------
float     pesoARetirar      =   0.0;    // Peso que se debe retirar para liberar la báscula (recipiente + alimentos)
float     pesoBascula       =   0.0;    // Peso utilizado en lugar de 'weight' para no tener en cuenta cambios mínimos
float     pesoRecipiente    =   0.0;    // Peso únicamente del recipiente
float     pesoPlato         =   0.0;    // Peso total del plato (recipiente +  alimentos)
float     pesoLastAlimento  =   0.0;    // Peso del último alimento colocado
// ------ FIN VARIABLES DE PESO --------------------------------------------------------


#include "State_Machine.h" // Debajo de las variables para que estén disponibles en su ámbito



/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    setupScale();
float   weighScale();
void    tareScale();
void    reiniciarPesos();
void    checkBascula();
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para inicializar la báscula.
 */
/*-----------------------------------------------------------------------------*/
void setupScale(){
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    //scale.set_scale(1093.48); // bad calibration!
    scale.set_scale(939.51); // para celda de carga que pesa 30 gr
    scale.tare(1);  
    //scale.get_units(10);
    #if defined(SM_DEBUG)
    SerialPC.println(F("Scale initialized"));
    #endif
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para pesar la báscula.
 * @return El peso medido por la báscula.
 */
/*-----------------------------------------------------------------------------*/
float weighScale(){
    return scale.get_units(1);
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Realiza la tara de la báscula y actualiza el peso base.
 */
 /*-----------------------------------------------------------------------------*/
void  tareScale(){ 
    scale.tare(1);  // 1 toma de valor
    pesoBascula = weighScale();;
    if(pesoBascula < 1.0) pesoBascula = 0.0; // Saturar a 0.0 el peso mostrado y utilizado (pesoBascula)
    tarado = true;
};



/*-----------------------------------------------------------------------------*/
/**
 * @brief Reinicia los pesos de recipiente, plato y alimento.
 */
 /*-----------------------------------------------------------------------------*/
void reiniciarPesos(){
    pesoRecipiente = 0.0;           // Se reinicia 'pesoRecipiente', que se sumará a 'pesoPlato' para saber el 'pesoARetirar'.
    pesoPlato = 0.0;                // Se reinicia 'pesoPlato', que se sumará a 'pesoRecipiente' para saber el 'pesoARetirar'.
    pesoLastAlimento = 0.0;         // Se reinicia 'pesoLastAlimento', que, si hubiera un último alimento que añadir en delete,
                                    // se sumará a 'pesoPlato' y luego a 'pesoRecipiente' para saber el 'peroARetirar'.
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para comprobar si ha habido algún evento en la báscula y determinar el tipo de evento.
 */
/*-----------------------------------------------------------------------------*/
void checkBascula(){
    static float newWeight = 0.0;
    static float lastWeight = 0.0;
    static float diffWeight = 0.0; 
    
    if (pesado){

        pesoARetirar = pesoRecipiente + pesoPlato;

        lastWeight = newWeight;
        newWeight = weight;
        diffWeight = abs(lastWeight - newWeight);


        // ----------------------------------------------------
        // ------- COMPROBAR SI HA HABIDO EVENTO --------------
        // ----------------------------------------------------

        if(tarado){
            #if defined(SM_DEBUG)
            SerialPC.print(F("\nTARANDO"));
            #endif
            eventoBascula = TARAR;
        }
        else{
            if(diffWeight > 2.0){ // Si ha habido una variación de peso de más de 2 gramos y no ha sido causada por una tara --> evento
                scaleEventOccurred = true;
                
                // 'pesoBascula' representa el peso evitando pequeños saltos en las medidas.
                //  Este valor es el que se usará como peso individual de los alimentos. 
                  
                pesoBascula = newWeight;
                if(pesoBascula < 1.0) pesoBascula = 0.0; // Saturar a 0.0 el peso mostrado y utilizado (pesoBascula)


                // ----------------------------------------------------
                // --------- RECONOCER EVENTO OCURRIDO ----------------
                // ----------------------------------------------------

                if(lastWeight < newWeight){ // Incremento de peso   
                    #if defined(SM_DEBUG)              
                    SerialPC.print(F("\nINCREMENTO"));
                    #endif
                    eventoBascula = INCREMENTO;
                }
                else { // Decremento de peso 
                    if(abs(abs(newWeight) - pesoARetirar) < 5.0){ //Nuevo peso (negativo) es contrario (-X = +X) al peso del plato + recipiente ==> se ha quitado todo
                        // Se ha puesto un umbral de 5 gr para saber si se ha retirado todo, pero podría reducirse a 1 gr
                        #if defined(SM_DEBUG)
                        SerialPC.print(F("\nLIBERADA"));
                        #endif
                        eventoBascula = LIBERAR;
                        flagRecipienteRetirado = true; // Se ha retirado el plato completo --> pantalla recipienteRetirado()
                    }
                    else{ // Se están retirando elementos de la báscula pero aún no se ha liberado
                        #if defined(SM_DEBUG)
                        SerialPC.print(F("\nDECREMENTO"));
                        #endif
                        eventoBascula = DECREMENTO;
                    }
                }


                #if defined(SM_DEBUG)
                SerialPC.println(F("\n--------------------------------------"));
                SerialPC.print(F("\nPeso anterior: ")); SerialPC.println(lastWeight); 
                SerialPC.print(F("Peso actual: ")); SerialPC.println(newWeight); 
                SerialPC.print(F("Peso Bascula: ")); SerialPC.println(pesoBascula);
                SerialPC.print(F("Peso a retirar: ")); SerialPC.println(pesoARetirar);
                SerialPC.print(F("Peso recipiente: ")); SerialPC.println(pesoRecipiente);
                SerialPC.print(F("Peso plato: ")); SerialPC.println(pesoPlato);
                SerialPC.println(F("\n--------------------------------------"));
                #endif

                addEventToBuffer(eventoBascula);
                flagEvent = true;
            }
            else scaleEventOccurred = false; // No ha habido evento
        }

        pesado = false;

        if(tarado) tarado = false;
    }


}



#endif
