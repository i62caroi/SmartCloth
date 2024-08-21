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
#include "Screen.h"

HX711 scale;
#define SerialPC Serial

// ------ HX711 circuit wiring -----------
const byte LOADCELL_DOUT_PIN = 2;
const byte LOADCELL_SCK_PIN = 3;
// ---------------------------------------


bool      scaleEventOccurred = false;   // Flag para indicar que ha cambiado el peso de la báscula

// ------ VARIABLES DE PESO ------------------------------------------------------------
float     pesoARetirar      =   0.0;    // Peso que se debe retirar para liberar la báscula (recipiente + alimentos)
float     pesoBascula       =   0.0;    // Peso utilizado en lugar de 'weight' para no tener en cuenta cambios mínimos
float     pesoRecipiente    =   0.0;    // Peso únicamente del recipiente
float     pesoPlato         =   0.0;    // Peso total del plato (recipiente +  alimentos)
float     pesoLastAlimento  =   0.0;    // Peso del último alimento colocado
// ------ FIN VARIABLES DE PESO --------------------------------------------------------

#define UMBRAL_MIN_CAMBIO_PESO 5.0      // Cambio mínimo del peso para considerar que se ha colocado/retirado algo de la báscula



/******************************************************************************/
void    setupScale();       // Inicializar báscula
float   weighScale();       // Pesar báscula
void    checkBascula();     // Comprobar si ha habido algún evento en la báscula y determinar el tipo de evento
/******************************************************************************/



void setupScale()
{
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN); // Inicializa la celda de carga con los pines especificados

    scale.set_scale(907.89); // Establecer escala inicial. Este valor se ajustará para calibrar la báscula.
    //scale.tare();  // Tarar tomando la media de 10 medidas
    scale.tare(5);
}



float weighScale(){
    return  scale.get_units(3);
}



void checkBascula(){
    static float newWeight = 0.0;
    static float lastWeight = 0.0;
    static float diffWeight = 0.0; 
    
    if (pesado)
    {
        lastWeight = newWeight;
        newWeight = weight;
        diffWeight = abs(lastWeight - newWeight);

        if(diffWeight > UMBRAL_MIN_CAMBIO_PESO){ // Si ha habido una variación de peso de más de 2 gramos y no ha sido causada por una tara --> evento
            
            scaleEventOccurred = true;

            // 'pesoBascula' representa el peso evitando pequeños saltos en las medidas.
            //  Este valor es el que se usará como peso individual de los alimentos. 
                
            pesoBascula = newWeight;
            if(pesoBascula < 1.0) pesoBascula = 0.0; // Saturar a 0.0 el peso mostrado y utilizado (pesoBascula)

            SerialPC.print("Peso: "); SerialPC.print(pesoBascula); SerialPC.println(" g");
            if(initScreen){ 
                tft.clearArea(280,240, 800, 400, BLUE);
                tft.setTextForegroundColor(WHITE);   
                tft.setCursor(300,250); 
                String cad = "PESO: " + String(pesoBascula) + " g";
                tft.println(cad); 
            }
        }
        else scaleEventOccurred = false; // No ha habido evento

        pesado = false;

    }


}




/******************************************************************************/
/******************************************************************************/


#endif
