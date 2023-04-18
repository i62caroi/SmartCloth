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
#define   ROJO          0xFA02 //0xF920 | 0x8000 | 0xfc10 | 0xf800
#define   NARANJA       0xFC80
#define   AMARILLO      0xFFC0
#define   VERDE         0x07C0
#define   CIAN          0x07FF
#define   AZUL          0x019F
#define   ROSA          0xFA1F
#define   MORADO        0x9112
#define   MARRON        0xABC8


#define   MARGEN_IZQ       25
#define   MARGEN_IZQ_ACC   550 //Margen izquierdo del bloque de valores del acumulado de hoy


/* Screen */
RA8876 tft = RA8876(RA8876_CS, RA8876_RESET);

int ANCHO; // X (1024)
int ALTO; // Y (600)

String cad;

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
/*void simplePrint(String cad){
    tft.clearScreen(0); //0x0000 --> Negro
    tft.selectInternalFont(RA8876_FONT_SIZE_24); //RA8876_FONT_ENCODING_8859_1
    tft.setTextColor(BLANCO);
    tft.setCursor((ANCHO / 5), ALTO / 3); //(x,y)
    tft.print(cad);
}*/




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
    tft.setTextScale(2);
    tft.setTextColor(VERDE);
    tft.print("Alimento: "); 
    tft.setCursor(tft.getCursorX(), 217); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(pesoBascula); tft.println(" g\n");

    // ------------ PLATO  -------------------------
    tft.setCursor(410, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Plato: "); 
    tft.setCursor(tft.getCursorX(), 217); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(platoActual.getPesoPlato()); tft.println(" g\n");

    // ------------ TOTAL  -------------------------
    /* El peso total se actualiza al guardar la comida final */
    tft.setCursor(700, 210); 
    tft.selectInternalFont(RA8876_FONT_SIZE_24);
    tft.print("Total: "); 
    tft.setCursor(tft.getCursorX(), 217); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.print(diaActual.getPesoDiario()); tft.println(" g\n");
}



/***************************************************************************************************/
/*------------------------------- VALORES NUTRICIONALES   -----------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printValoresComida(): Muestra los valores nutricionales de la comida actual
----------------------------------------------------------------------------------------------------------*/
void printValoresComida(){
    tft.setCursor(MARGEN_IZQ, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextScale(2);
    tft.setTextColor(BLANCO);
    tft.println("Comida Actual\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    tft.setCursor(MARGEN_IZQ, 375);
    tft.print("Carb: "); tft.print(comidaActual.getValoresComida().getCarbValores()); tft.println(" g (X raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Lip: "); tft.print(comidaActual.getValoresComida().getLipValores()); tft.println(" g (X raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Prot: "); tft.print(comidaActual.getValoresComida().getProtValores()); tft.println(" g (X raciones)");
    
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
    
    tft.setCursor(MARGEN_IZQ, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextColor(BLANCO);
    tft.println("Comida Actual\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    tft.setCursor(MARGEN_IZQ, 375);
    tft.print("Carb: "); tft.print(carb); tft.println(" g (X raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Lip: "); tft.print(lip); tft.println(" g (X raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Prot: "); tft.print(prot); tft.println(" g (X raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(kcal); tft.println(" Kcal");
}


/*---------------------------------------------------------------------------------------------------------
   printValoresDiario(): Muestra los valores nutricionales del acumulado del día
----------------------------------------------------------------------------------------------------------*/
void printValoresDiario(){
    tft.setCursor(MARGEN_IZQ_ACC, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextScale(2);
    tft.setTextColor(BLANCO);
    tft.println("Acumulado Hoy\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    tft.setCursor(MARGEN_IZQ_ACC, 375);
    tft.print("Carb: "); tft.print(diaActual.getValoresDiario().getCarbValores()); tft.println(" g (X raciones)");
    
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    tft.print("Lip: "); tft.print(diaActual.getValoresDiario().getLipValores()); tft.println(" g (X raciones)");
    
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    tft.print("Prot: "); tft.print(diaActual.getValoresDiario().getProtValores()); tft.println(" g (X raciones)");
    
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(diaActual.getValoresDiario().getKcalValores()); tft.println(" Kcal");
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
    
    tft.clearScreen(0);                                // Limpiar
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto
    tft.setTextScale(2);        
    cad = "\xA1""BIENVENIDO/A\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CIAN);                                
    tft.setTextScale(2);        
    if(comidaActual.isComidaEmpty()) cad = "Coloque un recipiente sobre la b\xE1scula";
    else cad = "Coloque un recipiente sobre la b\xE1scula o guarde la comida";
    tft.println(cad);           
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
}


/*---------------------------------------------------------------------------------------------------------
   printStatePlato(): Información del STATE_Plato
----------------------------------------------------------------------------------------------------------*/
void printStatePlato(){
    
    tft.clearScreen(0);                                // Limpiar
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto
    tft.setTextScale(2);        
    cad = "\xA1""RECIPIENTE COLOCADO\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CIAN);                                
    tft.setTextScale(2);        
    if(comidaActual.isComidaEmpty()) cad = "Escoja grupo de alimentos";
    else cad = "Escoja grupo de alimentos o guarde la comida";
    tft.println(cad);           
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
}




/*---------------------------------------------------------------------------------------------------------
   printStateABandProcessed(): Incluye grupo de alimentos, ejemplos, procesamiento (crudo/cocinado)
                                y valores nutricionales de la comida actual
----------------------------------------------------------------------------------------------------------*/
void printStateABandProcessed(){
    printEjemplosyGrupo();        // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    printCentral();               // 2 - Estructura central (peso 0.0)
    printValoresComida();         // 3 - Valores comida actual
    printValoresDiario();         // 4 - Valores acumulado hoy
}




/*---------------------------------------------------------------------------------------------------------
   printStateWeighted():  Incluye grupo de alimentos, ejemplos, procesamiento (crudo/cocinado)
                          y valores nutricionales de la comida actual junto con lo pesado temporalmente
----------------------------------------------------------------------------------------------------------*/
void printStateWeighted(){
    printEjemplosyGrupo();         // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    printCentral();                // 2 - Estructura central
    printValoresTemporales();      // 3 - Valores temporales de comida actual 
    printValoresDiario();          // 4 - Valores acumulado hoy
}




/*---------------------------------------------------------------------------------------------------------
   printStateAdded(): Información del STATE_Added
----------------------------------------------------------------------------------------------------------*/
void printStateAdded(){
    //simplePrint("Plato a\xF1""adido\n\n Retire el plato");
    tft.clearScreen(0);
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto
    tft.setTextScale(2);        
    cad = "\xA1""PLATO GUARDADO\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CIAN);                                
    tft.setTextScale(2);        
    cad = "Retire el plato para comenzar otro o guarde la comida";
    tft.println(cad);  
        
    printCentral();                                   // 2 - Estructura central
    printValoresComida();                             // 3 - Valores comida actual
    printValoresDiario();                             // 4 - Valores acumulado hoy
}





/*---------------------------------------------------------------------------------------------------------
   printStateDeleted(): Información del STATE_Deleted
----------------------------------------------------------------------------------------------------------*/
void printStateDeleted(){
    //simplePrint("Plato eliminado\n\n Retire el plato");

    tft.clearScreen(0);
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto
    tft.setTextScale(2);        
    cad = "\xA1""PLATO ELIMINADO\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CIAN);                                
    tft.setTextScale(2);        
    cad = "Retire el plato eliminado para empezarlo de nuevo o guarde \n\tla comida";
    tft.println(cad); 
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
}




/*---------------------------------------------------------------------------------------------------------
   printStateSaved(): Información del STATE_Saved
----------------------------------------------------------------------------------------------------------*/
void printStateSaved(){
    //simplePrint("Comida guardada\n\n Retire el plato");

    tft.clearScreen(0);
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto
    tft.setTextScale(2);        
    cad = "\xA1""COMIDA GUARDADA\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CIAN);                                
    tft.setTextScale(2);        
    cad = "Retire el plato";
    tft.println(cad); 
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
}














/*-------------------------------------------------------------------------------------------------------*/
/*-------------------------------------- ERRORES POR ESTADO ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------------
   printEventError(): Información de error según estado
          Parámetros:
                msg - String => mensaje mostrado según el estado
----------------------------------------------------------------------------------------------------------*/
void printEventError(String msg){    
    cad = "\xA1""ACCI\xD3N INCORRECTA\x21""";
    
    tft.clearScreen(0);                                // Limpiar
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(ROJO);                        // Color texto
    tft.setTextScale(2);        
    tft.println(cad);                                  // Imprimir texto

    tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.setTextScale(1);        
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CIAN);                                
    tft.println(msg);           

    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
}




#endif
