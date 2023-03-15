#ifndef SCALE_H
#define SCALE_H

#include "HX711.h"
//#include "ISR.h" 
#include "aux.h"
#include "STATE_MACHINE.h"


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;

HX711 scale;

float newWeight;
float lastWeight;

//float getWeight(){ return weight; }

void tareScale(){ scale.tare();  }
float weighScale(){  return scale.get_units(1); }

void setupScale(){
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(1093.48); // bad calibration!
    tareScale();  
    // read
    //scale.get_units(10);
}

void checkBascula(){
    if (pesado){
        lastWeight = newWeight;
        newWeight = weight;
        if (abs(lastWeight - newWeight) > 1.0){
            //Serial.println(F("\nSe ha colocado o retirado algo"));
            if(lastWeight < newWeight){
                Serial.print(F("\nINCREMENTO"));
                eventoBascula = INCREMENTO;
                flagEvent = true;
            }
            else if(lastWeight > newWeight){
                Serial.print(F("\nDECREMENTO"));
                eventoBascula = DECREMENTO;
                flagEvent = true;
            }
            Serial.print(F("\nPeso anterior: ")); Serial.println(lastWeight); 
            Serial.print(F("Peso nuevo: ")); Serial.println(newWeight); 
            addEventToBuffer(eventoBascula);
        }
        pesado = false;
    }
}

#endif
