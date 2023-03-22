#ifndef PLATO_H
#define PLATO_H

/*#include "grupos.h"

#define NUM_ALIMENTOS 10

typedef struct{
    float Kcal;
    float Prot,
    float Lip;
    float Carb;
}ValoresNutricionales;

class Plato{
  
  public:
    Plato();
    inline void setIdPlato(int id){ _ID = id; };
    void addGrupo(int botonGrande); //update valores
    void deleteGrupo(int botonGrande); //borrar ultimo pulsado y update valores
    inline int getIdPlato(){ return _ID; };
    void showInfoPlato();

  private:
    int _ID;
    grupo _alimentos[NUM_ALIMENTOS];
    ValoresNutricionales _valores;
    int firstGapAlimentos();
    int getGrupo(int boton);
    
};



Plato::Plato(){
    _ID = 1;
    _numAlimentos = 0;
    for (int i = 0; i < NUM_ALIMENTOS; i++){
          _alimentos[i] = 0;
    }
}

int Plato::firstGapAlimentos(){
    for (int i = 0; i < NUM_ALIMENTOS; i++){
        if(_alimentos[i] == 0){
            return i;
        }
    }
}




void Plato::updateValores(float Kcal, float Prot, float Lip, float Carb){
    _valores._Kcal = Kcal;
    _valores._Prot = Prot;
    _valores._Lip = Lip;
    _valores._Carb = Carb;
}


void Plato::addGrupo(int boton){
    int pos = this.firstGapAlimentos(); //Posicion del vector _Alimentos donde guardar
    int grupoEscogido = this.getGrupo(boton);   //Grupo de alimentos escogido
    ValoresNutricionales valoresAlimento;
    valoresAlimento.Kcal = gruposAlimentos[grupoEscogido]
    
    updateValores(kcal, prot, lip, carb); 
};


*/

#endif
