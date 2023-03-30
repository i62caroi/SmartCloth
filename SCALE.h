#ifndef SCALE_H
#define SCALE_H

#include "HX711.h"
#include "State_Machine.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

//HX711 scale;

float newWeight = 0.0;
float lastWeight = 0.0; 



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
    if (pesado){
        lastWeight = newWeight;
        newWeight = weight;
        diffWeight = abs(lastWeight - newWeight);
        if (diffWeight > 1.0){
            if(lastWeight < newWeight){
                Serial.print(F("\nINCREMENTO"));
                eventoBascula = INCREMENTO;
            }
            else if(lastWeight > newWeight){
                /*if(tarado){ //Decremento producido por la tara
                    Serial.print(F("\nDECREMENTO"));
                    eventoBascula = DECREMENTO;
                    tarado = false;
                }
                else{*/
                    if(newWeight > 1.0){ //Se ha quitado algo pero no todo
                      Serial.print(F("\nDECREMENTO"));
                      eventoBascula = DECREMENTO;
                      //if(tarado) tarado = false;
                    }
                    else{ //Se ha quitado todo de la báscula
                        Serial.print(F("\nLIBERADA"));
                        eventoBascula = LIBERAR;
                    }
                //}
            }
            Serial.print(F("\nPeso anterior: ")); Serial.println(lastWeight); 
            Serial.print(F("Peso actual: ")); Serial.println(newWeight); 
            addEventToBuffer(eventoBascula);
            flagEvent = true;
        }
        pesado = false;
    }
}

#endif
