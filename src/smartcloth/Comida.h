#ifndef COMIDA_H
#define COMIDA_H

#include "Plato.h"

#define NUM_PLATOS 5

class Comida{
  private:
    int _nPlatos;
    float _peso;
    Plato _platos[NUM_PLATOS];
    ValoresNutricionales _valoresComida;

    inline void setNumPlatos(int num){ _nPlatos = num; };
    inline int getNumPlatos(){ return _nPlatos; };
    inline int positionLastPlato(){ return (this->getNumPlatos()-1); };
    inline int firstGapComida(){ return this->getNumPlatos(); };

  public:
    Comida();

    inline void setPesoComida(float peso){ _peso = peso; };
    inline float getPesoComida(){ return _peso; };
    
    void addPlato(Plato &plato);     
    //void updateActualPlato(Ingrediente ing);

    void updateValoresComida(ValoresNutricionales val);
    inline ValoresNutricionales getValoresComida(){ return _valoresComida; };
};



Comida::Comida(){
  this->setNumPlatos(0);
  this->setPesoComida(0.0);
}


void Comida::addPlato(Plato &plato){
    _platos[this->firstGapComida()] = plato;               //Añadir plato
    this->setNumPlatos(this->getNumPlatos() + 1);          //Incrementar num platos
    this->setPesoComida(this->getPesoComida() + plato.getPesoPlato());            // Incrementar peso de la comida
    this->updateValoresComida(plato.getValoresPlato());    //Actualizar Valores Nutricionales de la comida
}

/*
void Comida::updateActualPlato(Ingrediente ing){
    int pos = this->positionLastPlato();
    _platos[pos].addIngrediente(ing);                       //Actualizamos plato actual con un nuevo ingrediente
    this->updateValoresComida(ing.getValoresIng());         //Actualizar Valores Nutricionales de la comida
}
*/



void Comida::updateValoresComida(ValoresNutricionales val){
    double carb = _valoresComida.getCarbValores() + val.getCarbValores();
    double lip = _valoresComida.getLipValores() + val.getLipValores();
    double prot = _valoresComida.getProtValores() + val.getProtValores();
    double kcal = _valoresComida.getKcalValores() + val.getKcalValores();
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    _valoresComida.setValores(valAux);
}




#endif
