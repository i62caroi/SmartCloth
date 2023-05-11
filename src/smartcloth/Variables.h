#ifndef VARIABLES_H
#define VARIABLES_H


#include "Scale.h"
#include "Diario.h" // Comida.h => Plato.h => Ingrediente.h => ValoresNutricionales.h
                    //                                      => Grupos.h



/*  -----   BOTONERAS  ----- */
int buttonGrande;                  //Botón pulsado en la botonera grande (checkAllButtons())
volatile int buttonMain = 0;



/* --------- EVENTOS ----------------- */
bool flagEvent;                   /* Para evitar que marque evento para cada interrupción, ya que
                                     lo marcaría cada medio segundo por la interrupción de la báscula.
                                     Con esta flag solo se da aviso de un evento real (pulsación, incremento o decremento) */



/*  -----   BÁSCULA  ----- */
volatile float weight = 0.0;        // Peso real tomado por ISR
volatile bool pesado = false;       // Flag de haber pesado por ISR
float pesoBascula = 0.0;            // Peso utilizado en lugar de 'weight' para no tener en cuenta cambios mínimos
float diffWeight = 0.0;             // Diferencia entre peso actual (weight) y peso anterior para ver cambios

float pesoARetirar;

HX711 scale; //Si se declara en Scale.h no se puede acceder desde State_Machine.h por inclusiones múltiples (?)


bool tarado;
void tareScale(){ 
  scale.tare(1);  //1 toma de valor
  pesoBascula = weight;
  if(pesoBascula < 1.0) pesoBascula = 0.0; //fixPesoBascula
  tarado = true;
};



/* ----- CRUDO/COCINADO ----- */
//String procesamiento; // "CRUDO" o "COCINADO"
//char fileRaw[30] = "bmp/crudo.bmp"; 
//char fileCooked[30] = "bmp/cocinado.bmp";


/* ----- PLATO/COMIDA/DIARIO ----- */
Plato platoActual;
float pesoRecipiente; 
float pesoPlato;
Comida comidaActual;
Diario diaActual;


/*  ------ SD ------ */
// LIBRERÍA SD LIMITA LA LONGITUD DEL NOMBRE DEL FICHERO A 8 CARACTERES Y 3 PARA EXTENSION
// Los 8 caracteres no incluyen el path hasta el fichero
char fileCSV[30] = "data/data-SC.csv"; //dd-mm-yy podría servir
//char fielBMP[30] = "bmp/logo3.bmp";
char fileBIN[30] = "bin/logo.bin";



#endif
