
/* --------- CONEXIÓN TFT/LCD CON ARDUINO DUE -----------
 
  Pantalla modelo: ER-TFTM070-6 de BuyDisplay
      - SPI 7"TFT LCD Dislay 1024x600 OPTL Capacitive Touch Screen
      https://www.buydisplay.com/spi-7-inch-tft-lcd-dislay-module-1024x600-ra8876-optl-touch-screen-panel
  
  TFT conectada al bus SPI de la siguiente forma:
    (pines de conexión en el JP1 de la TFT)
      ---------------------------------------------
      |  SPI  |   TFT   |       Arduino Due       |
      ---------------------------------------------
      |  MOSI |  pin 7  |      pin 4 (SPI)        |    
      |  MISO |  pin 6  |      pin 1 (SPI)        |
      |  CLK  |  pin 8  |      pin 3 (SPI)        |
      |  CS   |  pin 5  |   pin 12 (digital PWM)  | 
      ---------------------------------------------

  Otras conexiones de la TFT al Arduino:
    (pines de conexión en el JP1 de la TFT)
      -------------------------------------------------
      |        TFT           |      Arduino Due       |
      -------------------------------------------------
      |  pin 11 (Reset)      |  pin 11 (digital PWM)  |
      |  pin 14 (Backlight)  |  pin 10 (digital PWM)  |
      -------------------------------------------------

  Alimentación y referencia a tierra de la TFT: 
    (pines de conexión en el JP1 de la TFT)
                      ---------------------------------
                      |           TFT                 |
      -------------------------------------------------
      |  VDD (3.3 V)  |  pines 3, 4, 37 y 38          |   
      |  GND          |  pines 1, 2, 13, 31, 39 y 40  |
      -------------------------------------------------

----------------------------------------------------------------
*/


#ifndef SCREEN_H
#define SCREEN_H


//#include "RA8876.h"
#include "RA8876_v2.h"
#include "State_Machine.h"
#include "Variables.h"
#include "icons.h"  // iconos de 'crudo', 'cocinado' y 'smartcloth_icono' como bitmap


/* Screen circuit wiring */
#define RA8876_CS        12 
#define RA8876_RESET     11
#define RA8876_BACKLIGHT 10 


// 8bpp (1 byte/pixel ) => RGB332
// 16bpp (2 bytes/pixel) Colores en formato RGB565 ==> http://www.barth-dev.de/online/rgb565-color-picker/
// 24bpp (3 bytes/pixel o 4 bytes/pixel) ==> RGB888

#define   NEGRO         0x0000
#define   BLANCO        0xFFFF
#define   ROJO          0xF800 //0xF920 | 0x8000 | 0xfc10 | 0xfa02
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




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void setupScreen();
//void simplePrint(String cad);
void Welcome();
void printEjemplosyGrupo();
void printCentral();
void printValoresComida();
void printValoresTemporales();
void printValoresDiario();
void printStateEmpty();
void printStatePlato();
void printStateGroups();
void printStateRaw();
void printStateCooked();
void printStateWeighted();
void printStateAdded();
void printStateDeleted();
void printStateSaved();
void printEventError(String msg);
void printEmptyObjectError(String msg);
/*-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------
   setupScreen(): Inicializar pantalla
----------------------------------------------------------------------------------------------------------*/
void setupScreen(){
    // initialize the bus for a device on pin 10 ==> Extended SPI Library Usage with the Arduino Due
                                                //   (https://docs.arduino.cc/tutorials/due/due-extended-spi)
    //SPI.begin(RA8876_CS);

    pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
    digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight
    if (!tft.init()){
      Serial.println(F("Could not initialize RA8876"));
      while(1);
    }
    ANCHO = tft.getWidth(); // X
    ALTO = tft.getHeight(); // Y
    Serial.println(F("\nScreen startup complete..."));
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
/*---------------------------- BIENVENIDA A SMARTCLOTH   ------------------------------------------*/
/***************************************************************************************************/
void Welcome(){
    tft.clearScreen(0); //0x0000 --> Negro
    
    // ---- Fichero BMP desde SD -----
    // TERRIBLEMENTE LENTOOOO
    //char file[30] = "bmp/images/logo3.bmp";
    //tft.sdCardDraw24bppBMP(file, 250, 150);
    
    // ---- Word en icons.h desde flash Arduino ------
    // MUCHISIMO MÁS RÁPIDO QUE BMP DESDE SD
    tft.putPicture_16bpp(250,150,500,350,smartcloth_icono);

    // ---- Word en icons.h desde flash Arduino ------
    //tft.drawArray16bpp(250,150,500,175000,smartcloth_icono); //SE ATASCA. TODO
    
    delay(2000);
}



/***************************************************************************************************/
/*------------------------- EJEMPLOS Y GRUPO DE ALIMENTOS   ---------------------------------------*/
/***************************************************************************************************/
/*---------------------------------------------------------------------------------------------------------
   printEjemplosyGrupo(): Muestra ejemplos del grupo de alimentos seleccionado
----------------------------------------------------------------------------------------------------------*/
void printEjemplosyGrupo(){    
    //tft.clearScreen(0);           // Limpiar
    
    /* ----- EJEMPLOS ----- */
    tft.setTextScale(2);
    tft.selectInternalFont(RA8876_FONT_SIZE_16);       // Tamaño texto
    tft.setCursor(MARGEN_IZQ, 40);                     // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto                  
    tft.println(grupoEscogido.Ejemplos_grupo);         // Imprimir texto

    /* ----- GRUPO ----- */
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(MARGEN_IZQ, 120);                    // Posicion inicio texto => tft.getCursorY()
    tft.setTextColor(BLANCO);                          // Color texto      
    tft.setTextScale(2);        
    tft.println(grupoEscogido.Nombre_grupo);           // Imprimir texto

    /* ----- CRUDO/COCINADO ----- */
    /*tft.setCursor(850, 130); 
    tft.selectInternalFont(RA8876_FONT_SIZE_16);                           
    tft.setTextColor(CIAN);      
    tft.print(procesamiento);                          // Predeterminadamente "CRUDO"
    */
    
    
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
    int raciones;
    
    tft.setCursor(MARGEN_IZQ, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextScale(2);
    tft.setTextColor(BLANCO);
    tft.println("Comida Actual\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    tft.setCursor(MARGEN_IZQ, 375);
    raciones = round(comidaActual.getValoresComida().getCarbValores()/10);
    tft.print("Carb: "); tft.print(comidaActual.getValoresComida().getCarbValores()); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    raciones = round(comidaActual.getValoresComida().getLipValores()/10);
    tft.print("Lip:  "); tft.print(comidaActual.getValoresComida().getLipValores()); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    raciones = round(comidaActual.getValoresComida().getProtValores()/10);
    tft.print("Prot: "); tft.print(comidaActual.getValoresComida().getProtValores()); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)");
    
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

    int raciones;
    
    tft.setCursor(MARGEN_IZQ, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextColor(BLANCO);
    tft.println("Comida Actual\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    tft.setCursor(MARGEN_IZQ, 375);
    raciones = round(carb/10);
    tft.print("Carb: "); tft.print(carb); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    raciones = round(lip/10);
    tft.print("Lip:  "); tft.print(lip); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    raciones = round(prot/10);
    tft.print("Prot: "); tft.print(prot); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)");
    
    tft.setCursor(MARGEN_IZQ, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(kcal); tft.println(" Kcal");
}


/*---------------------------------------------------------------------------------------------------------
   printValoresDiario(): Muestra los valores nutricionales del acumulado del día
----------------------------------------------------------------------------------------------------------*/
void printValoresDiario(){
    int raciones;
    
    tft.setCursor(MARGEN_IZQ_ACC, 300);
    tft.selectInternalFont(RA8876_FONT_SIZE_24); 
    tft.setTextScale(2);
    tft.setTextColor(BLANCO);
    tft.println("Acumulado Hoy\n");

    tft.selectInternalFont(RA8876_FONT_SIZE_16);

    tft.setCursor(MARGEN_IZQ_ACC, 375);
    raciones = round(diaActual.getValoresDiario().getCarbValores()/10);
    tft.print("Carb: "); tft.print(diaActual.getValoresDiario().getCarbValores()); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)");
    
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    raciones = round(diaActual.getValoresDiario().getLipValores()/10);
    tft.print("Lip:  "); tft.print(diaActual.getValoresDiario().getLipValores()); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)");
    
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    raciones = round(diaActual.getValoresDiario().getProtValores()/10);
    tft.print("Prot: "); tft.print(diaActual.getValoresDiario().getProtValores()); tft.print(" g ("); tft.print(raciones); tft.println(" raciones)");
    
    tft.setCursor(MARGEN_IZQ_ACC, tft.getCursorY()+10);
    tft.print("Kcal: "); tft.print(diaActual.getValoresDiario().getKcalValores()); tft.println(" Kcal");
}


/*********************************************************************************************************/
/*-------------------------------------------------------------------------------------------------------*/
/*---------------------------------- INFORMACIÓN POR ESTADO ---------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------*/
/*********************************************************************************************************/



/*---------------------------------------------------------------------------------------------------------
   printStateEmpty(): Información del STATE_Empty
----------------------------------------------------------------------------------------------------------*/
void printStateEmpty(){
    //simplePrint("Escoja grupo de alimentos");
    
    tft.clearScreen(0);                                // Limpiar

    // Ya no se necesita el 'BIENVENIDO' porque no es la primera pantalla
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto
    tft.setTextScale(2);        
    cad = "B\xC1""SCULA LIBRE";                        // BÁSCULA LIBRE
    tft.println(cad);                                  // Imprimir texto
    
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CIAN);                                
    tft.setTextScale(2);        
    if(comidaActual.isComidaEmpty()) cad = "Coloque un recipiente sobre la b\xE1""scula";
    else cad = "Coloque un recipiente sobre la b\xE1""scula o guarde la comida";
    tft.println(cad);           
    
    printCentral();                                    // 2 - Estructura central
    printValoresComida();                              // 3 - Valores comida actual
    printValoresDiario();                              // 4 - Valores acumulado hoy
}


/*---------------------------------------------------------------------------------------------------------
   printStatePlato(): Información del STATE_Plato
----------------------------------------------------------------------------------------------------------*/
void printStatePlato(){
    
   // tft.clearScreen(0);                                // Limpiar
    tft.clearArea(0,0,tft.getWidth(),180,0); //(xOrig, yOrig, xDest, yDest, color) ==> Limpiar zona superior de Ejemplos y Grupo
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto
    tft.setTextScale(2);        
    cad = "\xA1""RECIPIENTE COLOCADO\x21"""; // ¡BIENVENIDO/A!
    tft.println(cad);                                  // Imprimir texto
    
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CIAN);                                
    tft.setTextScale(2);        
    /*
    // Esto solo sirve si se permite guardar comida desde STATE_Plato,
    // pero no tendría mucho sentido que alguien pusiera un recipiente
    // y luego guardara la comida. Tendría que quitar el recipiente y luego,
    // estando de nuevo en STATE_Empty, guardar la comida.
    if(comidaActual.isComidaEmpty()) cad = "Escoja grupo de alimentos";
    else{
        tft.selectInternalFont(RA8876_FONT_SIZE_16);      
        cad = "Escoja grupo de alimentos o guarde la comida";
    }*/
    cad = "Escoja un grupo de alimentos";
    tft.println(cad);           
    
    // Esto no hace falta si no se borra toda la pantalla, solo la zona superior
    //printCentral();                                    // 2 - Estructura central
    //printValoresComida();                              // 3 - Valores comida actual
    //printValoresDiario();                              // 4 - Valores acumulado hoy
}




/*---------------------------------------------------------------------------------------------------------
   printStateGroups(): Incluye grupo de alimentos, ejemplos y valores nutricionales de la comida actual
----------------------------------------------------------------------------------------------------------*/
void printStateGroups(){
    tft.clearScreen(0);           // Limpiar ==> TODO => Ver qué ha cambiado (grupo y valores) y solo borrar esa zona
    printEjemplosyGrupo();        // 1 - Ejemplos, grupo y procesamiento (crudo/cocinado) 
    printCentral();               // 2 - Estructura central (peso 0.0)
    printValoresComida();         // 3 - Valores comida actual
    printValoresDiario();         // 4 - Valores acumulado hoy
}

/*---------------------------------------------------------------------------------------------------------
   printStateRaw():
----------------------------------------------------------------------------------------------------------*/
void printStateRaw()
{
    //tft.clearArea(800,100,895,891,0); //(xOrig, yOrig, xDest, yDest, color)
    tft.clearArea(0,0,tft.getWidth(),195,0); //(xOrig, yOrig, xDest, yDest, color) ==> Limpiar zona superior de Ejemplos y Grupo
    printEjemplosyGrupo();        // Grupo (crudo) => Esto solo es para cambiar el nombre del grupo, pues hay algunos que tienen "(crudo)"

    //tft.sdCardDraw24bppBMP(fileRaw, 800, 100);
    tft.putPicture_16bpp(800,95,95,91,crudo);
}

/*---------------------------------------------------------------------------------------------------------
   printStateCooked():
----------------------------------------------------------------------------------------------------------*/
void printStateCooked()
{
    //tft.clearArea(800,100,895,891,0); //(xOrig, yOrig, xDest, yDest, color)
    tft.clearArea(0,0,tft.getWidth(),195,0); //(xOrig, yOrig, xDest, yDest, color) ==> Limpiar zona superior de Ejemplos y Grupo
    printEjemplosyGrupo();        // Grupo (cocinado) => Esto solo es para cambiar el nombre del grupo, pues hay algunos que tienen "(cocinado)"

    //tft.sdCardDraw24bppBMP(fileCooked, 800, 100);
    tft.putPicture_16bpp(800,95,95,91,cocinado); 
}




/*---------------------------------------------------------------------------------------------------------
   printStateWeighted():  Incluye grupo de alimentos, ejemplos, procesamiento (crudo/cocinado)
                          y valores nutricionales de la comida actual junto con lo pesado temporalmente
----------------------------------------------------------------------------------------------------------*/
void printStateWeighted(){
    tft.clearScreen(0);           // Limpiar
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
    tft.selectInternalFont(RA8876_FONT_SIZE_16);       
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
    tft.selectInternalFont(RA8876_FONT_SIZE_16);       
    cad = "Retire el plato eliminado para empezarlo de nuevo o guarde la comida";
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
    //tft.setTextScale(2);  
    tft.selectInternalFont(RA8876_FONT_SIZE_16);       
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
    
    //tft.clearScreen(0);                                // Limpiar
    tft.clearArea(0,0,tft.getWidth(),195,0); //(xOrig, yOrig, xDest, yDest, color) ==> Limpiar zona superior de Ejemplos y Grupo
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(300, 50);                            // Posicion inicio texto
    tft.setTextColor(ROJO);                        // Color texto
    tft.setTextScale(2);        
    tft.println(cad);                                  // Imprimir texto

    //tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    //tft.setTextScale(1);        
    tft.setCursor(MARGEN_IZQ, 120);                             
    tft.setTextColor(CIAN);                                
    tft.println(msg);           

    //printCentral();                                    // 2 - Estructura central
    //printValoresComida();                              // 3 - Valores comida actual
    //printValoresDiario();                              // 4 - Valores acumulado hoy
}



/*---------------------------------------------------------------------------------------------------------
   printEmptyObjectError(): Información de aviso por intentar guardar/borrar plato o comida estando vacíos
          Parámetros:
                msg - String => mensaje mostrado según el estado
----------------------------------------------------------------------------------------------------------*/
void printEmptyObjectError(String msg){    
    cad = "\xA1""\xA1""AVISO\x21""\x21""";
    
    //tft.clearScreen(0);                                // Limpiar
    tft.clearArea(0,0,tft.getWidth(),195,0); //(xOrig, yOrig, xDest, yDest, color) ==> Limpiar zona superior de Ejemplos y Grupo
    
    tft.selectInternalFont(RA8876_FONT_SIZE_24);       // Tamaño texto
    tft.setCursor(350, 50);                            // Posicion inicio texto
    tft.setTextColor(AMARILLO);                        // Color texto
    tft.setTextScale(2);        
    tft.println(cad);                                  // Imprimir texto

    //tft.selectInternalFont(RA8876_FONT_SIZE_32);
    tft.selectInternalFont(RA8876_FONT_SIZE_16);
    //tft.setTextScale(1);        
    tft.setCursor(MARGEN_IZQ, 120);                             
    //tft.setTextColor(CIAN);                                
    tft.println(msg);           

    //printCentral();                                    // 2 - Estructura central
    //printValoresComida();                              // 3 - Valores comida actual
    //printValoresDiario();                              // 4 - Valores acumulado hoy
}





#endif
