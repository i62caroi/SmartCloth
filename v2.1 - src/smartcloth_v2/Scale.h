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
      |   SCK    |   pin 3 (digital PWM Arduino Due) |   
      |   DOUT   |   pin 2 (digital PWM Arduino Due) |
      |   VCC    |   3.3 V                           |
      |   GND    |   GND                             |
      ------------------------------------------------
  
  ------------------------------------------------------------------
 
 */


#ifndef SCALE_H
#define SCALE_H

#include "ISR.h" 
#include "HX711.h"

#include "debug.h" // SM_DEBUG --> SerialPC

HX711 scale;


// ------ HX711 circuit wiring -----------
const byte LOADCELL_DOUT_PIN = 2;
const byte LOADCELL_SCK_PIN = 3;
// ---------------------------------------


bool      scaleEventOccurred = false;   // Flag para indicar que ha cambiado el peso de la báscula
bool      tarado = false;               // Flag para indicar que se ha tarado la báscula

// ------ VARIABLES DE PESO ------------------------------------------------------------
float     pesoARetirar      =   0.0;    // Peso que se debe retirar para liberar la báscula (recipiente + alimentos)
float     pesoBascula       =   0.0;    // Peso utilizado en lugar de 'weight' para no tener en cuenta cambios mínimos
float     pesoRecipiente    =   0.0;    // Peso únicamente del recipiente
float     pesoPlato         =   0.0;    // Peso total del plato (recipiente +  alimentos)
float     pesoLastAlimento  =   0.0;    // Peso del último alimento colocado
// ------ FIN VARIABLES DE PESO --------------------------------------------------------

#define UMBRAL_MIN_CAMBIO_PESO 5.0      // Cambio mínimo del peso para considerar que se ha colocado/retirado algo de la báscula
#define UMBRAL_RECIPIENTE_RETIRADO 20.0 // Umbral para considerar que se ha retirado todo (recipiente + alimentos) de la báscula
                                        // 20 gramos porque asumimos que un plato no pesará 20 gramos y así es más fácil detectar si se ha retirado todo



#include "State_Machine.h" // Debajo de las variables para que estén disponibles en su ámbito


/*******************************************************************************
/*******************************************************************************
                          DECLARACIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/
void            setupScale();                                   // Inicializar báscula
inline float    weighScale(){ return  scale.get_units(3); };    // Pesar báscula
void            tareScale();                                    // Tarar báscula
void            reiniciarPesos();                               // Reiniciar pesos de recipiente, plato y alimento
void            checkBascula();                                 // Comprobar si ha habido algún evento en la báscula y determinar el tipo de evento
/******************************************************************************/
/******************************************************************************/




/*******************************************************************************
/*******************************************************************************
                           DEFINICIÓN FUNCIONES
/******************************************************************************/
/******************************************************************************/


/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para inicializar la báscula.
 * 
 * Pasos para calibrar la báscula:
 *      1. Poner un objeto con peso conocido sobre la báscula. Por ejemplo, un objeto de 210 gramos.
 *      2. Observar cuánto "peso" (valor bruto) marca la celda de carga con el peso conocido encima. Supongamos que la celda de carga muestra un valor de 1000.
 *      3. Tomar el valor de escala inicial (el marcado en scale.set_scale(1058.22)) y multiplicarlo por el valor obtenido en el paso 2. Ejemplo: 1058.22 * 1000 = 1058220.
 *      4. El resultado obtenido en el paso 3 se divide por el peso conocido en gramos. Ejemplo: 1058220 / 210 = 5039.14. Este es el nuevo valor de escala.
 *      5. Usar el nuevo valor de escala obtenido en el paso 4 para ajustar la báscula correctamente llamando a scale.set_scale() con el nuevo valor. Ejemplo: scale.set_scale(5039.14);
 */
/*-----------------------------------------------------------------------------*/
void setupScale()
{
    #if defined(SM_DEBUG)
        SerialPC.println(F("\nInit Scale..."));
    #endif

    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // Inicializa la celda de carga con los pines especificados

    scale.set_scale(1005.27); // Establecer escala inicial. Este valor se ajustará para calibrar la báscula.
    //scale.tare();  // Tarar tomando la media de 10 medidas
    scale.tare(5);

    #if defined(SM_DEBUG)
        SerialPC.println(F("Scale initialized"));
        SerialPC.println("++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    #endif
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Realiza la tara de la báscula y actualiza el peso base.
 */
 /*-----------------------------------------------------------------------------*/
void  tareScale()
{ 
    scale.tare(1);  // 1 toma de valor
    pesoBascula = weighScale();;
    if(pesoBascula < 1.0) pesoBascula = 0.0; // Saturar a 0.0 el peso mostrado y utilizado (pesoBascula)
    tarado = true;
};



/*-----------------------------------------------------------------------------*/
/**
 * @brief Reinicia los pesos de recipiente, plato y alimento.
 */
 /*-----------------------------------------------------------------------------*/
void reiniciarPesos()
{
    pesoRecipiente = 0.0;           // Se reinicia 'pesoRecipiente', que se sumará a 'pesoPlato' para saber el 'pesoARetirar'.
    pesoPlato = 0.0;                // Se reinicia 'pesoPlato', que se sumará a 'pesoRecipiente' para saber el 'pesoARetirar'.
    pesoLastAlimento = 0.0;         // Se reinicia 'pesoLastAlimento', que, si hubiera un último alimento que añadir en delete,
                                    // se sumará a 'pesoPlato' y luego a 'pesoRecipiente' para saber el 'peroARetirar'.
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para comprobar si ha habido algún evento en la báscula y determinar el tipo de evento.
 */
/*-----------------------------------------------------------------------------*/
void checkBascula()
{
    static float newWeight = 0.0;
    static float lastWeight = 0.0;
    static float diffWeight = 0.0; 
    
    if (pesado)
    {
        pesoARetirar = pesoRecipiente + pesoPlato;

        lastWeight = newWeight;
        newWeight = weight;
        diffWeight = abs(lastWeight - newWeight);


        // ----------------------------------------------------
        // ------- COMPROBAR SI HA HABIDO EVENTO --------------
        // ----------------------------------------------------

        if(tarado)
        {
            #if defined(SM_DEBUG)
                SerialPC.println(F("\nTARANDO"));
            #endif
            eventoBascula = TARAR;
        }
        else
        {
            if(diffWeight > UMBRAL_MIN_CAMBIO_PESO) // Si ha habido una variación de peso de más de 2 gramos y no ha sido causada por una tara --> evento
            {
                scaleEventOccurred = true;
                
                // 'pesoBascula' representa el peso evitando pequeños saltos en las medidas.
                //  Este valor es el que se usará como peso individual de los alimentos. 
                  
                pesoBascula = newWeight;
                if(pesoBascula < 1.0) pesoBascula = 0.0; // Saturar a 0.0 el peso mostrado y utilizado (pesoBascula)


                // ----------------------------------------------------
                // --------- RECONOCER EVENTO OCURRIDO ----------------
                // ----------------------------------------------------

                if(lastWeight < newWeight) // Incremento de peso   
                {
                    #if defined(SM_DEBUG)              
                        SerialPC.print(F("\nINCREMENTO"));
                    #endif
                    eventoBascula = INCREMENTO;
                }
                else  // Decremento de peso 
                {
                    if(abs(abs(newWeight) - pesoARetirar) < UMBRAL_RECIPIENTE_RETIRADO) //Nuevo peso (negativo) es contrario (-X = +X) al peso del plato + recipiente ==> se ha quitado todo
                    {
                        #if defined(SM_DEBUG)
                            SerialPC.print(F("\nLIBERADA"));
                        #endif
                        eventoBascula = LIBERAR;
                        flagRecipienteRetirado = true; // Se ha retirado el plato completo --> pantalla recipienteRetirado()
                    }
                    else // Se están retirando elementos de la báscula pero aún no se ha liberado
                    {
                        #if defined(SM_DEBUG)
                            SerialPC.print(F("\nDECREMENTO"));
                        #endif
                        eventoBascula = DECREMENTO;
                    }
                }


                #if defined(SM_DEBUG)
                    SerialPC.println(F("\n--------------------------------------"));
                    SerialPC.print(F("\nPeso anterior: ")); SerialPC.println(lastWeight); 
                    SerialPC.print(F("Peso actual: ")); SerialPC.println(newWeight); 
                    SerialPC.print(F("Peso Bascula: ")); SerialPC.println(pesoBascula);
                    SerialPC.print(F("Peso a retirar: ")); SerialPC.println(pesoARetirar);
                    SerialPC.print(F("Peso recipiente: ")); SerialPC.println(pesoRecipiente);
                    SerialPC.print(F("Peso plato: ")); SerialPC.println(pesoPlato);
                    SerialPC.println(F("\n--------------------------------------"));
                #endif

                addEventToBuffer(eventoBascula);
                flagEvent = true;
            }
            else scaleEventOccurred = false; // No ha habido evento
        }

        pesado = false;

        if(tarado) tarado = false;
    }


}




/******************************************************************************/
/******************************************************************************/


#endif
