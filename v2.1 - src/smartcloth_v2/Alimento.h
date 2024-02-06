/**
 * @file Alimento.h
 * @brief Definición de la clase Alimento
 *
 * @author Irene Casares Rodríguez
 * @date 15/05/23
 * @version 1.0
 * 
 * Este archivo contiene la definición de la clase Alimento, que representa un alimento.
 * Incluye los atributos correspondientes al grupo al que pertenece el alimento, su peso
 * y los valores nutricionales asociados.
 * 
 * La clase Alimento permite establecer y obtener el grupo al que pertenece el alimento,
 * así como su peso y valores nutricionales. También proporciona métodos para actualizar
 * los valores nutricionales en base al grupo y peso especificados.
 * 
 * @note Este archivo asume la existencia de los siguientes archivos de encabezado:
 *       - "Valores_Nutricionales.h" para la definición de la clase ValoresNutricionales
 *       - "Grupos.h" para la definición de la clase Grupo
 * 
 * @see ValoresNutricionales.h
 * @see Grupos.h
 * 
 */

#ifndef ALIMENTO_H
#define ALIMENTO_H

#include "Valores_Nutricionales.h"
#include "Grupos.h"




// **************************************************************************************************************************
// *****************      DECLARACIÓN CLASE 'ALIMENTO'       ****************************************************************
// **************************************************************************************************************************

/**
 * @brief Clase que representa un alimento.
 */
class Alimento{

  private:
  
    Grupo                 _grupo;             /**< Grupo al que pertenece el alimento */
    float                 _peso;              /**< Peso del alimento */
    ValoresNutricionales  _valoresAlimento;   /**< Valores nutricionales del alimento */


  public:


    // ----------------------------------------------------------------------
    // -------     CONSTRUCTORES 'ALIMENTO'     -----------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Constructor por defecto de la clase Alimento.
     */
    Alimento();

    /**
     * @brief Constructor de la clase Alimento.
     *        Inicializa el grupo y el peso del alimento según los parámetros pasados como argumentos,
     *        y actualiza automáticamente los valores nutricionales del alimento.
     * 
     * @param grupo Grupo al que pertenece el alimento
     * @param peso Peso del alimento
     */
    Alimento(Grupo grupo, float peso); 



    // ----------------------------------------------------------------------
    // -------    GRUPO DEL ALIMENTO    -------------------------------------
    // ----------------------------------------------------------------------
    
    /**
     * @brief Establece el grupo al que pertenece el alimento.
     * 
     * @param grupo Grupo al que pertenece el alimento
     */
    inline void setGrupoAlimento(Grupo grupo){ _grupo = grupo; };

    /**
     * @brief Obtiene el grupo al que pertenece el alimento.
     * 
     * @return El grupo al que pertenece el alimento
     */
    inline Grupo getGrupoAlimento(){ return _grupo; };
    


    // ----------------------------------------------------------------------
    // -------    PESO DEL ALIMENTO    --------------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Establece el peso del alimento.
     * 
     * @param peso Peso del alimento
     */
    inline void setPesoAlimento(float peso){ _peso = peso; };

    /**
     * @brief Obtiene el peso del alimento.
     * 
     * @return El peso del alimento
     */
    inline float getPesoAlimento(){ return _peso; };



    // ----------------------------------------------------------------------
    // -------   VALORES NUTRICIONALES DEL ALIMENTO    ----------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Actualiza los valores nutricionales del alimento según el grupo y el peso especificados.
     * 
     * @param grupo Grupo al que pertenece el alimento
     * @param peso Peso del alimento
     */
    void updateValoresAlimento(Grupo grupo, float peso);

    /**
     * @brief Establece los valores nutricionales del alimento a partir de un objeto ValoresNutricionales.
     * 
     * @param val Objeto ValoresNutricionales a partir del cual se establecen los valores
     */
    inline void setValoresAlimento(ValoresNutricionales val){ _valoresAlimento.setValores(val); };

    /**
     * @brief Obtiene los valores nutricionales del alimento.
     * 
     * @return Los valores nutricionales del alimento
     */
    inline ValoresNutricionales getValoresAlimento(){ return _valoresAlimento; };

};





// **************************************************************************************************************************
// *****************      DEFINICIONES       ********************************************************************************
// **************************************************************************************************************************


// --------------------------------------------------------------------------------------------------------------------------
// *****************       CONSTRUCTORES 'ALIMENTO'       *******************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   Alimento(): Constructor de la clase Alimento que establece su peso a 0.0
----------------------------------------------------------------------------------------------------------*/
Alimento::Alimento(){
    setPesoAlimento(0.0);
}

/*---------------------------------------------------------------------------------------------------------
   Alimento(): Constructor de la clase Alimento que inicializa sus valores de grupo, peso y valores 
               nutricionales según los valores pasados como argumentos.

       Parámetros:
              grupo - Grupo al que pertenece el alimento
              peso - Peso de la porción de alimento
----------------------------------------------------------------------------------------------------------*/
Alimento::Alimento(Grupo grupo, float peso){
    setGrupoAlimento(grupo);
    setPesoAlimento(peso);
    updateValoresAlimento(grupo, peso);
}




// --------------------------------------------------------------------------------------------------------------------------
// *****************     VALORES NUTRICIONALES DEL ALIMENTO       ***********************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   updateValoresAlimento(): Calcula los valores nutricionales del alimento a partir del grupo al que pertenece,
                            distinguiendo grupos de alimentos crudos o cocinados, y su peso.

                            Los valores de macronutrientes los calcula multiplicando el peso del alimento 
                            por el valor/gr preestablecido para el macronutriente correspondiente.

                            La clase 'ValoresNutricionales' se encarga internamente de calcular las raciones
                            de macronutrientes a partir de sus valores en gramos.

          Parámetros: 
                  grupo - Grupo al que pertenece el alimento
                  peso - Peso de la porción de alimento
----------------------------------------------------------------------------------------------------------*/
void Alimento::updateValoresAlimento(Grupo grupo, float peso){
    float carb = grupo.Carb_g * peso;
    float lip = grupo.Lip_g * peso;
    float prot = grupo.Prot_g * peso;
    float kcal = grupo.Kcal_g * peso;
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    setValoresAlimento(valAux); 
}


#endif
