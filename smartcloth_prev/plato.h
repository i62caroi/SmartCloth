#ifndef PLATO_H
#define PLATO_H

#include "grupos.h"
#include "STATE_MACHINE.h"

#define NUM_ING 10

typedef struct{
    valoresNutricionales ingredientes[NUM_ING];
    valoresNutricionales valoresPlato = {0.0,0.0,0.0,0.0,0.0};
}Plato;


Plato platoActual;



/*---------------------------------------------------------------------------------------------------------
   firstGapPlato(): Primer hueco libre en el vector plato
----------------------------------------------------------------------------------------------------------*/
int firstGapPlato(){
    for (int i = 0; i < NUM_ING; i++){
        if(platoActual.ingredientes[i] == 0){
            return i;
        }
    }
}


void addIngredientePlato(){
    int posIng;
    /* Si se colocó algo en la báscula y se volvió a escoger un grupo de alimentos,
       se guarda en el plato actual la información del ingrediente colocado.
    */
    if((prevEvent == INCREMENTO) and ((newEvent == TIPO_A) or (newEvent == TIPO_B)){
      
        //Añadimos ingrediente al plato
        posIng = firstGapPlato();
        platoActual.ingredientes[posIng] = valoresActuales;
        
        //Actualizamos valores totales del plato
        platoActual.valoresPlato.Peso += valoresActuales.Peso;
        platoActual.valoresPlato.Kcal += valoresActuales.Kcal;
        platoActual.valoresPlato.Prot += valoresActuales.Prot;
        platoActual.valoresPlato.Lip += valoresActuales.Lip;
        platoActual.valoresPlato.Carb += valoresActuales.Carb;
    }
}











/*


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
