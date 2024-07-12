#include "Buttons.h"       
#include "Screen.h"
#include "RTC.h"
#include "Scale.h"
#include "SD_functions.h"
#include "Serial_functions.h"
#include "ISR.h"


bool initSD;
bool initScreen;

#define SerialPC Serial


/* Time */
const unsigned long   period = 50;
unsigned long         prevMillis = 0;
unsigned long         tiempoPrevio = 0;



void setup()
{
    // ****** INICIALIZACIONES **************************************************
    // ------ COMUNICACIÓN SERIAL PC -----------
    setupSerialPC();
    SerialPC.println("\n\n***** INICIALIZANDO COMPONENTES *****\n\n");
    delay(500); 
    // -----------------------------------------

    // ------ COMUNICACION SERIAL ESP32 --------
    setupSerialESP32();
    SerialPC.println("1) Comunicacion Serial con ESP32 configurada");
    delay(500);
    // -----------------------------------------

    // ------ RTC ------------------------------
    setupRTC(); 
    SerialPC.println("2) RTC configurado");
    delay(500); 
    // -----------------------------------------

    // ------ SD card --------------------------
    initSD = setupSDcard();
    if(initSD)  SerialPC.println("3) SD inicializada correctamente");
    else        SerialPC.println("3) Error al inicializar SD");
    delay(500); 
    // -----------------------------------------

    // ------ PANTALLA --------------------------
    initScreen = setupScreen();
    if(initScreen)  SerialPC.println("4) PANTALLA inicializada correctamente");
    else            SerialPC.println("4) Error al inicializar PANTALLA");
    delay(500);
    // -----------------------------------------

    // ------ SCALE ----------------------------
    // ---- PINES ------------
    setupScale();   
    SerialPC.println("5.A) Pines de la BASCULA configurados");

    // ---- INTERRUPCIONES -------
    attachDueInterrupt(HW_TIMER_INTERVAL_MS * 1000, TimerHandler, "ITimer");
    ISR_Timer.setInterval(TIMER_INTERVAL_5MS,  ISR_pesarBascula); //timer llama a 'ISR_pesarBascula' cada 100 ms
    SerialPC.println("5.B) Interrupcion de la BASCULA configurada");

    delay(500); 
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

    // ---- INTERRUPCIONES -------
    attachInterrupt(digitalPinToInterrupt(intPinCrudo), ISR_crudo, RISING);     
    attachInterrupt(digitalPinToInterrupt(intPinCocinado), ISR_cocinado, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinAddPlato), ISR_addPlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinDeletePlato), ISR_deletePlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinGuardar), ISR_guardar, RISING);
    attachInterrupt(digitalPinToInterrupt(interruptPinGrande), ISR_pulsandoButtonsGrande, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinBarcode), ISR_barcode, FALLING); // Interrupción en flanco de bajada
    SerialPC.println("6.B) Interrupciones de los BOTONES configuradas");

    delay(500);
    // -----------------------------------------
    // ***************************************************************************



    // ****** COMPROBACIONES AUTOMÁTICAS *****************************************
    SerialPC.println("\n\n\n***** COMPROBANDO COMPONENTES *****\n\n");

    // ------ COMUNICACION ESP32 ---------------
    SerialPC.println("1) Comprobando comunicacion con ESP32...");
    pingESP32();
    delay(1000);
    // -----------------------------------------

    // ------ RTC ------------------------------
    SerialPC.println("\n2) Comprobando RTC...");
    checkRTC(); // Obtiene la fecha y hora del RTC
    delay(1000);
    // -----------------------------------------

    // ------ SD card --------------------------
    if(initSD){ 
        SerialPC.println("\n3) Comprobando escritura SD...");
        checkSD(); // Escribe y lee un fichero en la SD
    }
    else SerialPC.println("\n3) No se puede comprobar SD porque fallo su inicializacion\n");
    delay(1000);
    // -----------------------------------------

    // ------ PANTALLA --------------------------
    if(initScreen){
        SerialPC.println("\n4) Comprobando lectura de SD y pantalla...");
        checkScreen(); // Dibuja pantalla con imagen de 'COCINADO'
    }
    else SerialPC.println("\n4) No se puede comprobar pantalla porque fallo su inicializacion\n");
    delay(1000);
    // -----------------------------------------
    // ***************************************************************************



    SerialPC.println("\n\n***** COMPROBACIONES MANUALES *****\n\n");


}


void loop()
{
    if (millis() - prevMillis > period){
        prevMillis = millis();

        checkAllButtons();  // Comprueba interrupción de botoneras 
        checkBascula();     // Comprueba interrupción de báscula 

    }

}