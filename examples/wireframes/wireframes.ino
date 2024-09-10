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

        //tft.clearScreen(DARKORANGE);

        loadPicturesShowHourglass();
        //arranque();
        //arranqueSlowOpacity();

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
            // ---------- ACCION REALIZADA -----------------------------------------------------------
            case 1:     showAccionRealizada(ADD_EXECUTED);                              break;
            case 2:     showAccionRealizada(DELETE_EXECUTED);                           break;
            case 3:     showAccionRealizada(SAVE_EXECUTED_FULL);                        break;
            case 4:     showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_ERROR_HTTP);       break;
            case 5:     showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_NO_WIFI);          break;
            case 6:     showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_TIMEOUT);          break;
            case 7:     showAccionRealizada(SAVE_EXECUTED_ONLY_LOCAL_UNKNOWN_ERROR);    break;
            case 8:     showAccionRealizada(SAVE_EXECUTED_ONLY_DATABASE);               break;
            case 9:     showAccionRealizada(ERROR_SAVING_DATA);                         break;

            // ---------- DASHBOARD ------------------------------------------------------------------
            case 10:     dashboard();                                                   break;
            case 11:     crudo_cocinado_sobre_dashboard();                              break; // sin transparencias ni alternancias, solo aparecer
            
            // ---------- PEDIR PLATO ----------------------------------------------------------------
            case 12:    pantalla_inicial();                                             break;
            
            // ---------- PEDIR GRUPO ----------------------------------------------------------------
            case 13:    select_Grupo();                                                 break; // Con movimiento de mano y 2º pulsación. Se ha cambiado la mano por un icono nuevo con borde rojo y fondo verde
            
            // ---------- PEDIR CRUDO-COCINADO -------------------------------------------------------
            case 14:    crudo_cocinado();                                               break; // Con 3º alternancia y aparición/desaparición paulatinas
            
            // ---------- PEDIR ALIMENTO -------------------------------------------------------------
            case 15:    colocar_alimento();                                             break; // Con aparición paulatina
            
            // ---------- AÑADIR PLATO ---------------------------------------------------------------
            case 16:    add_Plato();                                                    break; // Con movimiento de mano y 2º pulsación. Se ha cambiado la mano por un icono nuevo con borde rojo y fondo blanco
            
            // ---------- ELIMINAR PLATO -------------------------------------------------------------
            case 17:    delete_Plato();                                                 break; // Con movimiento de mano y 2º pulsación. Se ha cambiado la mano por un icono nuevo con borde rojo y fondo blanco
            
            // ---------- GUARDAR COMIDA -------------------------------------------------------------
            case 18:    save_Comida(SAVING_WIFI);                                       break; // Guardar con WiFi (tb a database)
            case 19:    save_Comida(SAVING_ONLY_LOCAL);                                 break; // Guardar sin WiFi (solo local)
            
            // ---------- ELIMINAR CSV ----------------------------------------------------------------
            case 20:    pedirConfirmacion_DELETE_CSV();                                 break;
            
            // ---------- SUGERIR ACCIÓN -------------------------------------------------------------
            case 21:    suggestAction();                                                break;
            
            // ---------- FALLO SD -------------------------------------------------------------------
            case 22:    showCriticFailureSD();                                          break;
            
            // ---------- SINCRONIZACION -------------------------------------------------------------
            //case 21:    showSyncState(DATA_TO_UPLOAD);                               break;  // Hay data, comprobando WiFi
            //case 22:    showSyncState(NO_INTERNET_CONNECTION);                        break;  // No hay WiFi
            case 23:    showSyncState(UPLOADING_DATA);                               break;  // Hay WiFi, subiendo data
            case 24:    showSyncState(ALL_MEALS_UPLOADED);                           break;  // Se ha completado la subida
            case 25:    showWarning(WARNING_MEALS_LEFT);                                   break;  // Se han quedado comidas sin guardar
            case 26:    showSyncState(ERROR_READING_TXT);                            break;  // Error al enviar la info
            case 27:    showSyncState(NO_INTERNET_CONNECTION);                       break;  // Error: se puso alimento sin crudo/cocinado
            case 28:    showSyncState(HTTP_ERROR);                                   break;  // Error al enviar la info
            case 29:    showSyncState(TIMEOUT);                                      break;  // Error al enviar la info
            case 30:    showSyncState(UNKNOWN_ERROR);                                break;  // Error al enviar la info

            // ---------- ERROR: SE PUSO ALIMENTO SIN CRUDO/COCINADO ---------------------------------
            case 31:    showError(ERROR_STATE_GROUP);                                      break;  // Error: se puso alimento sin crudo/cocinado 


            // ---------- BARCODE --------------------------------------------------------------------
            // --- ESCANEANDO ----------
            case 32:    scanningBarcode();                                                                  break;  // Escaneando código de barras  
            // --- BUSCANDO PRODUCTO ---
            case 33:    showSearchingProductInfo();                                                         break;  // Buscando información del producto
            // --- INFO PRODUCTO -------
            case 34:    showProductInfo_1("8437002353025", "Tortas de aceite", 0.6, 0.058, 0.23, 4.75);     break;  // Mostrar info del producto
            case 35:    showProductInfo_2("8437002353025", "Tortas de aceite", 0.6, 0.058, 0.23, 4.75);     break;
            case 36:    showProductInfo_3("8437002353025", "Tortas de aceite", 0.6, 0.058, 0.23, 4.75);     break;
            // --- AVISOS --------------
            case 37:    showWarning(WARNING_BARCODE_NOT_READ);                                              break;  // Código de barras no detectado
            case 38:    showWarning(WARNING_PRODUCT_NOT_FOUND, "1234567890123");                            break;  // Producto no encontrado
            case 39:    showWarning(WARNING_NO_INTERNET_NO_BARCODE);                                        break;  // No hay internet. No se puede buscar por código de barras
            case 40:    showWarning(WARNING_RAW_COOKED_NOT_NEEDED);                                         break;  // No es necesario indicar si es crudo o cocinado

            // ---------- GUARDANDO COMIDA -----------------------------------------------------------
            case 41:    showSavingMeal(SAVING_WIFI);                                                        break;
            case 42:    showSavingMeal(SAVING_ONLY_LOCAL);                                                  break;
            case 43:    showSavingMealBase(); completeSavingMeal(SAVING_ONLY_LOCAL);                        break;
            case 44:    showSavingMealBase(); completeSavingMeal(SAVING_WIFI);                              break;


            // ---------- NEW GUARDANDO COMIDA --------------------------------------------------------
            case 45:    showNewSavingMeal(SAVING_WIFI);                                                     break;
            case 46:    showNewSavingMeal(SAVING_ONLY_LOCAL);                                               break;
            
            // ---------- NEW SINCRONIZAR -------------------------------------------------------------
            // --- SINCRONIZANDO -------
            case 47:    showSincronizandoSM();                                                              break;
            case 48:    showNewSyncState(UPLOADING_DATA);                                                   break; // Sincronizando SM
            // --- SM SINCRONIZADO -----
            case 49:    showSMSincronizado();                                                               break;
            case 50:    showNewSyncState(ALL_MEALS_UPLOADED);                                               break; // SM sincronizado

            // ---------- NEW BARCODE -----------------------------------------------------------------
            // --- ESCANEANDO ----------
            case 51:    showNewScanningBarcode();                                                           break;  // Escaneando código de barras
            // --- BUSCANDO PRODUCTO ---
            case 52:    showNewSearchingProduct();                                                          break;  // Buscando información del producto
            // -- PRODUCTO ENCONTRADO --
            case 53:    showNewProductFound();                                                              break; // Código de barras y nombre del producto leído




            // Simulación de iconos con funciones gráficas
            /*case 45:    drawSmartClothDevice();                                                             break;
            case 46:    drawCenteredSmartClothDevice();                                                     break;

            case 47:    drawBigX(512, 300, 100, 45, 135, 5, RED);                                           break;
            case 48:    drawBigX(300, 400, 150, 45, 135, 10, RED);                                          break;

            case 49:    drawThickLineWithAngle_2(200, 300, 150, 0, 10, RED);                                break;
            case 50:    drawThickLineWithAngle_2(200, 300, 150, 90, 10, RED);                               break;
            */




            default:    Serial.println("Opcion desconocida: " + String(input));                            break;
        }
    }
}








