#ifndef ALIMENTO_H
#define ALIMENTO_H

#include "Valores_Nutricionales.h"
#include "Grupos.h"



class Alimento{
  private:
    Grupo _grupo;
    double _peso;
    ValoresNutricionales _valoresAlimento;

  public:
    Alimento();
    Alimento(Grupo grupo, double peso); 
    
    inline void setGrupoAlimento(Grupo grupo){ _grupo = grupo; };
    inline Grupo getGrupoAlimento(){ return _grupo; };
    
    inline void setPesoAlimento(double peso){ _peso = peso; };
    inline double getPesoAlimento(){ return _peso; };

    void updateValoresAlimento(Grupo grupo, double peso);
    inline void setValoresAlimento(ValoresNutricionales val){ _valoresAlimento.setValores(val); };
    inline ValoresNutricionales getValoresAlimento(){ return _valoresAlimento; };
};




Alimento::Alimento(){
    this->setPesoAlimento(0.0);
}

Alimento::Alimento(Grupo grupo, double peso){
    this->setGrupoAlimento(grupo);
    this->setPesoAlimento(peso);
    this->updateValoresAlimento(grupo, peso);
}


void Alimento::updateValoresAlimento(Grupo grupo, double peso){
    double carb = grupo.Carb_g * peso;
    double lip = grupo.Lip_g * peso;
    double prot = grupo.Prot_g * peso;
    double kcal = grupo.Kcal_g * peso;
    ValoresNutricionales valAux(carb, lip, prot, kcal);
    _valoresAlimento.setValores(valAux);
}


#endif
