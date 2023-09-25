/**
 * @file Diario.h
 * @brief Definición de la clase Diario que representa un diario de comidas.
 *
 * @author Irene Casares Rodríguez
 * @date 31/05/23
 * @version 1.0
 *
 * Esta clase permite agregar una comida al diario, actualizar los valores nutricionales totales del diario,
 * establecer y obtener el peso total del diario, y establecer y obtener los valores nutricionales del diario.
 *
 * @note Este archivo asume la existencia de los siguientes archivos de encabezado:
 *       - "Valores_Nutricionales.h" para la definición de la clase ValoresNutricionales
 *       - "Comida.h" para la definición de la clase Comida
 * 
 * @see ValoresNutricionales.h
 * @see Comida.h
 *
 */

#ifndef DIARIO_H
#define DIARIO_H

#include "Comida.h"

#define NUM_COMIDAS 10

/**
 * @brief Clase que representa un diario de comidas.
 */
class Diario{
  private:
    int                   _nComidas;        /**< Número de comidas en el diario. */
    float                 _peso;            /**< Peso total del diario. */
    //Comida              _comidas[NUM_COMIDAS];
    ValoresNutricionales  _valoresDiario;   /**< Valores nutricionales totales del diario. */

    /**
     * @brief Obtiene la primera posición disponible en el array de comidas.
     * 
     * @return La primera posición disponible en el array de comidas
     */
    inline int _firstGapDiario(){ return this->getNumComidas(); };

  public:
    /**
     * @brief Constructor de la clase Diario.
     */
    Diario();

    /**
     * @brief Establece el peso total del diario.
     * 
     * @param num Valor de número de comidas a establecer
     */
    inline void setNumComidas(int num){ _nComidas = num; };

    /**
     * @brief Obtiene el número de platos del diario.
     * 
     * @return El número de comidas del diario
     */
    inline int getNumComidas(){ return _nComidas; };

    /**
     * @brief Establece el peso total del diario.
     * 
     * @param peso Valor de peso a establecer
     */
    inline void setPesoDiario(float peso){ _peso = peso; };

    /**
     * @brief Obtiene el peso total del diario.
     * 
     * @return El peso total del diario
     */
    inline float getPesoDiario(){ return _peso; };
    
    /**
     * @brief Agrega una Comida al diario.
     * 
     * @param comida Objeto Comida a agregar
     */
    void addComida(Comida comida);      
    
    /**
     * @brief Actualiza los valores nutricionales del diario.
     * 
     * @param val Valores nutricionales con los que actualizar el diario
     */
    void updateValoresDiario(ValoresNutricionales val);

    /**
     * @brief Establece los valores nutricionales del diario a partir de un objeto ValoresNutricionales.
     * 
     * @param val Objeto ValoresNutricionales a partir del cual se establecen los valores
     */
    void setValoresDiario(ValoresNutricionales val){ this->_valoresDiario.setValores(val); };
    
    /**
     * @brief Obtiene los valores nutricionales del diario.
     * 
     * @return Los valores nutricionales del diario
     */
    inline ValoresNutricionales getValoresDiario(){ return _valoresDiario; };
};


Diario::Diario(){
  this->setNumComidas(0);
  this->setPesoDiario(0.0);
}

// --------------------------------------------------------------------
void Diario::addComida(Comida comida){
  //_comidas[this->_firstGapDiario()] = comida;                            // Añadir comida
  this->setNumComidas(this->getNumComidas()+1);                         // Incrementar num comidas
  this->setPesoDiario(this->getPesoDiario() + comida.getPesoComida());  // Incrementar peso
  this->updateValoresDiario(comida.getValoresComida());                       // Actualizar Valores Nutricionales
}



// --------------------------------------------------------------------

void Diario::updateValoresDiario(ValoresNutricionales val){
  float carb = _valoresDiario.getCarbValores() + val.getCarbValores();
  float lip = _valoresDiario.getLipValores() + val.getLipValores();
  float prot = _valoresDiario.getProtValores() + val.getProtValores();
  float kcal = _valoresDiario.getKcalValores() + val.getKcalValores();
  ValoresNutricionales valAux(carb, lip, prot, kcal);
  _valoresDiario.setValores(valAux);
}

Diario    diaActual; // Acumulado Hoy

#endif
