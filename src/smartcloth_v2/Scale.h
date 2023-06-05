
/* --------- CONEXIÓN CÉLULA DE CARGA CON ARDUINO DUE -----------
  
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

#include "HX711.h"
#include "State_Machine.h"



// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
float   weighScale();
void    setupScale();
void    checkBascula();
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------
   weighScale(): Pesar báscula
----------------------------------------------------------------------------------------------------------*/
float weighScale(){
    return scale.get_units(1);
}


/*---------------------------------------------------------------------------------------------------------
   setupScale(): Inicializar báscula
----------------------------------------------------------------------------------------------------------*/
void setupScale(){
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(1093.48); // bad calibration!
    scale.tare();  
    //scale.get_units(10);
    Serial.println(F("Scale initialized"));
}



/*---------------------------------------------------------------------------------------------------------
   checkBascula(): Comprobar si ha habido algún evento de incremento/decremento en la báscula
----------------------------------------------------------------------------------------------------------*/
void checkBascula(){
    static float newWeight = 0.0;
    static float lastWeight = 0.0;
    
    if (pesado){
        pesoARetirar = pesoRecipiente + pesoPlato;

        // ------- FORZAR REGRESO A INI EN CASOS ESPECIALES ------------------
        // Esto es necesario porque al volver a INI se tara, marcando eventoBascula = TARAR, por lo que se
        // debe forzar la liberación (eventoBascula = LIBERAR) para, tras guardar o no la comida (STATE_saved), 
        // o borrar o no plato (STATE_deleted), se pueda regresar a INI. 
        // Si no se hace esto, se queda atascado esperando una liberación que no va a llegar porque no hay 
        // cambios en el peso (diffWeight > 2.0). 
        /*if(((state_actual == STATE_saved) or (state_actual == STATE_deleted)) and (state_actual == STATE_Empty)){ //Solo si se ha querido guardar desde el STATE_Empty 
            Serial.print(F("\nLIBERADA sola"));
            eventoBascula = LIBERAR;
            addEventToBuffer(eventoBascula);
            flagEvent = true;
        }*/ // ------------------------------------------------------------------

            
        lastWeight = newWeight;
        newWeight = weight;
        diffWeight = abs(lastWeight - newWeight);
        if (diffWeight > 2.0){ // Cambio no causado por "interferencias" de la báscula
            
            /* 'pesoBascula' representa el peso evitando pequeños saltos en las medidas.
               Este valor es el que se usará como peso de los alimentos.
               Hasta ahora se usaba 'weight' directamente, pero al añadir un nuevo alimento
               al plato se tomaba el último valor de 'weight', que podría haber variado algo 
               respecto a lo que se mostraba por sucesivas medidas de la báscula. */
               
            pesoBascula = newWeight;
            if(pesoBascula < 1.0) pesoBascula = 0.0; //fixPesoBascula

            if(lastWeight < newWeight){ //INCREMENTO
                Serial.println(F("\nIncremento..."));
                if(lastWeight > -1.0){ //Incremento en positivo (salvando valores cercanos a 0)
                    Serial.print(F("\nINCREMENTO"));
                    eventoBascula = INCREMENTO;
                }
                else if(lastWeight <= -1.0){ //Incremento por tara viniendo de negativo
                    Serial.print(F("\nTARADO"));
                    eventoBascula = TARAR;
                }
            }
            else{ //DECREMENTO
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
                        }
                        else{ //Se están quitando elementos de la báscula tras haber tarado, por eso se baja a negativo. Se pasa por 'QUITAR' antes de 'LIBERAR'.
                            Serial.print(F("\nQUITANDO"));
                            eventoBascula = QUITAR;
                        }
                    }
                }
            }

            Serial.println(F("\n--------------------------------------"));
            Serial.print(F("\nPeso anterior: ")); Serial.println(lastWeight); 
            Serial.print(F("Peso actual: ")); Serial.println(newWeight); 
            Serial.print(F("Peso Bascula: ")); Serial.println(pesoBascula);
            Serial.print(F("Peso a retirar: ")); Serial.println(pesoARetirar);
            Serial.println(F("\n--------------------------------------"));

            addEventToBuffer(eventoBascula);
            flagEvent = true;
        }
        pesado = false;
    }


}

#endif
