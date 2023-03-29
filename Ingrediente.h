#ifndef INGREDIENTE_H
#define INGREDIENTE_H

#include "Valores_Nutricionales.h"
#include "Grupos.h"



class Ingrediente{
  private:
    Grupo _grupo;
    float _peso;
    ValoresNutricionales _valoresIng;

  public:
    Ingrediente();
    Ingrediente(Grupo grupo, float peso); 
    
    inline void setGrupoIng(Grupo grupo){ _grupo = grupo; };
    inline Grupo getGrupoIng(){ return _grupo; };
    
    inline void setPesoIng(float peso){ _peso = peso; };
    inline float getPesoIng(){ return _peso; };

    void updateValoresIng(Grupo grupo, float peso);
    inline void setValoresIng(ValoresNutricionales val){ _valoresIng.setValores(val); };
    inline ValoresNutricionales getValoresIng(){ return _valoresIng; };
};




Ingrediente::Ingrediente(){
    this->setPesoIng(0.0);
}

Ingrediente::Ingrediente(Grupo grupo, float peso){
    this->setGrupoIng(grupo);
    this->setPesoIng(peso);
    this->updateValoresIng(grupo, peso);
}


void Ingrediente::updateValoresIng(Grupo grupo, float peso){
    float carb = grupo.Carb_g * peso;
    float lip = grupo.Lip_g * peso;
    float prot = grupo.Prot_g * peso;
    float kcal = grupo.Kcal_g * peso;
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    _valoresIng.setValores(valAux);
}


#endif
