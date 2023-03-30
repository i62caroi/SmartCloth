#ifndef VARIABLES_H
#define VARIABLES_H

#include "Grupos.h"
#include "Plato.h"
#include "Scale.h"


/*  -----   BOTONERAS  ----- */
int buttonGrande;                  //Botón pulsado en la botonera grande (checkAllButtons())
volatile int buttonMain = 0;


/* --------- EVENTOS ----------------- */
bool flagEvent;                   /* Para evitar que marque evento para cada interrupción, ya que
                                     lo marcaría cada medio segundo por la interrupción de la báscula.
                                     Con esta flag solo se da aviso de un evento real (pulsación, incremento o decremento) */


/*  -----   BÁSCULA  ----- */
volatile float weight = 0.0;
volatile bool pesado = false;

float diffWeight = 0.0;

bool isScaleEmpty(){
    if(weight < 1.0) return true;
    else return false;
}

/*float displayedWeight;
void updateDisplayedWeight(){
    displayedWeight = diffWeight;
}*/

HX711 scale; //Si se declara en Scale.h no se puede acceder desde State_Machine.h, aunque tenga el include 

void tareScale(){ 
  //displayedWeight = 0.0; 
  scale.tare();  
};



/* ----- PLATO/COMIDA ----- */
Plato platoActual;


#endif
