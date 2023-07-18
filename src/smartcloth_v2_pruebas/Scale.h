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
      |   SCK    |   pin 2 (digital PWM Arduino Due) |   
      |   DOUT   |   pin 3 (digital PWM Arduino Due) |
      |   VCC    |   3.3 V                           |
      |   GND    |   GND                             |
      ------------------------------------------------
  
  ------------------------------------------------------------------
 
 */


#ifndef SCALE_H
#define SCALE_H

#include "ISR.h" 
#include "HX711.h"


HX711 scale;


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;


bool      scaleEventOccurred = false;
bool      tarado = false;
float     pesoARetirar = 0.0;         // Peso que se debe retirar para liberar la báscula (recipiente + alimentos)
float     pesoBascula = 0.0;          // Peso utilizado en lugar de 'weight' para no tener en cuenta cambios mínimos

#include "State_Machine.h" // Debajo de las variables para que estén disponibles en su ámbito

/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para pesar la báscula.
 * @return El peso medido por la báscula.
 */
/*-----------------------------------------------------------------------------*/

float   weighScale();
void    tareScale();
void    setupScale();
void    checkBascula();
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------
   weighScale(): Pesar báscula
----------------------------------------------------------------------------------------------------------*/
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
 * \brief Realiza la tara de la báscula y actualiza el peso base.
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
 * @brief Función para inicializar la báscula.
 */
/*-----------------------------------------------------------------------------*/
void setupScale(){
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(1093.48); // bad calibration!
    scale.tare();  
    //scale.get_units(10);
    Serial.println(F("Scale initialized"));
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para comprobar si ha habido algún evento de incremento/decremento en la báscula.
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

        if (diffWeight > 2.0){ // Cambio no causado por "interferencias" de la báscula
            scaleEventOccurred = true;
            
            // 'pesoBascula' representa el peso evitando pequeños saltos en las medidas.
            //  Este valor es el que se usará como peso de los alimentos. 
               
            pesoBascula = newWeight;
            if(pesoBascula < 1.0) pesoBascula = 0.0; // Saturar a 0.0 el peso mostrado y utilizado (pesoBascula)

            if(lastWeight < newWeight){ //INCREMENTO con diferencia de 5 gr
                Serial.println(F("\nIncremento..."));
                if(lastWeight > -1.0){ //Incremento en positivo (salvando valores cercanos a 0)
                    Serial.print(F("\nINCREMENTO"));
                    eventoBascula = INCREMENTO;
                }
                else if(lastWeight <= -1.0){ //Incremento por tara, viniendo de negativo
                    Serial.print(F("\nTARADO"));
                    eventoBascula = TARAR;
                }
            }
            else { //DECREMENTO con diferencia de 5 gr
                Serial.println(F("\nDecremento..."));
                if(newWeight >= 1.0){ //Se ha quitado algo pero no todo (sigue positivo)
                    Serial.print(F("\nDECREMENTO"));
                    eventoBascula = DECREMENTO;
                }
                else if(newWeight < 1.0){ //peso negativo o cercano a 0.0
                    if(tarado){ //Decremento debido a tara
                        Serial.print(F("\nTARADO"));
                        eventoBascula = TARAR;
                    }
                    else{ //Decremento por retirar objeto
                        if(abs(abs(newWeight) - pesoARetirar) < 5.0){ //Nuevo peso (negativo) es contrario (-X = +X) al peso del plato + recipiente ==> se ha quitado todo
                            // Se ha puesto un umbral de 5 gr para saber si se ha retirado todo, pero podría reducirse a 1 gr
                            Serial.print(F("\nLIBERADA"));
                            eventoBascula = LIBERAR;
                            flagRecipienteRetirado = true;
                        }
                        else{ //Se están quitando elementos de la báscula tras haber tarado, por eso se baja a negativo. Se pasa por 'QUITAR' antes de 'LIBERAR'.
                            Serial.print(F("\nQUITANDO"));
                            eventoBascula = QUITAR;
                        }
                    }
                }
            }

            /*Serial.println(F("\n--------------------------------------"));
            Serial.print(F("\nPeso anterior: ")); Serial.println(lastWeight); 
            Serial.print(F("Peso actual: ")); Serial.println(newWeight); 
            Serial.print(F("Peso Bascula: ")); Serial.println(pesoBascula);
            Serial.print(F("Peso a retirar: ")); Serial.println(pesoARetirar);
            Serial.print(F("Peso recipiente: ")); Serial.println(pesoRecipiente);
            Serial.print(F("Peso plato: ")); Serial.println(pesoPlato);
            Serial.println(F("\n--------------------------------------"));*/

            addEventToBuffer(eventoBascula);
            flagEvent = true;
        }
        else scaleEventOccurred = false; // No ha habido evento

        pesado = false;
    }


}

#endif
