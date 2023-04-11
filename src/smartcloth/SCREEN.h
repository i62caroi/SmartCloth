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


#define   MARGEN_IZQ    30


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
    tft.setCursor(MARGEN_IZQ, 40);                             // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto                  
    tft.println(grupoEscogido.Ejemplos_grupo);         // Imprimir texto

    /* ----- GRUPO ----- */
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(MARGEN_IZQ, 120);                            // Posicion inicio texto => tft.getCursorY()
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
    // ------------ ALIMENTO  -------------------------
    tft.setCursor(MARGEN_IZQ, 210); //tft.getCursorY()
    tft.selectInternalFont(RA8876_FONT_SIZE_24);  
    tft.setTextColor(VERDE);
    tft.print("Alimento: "); 
    tft.setCursor(tft.getCursorX(), 215); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(pesoBascula); tft.println(" g\n");

    // ------------ PLATO  -------------------------
    tft.setCursor(410, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Plato: "); 
    tft.setCursor(tft.getCursorX(), 215); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(platoActual.getPesoPlato()); tft.println(" g\n");

    // ------------ TOTAL  -------------------------
    /* El peso total se actualiza al guardar la comida final */
    tft.setCursor(700, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Total: "); 
    tft.setCursor(tft.getCursorX(), 215); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(diaActual.getPesoDiario()); tft.println(" g\n");

    tft.setCursor(MARGEN_IZQ, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextColor(BLANCO);
    tft.println("Comida Actual\n");
}



/***************************************************************************************************/
/*------------------------------- VALORES NUTRICIONALES   -----------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printValoresComida(): Muestra los valores nutricionales de la comida actual
----------------------------------------------------------------------------------------------------------*/
void printValoresComida(){
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(MARGEN_IZQ, 375);
    tft.setTextColor(BLANCO);
    
    tft.print("Carb: "); tft.print(comidaActual.getValoresComida().getCarbValores()); tft.println(" g");
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Lip: "); tft.print(comidaActual.getValoresComida().getLipValores()); tft.println(" g");
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Prot: "); tft.print(comidaActual.getValoresComida().getProtValores()); tft.println(" g");
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(comidaActual.getValoresComida().getKcalValores()); tft.println(" Kcal");
}


/*---------------------------------------------------------------------------------------------------------
   printValoresTemporales(): Muestra los valores nutricionales temporales causados por el cambio de peso
----------------------------------------------------------------------------------------------------------*/
void printValoresTemporales(){
    Ingrediente ingAux(grupoEscogido, pesoBascula);         // Ingrediente auxiliar usado para mostrar información variable de lo pesado
    
    float carb = ingAux.getValoresIng().getCarbValores() + comidaActual.getValoresComida().getCarbValores();
    float lip = ingAux.getValoresIng().getLipValores() + comidaActual.getValoresComida().getLipValores();
    float prot = ingAux.getValoresIng().getProtValores() + comidaActual.getValoresComida().getProtValores();
    float kcal = ingAux.getValoresIng().getKcalValores() + comidaActual.getValoresComida().getKcalValores();
    

    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(MARGEN_IZQ, 375);
    tft.setTextColor(BLANCO);
    
    tft.print("Carb: "); tft.print(carb); tft.println(" g");
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Lip: "); tft.print(lip); tft.println(" g");
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Prot: "); tft.print(prot); tft.println(" g");
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(kcal); tft.println(" Kcal");
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
    //simplePrint("BIENVENIDO\n\n           Escoja grupo de alimentos");
    String cad;
    
    tft.clearScreen(0);                                // Limpiar
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto
    tft.setTextScale(2);        
    cad = "\xA1""BIENVENIDO/A\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(ROJO);                                
    tft.setTextScale(2);        
    cad = "Escoja grupo de alimentos";
    tft.println(cad);           
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores nutricionales
}




/*---------------------------------------------------------------------------------------------------------
   printStateABandProcessed(): Incluye grupo de alimentos, ejemplos, procesamiento (crudo/cocinado)
                                y valores nutricionales de la comida actual
----------------------------------------------------------------------------------------------------------*/
void printStateABandProcessed(){
    printEjemplosyGrupo();        // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    printCentral();               // 2 - Estructura central (peso 0.0)
    printValoresComida();         // 3 - Valores nutricionales
}




/*---------------------------------------------------------------------------------------------------------
   printStateWeighted():  Incluye grupo de alimentos, ejemplos, procesamiento (crudo/cocinado)
                          y valores nutricionales de la comida actual junto con lo pesado temporalmente
----------------------------------------------------------------------------------------------------------*/
void printStateWeighted(){
    printEjemplosyGrupo();         // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    printCentral();                // 2 - Estructura central
    printValoresTemporales();      // 3 - Valores nutricionales
}




/*---------------------------------------------------------------------------------------------------------
   printStateAdded(): Información del STATE_Added
----------------------------------------------------------------------------------------------------------*/
void printStateAdded(){
    //simplePrint("Plato a\xF1""adido\n\n Retire el plato");
    
    tft.clearScreen(0);                               // Limpiar
    tft.selectInternalFont(RA8876_FONT_SIZE_24);      // Tamaño texto
    tft.setCursor(MARGEN_IZQ, 120);                           // Posicion inicio texto => tft.getCursorY()
    tft.setTextColor(ROJO);                           // Color texto      
    tft.setTextScale(2);        
    String cad = "Retire el plato actual y comience uno nuevo";
    tft.println(cad);                                 // Imprimir texto
    
    printCentral();                                   // 2 - Estructura central
    printValoresComida();                             // 3 - Valores nutricionales
}





/*---------------------------------------------------------------------------------------------------------
   printStateDeleted(): Información del STATE_Deleted
----------------------------------------------------------------------------------------------------------*/
void printStateDeleted(){
    //simplePrint("Plato eliminado\n\n Retire el plato");

    tft.clearScreen(0);                                // Limpiar
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(MARGEN_IZQ, 120);                            // Posicion inicio texto => tft.getCursorY()
    tft.setTextColor(ROJO);                            // Color texto      
    tft.setTextScale(2);        
    String cad = "Plato eliminado. Ret\xED""relo y comience de nuevo";
    tft.println(cad);                                  // Imprimir texto
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores nutricionales
}




/*---------------------------------------------------------------------------------------------------------
   printStateSaved(): Información del STATE_Saved
----------------------------------------------------------------------------------------------------------*/
void printStateSaved(){
    //simplePrint("Comida guardada\n\n Retire el plato");

    tft.clearScreen(0);                                // Limpiar
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(MARGEN_IZQ, 120);                            // Posicion inicio texto => tft.getCursorY()
    tft.setTextColor(ROJO);                            // Color texto      
    tft.setTextScale(2);        
    String cad = "Comida guardada";
    tft.println(cad);                                  // Imprimir texto
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores nutricionales
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
