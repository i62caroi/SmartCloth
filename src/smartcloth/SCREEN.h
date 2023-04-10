#ifndef SCREEN_H
#define SCREEN_H

#include "RA8876.h"
#include "State_Machine.h"
#include "Variables.h"


/* Screen circuit wiring */
#define RA8876_CS        12
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10


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

int ANCHO; // X (1024)
int ALTO; // Y (600)


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
    ANCHO = tft.getWidth(); // X
    ALTO = tft.getHeight(); // Y
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
    tft.setCursor((ANCHO / 5), ALTO / 3); //(x,y)
    tft.print(cad);
}




/***************************************************************************************************/
/*------------------------- EJEMPLOS Y GRUPO DE ALIMENTOS   ---------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printEjemplosyGrupo(): Muestra ejemplos del grupo de alimentos seleccionado
----------------------------------------------------------------------------------------------------------*/
void printEjemplosyGrupo(){    
    tft.clearScreen(0);           // Limpiar
    
    /* ----- EJEMPLOS ----- */
    tft.selectInternalFont(RA8876_FONT_SIZE_16);       // Tamaño texto
    tft.setCursor(50, 40);                             // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto                  
    tft.println(grupoEscogido.Ejemplos_grupo);         // Imprimir texto

    /* ----- GRUPO ----- */
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(50, 120);                            // Posicion inicio texto => tft.getCursorY()
    tft.setTextColor(BLANCO);                          // Color texto      
    tft.setTextScale(2);        
    tft.println(grupoEscogido.Nombre_grupo);           // Imprimir texto

    /* ----- CRUDO/COCINADO ----- */
    tft.setCursor(850, 130); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);                           
    tft.setTextColor(CIAN);      
    tft.print(procesamiento);                          // Predeterminadamente "CRUDO"
}



/***************************************************************************************************/
/*------------------------------- ESTRUCTURA CENTRAL   --------------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printCentral(): Muestra el peso del alimento y el plato
          Parámetros:
              ing - Ingrediente auxiliar temporal
----------------------------------------------------------------------------------------------------------*/
void printCentral(){
    tft.setCursor(50, 210); //tft.getCursorY()
    tft.selectInternalFont(RA8876_FONT_SIZE_24);  
    tft.setTextColor(VERDE);
    tft.print("Alimento: "); 
    tft.setCursor(tft.getCursorX(), 215); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(weight); tft.println("g\n");

    tft.setCursor(430, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Plato: "); 
    tft.setCursor(tft.getCursorX(), 215); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(platoActual.getPesoPlato()); tft.println("g\n");

    tft.setCursor(700, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Total: "); 
    tft.setCursor(tft.getCursorX(), 215); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(comidaActual.getPesoComida()); tft.println("g\n");

    tft.setCursor(50, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextColor(BLANCO);
    tft.println("Comida Actual\n");
}



/***************************************************************************************************/
/*------------------------------- VALORES NUTRICIONALES   -----------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printValores(): Muestra los valores nutricionales que se indiquen
          Parámetros:
              val - ValoresNutricionales
----------------------------------------------------------------------------------------------------------*/
void printValores(ValoresNutricionales val){
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(50, 375);
    tft.setTextColor(BLANCO);
    
    tft.print("Carb: "); tft.print(val.getCarbValores()); tft.println(" g");
    tft.setCursor(50, tft.getCursorY()+10);
    tft.print("Lip: "); tft.print(val.getLipValores()); tft.println(" g");
    tft.setCursor(50, tft.getCursorY()+10);
    tft.print("Prot: "); tft.print(val.getProtValores()); tft.println(" g");
    tft.setCursor(50, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(val.getKcalValores()); tft.println(" Kcal");
}





/*********************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------------- INFORMACIÓN POR ESTADO ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*********************************************************************************************************/



/*---------------------------------------------------------------------------------------------------------
   printStateInit(): Información del STATE_INI
----------------------------------------------------------------------------------------------------------*/
void printStateInit(){
    simplePrint("BIENVENIDO\n\n           Escoja grupo de alimentos");
}




/*---------------------------------------------------------------------------------------------------------
   printStateABandProcessed(): Incluye grupo de alimentos, ejemplos, procesamiento (crudo/cocinado)
                                y valores nutricionales del plato actual
----------------------------------------------------------------------------------------------------------*/
void printStateABandProcessed(){
    printEjemplosyGrupo();                          // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    printCentral();                                 // 2 - Estructura central (peso 0.0)
    printValores(platoActual.getValoresPlato());    // 3 - Valores nutricionales
    //printValores(comidaActual.getValoresComida()); 
}




/*---------------------------------------------------------------------------------------------------------
   printStateWeighted(): 
----------------------------------------------------------------------------------------------------------*/
void printStateWeighted(){
    printEjemplosyGrupo();                          // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    printCentral();                                 // 2 - Estructura central
    
    // ---------------
    Ingrediente ingAux(grupoEscogido, weight);         // Ingrediente auxiliar usado para mostrar información variable de lo pesado
    float carb = ingAux.getValoresIng().getCarbValores() + platoActual.getValoresPlato().getCarbValores();
    float lip = ingAux.getValoresIng().getLipValores() + platoActual.getValoresPlato().getLipValores();
    float prot = ingAux.getValoresIng().getProtValores() + platoActual.getValoresPlato().getProtValores();
    float kcal = ingAux.getValoresIng().getKcalValores() + platoActual.getValoresPlato().getKcalValores();
    /*
    float carb = ingAux.getValoresIng().getCarbValores() + comidaActual.getValoresComida().getCarbValores();
    float lip = ingAux.getValoresIng().getLipValores() + comidaActual.getValoresComida().getLipValores();
    float prot = ingAux.getValoresIng().getProtValores() + comidaActual.getValoresComida().getProtValores();
    float kcal = ingAux.getValoresIng().getKcalValores() + comidaActual.getValoresComida().getKcalValores();
    */
    ValoresNutricionales auxVal(carb, lip, prot, kcal);
    ingAux.setValoresIng(auxVal);
    // ---------------
    
    printValores(ingAux.getValoresIng());              // 3 - Valores nutricionales
}


/*---------------------------------------------------------------------------------------------------------
   printStateAdded(): Información del STATE_Added
----------------------------------------------------------------------------------------------------------*/
void printStateAdded(){
    //simplePrint("Plato a\xF1""adido\n\n Retire el plato");
    //printEjemplosyGrupo();                          // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    
    tft.clearScreen(0);           // Limpiar
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(50, 120);                            // Posicion inicio texto => tft.getCursorY()
    tft.setTextColor(ROJO);                          // Color texto      
    tft.setTextScale(2);        
    String cad = "Comience nuevo plato";
    tft.println(cad);           // Imprimir texto
    
    printCentral();                                 // 2 - Estructura central
    printValores(platoActual.getValoresPlato());    // 3 - Valores nutricionales
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
}
*/

#endif
