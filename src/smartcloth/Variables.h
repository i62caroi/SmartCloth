#ifndef VARIABLES_H
#define VARIABLES_H

#include "Grupos.h"
#include "Diario.h" // Comida.h (Plato.h)
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
float pesoBascula = 0.0;


HX711 scale; //Si se declara en Scale.h no se puede acceder desde State_Machine.h por inclusiones múltiples (?)

//bool tarado = false;
void tareScale(){ 
  scale.tare(1);  //5 tomas de valor
  //tarado = true;
};



/* ----- CRUDO/COCINADO ----- */
String procesamiento; // "CRUDO" o "COCINADO"


/* ----- PLATO/COMIDA/DIARIO ----- */
Plato platoActual;
//bool platoNuevo = true;
Comida comidaActual;
Diario diaActual;


#endif
