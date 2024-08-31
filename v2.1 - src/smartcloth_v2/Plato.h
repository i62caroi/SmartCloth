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



// **************************************************************************************************************************
// *****************      DECLARACIÓN CLASE 'PLATO'       *******************************************************************
// **************************************************************************************************************************


/**
 * @brief Clase que representa un plato compuesto por varios alimentos.
 * @note Esta clase permite gestionar los alimentos de un plato y calcular sus valores nutricionales.
 * @see Alimento, ValoresNutricionales
 */
class Plato{

  private:
  
    byte                    _nAlimentos;     /**< Número de alimentos en el plato */
    float                   _peso;           /**< Peso total del plato */
    ValoresNutricionales    _valoresPlato;   /**< Valores nutricionales del plato */


    // ----------------------------------------------------------------------
    // -------       Nº ALIMENTOS        ------------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Establece el número de alimentos.
     * 
     * @param num Número de alimentos a establecer
     */
    inline void _setNumAlimentos(byte num){ _nAlimentos = num; };

    /**
     * @brief Obtiene el número de alimentos del plato.
     * 
     * @return El número de alimentos del plato
     */
    inline byte _getNumAlimentos(){ return _nAlimentos; };



  public:

    // ----------------------------------------------------------------------
    // -------      CONSTRUCTOR 'PLATO'       -------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Constructor por defecto de la clase Plato.
     *        Inicializa el número de alimentos y el peso del plato a 0.
     */
    Plato();


    // ----------------------------------------------------------------------
    // -------      ¿PLATO VACÍO?         -----------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Comprueba si el plato está vacío (sin alimentos).
     * 
     * @return true si el plato está vacío, false en caso contrario
     */
    inline bool isPlatoEmpty(){ return _getNumAlimentos() == 0; };
    


    // ----------------------------------------------------------------------
    // -------       PESO PLATO        --------------------------------------
    // ----------------------------------------------------------------------

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
    


    // ----------------------------------------------------------------------
    // -------    AÑADIR ALIMENTO AL PLATO     ------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Añade un alimento al plato.
     * 
     * @param alimento Alimento a añadir
     */
    void addAlimentoPlato(Alimento alimento);         



    // ----------------------------------------------------------------------
    // -------      VALORES NUTRICIONALES       -----------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Establece los valores nutricionales del plato a partir de un objeto ValoresNutricionales.
     * 
     * @param val Objeto ValoresNutricionales a partir del cual se establecen los valores
     */
    inline void setValoresPlato(ValoresNutricionales val){ _valoresPlato.setValores(val); };

    /**
     * @brief Obtiene los valores nutricionales del plato.
     * 
     * @return Los valores nutricionales del plato
     */
    inline ValoresNutricionales getValoresPlato(){ return _valoresPlato; };

    /**
     * @brief Actualiza los valores nutricionales del plato según los valores de la porción de alimento pesado.
     * 
     * @param val Valores nutricionales del alimento
     */
    void updateValoresPlato(ValoresNutricionales val); 




    // ----------------------------------------------------------------------
    // -------       RESTAURAR PLATO        ---------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief "Reinicia" el plato, eliminando todos los alimentos y reiniciando los valores nutricionales.
     */
    void restorePlato();

};






// **************************************************************************************************************************
// *****************      DEFINICIONES       ********************************************************************************
// **************************************************************************************************************************


// --------------------------------------------------------------------------------------------------------------------------
// *****************      CONSTRUCTOR 'PLATO'       *************************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   Plato(): Constructor de la clase Plato que inicializa el número de alimentos a 0 y el peso total del
            plato a 0.0
----------------------------------------------------------------------------------------------------------*/
Plato::Plato(){
    _setNumAlimentos(0);
    setPesoPlato(0.0);
}



// --------------------------------------------------------------------------------------------------------------------------
// *****************      AÑADIR ALIMENTO AL PLATO      *********************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   addAlimentoPlato(): Añade la información nutricional de un Alimento al Plato. Incrementa el número de 
                       alimentos, el peso del plato y actualiza los valores nutricionales con los del
                       alimento.

          Parámetros: 
                  alimento - Objeto Alimento con la información nutricional y peso de la porción del 
                             alimento pesado.
----------------------------------------------------------------------------------------------------------*/
void Plato::addAlimentoPlato(Alimento alimento){
    _setNumAlimentos(_getNumAlimentos() + 1);                        // Incrementar num alimentos
    setPesoPlato(getPesoPlato() + alimento.getPesoAlimento());       // Incrementar peso del plato
    updateValoresPlato(alimento.getValoresAlimento());                     // Actualizar Valores Nutricionales
}




// --------------------------------------------------------------------------------------------------------------------------
// *****************      VALORES NUTRICIONALES      ************************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   updateValoresPlato(): Actualiza los valores nutricionales del plato sumando los que ya tuviera con los
                         pasados como argumento, correspondientes a la porción de alimento pesada. 

          Parámetros: 
                  val - Valores nutricionales del alimento añadido al plato
----------------------------------------------------------------------------------------------------------*/
void Plato::updateValoresPlato(ValoresNutricionales val){
    float carb = _valoresPlato.getCarbValores() + val.getCarbValores();  
    float lip = _valoresPlato.getLipValores() + val.getLipValores();
    float prot = _valoresPlato.getProtValores() + val.getProtValores();
    float kcal = _valoresPlato.getKcalValores() + val.getKcalValores();
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    setValoresPlato(valAux);
}




// --------------------------------------------------------------------------------------------------------------------------
// *****************      RESTAURAR PLATO      ******************************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   restorePlato(): Restaura o "reinicia" el plato, reseteando el número de alimentos, el peso y los
                   valores nutricionales del plato.
----------------------------------------------------------------------------------------------------------*/
void Plato::restorePlato(){
    _setNumAlimentos(0);                          // Nº alimentos = 0
    setPesoPlato(0.0);                            // Peso = 0.0
    ValoresNutricionales valAux(0.0, 0.0, 0.0, 0.0);    // Crea un objeto ValoresNutricionales con todos sus valores a 0.0
    setValoresPlato(valAux);                      // Valores = 0
}






// **************************************************************************************************************************
// *****************      OBJETOS 'PLATO'      ******************************************************************************
// **************************************************************************************************************************


Plato     platoActual;  // Objeto del plato actual en elaboración




#endif
