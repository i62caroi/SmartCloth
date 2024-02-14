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




// **************************************************************************************************************************
// *****************      DECLARACIÓN CLASE 'DIARIO'       ******************************************************************
// **************************************************************************************************************************


/**
 * @brief Clase que representa un diario de comidas.
 */
class Diario{

  private:
  
    byte                  _nComidas;        /**< Número de comidas en el diario. */
    float                 _peso;            /**< Peso total del diario. */
    ValoresNutricionales  _valoresDiario;   /**< Valores nutricionales totales del diario. */


  public:


    // ----------------------------------------------------------------------
    // -------      CONSTRUCTOR 'DIARIO'       ------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Constructor de la clase Diario.
     */
    Diario();



    // ----------------------------------------------------------------------
    // -------       Nº COMIDAS        --------------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Establece el peso total del diario.
     * 
     * @param num Valor de número de comidas a establecer
     */
    inline void setNumComidas(byte num){ _nComidas = num; };

    /**
     * @brief Obtiene el número de platos del diario.
     * 
     * @return El número de comidas del diario
     */
    inline byte getNumComidas(){ return _nComidas; };



    // ----------------------------------------------------------------------
    // -------       PESO DIARIO        -------------------------------------
    // ----------------------------------------------------------------------

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


    // ----------------------------------------------------------------------
    // -------    AÑADIR COMIDA AL DIARIO     -------------------------------
    // ----------------------------------------------------------------------
    
    /**
     * @brief Agrega una Comida al diario.
     * 
     * @param comida Objeto Comida a agregar
     */
    void addComida(Comida comida);      


    // ----------------------------------------------------------------------
    // -------      VALORES NUTRICIONALES       -----------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Establece los valores nutricionales del diario a partir de un objeto ValoresNutricionales.
     * 
     * @param val Objeto ValoresNutricionales a partir del cual se establecen los valores
     */
    void setValoresDiario(ValoresNutricionales val){ _valoresDiario.setValores(val); };
    
    /**
     * @brief Obtiene los valores nutricionales del diario.
     * 
     * @return Los valores nutricionales del diario
     */
    inline ValoresNutricionales getValoresDiario(){ return _valoresDiario; };

    /**
     * @brief Actualiza los valores nutricionales del diario.
     * 
     * @param val Valores nutricionales con los que actualizar el diario
     */
    void updateValoresDiario(ValoresNutricionales val);

};






// **************************************************************************************************************************
// *****************      DEFINICIONES       ********************************************************************************
// **************************************************************************************************************************


// --------------------------------------------------------------------------------------------------------------------------
// *****************      CONSTRUCTOR 'DIARIO'       ************************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   Diario(): Constructor de la clase Diario que inicializa el número de comidas a 0 y el peso total del
             diario a 0.0
----------------------------------------------------------------------------------------------------------*/
Diario::Diario(){
  setNumComidas(0);
  setPesoDiario(0.0);
}



// --------------------------------------------------------------------------------------------------------------------------
// *****************      AÑADIR COMIDA AL DIARIO    ************************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   addComida(): Añade la información nutricional de una comida al acumulado diario. Incrementa el número 
                de comidas, el peso del acumulado y actualiza los valores nutricionales con los de la comida.

          Parámetros: 
                  comida - Objeto Comida con la información nutricional y peso de la comida guardada.
----------------------------------------------------------------------------------------------------------*/
void Diario::addComida(Comida comida){
  setNumComidas(getNumComidas()+1);                           // Incrementar num comidas
  setPesoDiario(getPesoDiario() + comida.getPesoComida());    // Incrementar peso
  updateValoresDiario(comida.getValoresComida());                   // Actualizar Valores Nutricionales
}




// --------------------------------------------------------------------------------------------------------------------------
// *****************      VALORES NUTRICIONALES      ************************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   updateValoresDiario(): Actualiza los valores nutricionales del acumulado diario sumando los que ya tuviera 
                          con los pasados como argumento, correspondientes a la comida guardada. 

          Parámetros: 
                  val - Valores nutricionales de la comida guardada
----------------------------------------------------------------------------------------------------------*/
void Diario::updateValoresDiario(ValoresNutricionales val){
  float carb = _valoresDiario.getCarbValores() + val.getCarbValores();
  float lip = _valoresDiario.getLipValores() + val.getLipValores();
  float prot = _valoresDiario.getProtValores() + val.getProtValores();
  float kcal = _valoresDiario.getKcalValores() + val.getKcalValores();
  ValoresNutricionales valAux(carb, lip, prot, kcal);
  setValoresDiario(valAux);
}






// **************************************************************************************************************************
// *****************      OBJETOS 'DIARIO'      *****************************************************************************
// **************************************************************************************************************************


Diario    diaActual; // Objeto del acumulado de hoy





#endif
