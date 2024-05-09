/*
-------------- MEMORIA SDRAM --------------------------------------------------------------------
  * Memoria SDRAM tipo W9812G6KH-6 [3]:
	  		- 2 M words x 4 banks x 16 bits (128 Mbits)
	  		- 166 MHz de frecuencia de reloj
	  		- modo auto-refresh
	  		- 64 ms de tiempo de refresh
	  		- CAS Latency: 2 y 3
	  		- Velocidad de acceso CL3 (3 ciclos de reloj)
        - Row address: A0-A11
	  		- Column address: A0-A8

  	  Esto se corresponde con los siguientes valores de Datasheet 8.1.2 SDRAM Connection del RA8876:
        Density: 128 Mb (4 banks) 
        Addressing => row: [A0-A11]
                   => column: [A0-A8]  
  -------------------------------------------------------------------------------------------------
*/


#include "COLORS.h"
#include "functions.h" // Files.h


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
    Serial.begin(115200);
    //while (!Serial);
    delay(1000);

    Serial.println("\nStarting up...");

    if(!setupSD()) Serial.println("SD card failure");
    else{
        Serial.println("SD card initialized");

        setupScreen();
        delay(100);
        
        Serial.println("Startup complete...");

        loadPicturesShowHourglass();
        arranque();

        Serial.println("Indique pantalla a mostrar: ");
    }

   // setupScreen();
    
    //Serial.println("Indique pantalla a mostrar: ");
    //showAccionRealizada(SAVE_EXECUTED_FULL);
}


//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void loop() {
    if (Serial.available() > 0 ) {
        int input = Serial.parseInt();
        Serial.flush(); // Clear the serial buffer

        Serial.println("Indique pantalla a mostrar: ");

        // Se ha modularizado y automatizado el movimiento de la mano y las pulsaciones de la imagen correspondiente:
        //    desplazar_mano()    sin_pulsacion()     con_pulsacion()
        // option: 1 (añadir)   2 (eliminar)    3 (guardar)

        switch(input){
            case 1:     showAccionRealizada(ADD_EXECUTED);                              break;
            case 2:     showAccionRealizada(DELETE_EXECUTED);                           break;
            case 3:     showAccionRealizada(SAVE_EXECUTED_FULL);                        break;
            case 4:     showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP);       break;
            case 5:     showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI);          break;
            case 6:     showAccionRealizada(SAVE_ESP32_TIMEOUT);                        break;
            case 7:     showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR);    break;
            case 8:     dashboard();                                                    break;
            case 9:     crudo_cocinado_sobre_dashboard();                               break; // sin transparencias ni alternancias, solo aparecer
            case 10:    pantalla_inicial();                                             break;
            case 11:    select_Grupo();                                                 break; // Con movimiento de mano y 2º pulsación. Se ha cambiado la mano por un icono nuevo con borde rojo y fondo verde
            case 12:    crudo_cocinado();                                               break; // Con 3º alternancia y aparición/desaparición paulatinas
            case 13:    colocar_alimento();                                             break; // Con aparición paulatina
            case 14:    add_Plato();                                                    break; // Con movimiento de mano y 2º pulsación. Se ha cambiado la mano por un icono nuevo con borde rojo y fondo blanco
            case 15:    delete_Plato();                                                 break; // Con movimiento de mano y 2º pulsación. Se ha cambiado la mano por un icono nuevo con borde rojo y fondo blanco
            case 16:    save_Comida(true);                                              break; // Guardar con WiFi (tb a database)
            case 17:    save_Comida(false);                                             break; // Guardar sin WiFi (solo local)
            case 18:    pedirConfirmacion_DELETE_CSV();                                 break;
            case 19:    suggestAction();                                                break;
            case 20:    showCriticFailureSD();                                          break;
            case 21:    showDataToUpload(DATA_TO_UPLOAD);                               break;  // Hay data, comprobando WiFi
            case 22:    showDataToUpload(NO_INTERNET_CONECTION);                        break;  // No hay WiFi
            case 23:    showDataToUpload(UPLOADING_DATA);                               break;  // Hay WiFi, subiendo data
            case 24:    showDataToUpload(UPLOADED_DATA);                                break;  // Se ha completado la subida
            case 25:    showDataToUpload(HTTP_ERROR);                                   break;  // Error al enviar la info
            case 26:    showError(ERROR_STATE_GROUP);                                   break;  // Error: se puso alimento sin crudo/cocinado 

            default:    Serial.println("Opción desconocida: " + String(input));         break;
        }
    }
}








