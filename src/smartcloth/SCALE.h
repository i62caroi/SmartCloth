#ifndef SCALE_H
#define SCALE_H

#include "HX711.h"
#include "State_Machine.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

//HX711 scale; 

float pesoRetirado = 0.0;


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
}



/*---------------------------------------------------------------------------------------------------------
   checkBascula(): Comprobar si ha habido algún evento de incremento/decremento en la báscula
----------------------------------------------------------------------------------------------------------*/
void checkBascula(){
    static float newWeight = 0.0;
    static float lastWeight = 0.0;
    static float diffWeight = 0.0;
    
    if (pesado){
        lastWeight = pesoBascula;
        newWeight = weight;
        diffWeight = abs(lastWeight - newWeight);
        if (diffWeight > 1.0){ // Cambio no causado por "interferencias" de la báscula
            /* 'pesoBascula' representa el peso evitando pequeños saltos en las medidas.
               Este valor es el que se usará como peso de los ingredientes.
               Hasta ahora se usaba 'weight' directamente, pero al añadir un nuevo ingrediente
               al plato se tomaba el último valor de 'weight', que podría haber variado algo 
               respecto a lo que se mostraba por sucesivas medidas de la báscula. */
               
            pesoBascula = newWeight;
            fixPesoBascula();
            
            if(lastWeight < newWeight){ //INCREMENTO
                Serial.println(F("\nIncremento..."));
                if(lastWeight > -1.0){ //Incremento pero sin venir de negativo (salvando valores cercanos a 0)
                    Serial.print(F("\nINCREMENTO"));
                    eventoBascula = INCREMENTO;
                    pesoRetirado = 0.0;
                }
                else if(lastWeight <= -1.0){ //Incremento por tara viniendo de negativo
                    Serial.print(F("\nTARADO"));
                    eventoBascula = TARAR;
                    pesoRetirado = 0.0;
                }
            }
            else{ //DECREMENTO
              
                    Serial.println(F("\nDecremento..."));
                    if(newWeight > 1.0){ //Se ha quitado algo pero no todo
                      Serial.print(F("\nDECREMENTO"));
                      eventoBascula = DECREMENTO;
                    }
                    else if(newWeight < 1.0){ //Se ha quitado todo de la báscula (newWeight < 1.0)
                        if(tarado){ //Decremento debido a tara
                            Serial.print(F("\nTARADO"));
                            eventoBascula = TARAR;
                        }
                        else{ //Decremento por retirar objeto
                            pesoRetirado += diffWeight; //Actualizamos el peso retirado
                            if((abs(pesoRetirado - pesoPlato ) < 5.0) or (newWeight < 1.0)){ //Bascula vacía real habiendo guardado el plato o no
                                Serial.print(F("\nLIBERADA"));
                                eventoBascula = LIBERAR;
                            }
                            else{ //Se están quitando elementos de la báscula
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
            Serial.print(F("Peso retirado: ")); Serial.println(pesoRetirado);
            Serial.print(F("Peso plato: ")); Serial.println(pesoPlato);
            Serial.println(F("\n--------------------------------------"));
            addEventToBuffer(eventoBascula);
            flagEvent = true;
        }
        pesado = false;
    }

    /*if(tarado){
        if(abs(pesoBascula) < 1.0) pesoBascula = 0.0;
        tarado = false;
    }*/
}

#endif
