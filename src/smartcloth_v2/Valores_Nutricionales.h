#ifndef VALORES_NUTRICIONALES_H
#define VALORES_NUTRICIONALES_H


class ValoresNutricionales{
  private:
    float _carb;
    float _lip;
    float _prot;
    float _kcal;
    int _carb_R;
    int _lip_R;
    int _prot_R;

  public:
    ValoresNutricionales();
    ValoresNutricionales(float carb, float lip, float prot, float kcal);

    void setCarbValores(float carb);
    inline float getCarbValores(){ return _carb; };
    inline int getCarbRaciones(){ return _carb_R; };

    void setLipValores(float lip);
    inline float getLipValores(){ return _lip; };
    inline int getLipRaciones(){ return _lip_R; };

    inline void setProtValores(float prot);
    inline float getProtValores(){ return _prot; };
    inline int getProtRaciones(){ return _prot_R; };

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


void ValoresNutricionales::setCarbValores(float carb){
  _carb = carb;
  _carb_R = round(2.0*(_carb/10));   // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
  _carb_R = _carb_R/2;
}

void ValoresNutricionales::setLipValores(float lip){
  _lip = lip;
  _lip_R = round(2.0*(_lip/10));    // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
  _lip_R = _lip_R/2;
}

void ValoresNutricionales::setProtValores(float prot){
  _prot = prot;
  _prot_R = round(2.0*(_prot/10));  // 0.3 <= raciones <= 0.7   -->  raciones = 0.5
  _prot_R = _prot_R/2;
}


void ValoresNutricionales::setValores(ValoresNutricionales val){
  this->setCarbValores(val.getCarbValores());
  this->setLipValores(val.getLipValores());
  this->setProtValores(val.getProtValores());
  this->setKcalValores(val.getKcalValores());
}


#endif
