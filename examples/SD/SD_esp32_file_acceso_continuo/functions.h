/**
 * @file functions.h
 * @brief Este archivo contiene funciones auxiliares para generar peso y obtener el par grupo-peso.
 * 
 * @author Irene Casares Rodríguez
 * @date 01/12/2023
 */

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "Buttons.h"


/*-----------------------------------------------------------------------------*/
/**
 * @brief Genera un peso aleatorio entre 10 y 120 con un decimal de precisión.
 * @return Peso aleatorio generado.
 */
/*-----------------------------------------------------------------------------*/
float generarPeso()
{
    float peso = random(10, 120) + 0.5;
    return peso;
}


/*-----------------------------------------------------------------------------*/
/**
 * @brief Obtiene una cadena con el formato "ALIMENTO,<grupoAlimento>,<pesoAlimento>".
 * @return Cadena con el par grupo-peso.
 */
/*-----------------------------------------------------------------------------*/
String getParGRupoPeso()
{
    String parGrupoPeso = "ALIMENTO," + String(grupoAlimento) + "," + String(pesoAlimento);
    return parGrupoPeso;
}





#endif