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
    
    void addIngrediente(Ingrediente ing);     
    //void deleteLastIngrediente();
    
    void updateValoresPlato(ValoresNutricionales val);            // + val al addIngrediente()
    //void restoreValoresPlato(ValoresNutricionales val);         // - val al deleteLastIngrediente()
    inline ValoresNutricionales getValoresPlato(){ return _valoresPlato; };


};



Plato::Plato(){
    this->setNumIng(0);
    this->setPesoPlato(0.0);
}


void Plato::addIngrediente(Ingrediente ing){
    _ingredientes[this->firstGapPlato()] = ing;                                   // Añadir ingrediente
    this->setNumIng(this->getNumIng() + 1);                                 // Incrementar num ingredientes
    this->setPesoPlato(this->getPesoPlato() + ing.getPesoIng());            // Incrementar peso del plato
    this->updateValoresPlato(ing.getValoresIng());                          // Actualizar Valores Nutricionales
}

/*void Plato::deleteLastIngrediente(){
    if(this->getNumIng() != 0){
        Ingrediente lastIng = _ingredientes[this->positionLastIng()];
        this->restoreValoresPlato(lastIng.getValoresIng());                     // Eliminar valores del ingrediente del total del plato
        this->setPesoPlato(this->getPesoPlato() - lastIng.getPesoIng());        // Decrementar peso del plato
        this->setNumIng(this->getNumIng() - 1);                                 // Decrementar num ingredientes
    }
}*/


void Plato::updateValoresPlato(ValoresNutricionales val){
    float carb = _valoresPlato.getCarbValores() + val.getCarbValores();
    float lip = _valoresPlato.getLipValores() + val.getLipValores();
    float prot = _valoresPlato.getProtValores() + val.getProtValores();
    float kcal = _valoresPlato.getKcalValores() + val.getKcalValores();
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    _valoresPlato.setValores(valAux);
}

/*void Plato::restoreValoresPlato(ValoresNutricionales val){
    float carb = _valoresPlato.getCarbValores() - val.getCarbValores();
    float lip = _valoresPlato.getLipValores() - val.getLipValores();
    float prot = _valoresPlato.getProtValores() - val.getProtValores();
    float kcal = _valoresPlato.getKcalValores() - val.getKcalValores();
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    _valoresPlato.setValores(valAux);
}*/



#endif
