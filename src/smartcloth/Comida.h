#ifndef COMIDA_H
#define COMIDA_H

#include "Plato.h"

#define NUM_PLATOS 5

class Comida{
  private:
    int _nPlatos;
    Plato _platos[NUM_PLATOS];
    ValoresNutricionales _valoresComida;

    inline void setNumPlatos(int num){ _nPlatos = num; };
    inline int getNumPlatos(){ return _nPlatos; };
    inline int positionLastPlato(){ return (this->getNumPlatos()-1); };
    inline int firstGapComida(){ return this->getNumPlatos(); };

  public:
    Comida();
    
    void addPlato(Plato plato);     
    void updateActualPlato(Ingrediente ing);
    void deleteActualPlato();
    
    void updateValoresComida(ValoresNutricionales val);
    inline ValoresNutricionales getValoresComida(){ return _valoresComida; };
};



Comida::Comida(){
  this->setNumPlatos(0);
}


void Comida::addPlato(Plato plato){
    _platos[this->firstGapComida()] = plato;               //Añadir plato
    this->setNumPlatos(this->getNumPlatos() + 1);          //Incrementar num platos
    this->updateValoresComida(plato.getValoresPlato());    //Actualizar Valores Nutricionales de la comida
}


void Comida::updateActualPlato(Ingrediente ing){
    int pos = this->positionLastPlato();
    _platos[pos].addIngrediente(ing);                       //Actualizamos plato actual con un nuevo ingrediente
    this->updateValoresComida(ing.getValoresIng());         //Actualizar Valores Nutricionales de la comida
}


void Comida::deleteActualPlato(){
    this->setNumPlatos(this->getNumPlatos() - 1);           //El proximo plato sobreescribirá el 'eliminado'
}


void Comida::updateValoresComida(ValoresNutricionales val){
    float carb = _valoresComida.getCarbValores() + val.getCarbValores();
    float lip = _valoresComida.getLipValores() + val.getLipValores();
    float prot = _valoresComida.getProtValores() + val.getProtValores();
    float kcal = _valoresComida.getKcalValores() + val.getKcalValores();
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    _valoresComida.setValores(valAux);
}




#endif
