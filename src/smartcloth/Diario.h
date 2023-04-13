#ifndef DIARIO_H
#define DIARIO_H

#include "Comida.h"

#define NUM_COMIDAS 5

class Diario{
  private:
    int _nComidas;
    float _peso;
    Comida _comidas[NUM_COMIDAS];
    ValoresNutricionales _valoresDiario;

    inline void setNumComidas(int num){ _nComidas = num; };
    inline int getNumComidas(){ return _nComidas; };
    inline int firstGapDiario(){ return this->getNumComidas(); };

  public:
    Diario();

    inline void setPesoDiario(float peso){ _peso = peso; };
    inline float getPesoDiario(){ return _peso; };
    
    void addComida(Comida comida);      
    
    void updateValoresDiario(ValoresNutricionales val);
    inline ValoresNutricionales getValoresDiario(){ return _valoresDiario; };
};


Diario::Diario(){
  this->setNumComidas(0);
  this->setPesoDiario(0.0);
}

// --------------------------------------------------------------------
void Diario::addComida(Comida comida){
  _comidas[this->firstGapDiario()] = comida;                            // Añadir comida
  this->setNumComidas(this->getNumComidas()+1);                         // Incrementar num comidas
  this->setPesoDiario(this->getPesoDiario() + comida.getPesoComida());  // Incrementar peso
  this->updateValoresDiario(comida.getValoresComida());                       // Actualizar Valores Nutricionales
}



// --------------------------------------------------------------------

void Diario::updateValoresDiario(ValoresNutricionales val){
  float carb = _valoresDiario.getCarbValores() + val.getCarbValores();
  float lip = _valoresDiario.getLipValores() + val.getLipValores();
  float prot = _valoresDiario.getProtValores() + val.getProtValores();
  float kcal = _valoresDiario.getKcalValores() + val.getKcalValores();
  ValoresNutricionales valAux(carb, lip, prot, kcal);
  _valoresDiario.setValores(valAux);
}

#endif
