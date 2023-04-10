#ifndef INGREDIENTE_H
#define INGREDIENTE_H

#include "Valores_Nutricionales.h"
#include "Grupos.h"



class Ingrediente{
  private:
    Grupo _grupo;
    double _peso;
    ValoresNutricionales _valoresIng;

  public:
    Ingrediente();
    Ingrediente(Grupo grupo, double peso); 
    
    inline void setGrupoIng(Grupo grupo){ _grupo = grupo; };
    inline Grupo getGrupoIng(){ return _grupo; };
    
    inline void setPesoIng(double peso){ _peso = peso; };
    inline double getPesoIng(){ return _peso; };

    void updateValoresIng(Grupo grupo, double peso);
    inline void setValoresIng(ValoresNutricionales val){ _valoresIng.setValores(val); };
    inline ValoresNutricionales getValoresIng(){ return _valoresIng; };
};




Ingrediente::Ingrediente(){
    this->setPesoIng(0.0);
}

Ingrediente::Ingrediente(Grupo grupo, double peso){
    this->setGrupoIng(grupo);
    this->setPesoIng(peso);
    this->updateValoresIng(grupo, peso);
}


void Ingrediente::updateValoresIng(Grupo grupo, double peso){
    double carb = grupo.Carb_g * peso;
    double lip = grupo.Lip_g * peso;
    double prot = grupo.Prot_g * peso;
    double kcal = grupo.Kcal_g * peso;
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    _valoresIng.setValores(valAux);
}


#endif
