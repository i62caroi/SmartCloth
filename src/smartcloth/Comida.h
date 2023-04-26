#ifndef COMIDA_H
#define COMIDA_H

#include "Plato.h"

#define NUM_PLATOS 10

class Comida{
  private:
    int _nPlatos;
    float _peso;
    Plato _platos[NUM_PLATOS];
    ValoresNutricionales _valoresComida;

    inline void setNumPlatos(int num){ _nPlatos = num; };
    inline int getNumPlatos(){ return _nPlatos; };
    inline int firstGapComida(){ return this->getNumPlatos(); };
    inline int getLastPositionPlato(){ return this->getNumPlatos() - 1; };

  public:
    Comida();

    inline bool isComidaEmpty(){ if(getNumPlatos() == 0) return true; else return false;};

    inline void setPesoComida(float peso){ _peso = peso; };
    inline float getPesoComida(){ return _peso; };

    void addIngComida(Ingrediente &ing);
    void addPlato(Plato plato);
    void deletePlato(Plato &plato);

    void updateValoresComida(bool suma, ValoresNutricionales val);
    inline ValoresNutricionales getValoresComida(){ return _valoresComida; };

    void restoreComida();
};



Comida::Comida(){
  this->setNumPlatos(0);
  this->setPesoComida(0.0);
}

// --------------------------------------------------------------------

void Comida::addIngComida(Ingrediente &ing){   
    this->setPesoComida(this->getPesoComida() + ing.getPesoIng());       // Incrementar peso de la comida
    this->updateValoresComida(true, ing.getValoresIng());                // Sumar (suma = true) Valores Nutricionales de la comida
}

void Comida::addPlato(Plato plato){
    _platos[this->firstGapComida()] = plato;                            // Añadir plato
    this->setNumPlatos(this->getNumPlatos() + 1);                       // Incrementar num platos
}

// --------------------------------------------------------------------
void Comida::deletePlato(Plato &plato){
    /*Plato plato = _platos[this->getLastPositionPlato()];
      this->setNumPlatos(this->getNumPlatos() - 1);           */                    // Decrementar num platos
    /* Lo anterior se haría si se quisieran borrar platos previamente
       guardados. La funcionalidad de SmartCloth es borrar el plato
       actual para empezarlo de nuevo, pero los anteriores ya no se
       pueden cambiar. */
       // No hace falta decrementar nPlatos porque no se ha llegado a guardar 
       // el plato ni, por tanto, incrementar el numero platos
    this->setPesoComida(this->getPesoComida() - plato.getPesoPlato());          // Decrementar peso de la comida según peso del plato
    this->updateValoresComida(false, plato.getValoresPlato());                  // Restar (suma = false) Valores Nutricionales de la comida
}


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
    _valoresComida.setValores(valAux);
}


// "Reiniciar" comida
void Comida::restoreComida(){
    this->setNumPlatos(0);                                   // Nº ing = 0
    this->setPesoComida(0.0);                              // Peso = 0.0
    ValoresNutricionales valAux(0.0, 0.0, 0.0, 0.0);
    _valoresComida.setValores(valAux);                     // Valores = 0
}


#endif
