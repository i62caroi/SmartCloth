#ifndef SCREEN_H
#define SCREEN_H

#include "RA8876.h"
#include "aux.h"
#include "grupos.h"
#include "STATE_MACHINE.h"

/* Screen circuit wiring */
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10

int ANCHO;
int ALTO;


// Colores en formato RGB565 ==> http://www.barth-dev.de/online/rgb565-color-picker/

#define   NEGRO         0x0000
#define   BLANCO        0xFFFF
#define   ROJO          0xF920
#define   NARANJA       0xFC80
#define   AMARILLO      0xFFC0
#define   VERDE         0x07C0
#define   CIAN          0x07FF
#define   AZUL          0x019F
#define   ROSA          0xFA1F
#define   MORADO        0x9112
#define   MARRON        0xABC8


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
    tft.clearScreen(0); //0x0000 --> Negro
    tft.setTextScale(2);
    tft.selectInternalFont(RA8876_FONT_SIZE_32, RA8876_FONT_ENCODING_8859_1);
    ANCHO = tft.getWidth();
    ALTO = tft.getHeight();
    Serial.println(F("\nStartup complete..."));
    delay(200);
}



/*---------------------------------------------------------------------------------------------------------
   simplePrint(): Mostrar por pantalla una cadena de texto
                    Parámetro: 
                            cad   -  cadena a mostrar
----------------------------------------------------------------------------------------------------------*/
void simplePrint(String cad){
    tft.clearScreen(0); //0x0000 --> Negro
    tft.selectInternalFont(RA8876_FONT_SIZE_24); //RA8876_FONT_ENCODING_8859_1
    tft.setTextColor(BLANCO);
    tft.setCursor((ANCHO / 5), ALTO / 3);
    tft.print(cad);
}



/*---------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------------*/
void printEjemplos(){    
    tft.selectInternalFont(RA8876_FONT_SIZE_16);                // Tamaño texto
    tft.setCursor(50, 40);                                     // Posicion inicio texto
    tft.setTextColor(AMARILLO);                                   // Color texto                  
    tft.println(grupoEscogido.Ejemplos_grupo); tft.print("\n");                    // Imprimir texto
}

/*---------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------------*/
void printGrupo(){
    tft.selectInternalFont(RA8876_FONT_SIZE_24);                // Tamaño texto
    tft.setCursor(50, tft.getCursorY());                            // Posicion inicio texto
    tft.setTextColor(BLANCO);                                 // Color texto      
    tft.setTextScale(2);        
    tft.println(grupoEscogido.Nombre_grupo); tft.print("\n");                      // Imprimir texto
}

/*---------------------------------------------------------------------------------------------------------

----------------------------------------------------------------------------------------------------------*/
void printValoresActuales(){
    tft.setCursor(50, tft.getCursorY());
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextColor(ROJO);
    tft.print("Alimento: "); tft.print(weight); tft.println(" g\n"); //valoresActuales.peso
   
    tft.setCursor(50, tft.getCursorY());
    tft.setTextColor(BLANCO);
    tft.println("Comida Actual");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(50, tft.getCursorY()+20);
    tft.print("Carb: "); tft.print(valoresActuales.Carb); tft.println(" g");
    tft.setCursor(50, tft.getCursorY());
    tft.print("Lip: "); tft.print(valoresActuales.Lip); tft.println(" g");
    tft.setCursor(50, tft.getCursorY());
    tft.print("Prot: "); tft.print(valoresActuales.Prot); tft.println(" g");
    tft.setCursor(50, tft.getCursorY());
    tft.print("Kcal: "); tft.print(valoresActuales.Kcal); tft.println(" Kcal");
    tft.setCursor(50, tft.getCursorY());
}


void generalPrint(){
    tft.clearScreen(0);   
    printEjemplos();   
    printGrupo();      
    printValoresActuales(); 
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
/*void printStateA(){
    //String cad = "Alimento de tipo A: " + String(buttonGrande) + " \n\nIndique si el alimento es crudo o cocinado \n    o escoja otro grupo";
    //String cad = grupoEscogido.Nombre_grupo + "\n\n" + grupoEscogido.Ejemplos_grupo; 
    //simplePrint(cad);

    tft.clearScreen(0);   
    printEjemplos();   
    printGrupo();      
    printValoresActuales();                     
}*/



/*---------------------------------------------------------------------------------------------------------
   printStateB(): Información del STATE_groupB
----------------------------------------------------------------------------------------------------------*/
/*void printStateB(){
    //String cad = "Alimento de tipo B: " + String(buttonGrande) + " \n\nColoque el alimento sobre la báscula \n    o escoja otro grupo";
    //String cad = grupoEscogido.Nombre_grupo + "\n\n" + grupoEscogido.Ejemplos_grupo; 
    //simplePrint(cad);

    tft.clearScreen(0);
    printEjemplos();  
    printGrupo();     
    printValoresActuales();
}
*/


/*---------------------------------------------------------------------------------------------------------
   printStateRaw(): Información del STATE_Raw
----------------------------------------------------------------------------------------------------------*/
/*void printStateRaw(){
    //simplePrint("Alimento CRUDO\n\n Coloque el alimento sobre la báscula");
    String cad = "CRUDO";
    tft.clearScreen(0); 
    printGrupo(1); //Menor tamaño que antes
    tft.setCursor(700, 80);                        
    tft.setTextColor(ROJO);      
    tft.print(cad);                 
    //tft.setCursor(     
}*/


/*---------------------------------------------------------------------------------------------------------
   printStateCooked(): Información del STATE_Cooked
----------------------------------------------------------------------------------------------------------*/
/*void printStateCooked(){
    //simplePrint("Alimento COCINADO\n\n Coloque el alimento sobre la báscula");
    String cad = "COCINADO";
    tft.clearScreen(0); 
    printGrupo(1); //Menor tamaño que antes
    tft.setCursor(700, 80);                        
    tft.setTextColor(ROJO); 
    tft.print(cad);                      
}*/


/*---------------------------------------------------------------------------------------------------------
   printStateRawCooked(): Información de STATE_Raw o STATE_Cooked
----------------------------------------------------------------------------------------------------------*/
void printStateRawCooked(String cad){
    tft.clearScreen(0); 
    printEjemplos();  
    printGrupo();
    tft.setCursor(700, tft.getCursorY()); // CRUDO/COCINADO arriba derecha
    tft.setTextColor(MORADO);      
    tft.print(cad);     
    printValoresActuales();
}


/*---------------------------------------------------------------------------------------------------------
   printStateWeighted(): Información del STATE_Weighted
----------------------------------------------------------------------------------------------------------*/
//void printStateWeighted(){
    /*tft.clearScreen(0); //0x0000 --> Negro
    tft.setCursor((ANCHO / 3), ALTO / 3);
    tft.print("Peso: ");
    tft.print(weight);
    tft.println(" g\n\n");
    tft.selectInternalFont(RA8876_FONT_SIZE_24); //RA8876_FONT_ENCODING_8859_1
    tft.setCursor((ANCHO / 5), ALTO / 2);
    tft.print("Puede a\xF1""adir otro alimento, guardar el plato, eliminarlo o guardar la comida completa");*/

 /*   tft.clearScreen(0); 
    printEjemplos();  
    printGrupo(); 
    printValoresActuales();
}*/

/*---------------------------------------------------------------------------------------------------------
   printStateAdded(): Información del STATE_Added
----------------------------------------------------------------------------------------------------------*/
void printStateAdded(){
    simplePrint("Plato a\xF1""adido\n\n Retire el plato");
}

/*---------------------------------------------------------------------------------------------------------
   printStateDeleted(): Información del STATE_Deleted
----------------------------------------------------------------------------------------------------------*/
void printStateDeleted(){
    simplePrint("Plato eliminado\n\n Retire el plato");
}

/*---------------------------------------------------------------------------------------------------------
   printStateSaved(): Información del STATE_Saved
----------------------------------------------------------------------------------------------------------*/
void printStateSaved(){
    simplePrint("Comida guardada\n\n Retire el plato");
}



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
