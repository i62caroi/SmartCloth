#ifndef PLATO_H
#define PLATO_H

#include "Ingrediente.h"

#define NUM_ING 10



class Plato{
  private:
    int _nIng;
    float _peso;
    Ingrediente _ingredientes[NUM_ING];
    ValoresNutricionales _valoresPlato;

    inline void setNumIng(int num){ _nIng = num; };
    inline int getNumIng(){ return _nIng; };
    inline int positionLastIng(){ return (this->getNumIng()-1); };
    inline int firstGapPlato(){ return this->getNumIng(); };           

  public:
    Plato();
    
    inline void setPesoPlato(float peso){ _peso = peso; };
    inline float getPesoPlato(){ return _peso; };
    
    void addIngPlato(Ingrediente ing);         
    
    inline ValoresNutricionales getValoresPlato(){ return _valoresPlato; };
    void updateValoresPlato(ValoresNutricionales val); 

    void restorePlato();


};



Plato::Plato(){
    this->setNumIng(0);
    this->setPesoPlato(0.0);
}


void Plato::addIngPlato(Ingrediente ing){
    _ingredientes[this->firstGapPlato()] = ing;                                   // Añadir ingrediente
    this->setNumIng(this->getNumIng() + 1);                                 // Incrementar num ingredientes
    this->setPesoPlato(this->getPesoPlato() + ing.getPesoIng());            // Incrementar peso del plato
    this->updateValoresPlato(ing.getValoresIng());                          // Actualizar Valores Nutricionales
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
    this->setNumIng(0);                                   // Nº ing = 0
    this->setPesoPlato(0.0);                              // Peso = 0.0
    ValoresNutricionales valAux(0.0, 0.0, 0.0, 0.0);
    _valoresPlato.setValores(valAux);                     // Valores = 0
}



#endif
