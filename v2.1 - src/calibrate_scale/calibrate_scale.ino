#include "HX711.h"


// ------ HX711 circuit wiring -----------
// SmartCloth v2.1
//const byte LOADCELL_DOUT_PIN = 3;
//const byte LOADCELL_SCK_PIN = 2;
// SmartCloth v2.2
const byte LOADCELL_DOUT_PIN = 2;
const byte LOADCELL_SCK_PIN = 3;
// ---------------------------------------

HX711 scale;


void setupScale();
float weighScale();


void setup()
{
  Serial.begin(115200);
  while (!Serial);
  delay(100);

  Serial.println("Starting up...");

  setupScale(); 

  Serial.println("Startup complete...");
}



void loop()
{
  float peso = weighScale();
  Serial.print("Peso: "); Serial.print(peso); Serial.println(" g");
  delay(500);
}





void setupScale()
{
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    scale.set_scale(907.89); 
    scale.tare(5);  // Media de 5 medidas (5 por defecto)
    //scale.get_units(10);
    Serial.println(F("Scale initialized"));
}

float weighScale()
{
    return scale.get_units(5);
}