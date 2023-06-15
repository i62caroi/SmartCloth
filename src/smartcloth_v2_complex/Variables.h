/** 
 * @file Variables.h
 * @brief Contiene algunas variables y declaraciones utilizadas en la máquina de estados.
 *
 * @author Irene Casares Rodríguez
 * @date 14/06/23
 * @version 1.0
 *
 *  Este archivo contiene las definiciones de los estados y eventos, además de las funciones
 *  realizadas en cada estado y para el funcionamiento de la máquina de estados, incluyendo el
 *  manejo de errores y avisos.
 *  
 */

#ifndef VARIABLES_H
#define VARIABLES_H


#include "Scale.h"
#include "Diario.h" // Comida.h => Plato.h => Alimento.h => ValoresNutricionales.h
                    //                                      => Grupos.h



/*  -----   BOTONERAS  ----- */
int           buttonGrande;                  //Botón pulsado en la botonera grande (checkAllButtons())
volatile int  buttonMain = 0;



/* --------- EVENTOS ----------------- */
bool  flagEvent;                   /* Para evitar que marque evento para cada interrupción, ya que
                                     lo marcaría cada medio segundo por la interrupción de la báscula.
                                     Con esta flag solo se da aviso de un evento real (pulsación, incremento o decremento) */
bool  flagError;                   // Aunque ERROR sea un evento, es de diferente naturaleza porque es ficticio, por eso
                                   // se utiliza una flag diferente para cuando ocurra.



/*  -----   BÁSCULA  ----- */
volatile float  weight = 0.0;         ///< Peso real tomado por ISR
volatile bool   pesado = false;       // Flag de haber pesado por ISR
float           pesoBascula = 0.0;    // Peso utilizado en lugar de 'weight' para no tener en cuenta cambios mínimos
float           diffWeight = 0.0;     // Diferencia entre peso actual (weight) y peso anterior para ver cambios

float           pesoARetirar;         // Peso que se debe retirar para liberar la báscula

HX711           scale;                // Si se declara en Scale.h no se puede acceder desde State_Machine.h por inclusiones múltiples (?)

bool            tarado;

/**
 * \brief Obtiene el peso de la báscula.
 * \return El peso actual de la báscula.
 */
float           weighScale(){ return scale.get_units(1); }

/**
 * \brief Realiza la tara de la báscula y actualiza el peso base.
 */
void            tareScale(){ 
                    scale.tare(1);  //1 toma de valor
                    pesoBascula = weighScale();;
                    if(pesoBascula < 1.0) pesoBascula = 0.0; //fixPesoBascula
                    tarado = true;
};



/* ----- CRUDO/COCINADO ----- */
bool      procesado = false; // procesado = false --> crudo    procesado = true --> cocinado     
//String procesamiento; // "CRUDO" o "COCINADO"



/* ----- PLATO/COMIDA/DIARIO ----- */
Plato     platoActual;
float     pesoRecipiente; 
float     pesoPlato;
float     pesoLastAlimento;
Comida    comidaActual;
Diario    diaActual; // Acumulado Hoy


/*  ------ SD ------ */
// LIBRERÍA SD LIMITA LA LONGITUD DEL NOMBRE DEL FICHERO A 8 CARACTERES Y 3 PARA EXTENSION
// Los 8 caracteres no incluyen el path hasta el fichero

// --- FICHERO GUARDAR INFO ---
char    fileCSV[30] = "data/data-SC.csv"; ///< Nombre del archivo para guardar las comidas realizadas

// --- IMAGENES RELOJ ARENA ---
char    fileReloj1[30]    = "bin/carga/cuarto/reloj1.bin";
char    fileReloj2[30]    = "bin/carga/cuarto/reloj2.bin";
char    fileReloj3[30]    = "bin/carga/cuarto/reloj3.bin";
char    fileReloj4[30]    = "bin/carga/cuarto/reloj4.bin";
char    fileRelGir1[30]   = "bin/carga/cuarto/relGir1.bin";
char    fileRelGir2[30]   = "bin/carga/cuarto/relGir2.bin";
char    fileRelGir3[30]   = "bin/carga/cuarto/relGir3.bin";
char    fileRelGir4[30]   = "bin/carga/cuarto/relGir4.bin";
char    fileRelGir5[30]   = "bin/carga/cuarto/relGir5.bin";
char    fileRelGir6[30]   = "bin/carga/cuarto/relGir6.bin";

// --- IMAGENES LETRAS LOGO ARRANQUE ---
char    fileS[30]     = "bin/arranque/S.bin";
char    fileM[30]     = "bin/arranque/M.bin";
char    fileA[30]     = "bin/arranque/A.bin";
char    fileR[30]     = "bin/arranque/R.bin";
char    fileT[30]     = "bin/arranque/T.bin";
char    fileC[30]     = "bin/arranque/C.bin";
char    fileL[30]     = "bin/arranque/L.bin";
char    fileO[30]     = "bin/arranque/O.bin";
char    fileH[30]     = "bin/arranque/H.bin";
char    fileLogo[30]  = "bin/arranque/Log.bin";

// --- IMAGENES PANTALLA INICIAL ---
char    fileBrain1[30]  = "bin/inicial/brain1.bin";
char    fileBrain2[30]  = "bin/inicial/brain2.bin";

// --- IMAGENES GRUPOS Y MANO ---
char    fileGrupo1[30]  = "bin/grupo/grupo1.bin";
char    fileGrupo2[30]  = "bin/grupo/grupo2.bin";
char    fileGrupo3[30]  = "bin/grupo/grupo3.bin";
char    fileGrupo4[30]  = "bin/grupo/grupo4.bin";
//char    fileHandW[30]   = "bin/grupo/handW.bin";
char    fileManoW[30]   = "bin/grupo/manoW.bin";

// --- IMAGENES BOTONES ---
char    fileBotonAnadir[30]     = "bin/botones/anadir.bin";
char    fileBotonEliminar[30]   = "bin/botones/borrar.bin";
char    fileBotonGuardar[30]    = "bin/botones/guardar.bin";
char    fileCocinadoGrande[30]  = "bin/botones/cociGra.bin";
char    fileCrudoGrande[30]     = "bin/botones/crudoGra.bin";

// --- IMAGENES DASHBOARD ---
char    fileCocinadoPeq[30]     = "bin/dash/cociPeq.bin";
char    fileCrudoPeq[30]        = "bin/dash/crudoPeq.bin";
//char    fileKcal[30]            = "bin/dash/kcal.bin";
char    fileKcal20[30]          = "bin/dash/kcal_20.bin";

// --- IMAGENES ERROR Y AVISO ---
char    fileCruz[30]              = "bin/error/cruz.bin";
char    fileAvisoRed[30]          = "bin/aviso/aviso2R.bin";
char    fileAvisoDarkOrange[30]   = "bin/aviso/aviso2O.bin";



/* ------- RTC ------- */
//char *today; // Fecha de hoy



#endif
