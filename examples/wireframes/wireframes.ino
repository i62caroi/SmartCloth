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
            case 17:    pedirConfirmacion(ASK_CONFIRMATION_ADD);                        break;  // Pedir confirmación para añadir

            // ---------- ELIMINAR PLATO -------------------------------------------------------------
            case 18:    delete_Plato();                                                 break; // Con movimiento de mano y 2º pulsación. Se ha cambiado la mano por un icono nuevo con borde rojo y fondo blanco
            case 19:    pedirConfirmacion(ASK_CONFIRMATION_DELETE);                     break;  // Pedir confirmación para eliminar

            // ---------- GUARDAR COMIDA -------------------------------------------------------------
            case 20:    save_Comida(SAVING_WIFI);                                       break; // Guardar con WiFi (tb a database)
            case 21:    save_Comida(SAVING_ONLY_LOCAL);                                 break; // Guardar sin WiFi (solo local)
            case 22:    pedirConfirmacion(ASK_CONFIRMATION_SAVE);                       break;  // Pedir confirmación para guardar
            
            // ---------- ELIMINAR CSV ----------------------------------------------------------------
            case 23:    pedirConfirmacion_DELETE_CSV();                                 break;
        
            // ---------- SUGERIR ACCIÓN -------------------------------------------------------------
            case 24:    suggestAction();                                                break;
            
            // ---------- FALLO SD -------------------------------------------------------------------
            case 25:    showCriticFailureSD();                                          break;
            
            // ---------- SINCRONIZACION -------------------------------------------------------------
            //case 21:    showSyncState(DATA_TO_UPLOAD);                               break;  // Hay data, comprobando WiFi
            //case 22:    showSyncState(NO_INTERNET_CONNECTION);                        break;  // No hay WiFi
            case 26:    showSyncState(UPLOADING_DATA);                               break;  // Hay WiFi, subiendo data
            case 27:    showSyncState(ALL_MEALS_UPLOADED);                           break;  // Se ha completado la subida
            case 28:    showWarning(WARNING_MEALS_LEFT);                                   break;  // Se han quedado comidas sin guardar
            case 29:    showSyncState(ERROR_READING_TXT);                            break;  // Error al enviar la info
            case 30:    showSyncState(NO_INTERNET_CONNECTION);                       break;  // Error: se puso alimento sin crudo/cocinado
            case 31:    showSyncState(HTTP_ERROR);                                   break;  // Error al enviar la info
            case 32:    showSyncState(TIMEOUT);                                      break;  // Error al enviar la info
            case 33:    showSyncState(UNKNOWN_ERROR);                                break;  // Error al enviar la info

            // ---------- ERROR: SE PUSO ALIMENTO SIN CRUDO/COCINADO ---------------------------------
            case 34:    showError(ERROR_STATE_GROUP);                                      break;  // Error: se puso alimento sin crudo/cocinado 


            // ---------- BARCODE --------------------------------------------------------------------
            // --- ESCANEANDO ----------
            case 35:    scanningBarcode();                                                                  break;  // Escaneando código de barras  
            // --- BUSCANDO PRODUCTO ---
            case 36:    showSearchingProductInfo();                                                         break;  // Buscando información del producto
            
            // --- INFO PRODUCTO -------
            case 37:    showProductInfo_1("8437002353025", "Tortas de aceite", 0.6, 0.058, 0.23, 4.75);     break;  // Mostrar info del producto
            case 38:    showProductInfo_2("8437002353025", "Tortas de aceite", 0.6, 0.058, 0.23, 4.75);     break;
            case 39:    showProductInfo_3("8437002353025", "Tortas de aceite", 0.6, 0.058, 0.23, 4.75);     break;
            // --- AVISOS --------------
            case 40:    showWarning(WARNING_BARCODE_NOT_READ);                                              break;  // Código de barras no detectado
            case 41:    showWarning(WARNING_PRODUCT_NOT_FOUND, "1234567890123");                            break;  // Producto no encontrado
            case 42:    showWarning(WARNING_NO_INTERNET_NO_BARCODE);                                        break;  // No hay internet. No se puede buscar por código de barras
            case 43:    showWarning(WARNING_RAW_COOKED_NOT_NEEDED);                                         break;  // No es necesario indicar si es crudo o cocinado

            // ---------- GUARDANDO COMIDA -----------------------------------------------------------
            case 44:    showSavingMeal(SAVING_WIFI);                                                        break;
            case 45:    showSavingMeal(SAVING_ONLY_LOCAL);                                                  break;
            case 46:    showSavingMealBase(); completeSavingMeal(SAVING_ONLY_LOCAL);                        break;
            case 47:    showSavingMealBase(); completeSavingMeal(SAVING_WIFI);                              break;


            // ---------- NEW GUARDANDO COMIDA --------------------------------------------------------
            case 48:    showNewSavingMeal(SAVING_WIFI);                                                     break;
            case 49:    showNewSavingMeal(SAVING_ONLY_LOCAL);                                               break;
            case 50:    showNewSavingMeal_Complete(SAVING_WIFI);                                            break; // Primero dibujar la base de la pantalla y luego el mensaje de WiFi
            case 51:    showNewSavingMeal_Complete(SAVING_ONLY_LOCAL);                                      break; // Primero dibujar la base de la pantalla y luego el mensaje de no WiFi
            
            // ---------- NEW SINCRONIZAR -------------------------------------------------------------
            // --- SINCRONIZANDO -------
            case 52:    showSincronizandoSM();                                                              break;
            case 53:    showNewSyncState(UPLOADING_DATA);                                                   break; // Sincronizando SM
            // --- SM SINCRONIZADO -----
            case 54:    showSMSincronizado();                                                               break;
            case 55:    showNewSyncState(ALL_MEALS_UPLOADED);                                               break; // SM sincronizado
            // --- ERROR SINCRONIZANDO --
            case 56:    showNewSyncState(ERROR_READING_TXT);                                                break; // Error al enviar la info
            case 57:    showNewSyncState(NO_INTERNET_CONNECTION);                                           break; // No hay WiFi
            case 58:    showNewSyncState(HTTP_ERROR);                                                       break; // Error al enviar la info
            case 59:    showNewSyncState(TIMEOUT);                                                          break; // Error al enviar la info
            case 60:    showNewSyncState(UNKNOWN_ERROR);                                                    break; // Error al enviar la info

            // ---------- NEW BARCODE -----------------------------------------------------------------
            // --- ESCANEANDO ----------
            case 61:    showNewScanningBarcode();                                                           break;  // Escaneando código de barras
            // --- BUSCANDO PRODUCTO ---
            case 62:    showNewSearchingProduct();                                                          break;  // Buscando información del producto
            // -- PRODUCTO ENCONTRADO --
            case 63:    showNewProductFound();                                                              break; // Código de barras y nombre del producto leído
            case 64:    showNewProductFound("PRODUCT:8437002353025;Tortas de Andalucía;0.0;0.0;0.0;0.0");      break; // Info del producto leído

            case 65:    dashboard_barcode(); break;


            // Simulación de iconos con funciones gráficas
            /*case 45:    drawSmartClothDevice();                                                             break;
            case 46:    drawCenteredSmartClothDevice();                                                     break;

            case 47:    drawBigX(512, 300, 100, 45, 135, 5, RED);                                           break;
            case 48:    drawBigX(300, 400, 150, 45, 135, 10, RED);                                          break;

            case 49:    drawThickLineWithAngle_2(200, 300, 150, 0, 10, RED);                                break;
            case 50:    drawThickLineWithAngle_2(200, 300, 150, 90, 10, RED);                               break;
            */




            // --- PRODUCTO CANCELADO --
            case 66:    showProductoCancelado();                                                            break;  // Producto cancelado
            
            //case 67:    dashboard(DASH_GRUPOS);                                                             break;  // Dashboard de grupos y ejemplos
            //case 68:    dashboard(DASH_BARCODE);                                                            break;  // Dashboard de barcode




            default:    Serial.println("Opcion desconocida: " + String(input));                            break;
        }
    }
}








