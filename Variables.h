#ifndef VARIABLES_H
#define VARIABLES_H

#include "Grupos.h"
#include "Plato.h"



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
bool isScaleEmpty(){
    if(weight < 1.0) return true;
    else return false;
}


/* ----- PLATO/COMIDA ----- */
Plato platoActual;


#endif
