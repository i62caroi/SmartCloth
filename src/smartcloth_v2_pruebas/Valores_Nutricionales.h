/**
 * @file Valores_Nutricionales.h
 * @brief Definición de la clase ValoresNutricionales
 *
 * @author Irene Casares Rodríguez
 * @date 06/06/23
 * @version 1.0
 * 
 * Este archivo contiene la definición de la clase ValoresNutricionales, que representa al conjunto
 * de carbohidratos, lípidos, proteínas y sus respectivas raciones, así como kilocalorías.
 * 
 * La clase ValoresNutricionales permite establecer y obtener la cantidad de carbohidratos,
 * lípidos, proteínas y kcal, además de las raciones. 
 * El objeto ValoresNutricionales se incluye en las clases Alimento, Plato, Comida y Diario.
 * 
 * @see Alimento, Plato, Comida y Diario
 * 
 */

#ifndef VALORES_NUTRICIONALES_H
#define VALORES_NUTRICIONALES_H


/**
 * @brief Clase que representa los valores nutricionales.
 */
class ValoresNutricionales{
  private:
    float   _carb;    /**< Valor de carbohidratos */
    float   _lip;     /**< Valor de lípidos */
    float   _prot;    /**< Valor de proteínas */
    float   _kcal;    /**< Valor calórico */
    float   _carb_R;  /**< Raciones de carbohidratos */
    float   _lip_R;   /**< Raciones de lípidos */
    float   _prot_R;  /**< Raciones de proteínas */

  public:
    /**
     * @brief Constructor por defecto de la clase ValoresNutricionales.
     *        Inicializa los valores a 0.0 y calcula automáticamente las raciones correspondientes.
     */
    ValoresNutricionales();

    /**
     * @brief Constructor de la clase ValoresNutricionales.
     *        Inicializa los valores según los parámetros pasados como argumentos y calcula automáticamente las raciones correspondientes.
     * 
     * @param carb Valor de carbohidratos
     * @param lip Valor de lípidos
     * @param prot Valor de proteínas
     * @param kcal Valor calórico
     */
    ValoresNutricionales(float carb, float lip, float prot, float kcal);

    /**
     * @brief Establece el valor de carbohidratos y calcula las raciones correspondientes.
     * 
     * @param carb Valor de carbohidratos a establecer
     */
    void setCarbValores(float carb);

    /**
     * @brief Calcula las raciones de carbohidratos a partir del valor establecido.
     */
    void computeCarbRaciones();

    /**
     * @brief Obtiene el valor de carbohidratos.
     * 
     * @return El valor de carbohidratos
     */
    inline float getCarbValores(){ return _carb; };

    /**
     * @brief Obtiene las raciones de carbohidratos.
     * 
     * @return Las raciones de carbohidratos
     */
    inline float getCarbRaciones(){ return _carb_R; };

    /**
     * @brief Establece el valor de lípidos y calcula las raciones correspondientes.
     * 
     * @param lip Valor de lípidos a establecer
     */
    void setLipValores(float lip);

    /**
     * @brief Calcula las raciones de lípidos a partir del valor establecido.
     */
    void computeLipRaciones();

    /**
     * @brief Obtiene el valor de lípidos.
     * 
     * @return El valor de lípidos
     */
    inline float getLipValores(){ return _lip; };

    /**
     * @brief Obtiene las raciones de lípidos.
     * 
     * @return Las raciones de lípidos
     */
    inline float getLipRaciones(){ return _lip_R; };

    /**
     * @brief Establece el valor de proteínas y calcula las raciones correspondientes.
     * 
     * @param prot Valor de proteínas a establecer
     */
    void setProtValores(float prot);

    /**
     * @brief Calcula las raciones de proteínas a partir del valor establecido.
     */
    void computeProtRaciones();

    /**
     * @brief Obtiene el valor de proteínas.
     * 
     * @return El valor de proteínas
     */
    inline float getProtValores(){ return _prot; };

    /**
     * @brief Obtiene las raciones de proteínas.
     * 
     * @return Las raciones de proteínas
     */
    inline float getProtRaciones(){ return _prot_R; };

    /**
     * @brief Establece el valor calórico.
     * 
     * @param Kcal Valor calórico a establecer
     */
    inline void setKcalValores(float Kcal){ _kcal = Kcal; };

    /**
     * @brief Obtiene el valor calórico.
     * 
     * @return El valor calórico
     */
    inline float getKcalValores(){ return _kcal; };

    /**
     * @brief Establece los valores de carbohidratos, lípidos, proteínas y valor calórico a partir de un objeto ValoresNutricionales.
     * 
     * @param val Objeto ValoresNutricionales a partir del cual se establecen los valores
     */
    void setValores(ValoresNutricionales val);

    /**
     * @brief Establece los valores de carbohidratos, lípidos, proteínas y valor calórico a partir de los parámetros pasados como argumentos.
     * 
     * @param carb Valor de carbohidratos
     * @param lip Valor de lípidos
     * @param prot Valor de proteínas
     * @param kcal Valor calórico
     */
    void setValores(float carb, float lip, float prot, float kcal);
};


/*---------------------------------------------------------------------------------------------------------
   ValoresNutricionales(): Constructor de la clase ValoresNutricionales que inicializa sus valores 
                           a 0.0, calculando automáticamente las raciones correspondientes a partir
                           de esos valores. Es decir, 0.0 también.
----------------------------------------------------------------------------------------------------------*/
ValoresNutricionales::ValoresNutricionales(){
  this->setCarbValores(0.0);
  this->setLipValores(0.0);
  this->setProtValores(0.0);
  this->setKcalValores(0.0);
  //this->setValores(0.0,0.0,0.0,0.0);
}

/*---------------------------------------------------------------------------------------------------------
   ValoresNutricionales(): Constructor de la clase ValoresNutricionales que inicializa sus valores 
                           según los parámetros pasados como argumentos, calculando automáticamente 
                           las raciones correspondientes.
                           
----------------------------------------------------------------------------------------------------------*/
ValoresNutricionales::ValoresNutricionales(float carb, float lip, float prot, float kcal){
  this->setCarbValores(carb);
  this->setLipValores(lip);
  this->setProtValores(prot);
  this->setKcalValores(kcal);
  //this->setValores(carb,lip,prot,kcal);
}


void ValoresNutricionales::setCarbValores(float carb){
  _carb = carb;
  this->computeCarbRaciones();
}

void ValoresNutricionales::computeCarbRaciones(){
  _carb_R = round(2.0*(this->getCarbValores()/10));   // 0.3 <= raciones <= 0.7   ------->  raciones = 0.5
  _carb_R = _carb_R/2;
}

void ValoresNutricionales::setLipValores(float lip){
  _lip = lip;
  this->computeLipRaciones();
}

void ValoresNutricionales::computeLipRaciones(){
  _lip_R = round(2.0*(this->getLipValores()/10));   // 0.3 <= raciones <= 0.7   ------->  raciones = 0.5
  _lip_R = _lip_R/2;
}

void ValoresNutricionales::setProtValores(float prot){
  _prot = prot;
  this->computeProtRaciones();
}

void ValoresNutricionales::computeProtRaciones(){
  _prot_R = round(2.0*(this->getProtValores()/10));   // 0.3 <= raciones <= 0.7   ------->  raciones = 0.5
  _prot_R = _prot_R/2;
}


void ValoresNutricionales::setValores(ValoresNutricionales val){
  this->setCarbValores(val.getCarbValores());
  this->setLipValores(val.getLipValores());
  this->setProtValores(val.getProtValores());
  this->setKcalValores(val.getKcalValores());
  //this->setValores(val.getCarbValores(), val.getLipValores(), val.getProtValores(), val.getKcalValores());
}

void ValoresNutricionales::setValores(float carb, float lip, float prot, float kcal){
  this->setCarbValores(carb);
  this->setLipValores(lip);
  this->setProtValores(prot);
  this->setKcalValores(kcal);
}


#endif
