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
      |   SCK    |   pin 2 (digital PWM Arduino Due) |   
      |   DOUT   |   pin 3 (digital PWM Arduino Due) |
      |   VCC    |   3.3 V                           |
      |   GND    |   GND                             |
      ------------------------------------------------
  
  ------------------------------------------------------------------
 
 */


#ifndef SCALE_H
#define SCALE_H

#include "ISR.h" 
#include "HX711.h"


HX711 scale;


// HX711 circuit wiring
const int LOADCELL_DOUT_PIN = 3;
const int LOADCELL_SCK_PIN = 2;


bool      scaleEventOccurred = false;
bool      tarado = false;
float     pesoARetirar = 0.0;         // Peso que se debe retirar para liberar la báscula (recipiente + alimentos)
float     pesoBascula = 0.0;          // Peso utilizado en lugar de 'weight' para no tener en cuenta cambios mínimos

#include "State_Machine.h" // Debajo de las variables para que estén disponibles en su ámbito



/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    setupScale();
float   weighScale();
void    tareScale();
void    checkBascula();
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para inicializar la báscula.
 */
/*-----------------------------------------------------------------------------*/
void setupScale(){
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(1093.48); // bad calibration!
    scale.tare();  
    //scale.get_units(10);
    Serial.println(F("Scale initialized"));
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para pesar la báscula.
 * @return El peso medido por la báscula.
 */
/*-----------------------------------------------------------------------------*/
float weighScale(){
    return scale.get_units(1);
}


/*-----------------------------------------------------------------------------*/
/**
 * \brief Realiza la tara de la báscula y actualiza el peso base.
 */
 /*-----------------------------------------------------------------------------*/
void  tareScale(){ 
    scale.tare(1);  // 1 toma de valor
    pesoBascula = weighScale();;
    if(pesoBascula < 1.0) pesoBascula = 0.0; // Saturar a 0.0 el peso mostrado y utilizado (pesoBascula)
    tarado = true;
};



/*-----------------------------------------------------------------------------*/
/**
 * @brief Función para comprobar si ha habido algún evento en la báscula.
 */
/*-----------------------------------------------------------------------------*/
void checkBascula(){
    static float newWeight = 0.0;
    static float lastWeight = 0.0;
    static float diffWeight = 0.0; 
    
    if (pesado){

        pesoARetirar = pesoRecipiente + pesoPlato;

        lastWeight = newWeight;
        newWeight = weight;
        diffWeight = abs(lastWeight - newWeight);


        // ----------------------------------------------------
        // ------- COMPROBAR SI HA HABIDO EVENTO --------------
        // ----------------------------------------------------

        if (diffWeight > 2.0){ // Cambio no causado por "interferencias" de la báscula con diferencia de 2 gr
            scaleEventOccurred = true;
            
            // 'pesoBascula' representa el peso evitando pequeños saltos en las medidas.
            //  Este valor es el que se usará como peso de los alimentos. 
               
            pesoBascula = newWeight;
            if(pesoBascula < 1.0) pesoBascula = 0.0; // Saturar a 0.0 el peso mostrado y utilizado (pesoBascula)


            // ----------------------------------------------------
            // --------- RECONOCER EVENTO OCURRIDO ----------------
            // ----------------------------------------------------

            if(lastWeight < newWeight){ // Incremento de peso                 
                if(tarado){
                    Serial.print(F("\nTARADO"));
                    eventoBascula = TARAR;
                }
                else{
                    Serial.print(F("\nINCREMENTO"));
                    eventoBascula = INCREMENTO;
                }

            }
            else { // Decremento de peso 
                if(tarado){
                    Serial.print(F("\nTARADO"));
                    eventoBascula = TARAR;  
                }
                else{
                    if(abs(abs(newWeight) - pesoARetirar) < 5.0){ //Nuevo peso (negativo) es contrario (-X = +X) al peso del plato + recipiente ==> se ha quitado todo
                        // Se ha puesto un umbral de 5 gr para saber si se ha retirado todo, pero podría reducirse a 1 gr
                        Serial.print(F("\nLIBERADA"));
                        eventoBascula = LIBERAR;
                        flagRecipienteRetirado = true; // Se ha retirado el plato completo --> pantalla recipienteRetirado()
                    }
                    else{ // Se están retirando elementos de la báscula pero aún no se ha liberado
                        Serial.print(F("\nDECREMENTO"));
                        eventoBascula = DECREMENTO;
                    }
                }

            }


            Serial.println(F("\n--------------------------------------"));
            Serial.print(F("\nPeso anterior: ")); Serial.println(lastWeight); 
            Serial.print(F("Peso actual: ")); Serial.println(newWeight); 
            Serial.print(F("Peso Bascula: ")); Serial.println(pesoBascula);
            Serial.print(F("Peso a retirar: ")); Serial.println(pesoARetirar);
            Serial.print(F("Peso recipiente: ")); Serial.println(pesoRecipiente);
            Serial.print(F("Peso plato: ")); Serial.println(pesoPlato);
            Serial.println(F("\n--------------------------------------"));

            addEventToBuffer(eventoBascula);
            flagEvent = true;
        }
        else scaleEventOccurred = false; // No ha habido evento

        pesado = false;
    }


}

#endif
