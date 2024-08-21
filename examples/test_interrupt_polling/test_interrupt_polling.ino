#include "BUTTONS.h" // Polling e impresión de botoneras
#include "ISR.h"     // Funciones de interrupción

/* Time */
const unsigned long period = 50;
unsigned long prevMillis = 0;
unsigned long tiempoPrevio = 0;



/*---------------------------------------------------------------------------------------------------------
   setup(): Función para inicializar la pantalla, las botoneras y las interrupciones
----------------------------------------------------------------------------------------------------------*/
void setup() {
    Serial.begin(115200);
    delay(1000);
    while (!Serial);

     //setting up the screen
    //pinMode(RA8876_BACKLIGHT, OUTPUT);  // Set backlight pin to OUTPUT mode
    //digitalWrite(RA8876_BACKLIGHT, HIGH);  // Turn on backlight
    
    /*if (!tft.init()){
      Serial.println(F("Could not initialize RA8876"));
    }
    tft.clearScreen(0);
    tft.setTextScale(2);
    tft.selectInternalFont(RA8876_FONT_SIZE_32, RA8876_FONT_ENCODING_8859_1);*/
    Serial.println(F("Startup complete..."));

    /* INTERRUPTIONS */
    //  -----   MAIN    -----
    //attachInterrupt(digitalPinToInterrupt(interruptPinMain), pulsandoButtonsMain, RISING);
    //attachInterrupt(digitalPinToInterrupt(interruptPinMainFalling), keyReleasedMain, FALLING);
    //doIntMainRising = true;
    //doIntMainFalling = true;
    
    //  -----   Grande    -----
    attachInterrupt(digitalPinToInterrupt(interruptPinGrande), pulsandoButtonsGrande, RISING);
    //attachInterrupt(digitalPinToInterrupt(interruptPinGFalling), keyReleasedGrande, FALLING);
    doIntGrandeRising = true;
    //doIntGrandeFalling = true;*/
    

    /* BUTTONS PINS */
    //  -----   MAIN    -----
    /*for (byte i = 0; i < countButtons; i++){
        pinMode(buttonsMain[i], INPUT);  
    }*/
    //  -----   Grande    -----
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT);
        digitalWrite(columnsPins[c], HIGH);
    }
    for (byte r = 0; r < countRows; r++){
        pinMode(rowsPins[r], INPUT);
    }
    
    Serial.println(F("Pulsa una tecla"));
    
}



/*---------------------------------------------------------------------------------------------------------
   loop(): Función principal ejecutada continuamente
----------------------------------------------------------------------------------------------------------*/
void loop() {
    //printInit();
    
    if (millis() - prevMillis > period){
        prevMillis = millis();
        
        // ---------- TECLADO MAIN ----------------- 
        
       /*if (pulsandoMain){ // Se está pulsando una tecla 
            if (!msgEscritoMain){
                Serial.println(F("\n\nINTERRUPCION MAIN de tecla pulsada!!"));
                msgEscritoMain = true;
            }
            readButtonsMain(); // Qué tecla se está pulsando 
            Serial.print(F("TECLA PULSADA (MAIN): "));
            Serial.println(buttonsMainKeyboard[keyMain]);
            printButton(buttonsMainKeyboard[keyMain]);
            pulsandoMain = false;
            /*doIntMainRising = false;
            doIntMainFalling = true;*/
        //}
        /*else if(teclaSoltadaMain){ // Se ha soltado la tecla
            Serial.print(F("TECLA SOLTADA (MAIN): "));
            Serial.println(buttonsMainKeyboard[keyMain]);
            printButton(buttonsMainKeyboard[keyMain]);
            teclaSoltadaMain = false;
            doIntMainFalling = false;
            doIntMainRising = true;
        }*/


        // ---------- TECLADO GRANDE ----------------- 
        
        if (pulsandoGrande){ // Se está pulsando una tecla
            if (!msgEscritoGrande){
                Serial.println(F("\n\nINT grande de tecla pulsada!!"));
                msgEscritoGrande = true;
            }
            readButtonsGrande(); // Qué tecla se está pulsando 
            Serial.print(F("tecla pulsada (teclado grande): "));
            Serial.println(buttons[iRow][iCol]);
            //printButton(buttons[iRow][iCol]); 
            pulsandoGrande = false;
            /*doIntGrandeRising = false;
            doIntGrandeFalling = true;   */
        }
        /*else if(teclaSoltadaGrande){ // Se ha soltado la tecla
            Serial.print(F("tecla soltada (teclado grande): "));
            Serial.println(buttons[iRow][iCol]);
            printButton(buttons[iRow][iCol]);
            teclaSoltadaGrande = false;
            doIntGrandeFalling = false;
            doIntGrandeRising = true;
        }*/
        
    }
    
}
