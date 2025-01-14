/*
  Irene Casares Rodríguez
  07/05/24

  Al pulsar el botón (Lector), el Due envía por Serial al ESP32-CAM el mensaje "GET-BARCODE"
  para que "active" el lector. 
  En realidad, el lector siempre está encendido, pero con el mensaje "GET-BARCODE" el esp32 se dispone
  a "escuchar" el código de barras. El lector está leyendo códigos de barras continuamente, pero se 
  ignoran hasta que se le indique al esp32 desde el Due que atienda al lector.


  MODIFICACION: El Due ya no espera dos mensajes seguidos, sino que pide leer barcode con "GET-BARCODE" y espera "BARCODE:<barcode>". 
  Después, indica al ESP32 que busque la información del producto con "GET-PRODUCT:<barcode>" y el ESP32 le responderá, si lo encuentra, 
  con "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>".

  Además, como hemos dividido el funcionamiento del barcode en varios estados, los mensajes más escalonados permiten buscar en la SD si el
  barcode ya se ha leído antes y, si es así, no buscarlo en OpenFoodFacts.

    1. Due --> ESP32: 
        "GET-BARCODE"
    2. ESP32 --> Due: 
        "BARCODE:<barcode>"
    3. Due busca en SD el barcode
    4.a. El barcode se leyó antes, así que usa como grupo.
    4.b. El barcode no está en SD, Due --> ESP32: 
            "GET-PRODUCT:<barcode>"
        5. ESP32 --> Due: 
            "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"


    BOTON BARCODE conectado de la siguiente forma (indistintamente):
        -----------------
        | GND | Barcode |
        -----------------
        | GND |   53    |       
        -----------------
    ¡IMPORTANTE! El GND junto al D53 en el screw shield no está conectado a nada. Usar alguno
    de los que están junto a 5V o 3.3V.



    -------- MENSAJES ARDUINO -> ESP32 --------------
        ----- CONEXIÓN A INTERNET -----
        1) Check wifi:
            "CHECK-WIFI"

        ----- BARCODE ----------------
        2) Leer código de barras:
            "GET-BARCODE"

        3) Buscar producto:
            "GET-PRODUCT:<barcode>"



    -------- MENSAJES ESP32 -> ARDUINO -------------- 
        ----- CONEXIÓN A INTERNET -----
        1) Hay wifi:
            "WIFI-OK"

        2) No hay wifi:
            "NO-WIFI"

        ----- BARCODE ----------------
            ----- LEER BARCODE -----
            3) Código de barras leído:
                "BARCODE:<barcode>"
        
            4) No se ha podido leer el código de barras (no se ve bien o el lector no responde):
                "NO-BARCODE"
    
            ----- BUSCAR PRODUCTO -----
            5) Información del producto:
                "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
            
            6) No se ha encontrado el producto en OpenFoodFacts (error HTTP 404 not found):
                "NO-PRODUCT"

            7) Error al buscar producto (diferente a no encontrado):
                "ERROR-HTTP:<codigo_error>"

            8) El servidor de OpenFoodFacts no responde:
                "PRODUCT-TIMEOUT"

*/

#include "ISR.h"
#include "Serial_functions.h"




// ------ CODIGO DE BARRAS -------------------------------------------------------------
// Global para que puedan acceder STATE_Grupo, STATE_Barcode, STATE_added y STATE_saved
String barcode = "";      // Código de barras leído 
String productInfo = "";  // Información del producto obtenida de la base de datos
// ------ FIN CODIGO DE BARRAS ---------------------------------------------------------



void setup() 
{
    SerialPC.println("\n\n++++++++++++++++++++++++++++++++++++++++++++++++++\n");

    // ------ COMUNICACIÓN SERIAL --------------
    setupSerialPC();
    // -----------------------------------------

    // ------ SD card --------------------------
    // Incluye crear fichero .csv, si no existe, y sumar en "Acumulado hoy" las comidas guardadas el día de hoy
    setupSDcard();
    delay(100); 
    // -----------------------------------------

    // ---------- BARCODE -----------------------
    // Pin interrupción
    pinMode(intPinBarcode, INPUT_PULLUP); // Entrada con resistencia pull-up interna
    //  Interrupción
    // FALLING porque su resistencia (la interna del Due) está modo pull-up, así que el botón se activa a la baja
    attachInterrupt(digitalPinToInterrupt(intPinBarcode), ISR_barcode, FALLING); // Interrupción en flanco de bajada
    // -----------------------------------------

    // ------ COMUNICACIÓN SERIAL --------------
    // Establecer el Serial del ESP32 al final para que le dé tiempo al ESP32 a encenderse, porque tarda más que el Due.
    // Si se hace al principio, el Due no puede configurar correctamente la comunicación Serial.
    setupSerialESP32();
    delay(100);
    // -----------------------------------------

    SerialPC.println("++++++++++++++++++++++++++++++++++++++++++++++++++\n");


}






void loop() 
{
    // ----- ENVIAR MENSAJE AL PULSAR BOTÓN -----
    if (barcodeButtonInterruptOccurred())  // Si se pulsa el botón (interrupción)
    {
        // Obtener barcode y esperar info del producto
        //getBarcodeAndProductInfo();

        // ----- LEER BARCODE -----------------------------
        byte resultFromReadingBarcode = askForBarcode(barcode);  // Pedir código de barras. Se va a quedar aquí hasta 10.5 segundos esperando respuesta del ESP32.

        switch(resultFromReadingBarcode)
        {
            // --- BARCODE LEÍDO -----------
            case BARCODE_READ: // Si se ha leído el barcode, buscar su información
                        SerialPC.println(F("\nBarcode leido, buscando su info..."));
                        // ----- PRODUCTO RECURRENTE --------
                        if(searchBarcodeInSD(barcode, productInfo)) // Buscar en SD si el barcode ya se ha leído antes
                        {
                            SerialPC.println(F("\nPRODUCTO RECURRENTE"));
                            SerialPC.println("\nInformacion del producto: " + productInfo);
                        }
                        // ----- FIN PRODUCTO RECURRENTE ----

                        // ----- PRODUCTO NUEVO -------------
                        else // Si no se ha encontrado el producto en SD, buscarlo en OpenFoodFacts
                        {
                            SerialPC.println(F("\nPRODUCTO NUEVO"));
                            SerialPC.println(F("\nBuscando info del producto en OpenFoodFacts...\n"));

                            // --- HAY INTERNET ---
                            if(checkWifiConnection()) // Hay WiFi
                            {
                                // Buscar info del producto en OpenFoodFacts. Se va a quedar aquí hasta 10.5 segundos esperando respuesta del ESP32.
                                byte resultFromGettingProductInfo = getProductInfo(barcode, productInfo); 

                                switch(resultFromGettingProductInfo)
                                {
                                    // --- PRODUCTO ENCONTRADO ----
                                    case PRODUCT_FOUND: // Si se ha encontrado el producto, se muestra su información
                                                SerialPC.println("\nInformacion del producto: " + productInfo);
                                                // Guardar producto en SD para la próxima lectura
                                                writeProductInfoInSD(productInfo);
                                                break;
                                    // -----------------------------

                                    // --- PRODUCTO NO ENCONTRADO ---
                                    case PRODUCT_NOT_FOUND: // Si no se ha encontrado el producto, se muestra el mensaje "No se ha encontrado el producto <barcode> en OpenFoodFacts"
                                                SerialPC.print("\nNo se ha encontrado el producto " + barcode); SerialPC.println(" en OpenFoodFacts");
                                                break;
                                    // -----------------------------

                                    // --- ERROR HTTP ---
                                    case HTTP_ERROR: // Si ha habido un error HTTP, se muestra el mensaje "Error al buscar info del producto (peticion HTTP GET): <error>"
                                                SerialPC.println("Error al buscar info del producto (peticion HTTP GET): " + productInfo);
                                                break;
                                    // -----------------------------

                                    // --- TIMEOUT ---
                                    case PRODUCT_TIMEOUT: // Si ha habido un timeout, se muestra el mensaje "El servidor de OpenFoodFacts no responde"
                                                SerialPC.println("\nEl servidor de OpenFoodFacts no responde");
                                                break;
                                    // -----------------------------

                                    // --- DESCONOCIDO ---
                                    case UNKNOWN_ERROR: // Si ha habido un error desconocido, se muestra el mensaje "Mensaje no reconocido: <msg>"
                                                SerialPC.println("Mensaje no reconocido: " + productInfo);
                                                break;
                                    // -----------------------------
                                }
                            }
                            // --- FIN HAY INTERNET --

                            // --- NO HAY INTERNET ---
                            else // Si el ESP32 está desconectado, no hay WiFi o TIMEOUT
                            {
                                SerialPC.println(F("No se procede a buscar producto porque ESP32 esta desconectado o no tiene WiFi. Producto no encontrado"));
                            }
                            // --- FIN NO HAY INTERNET ---
                            // ---- FIN CHEQUEO INTERNET --------------------
                        }
                        // ----- FIN PRODUCTO NUEVO ---------
                        break;          
            // -----------------------------

            // -- AVISO: BARCODE NO LEÍDO --
            case BARCODE_NOT_READ: // Si no se ha leído el barcode, se muestra el mensaje "Código de barras no detectado"
                        SerialPC.println(F("\nNo se ha detectado un barcode."));
                        break;
            // -----------------------------

            // -- INTERRUPCIÓN (CANCELAR) --
            // Para este programa de ejemplo vamos a ignorar la cancelación de la lectura de barcode 
            // ----------------------------

            // -- TIMEOUT O DESCONOCIDO ---
            default: // TIMEOUT o UNKNOWN_ERROR
                        SerialPC.println(F("\nTimeout o Desconocido, asumimos barcode no leido. "));
                        break;
            // ----------------------------
        }

            
        pulsandoBarcode = false; // Desactivar flag de pulsación
        // ----- FIN LEER BARCODE -------------------------
        
    }

  
    // ----- RECIBIR MENSAJE -----
    if (SerialESP32.available() > 0) {
        String msgFromESP32;
        readMsgFromSerialESP32(msgFromESP32);

        SerialPC.print("\nMensaje recibido: |"); SerialPC.print(msgFromESP32); SerialPC.println("|\n");
    }

    delay(100);
}





