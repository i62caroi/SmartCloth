/**
 * @file Plato.h
 * @brief Definición de la clase Plato que representa una plato compuesto por varios alimentos.
 *
 * @author Irene Casares Rodríguez
 * @date 15/05/23
 * @version 1.0
 *
 * Esta clase permite agregar y eliminar alimentos del plato, calcular el peso total del plato
 * y actualizar los valores nutricionales totales del mismo.
 *
 * @note Este archivo asume la existencia del siguiente archivo de encabezado:
 *       - "Alimento.h" para la definición de la clase Alimento
 * 
 * @see Alimento.h
 * 
 */

#ifndef PLATO_H
#define PLATO_H

#include "Alimento.h"

#define NUM_ALIMENTOS 10



/**
 * @brief Clase que representa un plato compuesto por varios alimentos.
 * @note Esta clase permite gestionar los alimentos de un plato y calcular sus valores nutricionales.
 * @see Alimento, ValoresNutricionales
 */
class Plato{
  private:
    int _nAlimentos;                      /**< Número de alimentos en el plato */
    float _peso;                          /**< Peso total del plato */
    Alimento _alimentos[NUM_ALIMENTOS];   /**< Array de alimentos del plato */
    ValoresNutricionales _valoresPlato;   /**< Valores nutricionales del plato */

    inline void _setNumAlimentos(int num){ _nAlimentos = num; };
    inline int _getNumAlimentos(){ return _nAlimentos; };
    inline int _positionLastAlimento(){ return (this->_getNumAlimentos()-1); };
    inline int _firstGapPlato(){ return this->_getNumAlimentos(); };           

  public:
    /**
     * @brief Constructor por defecto de la clase Plato.
     *        Inicializa el número de alimentos y el peso del plato a 0.
     */
    Plato();

    /**
     * @brief Comprueba si el plato está vacío (sin alimentos).
     * 
     * @return true si el plato está vacío, false en caso contrario
     */
    inline bool isPlatoEmpty(){ if(this->_getNumAlimentos() == 0) return true; else return false; };
    
    /**
     * @brief Establece el peso total del plato.
     * 
     * @param peso Peso del plato
     */
    inline void setPesoPlato(float peso){ _peso = peso; };

    /**
     * @brief Obtiene el peso total del plato.
     * 
     * @return Peso del plato
     */
    inline float getPesoPlato(){ return _peso; };
    
    /**
     * @brief Añade un alimento al plato.
     * 
     * @param alimento Alimento a añadir
     */
    void addAlimentoPlato(Alimento alimento);         
    
    /**
     * @brief Obtiene los valores nutricionales del plato.
     * 
     * @return Los valores nutricionales del plato
     */
    inline ValoresNutricionales getValoresPlato(){ return _valoresPlato; };

    /**
     * @brief Actualiza los valores nutricionales del plato según los valores del alimento especificado.
     * 
     * @param val Valores nutricionales del alimento
     */
    void updateValoresPlato(ValoresNutricionales val); 

    /**
     * @brief "Reinicia" el plato, eliminando todos los alimentos y reiniciando los valores nutricionales.
     */
    void restorePlato();
};




Plato::Plato(){
    this->_setNumAlimentos(0);
    this->setPesoPlato(0.0);
}


void Plato::addAlimentoPlato(Alimento alimento){
    _alimentos[this->_firstGapPlato()] = alimento;                                // Añadir alimento
    this->_setNumAlimentos(this->_getNumAlimentos() + 1);                          // Incrementar num alimentos
    this->setPesoPlato(this->getPesoPlato() + alimento.getPesoAlimento());       // Incrementar peso del plato
    this->updateValoresPlato(alimento.getValoresAlimento());                     // Actualizar Valores Nutricionales
}


void Plato::updateValoresPlato(ValoresNutricionales val){
    float carb = _valoresPlato.getCarbValores() + val.getCarbValores();
    float lip = _valoresPlato.getLipValores() + val.getLipValores();
    float prot = _valoresPlato.getProtValores() + val.getProtValores();
    float kcal = _valoresPlato.getKcalValores() + val.getKcalValores();
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    _valoresPlato.setValores(valAux);
}


// "Reiniciar" plato
void Plato::restorePlato(){
    this->_setNumAlimentos(0);                             // Nº alimentos = 0
    this->setPesoPlato(0.0);                              // Peso = 0.0
    ValoresNutricionales valAux(0.0, 0.0, 0.0, 0.0);
    _valoresPlato.setValores(valAux);                     // Valores = 0
}



#endif
