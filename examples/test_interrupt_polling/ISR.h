#ifndef ISR_H
#define ISR_H

/* Interruptions */
/*  -----   MAIN  ----- */
const byte interruptPinMain = 31;                      // Pin de interrupcion RISING para Main
//const byte interruptPinMainFalling = 33;               // Pin de interrupcion FALLING para Main
volatile byte pulsandoMain = false;                    // Flag de estar pulsando algo en Main 
//volatile byte teclaSoltadaMain = false;                // Flag de haber soltado botón en Main
byte doIntMainRising;
// byte doIntMainFalling;       
byte msgEscritoMain = true;

/*  -----   GRANDE  ----- */
//const byte interruptPinGrande = 30;                    // Pin de interrupcion RISING para Grande
const byte interruptPinGrande = 38;
//const byte interruptPinGFalling = 32;                  // Pin de interrupcion FALING para Main
volatile byte pulsandoGrande = false;                  // Flag de estar pulsando algo en Grande
//volatile byte teclaSoltadaGrande = false;              // Flag de haber soltado botón en Grande
byte doIntGrandeRising;
// byte doIntGrandeFalling;   
byte msgEscritoGrande = true;



/*---------------------------------------------------------------------------------------------------------
   pulsandoButtonsMain(): Función ativada por interrupción RISING de botonera Main
----------------------------------------------------------------------------------------------------------*/
void pulsandoButtonsMain(){
    if(doIntMainRising){
        pulsandoMain = true;
        msgEscritoMain = false;
    }
}

/*---------------------------------------------------------------------------------------------------------
   pulsandoButtonsGrande(): Función ativada por interrupción RISING de botonera Grande
----------------------------------------------------------------------------------------------------------*/
void pulsandoButtonsGrande(){
    //if(doIntGrandeRising){
        pulsandoGrande = true;
        msgEscritoGrande = false;
    //}
}

/*---------------------------------------------------------------------------------------------------------
   keyReleasedMain(): Función ativada por interrupción FALLING de botonera Main
----------------------------------------------------------------------------------------------------------*/
/*void keyReleasedMain(){
    if(doIntMainFalling){
        teclaSoltadaMain = true;
    }
}*/

/*---------------------------------------------------------------------------------------------------------
   keyReleasedGrande(): Función ativada por interrupción FALLING de botonera Grande
----------------------------------------------------------------------------------------------------------*/
/*void keyReleasedGrande(){
    if(doIntGrandeFalling){
        teclaSoltadaGrande = true;
    }
}*/


#endif
