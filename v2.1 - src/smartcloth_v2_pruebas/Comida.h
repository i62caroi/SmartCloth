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


/**
 * @brief Clase que representa una comida compuesta por varios platos.
 */
class Comida{
  private:
    int                   _nPlatos;                 /**< Número de platos en la comida. */
    float                 _peso;                    /**< Peso total de la comida. */
    ValoresNutricionales  _valoresComida;           /**< Valores nutricionales totales de la comida. */

    /**
     * @brief Establece el número de platos.
     * 
     * @param num Número de platos a establecer
     */
    inline void _setNumPlatos(int num){ _nPlatos = num; };
    
    /**
     * @brief Obtiene el número de platos de la comida.
     * 
     * @return El número de platos de la comida
     */
    inline int _getNumPlatos(){ return _nPlatos; };
  
  




  public:
    /**
     * @brief Constructor de la clase Comida.
     */
    Comida();

    /**
     * @brief Verifica si la comida está vacía.
     * 
     * @return True si la comida está vacía, False en caso contrario.
     */
    inline bool isComidaEmpty(){ if(this->_getNumPlatos() == 0) return true; else return false;}; 

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

    /**
     * @brief Agrega un Alimento a la comida.
     * 
     * @param alimento Objeto Alimento a agregar
     */
    void addAlimentoComida(Alimento &alimento); 

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

    //void deletePlato();

    /**
     * @brief Establece los valores nutricionales de la comida a partir de un objeto ValoresNutricionales.
     * 
     * @param val Objeto ValoresNutricionales a partir del cual se establecen los valores
     */
    void setValoresComida(ValoresNutricionales val){ this->_valoresComida.setValores(val); };
    
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

    /**
     * @brief Copiar los valores de un objeto 'Comida' (valores nutricionales, platos y peso) en el objeto Comida tratado.
     * @param comida Objeto Comida a copiar
     */
    void copyComida(Comida comida); 

    /**
     * @brief "Reinicia" la comida, eliminando todos los platos y reiniciando los valores nutricionales.
     */
    void restoreComida(); 

    /**
     * @brief Obtiene todos los valores de la comida en formato de cadena de texto
     * 
     * @return Cadena de texto con los valores nutricionales de la comida
     */
    String getComidaAllValues(); 
};



Comida::Comida(){
  this->_setNumPlatos(0);
  this->setPesoComida(0.0);
}

// --------------------------------------------------------------------

void Comida::addAlimentoComida(Alimento &alimento){   
    this->setPesoComida(this->getPesoComida() + alimento.getPesoAlimento());       // Incrementar peso de la comida
    this->updateValoresComida(true, alimento.getValoresAlimento());                // Sumar (suma = true) Valores Nutricionales de la comida
}

// --------------------------------------------------------------------

void Comida::addPlato(Plato plato){
//void Comida::addPlato(Plato &plato){
  // En realidad no hace falta guardar el objeto plato porque sus elementos
  // (valores nutricionales y peso de los alimentos) se han ido guardando uno a
  // uno. Solo haría falta aumentar el número de platos para indicar que se ha "guardado"
  // Posteriormente, al borrarlo, se restan sus valores nutricionales y peso
    this->_setNumPlatos(this->_getNumPlatos() + 1);                       // Incrementar num platos
}
// --------------------------------------------------------------------


// --------------------- DELETE SOLO ACTUAL -----------------------------------
void Comida::deletePlato(Plato &plato){
       // No hace falta decrementar nPlatos porque no se ha llegado a guardar 
       // el plato ni, por tanto, incrementar el numero platos. 
    this->setPesoComida(this->getPesoComida() - plato.getPesoPlato());          // Decrementar peso de la comida según peso del plato
    this->updateValoresComida(false, plato.getValoresPlato());                  // Restar (suma = false) Valores Nutricionales de la comida
}
// ----------------------------------------------------------------------------


// --------------------- DELETE RETROACTIVO -----------------------------------
/*void Comida::deletePlato(){
    // Se decrementa peso de comida según peso completo del plato. 
    this->setPesoComida(this->getPesoComida() - _platos[this->_getLastPositionPlato()].getPesoPlato());        // Decrementar peso de la comida según peso del último plato (actual)

    this->updateValoresComida(false, _platos[this->_getLastPositionPlato()].getValoresPlato());                // Restar (suma = false) Valores Nutricionales de la comida

    // Hay que decrementar nPlatos aunque se borre el plato actual porque se ha guardado temporalmente
    this->_setNumPlatos(this->_getNumPlatos() - 1);                                                             // Decrementar número de platos
}*/
// ----------------------------------------------------------------------------


// --------------------------------------------------------------------

void Comida::updateValoresComida(bool suma, ValoresNutricionales val){
    float carb, lip, prot, kcal;
    if(suma){        // Añadir plato
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
    //this->_valoresComida.setValores(valAux);
    this->setValoresComida(valAux);
}

// --------------------------------------------------------------------

// Copiar comida
void Comida::copyComida(Comida comida){
    this->_setNumPlatos(comida._getNumPlatos());                  // Nº platos 
    this->setPesoComida(comida.getPesoComida());                  // Peso
    //this->_valoresComida.setValores(comida.getValoresComida());   // Valores 
    this->setValoresComida(comida.getValoresComida());
}
// --------------------------------------------------------------------


// "Reiniciar" comida
void Comida::restoreComida(){
    this->_setNumPlatos(0);                                  // Nº platos = 0
    this->setPesoComida(0.0);                                // Peso = 0.0
    ValoresNutricionales valAux; //(0.0, 0.0, 0.0, 0.0)
    //this->_valoresComida.setValores(valAux);                 // Valores = 0.0
    this->setValoresComida(valAux);
}

// --------------------------------------------------------------------


String Comida::getComidaAllValues(){
    String dataString = String(_valoresComida.getCarbValores()) + ";" + String(_valoresComida.getCarbRaciones()) + ";" + 
                        String(_valoresComida.getLipValores()) + ";" + String(_valoresComida.getLipRaciones()) + ";" + 
                        String(_valoresComida.getProtValores()) + ";" + String(_valoresComida.getProtRaciones()) + ";" + 
                        String(_valoresComida.getKcalValores()); 

    return dataString;
}



Comida    comidaActual;       // Objeto de comida actual real, actualizada en tiempo real
Comida    comidaActualCopia;  // Objeto de comida copiada de la actual real, solamente para mostrar sus valores tras guardarla en el acumulado y limpiar el objeto 'comidaActual'.

#endif
