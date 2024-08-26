#include "Buttons.h"       
#include "Screen.h"
#include "RTC.h"
#include "Scale.h"
#include "SD_functions.h"
#include "Serial_functions.h"
#include "ISR.h"


#define SerialPC Serial


/* Time */
const unsigned long   period = 50;
unsigned long         prevMillis = 0;
unsigned long         tiempoPrevio = 0;

bool initSD = false;
bool initScreen = false;

void setup()
{
    // ****** INICIALIZACIONES **************************************************
    // ------ COMUNICACIÓN SERIAL PC -----------
    setupSerialPC();
    SerialPC.println("\n\n***** INICIALIZANDO COMPONENTES *****\n\n");
    delay(500); 
    // -----------------------------------------

    // ------ PANTALLA --------------------------
    initScreen = setupScreen();
    if(initScreen){ 
        SerialPC.println("1) PANTALLA inicializada correctamente");
        tft.canvasImageStartAddress(PAGE1_START_ADDR); // Establecer la dirección de inicio de la imagen de la pantalla
        tft.clearScreen(BLUE); // Limpiar pantalla
        tft.selectInternalFont(RA8876_FONT_SIZE_24);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
        tft.setTextForegroundColor(WHITE);
        tft.setCursor(50,50);
        tft.println("1) PANTALLA INICIALIZADA");
    }
    else            SerialPC.println("1) Error al inicializar PANTALLA");
    delay(1000);
    // -----------------------------------------


    // ------ SD card --------------------------
    initSD = setupSDcard();
    if(initSD){  
        SerialPC.println("2) SD inicializada correctamente");
        if(initScreen){ tft.setCursor(50,100); tft.println("2) SD INICIALIZADA"); }
    }
    else{        
        SerialPC.println("2) Error al inicializar SD");
        if(initScreen){ tft.setCursor(50,100); tft.println("2) ERROR AL INICIALIZAR TARJETA SD"); }
    }
    delay(1000); 
    // -----------------------------------------


    // ------ RTC ------------------------------
    setupRTC(); 
    SerialPC.println("3) RTC configurado");
    if(initScreen){ tft.setCursor(50,150); tft.println("3) RTC INICIALIZADO"); }
    delay(1000); 
    // -----------------------------------------


    // ------ COMUNICACION SERIAL ESP32 --------
    setupSerialESP32();
    SerialPC.println("4) Comunicacion Serial con ESP32 configurada");
    if(initScreen){ tft.setCursor(50,200); tft.println("4) SERIAL CON ESP32 CONFIGURADA"); }
    delay(1000);
    // -----------------------------------------


    // ------ SCALE ----------------------------
    // ---- PINES ------------
    setupScale();   
    SerialPC.println("5.A) Pines de la BASCULA configurados");

    // ---- INTERRUPCIONES -------
    attachDueInterrupt(HW_TIMER_INTERVAL_MS * 1000, TimerHandler, "ITimer");
    ISR_Timer.setInterval(TIMER_INTERVAL_5MS,  ISR_pesarBascula); //timer llama a 'ISR_pesarBascula' cada 100 ms
    SerialPC.println("5.B) Interrupcion de la BASCULA configurada");

    if(initScreen){ tft.setCursor(50,250); tft.println("5) PINES E INTERRUPCION DE LA BASCULA CONFIGURADOS"); }

    delay(1000); 
    // -----------------------------------------


    // ------ BOTONES --------------------------
    // ---- PINES ------------
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT);
        digitalWrite(columnsPins[c], HIGH);
    }
    for (byte r = 0; r < countRows; r++){
        pinMode(rowsPins[r], INPUT);
    }
    pinMode(intPinCrudo, INPUT);
    pinMode(intPinCocinado, INPUT);
    pinMode(intPinAddPlato, INPUT);
    pinMode(intPinDeletePlato, INPUT);
    pinMode(intPinGuardar, INPUT);
    pinMode(intPinBarcode, INPUT_PULLUP); // Entrada con resistencia pull-up interna
    SerialPC.println("6.A) Pines de los BOTONES configurados");
    delay(200);

    // ---- INTERRUPCIONES -------
    attachInterrupt(digitalPinToInterrupt(intPinCrudo), ISR_crudo, RISING);     
    attachInterrupt(digitalPinToInterrupt(intPinCocinado), ISR_cocinado, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinAddPlato), ISR_addPlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinDeletePlato), ISR_deletePlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinGuardar), ISR_guardar, RISING);
    attachInterrupt(digitalPinToInterrupt(interruptPinGrande), ISR_pulsandoButtonsGrande, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinBarcode), ISR_barcode, FALLING); // Interrupción en flanco de bajada
    SerialPC.println("6.B) Interrupciones de los BOTONES configuradas");

    if(initScreen){ tft.setCursor(50,300); tft.println("6) PINES E INTERRUPCIONES DE LAS BOTONERAS CONFIGURADOS"); }

    delay(2000);
    // -----------------------------------------
    // ***************************************************************************
    // ***************************************************************************



    // ***************************************************************************
    // ****** COMPROBACIONES AUTOMÁTICAS *****************************************
    SerialPC.println("\n\n\n***** COMPROBANDO COMPONENTES *****\n\n");
    if(initScreen){
        tft.clearScreen(BLUE); 
        tft.setTextForegroundColor(WHITE);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setCursor(200,50); tft.println("COMPROBANDO COMPONENTES");
        tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
        delay(2000);
    }

    // ------ PANTALLA --------------------------
    if(initScreen){
        SerialPC.println("\n1) Comprobando lectura de SD y pantalla...");
        checkScreen(); // Dibuja pantalla con imagen de 'COCINADO'

        tft.clearScreen(BLUE); 
        tft.setTextForegroundColor(WHITE);
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setCursor(200,50); tft.println("COMPROBANDO COMPONENTES");
        tft.setTextScale(RA8876_TEXT_W_SCALE_X1, RA8876_TEXT_H_SCALE_X1); 
        if(initSD){ tft.setCursor(50,130); tft.println("1) PANTALLA OK. OBTENCION IMAGENES SD OK."); }
        else{ tft.setCursor(50,130); tft.println("1) PANTALLA OK. ERROR AL INICIALIZAR SD"); }
    }
    else SerialPC.println("\n1) No se puede comprobar pantalla porque fallo su inicializacion\n");
    delay(2000);
    // -----------------------------------------


    // ------ SD card --------------------------
    if(initSD){ 
        SerialPC.println("\n2) Comprobando escritura SD...");
        if(initScreen){tft.setCursor(50,180); tft.println("2) COMPROBANDO ESCRITURA Y LECTURA EN SD..."); }
        //checkSD(); // Escribe y lee un fichero en la SD
        checkSD_CSV(); // Escribe y lee un fichero CSV en la SD
    }
    else{ 
        SerialPC.println("\n2) No se puede comprobar SD porque fallo su inicializacion\n");
        if(initScreen){ tft.setCursor(50,180); tft.println("2) NO SE PUEDE COMPROBAR SD. FALLO SU INIT."); }
    }
    delay(2000);
    // -----------------------------------------

    // ------ RTC ------------------------------
    SerialPC.println("\n3) Comprobando RTC...");
    if(initScreen){tft.setCursor(50,300); tft.println("3) COMPROBANDO RTC..."); }
    checkRTC(); // Obtiene la fecha y hora del RTC
    delay(2000);
    // -----------------------------------------

    // ------ COMUNICACION ESP32 ---------------
    SerialPC.println("4) Comprobando comunicacion con ESP32...");
    if(initScreen){tft.setCursor(50,430); tft.println("4) COMPROBANDO COMUNICACION CON ESP32..."); }
    pingESP32();
    delay(2000);
    // -----------------------------------------

    

    

    
    // ***************************************************************************



    SerialPC.println("\n\n***** COMPROBACIONES MANUALES *****\n\n");
    if(initScreen){
        tft.clearScreen(BLUE); 
        tft.setTextForegroundColor(WHITE);        
        tft.setTextScale(RA8876_TEXT_W_SCALE_X2, RA8876_TEXT_H_SCALE_X2); 
        tft.setCursor(200,50); tft.println("COMPROBACIONES MANUALES"); 
    }


}


void loop()
{
    if (millis() - prevMillis > period){
        prevMillis = millis();

        checkAllButtons();  // Comprueba interrupción de botoneras 
        checkBascula();     // Comprueba interrupción de báscula 

    }

}