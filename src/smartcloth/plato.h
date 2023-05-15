#ifndef PLATO_H
#define PLATO_H

#include "Alimento.h"

#define NUM_ALIMENTOS 10



class Plato{
  private:
    int _nAlimentos;
    float _peso;
    Alimento _alimentos[NUM_ALIMENTOS];
    ValoresNutricionales _valoresPlato;

    inline void setNumAlimentos(int num){ _nAlimentos = num; };
    inline int getNumAlimentos(){ return _nAlimentos; };
    inline int positionLastAlimento(){ return (this->getNumAlimentos()-1); };
    inline int firstGapPlato(){ return this->getNumAlimentos(); };           

  public:
    Plato();

    inline bool isPlatoEmpty(){ if(getNumAlimentos() == 0) return true; else return false; };
    
    inline void setPesoPlato(float peso){ _peso = peso; };
    inline float getPesoPlato(){ return _peso; };
    
    void addAlimentoPlato(Alimento alimento);         
    
    inline ValoresNutricionales getValoresPlato(){ return _valoresPlato; };
    void updateValoresPlato(ValoresNutricionales val); 

    void restorePlato();


};



Plato::Plato(){
    this->setNumAlimentos(0);
    this->setPesoPlato(0.0);
}


void Plato::addAlimentoPlato(Alimento alimento){
    _alimentos[this->firstGapPlato()] = alimento;                                // Añadir alimento
    this->setNumAlimentos(this->getNumAlimentos() + 1);                          // Incrementar num alimentos
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
    this->setNumAlimentos(0);                             // Nº alimentos = 0
    this->setPesoPlato(0.0);                              // Peso = 0.0
    ValoresNutricionales valAux(0.0, 0.0, 0.0, 0.0);
    _valoresPlato.setValores(valAux);                     // Valores = 0
}



#endif
