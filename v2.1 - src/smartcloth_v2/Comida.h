/**
 * @file Comida.h
 * @brief Definición de la clase Comida que representa una comida compuesta por varios platos.
 *
 * @author Irene Casares Rodríguez
 * @date 31/05/23
 * @version 1.0
 *
 * Esta clase permite agregar y eliminar platos de la comida, calcular el peso total de la comida
 * y actualizar los valores nutricionales totales de la misma.
 *
 * @note Este archivo asume la existencia de los siguientes archivos de encabezado:
 *       - "Valores_Nutricionales.h" para la definición de la clase ValoresNutricionales
 *       - "Plato.h" para la definición de la clase Plato
 * 
 * @see ValoresNutricionales.h
 * @see Plato.h
 * 
 */

#ifndef COMIDA_H
#define COMIDA_H

#include "Plato.h"



// **************************************************************************************************************************
// *****************      DECLARACIÓN CLASE 'COMIDA'       ******************************************************************
// **************************************************************************************************************************


/**
 * @brief Clase que representa una comida compuesta por varios platos.
 */
class Comida{

  private:
  
    byte                  _nPlatos;                 /**< Número de platos en la comida (mínimo 1). */
    float                 _peso;                    /**< Peso total de la comida (todos los alimentos, sin incluir recipiente). */
    ValoresNutricionales  _valoresComida;           /**< Valores nutricionales totales de la comida. */

    
    // ----------------------------------------------------------------------
    // -------       Nº PLATOS        ---------------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Establece el número de platos.
     * 
     * @param num Número de platos a establecer
     */
    inline void _setNumPlatos(byte num){ _nPlatos = num; };
    
    /**
     * @brief Obtiene el número de platos de la comida.
     * 
     * @return El número de platos de la comida
     */
    inline byte _getNumPlatos(){ return _nPlatos; };
  
  

  public:


    // ----------------------------------------------------------------------
    // -------      CONSTRUCTOR 'COMIDA'       ------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Constructor de la clase Comida.
     */
    Comida();


    // ----------------------------------------------------------------------
    // -------      ¿COMIDA VACÍA?         ----------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Verifica si la comida está vacía.
     * 
     * @return True si la comida está vacía, False en caso contrario.
     */
    //inline bool isComidaEmpty(){ return _getNumPlatos() == 0; }; 
    // Creo que la mejor forma de comprobar si la comida está vacía es comprobando si el peso es 0.0
    // ya que la comida como mínimo tendrá 1 plato (he modificado para que el número de platos al inicio sea 1 por defecto)
    inline bool isComidaEmpty(){ return getPesoComida() == 0.0; };



    // ----------------------------------------------------------------------
    // -------       PESO COMIDA        -------------------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Establece el peso total de la comida.
     * 
     * @param peso Valor de peso a establecer
     */
    inline void setPesoComida(float peso){ _peso = peso; }; 
    
    /**
     * @brief Obtiene el peso total de la comida.
     * 
     * @return El peso total de la comida
     */
    inline float getPesoComida(){ return _peso; }; 



    // ----------------------------------------------------------------------
    // -------    AÑADIR ALIMENTO A LA COMIDA     ---------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Agrega un Alimento a la comida.
     * 
     * @param alimento Objeto Alimento a agregar
     */
    void addAlimentoComida(Alimento &alimento); 



    // ----------------------------------------------------------------------
    // -------    AÑADIR/BORRAR PLATO A/DE LA COMIDA     --------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Agrega un Plato a la comida.
     * 
     * @param plato Objeto Plato a agregar
     */
    void addPlato(Plato plato);
    
    /**
     * @brief Elimina un plato de la comida (solo el plato actual).
     * 
     * @param plato Plato a eliminar
     */
    void deletePlato(Plato &plato); 



    // ----------------------------------------------------------------------
    // -------      VALORES NUTRICIONALES       -----------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Establece los valores nutricionales de la comida a partir de un objeto ValoresNutricionales.
     * 
     * @param val Objeto ValoresNutricionales a partir del cual se establecen los valores
     */
    inline void setValoresComida(ValoresNutricionales val){ _valoresComida.setValores(val); };
    
    /**
     * @brief Obtiene los valores nutricionales totales de la comida. 
     * 
     * @return Los valores nutricionales de la comida
     */
    inline ValoresNutricionales getValoresComida(){ return _valoresComida; }; 

    /**
     * @brief Actualiza los valores nutricionales de la comida.
     * 
     * @param suma True si se quieren sumar los valores nutricionales (al agregar alimento), False si se quieren restar (al eliminar plato)
     * @param val Valores nutricionales con los que actualizar la comida
     */
    void updateValoresComida(bool suma, ValoresNutricionales val); 



    // ----------------------------------------------------------------------
    // -------      COPIAR/RESTAURAR COMIDA        --------------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Copiar los valores de un objeto 'Comida' (valores nutricionales, platos y peso) en el objeto Comida tratado.
     * @param comida Objeto Comida a copiar
     */
    void copyComida(Comida comida); 

    /**
     * @brief "Reinicia" la comida, eliminando todos los platos y reiniciando los valores nutricionales.
     */
    void restoreComida(); 




    // ----------------------------------------------------------------------
    // -------      STRING DE VALORES DE LA COMIDA        -------------------
    // ----------------------------------------------------------------------

    /**
     * @brief Obtiene todos los valores de la comida en formato de cadena de texto
     * 
     * @return Cadena de texto con los valores nutricionales de la comida
     */
    String getComidaAllValues(); 


    /**
     * @brief Obtiene todos los valores de la comida en formato de cadena de texto listos para añadirlos a la petición HTTP POST
     * 
     * @return Cadena de texto con los valores nutricionales de la comida
     */
    String getComidaAllValuesHttpRequest(); 

};






// **************************************************************************************************************************
// *****************      DEFINICIONES       ********************************************************************************
// **************************************************************************************************************************


// --------------------------------------------------------------------------------------------------------------------------
// *****************      CONSTRUCTOR 'COMIDA'       ************************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   Comida(): Constructor de la clase Comida que inicializa el número de platos a 0 y el peso total de
             la comida a 0.0
----------------------------------------------------------------------------------------------------------*/
Comida::Comida(){
  _setNumPlatos(1);
  setPesoComida(0.0);
}



// --------------------------------------------------------------------------------------------------------------------------
// *****************      AÑADIR ALIMENTO A LA COMIDA      ******************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   addAlimentoComida(): Añade la información nutricional de un alimento a la comida. Incrementa el peso de 
                        la comida y actualiza los valores nutricionales con los del alimento.

          Parámetros: 
                  alimento - Objeto Alimento con la información nutricional y peso de la porción del 
                             alimento pesado.
----------------------------------------------------------------------------------------------------------*/
void Comida::addAlimentoComida(Alimento &alimento){   
    setPesoComida(getPesoComida() + alimento.getPesoAlimento());       // Incrementar peso de la comida
    updateValoresComida(true, alimento.getValoresAlimento());                // Sumar (suma = true) Valores Nutricionales de la comida
}



// --------------------------------------------------------------------------------------------------------------------------
// *****************      AÑADIR/BORRAR PLATO A/DE LA COMIDA      ***********************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   addPlato(): Añade oficialmente un plato a la comida incrementando el número de platos. Los valores
               nutricionales de los alimentos presentes en el Plato se van incluyendo a la comida conforme
               se añaden al plato.

          Parámetros: 
                  plato - Objeto Plato oficialmente "guardado".
----------------------------------------------------------------------------------------------------------*/
void Comida::addPlato(Plato plato){
//void Comida::addPlato(Plato &plato){
  // En realidad no hace falta guardar el objeto plato porque sus elementos
  // (valores nutricionales y peso de los alimentos) se han ido guardando uno a
  // uno. Solo haría falta aumentar el número de platos para indicar que se ha "guardado"
  // Posteriormente, al borrarlo, se restan sus valores nutricionales y peso
    _setNumPlatos(_getNumPlatos() + 1);                       // Incrementar num platos
}



/*---------------------------------------------------------------------------------------------------------
   deletePlato(): Elimina un plato de la comida disminuyendo su peso según el peso del plato y eliminando
                  los valores nutricionales del plato del total de la comida.
                  No se decrementa el número de platos porque si se ha decidido borrar el plato (actual),
                  entonces no se ha llegado a "guardar" incrementando el número de platos, lo cual se haría 
                  si se hubiera elegido "Añadir plato" en lugar de "Borrar plato".

          Parámetros: 
                  plato - Objeto Plato a borrar
----------------------------------------------------------------------------------------------------------*/
void Comida::deletePlato(Plato &plato){
       // No hace falta decrementar nPlatos porque no se ha llegado a guardar 
       // el plato ni, por tanto, incrementar el numero platos. 
    setPesoComida(getPesoComida() - plato.getPesoPlato());          // Decrementar peso de la comida según peso del plato
    updateValoresComida(false, plato.getValoresPlato());                  // Restar (suma = false) Valores Nutricionales de la comida
}




// --------------------------------------------------------------------------------------------------------------------------
// *****************      VALORES NUTRICIONALES      ************************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   updateValoresComida(): Actualiza los valores nutricionales de la comida sumando o restando a los que ya 
                          tuviera los pasados como argumento, correspondientes a la porción de alimento pesada
                          o al plato a eliminar. 

          Parámetros: 
                  suma - true: sumar valores    false: restar valores
                  val - Valores nutricionales del alimento pesado o del plato a eliminar
----------------------------------------------------------------------------------------------------------*/
void Comida::updateValoresComida(bool suma, ValoresNutricionales val){
    float carb, lip, prot, kcal;
    if(suma){        // Añadir alimento
        carb = _valoresComida.getCarbValores() + val.getCarbValores();
        lip = _valoresComida.getLipValores() + val.getLipValores();
        prot = _valoresComida.getProtValores() + val.getProtValores();
        kcal = _valoresComida.getKcalValores() + val.getKcalValores();
    }
    else{            // Eliminar plato 
        carb = _valoresComida.getCarbValores() - val.getCarbValores();
        lip = _valoresComida.getLipValores() - val.getLipValores();
        prot = _valoresComida.getProtValores() - val.getProtValores();
        kcal = _valoresComida.getKcalValores() - val.getKcalValores();
    }
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    setValoresComida(valAux);
}



// --------------------------------------------------------------------------------------------------------------------------
// *****************      COPIAR/RESTAURAR COMIDA      **********************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   copyComida(): Establece los datos del objeto actual a partir de otro. Es decir, "copia" el número de platos,
                 el peso de la comida y los valores nutricionales.
                 Esto es útil tras guardar la comida, pues en ese momento se borra el objeto Comida que incluía
                 los valores guardados, pero se quiere mostrar la información guardada al usuario para que la
                 pueda analizar.
----------------------------------------------------------------------------------------------------------*/
void Comida::copyComida(Comida comida){
    _setNumPlatos(comida._getNumPlatos());          // Nº platos 
    setPesoComida(comida.getPesoComida());          // Peso
    setValoresComida(comida.getValoresComida());    // Valores 
}



/*---------------------------------------------------------------------------------------------------------
   restoreComida(): Restaura o "reinicia" la comida, reseteando el número de platos, el peso y los
                    valores nutricionales de la comida.
----------------------------------------------------------------------------------------------------------*/
void Comida::restoreComida(){
    _setNumPlatos(0);                 // Nº platos = 0
    setPesoComida(0.0);               // Peso = 0.0
    ValoresNutricionales valAux;            // Crea un objeto ValoresNutricionales con todos sus valores a 0.0
    setValoresComida(valAux);         // Valores = 0.0
}



// --------------------------------------------------------------------------------------------------------------------------
// *****************      STRING DE VALORES DE LA COMIDA      ***************************************************************
// --------------------------------------------------------------------------------------------------------------------------

/*---------------------------------------------------------------------------------------------------------
   getComidaAllValues(): Conforma una cadena con los valores nutricionales de la comida separados por
                         el caracter ';' para poderlos escribir en el fichero .csv al guardarla. También
                         incluye el peso
----------------------------------------------------------------------------------------------------------*/
String Comida::getComidaAllValues(){
    String dataString = String(_valoresComida.getCarbValores()) + ";" + String(_valoresComida.getCarbRaciones()) + ";" + 
                        String(_valoresComida.getLipValores()) + ";" + String(_valoresComida.getLipRaciones()) + ";" + 
                        String(_valoresComida.getProtValores()) + ";" + String(_valoresComida.getProtRaciones()) + ";" + 
                        String(_valoresComida.getKcalValores()); 

    return dataString;
}



/*---------------------------------------------------------------------------------------------------------
   getComidaAllValuesHttpRequest(): Conforma una cadena con los valores nutricionales de la comida separados por
                         el caracter '&' para poderlos insertar en la petición HTTP POST del esp32 directamente.
----------------------------------------------------------------------------------------------------------*/
String Comida::getComidaAllValuesHttpRequest(){
    String dataString = "&carb=" + String(_valoresComida.getCarbValores()) + "&carb_R=" + String(_valoresComida.getCarbRaciones()) + "&lip=" + 
                        String(_valoresComida.getLipValores()) + "&lip_R=" + String(_valoresComida.getLipRaciones()) + "&prot=" + 
                        String(_valoresComida.getProtValores()) + "&prot_R" + String(_valoresComida.getProtRaciones()) + "&kcal" + 
                        String(_valoresComida.getKcalValores()) + "&peso=" + String(getPesoComida()); 

    return dataString;
}







// **************************************************************************************************************************
// *****************      OBJETOS 'COMIDA'      *****************************************************************************
// **************************************************************************************************************************

Comida    comidaActual;       // Objeto de comida actual real, actualizada en tiempo real
Comida    comidaActualCopia;  // Objeto de comida copiada de la actual real, solamente para mostrar sus valores tras guardarla en el acumulado y limpiar el objeto 'comidaActual'.




#endif
