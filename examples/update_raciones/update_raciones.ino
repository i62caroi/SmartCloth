#include "Valores_Nutricionales.h"

ValoresNutricionales valoresNutricionales;

void setup() {
    Serial.begin(115200); 
    while (!Serial);  
    
    valoresNutricionales.setCarbValores(1.3); // Establece un valor de carbohidratos y las raciones
    valoresNutricionales.setProtValores(0.4);
    valoresNutricionales.setLipValores(9.9);
    
    float carb_R = valoresNutricionales.getCarbRaciones(); 
    float prot_R = valoresNutricionales.getProtRaciones(); 
    float lip_R = valoresNutricionales.getLipRaciones(); 

    Serial.print("Raciones CARB: "); Serial.println(carb_R); 
    Serial.print("Raciones PROT: "); Serial.println(prot_R); 
    Serial.print("Raciones LIP: "); Serial.println(lip_R); 

}

void loop() {
  // Nada que hacer aquí para esta prueba
}