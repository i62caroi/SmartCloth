#ifndef SCREEN_H
#define SCREEN_H

#include "RA8876.h"
//#include "BUTTONS.h"
#include "aux.h"
#include "STATE_MACHINE.h"

/* Screen circuit wiring */
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10

/* Screen */
RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);


/*---------------------------------------------------------------------------------------------------------
   setupScreen(): Inicializar pantalla
----------------------------------------------------------------------------------------------------------*/
void setupScreen(){
    pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
    digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight
    if (!tft.init()){
      Serial.println(F("Could not initialize RA8876"));
    }
    tft.clearScreen(0);
    tft.setTextScale(2);
    tft.selectInternalFont(RA8876_FONT_SIZE_32, RA8876_FONT_ENCODING_8859_1);
    Serial.println(F("Startup complete..."));
}



/*---------------------------------------------------------------------------------------------------------
   printInit(): Imprimir texto inicial en pantalla (pedir que se pulse botón)
   
                **Esta función se modificará por una que muestre la bienvenida a SmartCloth.**
----------------------------------------------------------------------------------------------------------*/
/*void printInit(){
    //tft.clearScreen(0);
    tft.setCursor((tft.getWidth() / 3), tft.getHeight() / 2);
    tft.print("                 ");
    tft.setCursor((tft.getWidth() / 3), tft.getHeight() / 2);
    tft.print("Pulsa un boton");
}*/ 


/*---------------------------------------------------------------------------------------------------------
   printButton(): Mostrar por pantalla la información asignada al botón pulsado
                    Parámetro: 
                            button   -  botón pulsado
----------------------------------------------------------------------------------------------------------*/
/*void printButton(String button){
    tft.clearScreen(0);
    tft.selectInternalFont(RA8876_FONT_SIZE_32, RA8876_FONT_ENCODING_8859_1);
    tft.setCursor((tft.getWidth() / 3), tft.getHeight() / 2);
    tft.print("                 ");
    tft.setCursor((tft.getWidth() / 5), tft.getHeight() / 2);
    tft.print("Boton pulsado: ");
    tft.print(button);
    delay(500);
    tft.clearScreen(0);
}*/


/*---------------------------------------------------------------------------------------------------------
   printScale(): Mostrar por pantalla el peso actual (último tomado)
----------------------------------------------------------------------------------------------------------*/
void printScale(){
    tft.clearScreen(0);
    tft.setCursor((tft.getWidth() / 3), tft.getHeight() / 3);
    tft.print("Peso: ");
    tft.print(weight);
    tft.println(" g\n\n");
    tft.selectInternalFont(RA8876_FONT_SIZE_24, RA8876_FONT_ENCODING_8859_1);
    tft.setCursor((tft.getWidth() / 5), tft.getHeight() / 2);
    tft.print("Puede añadir otro alimento, guardar el plato, eliminarlo o guardar la comida completa");
    //delay(1000);
}



/*---------------------------------------------------------------------------------------------------------
   simplePrint(): Mostrar por pantalla una cadena de texto
                    Parámetro: 
                            cad   -  cadena a mostrar
----------------------------------------------------------------------------------------------------------*/
void simplePrint(String cad){
    tft.clearScreen(0);
    tft.selectInternalFont(RA8876_FONT_SIZE_24, RA8876_FONT_ENCODING_8859_1);
    //tft.setCursor((tft.getWidth() / 3), tft.getHeight() / 2);
    //tft.print("                 ");
    tft.setCursor((tft.getWidth() / 5), tft.getHeight() / 3);
    tft.print(cad);
    //delay(50);
}



/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------------- INFORMACIÓN POR ESTADO ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   printStateInit(): Información del STATE_INI
----------------------------------------------------------------------------------------------------------*/
void printStateInit(){
    simplePrint("BIENVENIDO\n\n           Escoja grupo de alimentos");
}

/*---------------------------------------------------------------------------------------------------------
   printStateA(): Información del STATE_groupA
----------------------------------------------------------------------------------------------------------*/
void printStateA(){
  //TODO ejemplos grupo
    String buffer = "Alimento de tipo A: " + buttonGrande + " \n\nIndique si el alimento es crudo o cocinado \n    o escoja otro grupo";
    simplePrint(buffer);
}

/*---------------------------------------------------------------------------------------------------------
   printStateB(): Información del STATE_groupB
----------------------------------------------------------------------------------------------------------*/
void printStateB(){
  //TODO ejemplos grupo
    String buffer = "Alimento de tipo B: " + buttonGrande + " \n\nColoque el alimento sobre la báscula \n    o escoja otro grupo";
    simplePrint(buffer);
}

/*---------------------------------------------------------------------------------------------------------
   printStateRaw(): Información del STATE_Raw
----------------------------------------------------------------------------------------------------------*/
void printStateRaw(){
    simplePrint("Alimento CRUDO\n\n Coloque el alimento sobre la báscula");
}


/*---------------------------------------------------------------------------------------------------------
   printStateCooked(): Información del STATE_Cooked
----------------------------------------------------------------------------------------------------------*/
void printStateCooked(){
     simplePrint("Alimento COCINADO\n\n Coloque el alimento sobre la báscula");
}


/*---------------------------------------------------------------------------------------------------------
   printStateWeighted(): Información del STATE_Weighted
----------------------------------------------------------------------------------------------------------*/
void printStateWeighted(){
    printScale();
    //TODO valores alimento grupo X según peso
}

/*---------------------------------------------------------------------------------------------------------
   printStateAdded(): Información del STATE_Added
----------------------------------------------------------------------------------------------------------*/
/*void printStateAdded(){
    simplePrint("Plato añadido");
    simplePrint("Retire el plato");
}*/

/*---------------------------------------------------------------------------------------------------------
   printStateDeleted(): Información del STATE_Deleted
----------------------------------------------------------------------------------------------------------*/
/*void printStateDeleted(){
    simplePrint("Plato añadido");
    simplePrint("Retire el plato");
}*/

/*---------------------------------------------------------------------------------------------------------
   printStateSaved(): Información del STATE_Saved
----------------------------------------------------------------------------------------------------------*/
/*void printStateSaved(){
    simplePrint("Comida guardada");
    simplePrint("Retire el plato");
}*/



/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- ERRORES POR ESTADO ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   printEventError(): Información de error según estado cuando ocurre un evento que no correspondía
   
                      **Esta función se desplegará en otras más específicas de error para cada estado**
----------------------------------------------------------------------------------------------------------*/
/*void printEventError(){
    simplePrint("ACCIÓN ERRÓNEA");
    switch(state_actual){
        case 1: printStateInit();       break;
        case 2: printStateA();          break;
        case 3: printStateB();          break;
        case 4: printStateProcessed();  break;
        case 5: printStateWeighted();   break;
        case 6: printStateAdded();      break;
        case 7: printStateDeleted();    break;
        case 8: printStateSaved();      break;
    }
}
*/

#endif
