/*
  Irene Casares Rodríguez
  07/05/24

  Al pulsar el botón (Lector), el Due envía por Serial al ESP32-CAM el mensaje "GEt-BARCODE"
  para que "active" el lector. 
  En realidad, el lector siempre está encendido, pero con el mensaje "GET-BARCODE" el esp32 se dispone
  a "escuchar" el código de barras. El lector está leyendo códigos de barras continuamente, pero se 
  ignoran hasta que se le indique al esp32 desde el Due que atienda al lector.

*/

/*
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

        ----- INFO COMIDAS ------------
        2) Guardar info en base de datos al inicio:
            2.1. Indicar guardado:
                "SAVE"
            2.2. Mandar datos a guardar, línea a línea:
                "INICIO-COMIDA" "INICIO-PLATO" "ALIMENTO,9,54.35" ... --> sendFileToEsp32MealByMeal()

        ----- BARCODE ----------------
        3) Leer código de barras:
            "GET-BARCODE"



    -------- MENSAJES ESP32 -> ARDUINO -------------- 
        ----- CONEXIÓN A INTERNET -----
        1) Hay wifi:
            "WIFI-OK"

        2) No hay wifi:
            "NO-WIFI"
        
        ----- INFO COMIDAS ------------
        3) Esperando datos a subir:
            "WAITING-FOR-DATA"

        4) JSON guardado correctamente:
            "SAVED-OK"

        5) Error en el guardado de la comida (petición HTTP POST):
            "ERROR-HTTP:<codigo_error>"

        ----- BARCODE ----------------
            ----- LEER BARCODE -----
            6) Código de barras leído. Buscando información del producto:
                "BARCODE:<barcode>"
        
            7) No se ha podido leer el código de barras (no se ve bien o el lector no responde):
                "NO-BARCODE"
    
            ----- BUSCAR PRODUCTO -----
            8) Información del producto:
                "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
            
            9) No se ha encontrado el producto en OpenFoodFacts (error HTTP 404 not found):
                "NO-PRODUCT"

            10) Error al buscar producto (diferente a no encontrado):
                "ERROR-HTTP:<codigo_error>"

            11) El servidor de OpenFoodFacts no responde:
                "PRODUCT-TIMEOUT"

*/

#include "ISR.h"
#include "Serial_functions.h"


void showProductInfo(String &productInfo); // Muestra la información del producto




void setup() {
    SerialPC.begin(115200);
    while (!SerialPC);
    delay(100);

    SerialESP32.begin(115200);
    while (!SerialESP32);
    delay(100);

    SerialPC.println("\nArduino Due\n");

    // ----- BARCODE  -----
    // Inicializar   
    pinMode(intPinBarcode, INPUT_PULLUP); // Entrada con resistencia pull-up interna

    // Interruption   -----
    // FALLING porque su resistencia (la interna del Due) está modo pull-up, así que el botón se activa a la baja
    attachInterrupt(digitalPinToInterrupt(intPinBarcode), ISR_barcode, FALLING); // Interrupción en flanco de bajada

    pingESP32(); // Comprobar comunicación con ESP32-CAM
}






void loop() 
{
    // ----- ENVIAR MENSAJE AL PULSAR BOTÓN -----
    if (pulsandoBarcode)  // Si se pulsa el botón (interrupción)
    {
        // Obtener barcode y esperar info del producto
        getBarcodeAndProductInfo();
        
        pulsandoBarcode = false;
    }

  
    // ----- RECIBIR MENSAJE -----
    if (SerialESP32.available() > 0) {
        String msgFromESP32;
        readMsgFromSerialESP32(msgFromESP32);

        SerialPC.print("\nMensaje recibido: |"); SerialPC.print(msgFromESP32); SerialPC.println("|\n");
    }

    delay(100);
}





