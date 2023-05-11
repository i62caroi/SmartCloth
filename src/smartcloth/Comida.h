#ifndef COMIDA_H
#define COMIDA_H

#include "Plato.h"
//#include "Variables.h" //platoActual

#define NUM_PLATOS 10

class Comida{
  private:
    int _nPlatos;
    float _peso;
    Plato _platos[NUM_PLATOS];
    ValoresNutricionales _valoresComida;

    inline void setNumPlatos(int num){ _nPlatos = num; };
    inline int getNumPlatos(){ return _nPlatos; };
    inline int firstGapComida(){ return this->getNumPlatos(); }; // Si hay algún plato en esa posición (porque no se borran realmente), se sobreescribe
    inline int getLastPositionPlato(){ return this->getNumPlatos() - 1; };

  public:
    Comida();

    inline bool isComidaEmpty(){ if(getNumPlatos() == 0) return true; else return false;};

    inline void setPesoComida(float peso){ _peso = peso; };
    inline float getPesoComida(){ return _peso; };

    void addIngComida(Ingrediente &ing);

    void addPlato(Plato plato);
    
    //void deletePlato(Plato &plato);
    void deletePlato(); //actual o previo

    void updateValoresComida(bool suma, ValoresNutricionales val);
    inline ValoresNutricionales getValoresComida(){ return _valoresComida; };

    void restoreComida();

    String getComidaAllValues(); //Para guardar en SD
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

// --------------------------------------------------------------------

void Comida::addPlato(Plato plato){
//void Comida::addPlato(Plato &plato){
  // En realidad no hace falta guardar el objeto plato porque sus elementos
  // (valores nutricionales y peso de los alimentos) se han ido guardando uno a
  // uno. Solo haría falta aumentar el número de platos para indicar que se ha "guardado"
  // Posteriormente, al borrarlo, se restan sus valores nutricionales y peso
    _platos[this->firstGapComida()] = plato;                            // Añadir plato
    this->setNumPlatos(this->getNumPlatos() + 1);                       // Incrementar num platos
}
// --------------------------------------------------------------------


// --------------------- DELETE SOLO ACTUAL -----------------------------------
//void Comida::deletePlato(Plato &plato){
    /*Plato plato = _platos[this->getLastPositionPlato()];
      this->setNumPlatos(this->getNumPlatos() - 1);           */                    // Decrementar num platos
    /* Lo anterior se haría si se quisieran borrar platos previamente
       guardados. La funcionalidad de SmartCloth es borrar el plato
       actual para empezarlo de nuevo, pero los anteriores ya no se
       pueden cambiar. <===> JUSTO AL CONTRARIOOOO !!!!!!! */
       // No hace falta decrementar nPlatos porque no se ha llegado a guardar 
       // el plato ni, por tanto, incrementar el numero platos
//    this->setPesoComida(this->getPesoComida() - plato.getPesoPlato());          // Decrementar peso de la comida según peso del plato
//    this->updateValoresComida(false, plato.getValoresPlato());                  // Restar (suma = false) Valores Nutricionales de la comida
//}
// ----------------------------------------------------------------------------


// --------------------- DELETE RETROACTIVO -----------------------------------
void Comida::deletePlato(){
    // Se decrementa peso de comida según peso completo del plato. 
    this->setPesoComida(this->getPesoComida() - _platos[this->getLastPositionPlato()].getPesoPlato());        // Decrementar peso de la comida según peso del plato

    this->updateValoresComida(false, _platos[this->getLastPositionPlato()].getValoresPlato());                // Restar (suma = false) Valores Nutricionales de la comida

    // Hay que decrementar nPlatos aunque se borre el plato actual porque se ha guardado temporalmente
    this->setNumPlatos(this->getNumPlatos() - 1);                                                             // Decrementar número de platos
}
// ----------------------------------------------------------------------------


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

// --------------------------------------------------------------------


// "Reiniciar" comida
void Comida::restoreComida(){
    this->setNumPlatos(0);                                   // Nº platos = 0
    this->setPesoComida(0.0);                                // Peso = 0.0
    //ValoresNutricionales valAux(0.0, 0.0, 0.0, 0.0);
    ValoresNutricionales valAux; //(0.0, 0.0, 0.0, 0.0)
    this->_valoresComida.setValores(valAux);                 // Valores = 0.0
}

// --------------------------------------------------------------------


String Comida::getComidaAllValues(){
    int carb_R = round(_valoresComida.getCarbValores()/10);
    int lip_R = round(_valoresComida.getLipValores()/10);
    int prot_R = round(_valoresComida.getProtValores()/10);

    String dataString = String(_valoresComida.getCarbValores()) + ";" + String(carb_R) + ";" + 
                        String(_valoresComida.getLipValores()) + ";" + String(lip_R) + ";" + 
                        String(_valoresComida.getProtValores()) + ";" + String(prot_R) + ";" + 
                        String(_valoresComida.getKcalValores()); 

    return dataString;
}


#endif
