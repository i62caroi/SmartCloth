#ifndef VALORES_NUTRICIONALES_H
#define VALORES_NUTRICIONALES_H


class ValoresNutricionales{
  private:
    float _carb;
    float _lip;
    float _prot;
    float _kcal;

  public:
    ValoresNutricionales();
    ValoresNutricionales(float carb, float lip, float prot, float kcal);

    inline void setCarbValores(float carb){ _carb = carb; };
    inline float getCarbValores(){ return _carb; };

    inline void setLipValores(float lip){ _lip = lip; };
    inline float getLipValores(){ return _lip; };

    inline void setProtValores(float prot){ _prot = prot; };
    inline float getProtValores(){ return _prot; };

    inline void setKcalValores(float Kcal){ _kcal = Kcal; };
    inline float getKcalValores(){ return _kcal; };

    void setValores(ValoresNutricionales val);
};



ValoresNutricionales::ValoresNutricionales(){
  this->setCarbValores(0.0);
  this->setLipValores(0.0);
  this->setProtValores(0.0);
  this->setKcalValores(0.0);
}

ValoresNutricionales::ValoresNutricionales(float carb, float lip, float prot, float kcal){
  this->setCarbValores(carb);
  this->setLipValores(lip);
  this->setProtValores(prot);
  this->setKcalValores(kcal);
}

void ValoresNutricionales::setValores(ValoresNutricionales val){
  this->setCarbValores(val.getCarbValores());
  this->setLipValores(val.getLipValores());
  this->setProtValores(val.getProtValores());
  this->setKcalValores(val.getKcalValores());
}


#endif
