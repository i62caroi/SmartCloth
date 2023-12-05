/**
 * @file SD_all.ino
 * @brief Este programa guarda la comida en un archivo .txt con palabras clave para enviar al ESP32 cuando haya wifi.
 * 
 * @author Irene Casares Rodríguez
 * @date 01/12/2023
 */

/*
    Este programa trabaja con todos los botones para hacer determinadas acciones cuando se pulse
    cada uno pero sin necesidad de usar la Máquina de Estados.

    Se utiliza el botón de COCINADO para generar un peso aleatorio y no tener que trabajar con la báscula.

    Se utiliza el botón de CRUDO para dar por finalizado el pesado de un alimento. Es lo mismo que hace SmartCloth
    cuando hay peso en la báscula por un alimento recién colocado y se pulsa botón de un nuevo grupo, se entiende
    que ya se ha terminado de pesar ese alimento.

*/



#include "SD_functions_ESP32.h" // incluye RTC.h
#include "Buttons.h"

#include "list_functions.h"
#include "lista.h"


/*-----------------------------------------------------------------------------
                          DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void doCosasMain(int buttonM);
/*-----------------------------------------------------------------------------*/



void setup()
{
    Serial.begin(115200);
    while (!Serial){};

    // Inicializar RTC
    setupRTC();
    delay(100);
     
    // Inicializar SD
    if (!setupSDcard()){
        Serial.println("Card failed, or not present");
        return;
    }
    delay(100);
    

    // ---------- BUTTONS PINS --------------- 
    //  -----   Grande    -----
    for (byte c = 0; c < countColumns; c++){
        pinMode(columnsPins[c], OUTPUT);
        digitalWrite(columnsPins[c], HIGH);
    }
    for (byte r = 0; r < countRows; r++){
        pinMode(rowsPins[r], INPUT);
    }

    //  -----   MAIN    -----
    pinMode(intPinAddPlato, INPUT);
    pinMode(intPinDeletePlato, INPUT);
    pinMode(intPinGuardar, INPUT);


    // --------- INTERRUPTIONS ----------------- 
    //  -----   MAIN    -----
    attachInterrupt(digitalPinToInterrupt(intPinAddPlato), ISR_addPlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinDeletePlato), ISR_deletePlato, RISING);
    attachInterrupt(digitalPinToInterrupt(intPinGuardar), ISR_guardar, RISING);

    attachInterrupt(digitalPinToInterrupt(intPinCocinado), ISR_cocinado, RISING); // Peso aleatorio

    attachInterrupt(digitalPinToInterrupt(intPinCrudo), ISR_crudo, RISING); // Terminar pesado alimento
    
    //  -----   Grande    -----
    attachInterrupt(digitalPinToInterrupt(interruptPinGrande), ISR_pulsandoButtonsGrande, RISING);



    // --------- FICHERO ESP32 ----------------- 
    // Borrar fichero esp32 antes de escribir porque al abrir la terminal se reinicia Arduino y vuelve a escribir
    // las lineas de INICIO-COMIDA e INICIO-PLATO
    borrarFileESP32();

    // Iniciar comida
    //iniciarComida();
    listaComidaESP32.iniciarComida();

    // Iniciar plato
    //iniciarPlato();
    //listaComidaESP32.iniciarPlato();

    // Mostrar lista
    //leerLista();
    listaComidaESP32.leerLista();

}




void loop() {
    
    // --- MAIN ---
    int buttonM = checkButtonsMain();
    if(buttonM != 0) doCosasMain(buttonM);


    // --- GRANDE ---
    if(checkButtonsGrande()){ // Si se ha cambiado de grupo
        Serial.print("Grupo Alimento: "); Serial.println(grupoAlimento);
    }
    

   delay(100);
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Función que realiza acciones dependiendo del botón pulsado en Main.
 * @param buttonM El ID del botón pulsado.
 */
/*-----------------------------------------------------------------------------*/
void doCosasMain(int buttonM)
{
    switch (buttonM) {
        case 1: // Crudo --> dar por terminado pesado aleatorio --> guardar alimento y peso
            //addAlimento();
            listaComidaESP32.addAlimento(grupoAlimento, pesoAlimento);
            Serial.println("\nTerminando pesado de alimento...\n");
            //leerLista();
            listaComidaESP32.leerLista();
            break;

        case 2: // Cocinado --> báscula peso aleatorio
            pesoAlimento = generarPeso();
            Serial.println("\nGenerando peso aleatorio...\n");
            break;

        case 3: // añadir
            Serial.println("\nAÑADIR");
            //iniciarPlato();
            //leerLista();
            listaComidaESP32.iniciarPlato();
            listaComidaESP32.leerLista();
            break;

        case 4: // borrar
            Serial.println("\nBORRAR");
            /*borrarLastPlato(); // Borrar último INICIO-PLATO y escribirlo de nuevo (true)
            leerLista();
            iniciarPlato();
            leerLista();*/
            listaComidaESP32.borrarLastPlato();
            listaComidaESP32.leerLista();
            break;

        case 5: // guardar
            Serial.println("\nGUARDAR");
            saveComidaSD(); // Escribe FIN-COMIDA en fileESP32. Antes borra INICIO-PLATO si es lo último escrito
            readFileESP32();
            break;
    }
}












