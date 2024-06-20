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

#define SerialPC Serial
#define SerialESP32 Serial1

const int pinBarcode = 53;   

void showProductInfo(String &productInfo); // Muestra la información del producto

void setup() {
    SerialPC.begin(115200);
    while (!SerialPC);
    delay(100);

    SerialESP32.begin(115200);
    while (!SerialESP32);
    delay(100);

    SerialPC.println("Arduino Due");

    // ----- BARCODE  -----
    // Inicializar   
    pinMode(pinBarcode, INPUT_PULLUP); // Entrada con resistencia pull-up interna

    // Interruption   -----
    // FALLING porque su resistencia (la interna del Due) está modo pull-up, así que el botón se activa a la baja
    attachInterrupt(digitalPinToInterrupt(intPinBarcode), ISR_barcode, FALLING); // Interrupción en flanco de bajada
}

void loop() {
    // ----- ENVIAR MENSAJE AL PULSAR BOTÓN -----
    if (pulsandoBarcode) { // Si se pulsa el botón (interrupción)
        SerialPC.println("Pulsando boton barcode");
        SerialESP32.println("GET-BARCODE"); // Envía al ESP32-CAM

        pulsandoBarcode = false;
    }

  
    // ----- RECIBIR MENSAJE -----
    if (SerialESP32.available() > 0) {
        String msg = SerialESP32.readStringUntil('\n');
        msg.trim();

        SerialPC.println("\nMensaje recibido: " + msg); 
        SerialPC.println();
        String barcode;

        if(msg.startsWith("BARCODE:")) {
            barcode = msg.substring(8);
            SerialPC.println("Código de barras leído: " + barcode);
        }
        else if(msg.startsWith("PRODUCT:")) {
            showProductInfo(msg);
        }
        else if(msg.startsWith("NO-BARCODE")) {
            SerialPC.println("No se ha podido leer el código de barras");
        }
        else if(msg.startsWith("NO-PRODUCT")) {
            SerialPC.println("No se ha encontrado el producto " + barcode + " en OpenFoodFacts");
        }
        else if(msg.startsWith("PRODUCT-TIMEOUT")) {
            SerialPC.println("El servidor de OpenFoodFacts no responde");
        }
        else if(msg.startsWith("ERROR-HTTP:")) {
            String error = msg.substring(11);
            SerialPC.println("Error en el guardado (petición HTTP POST): " + error);
        }
        else if(msg.startsWith("NO-WIFI")) {
            SerialPC.println("No hay wifi");
        }
        else {
            SerialPC.println("Mensaje no reconocido: " + msg);
        }
    }

    delay(100);
}



/*-----------------------------------------------------------------------------*/
/**
 * Muestra la información del producto.
 * 
 * Esta función toma una cadena de información del producto y muestra los detalles del producto en el monitor serie.
 * 
 * @param productInfo La cadena de información del producto que contiene los detalles del producto.
 */
/*-----------------------------------------------------------------------------*/
void showProductInfo(String &productInfo) 
{
    String cad = productInfo.substring(8); // Elimina el prefijo "PRODUCT:"

    int idx_nombre = cad.indexOf(';');
    int idx_carb = cad.indexOf(';', idx_nombre + 1);
    int idx_lip = cad.indexOf(';', idx_carb + 1);
    int idx_prot = cad.indexOf(';', idx_lip + 1);
    int idx_kcal = cad.indexOf(';', idx_prot + 1);

    String barcode = cad.substring(0, idx_nombre);
    String nombre_producto = cad.substring(idx_nombre + 1, idx_carb);
    String carb_1g = cad.substring(idx_carb + 1, idx_lip);
    String lip_1g = cad.substring(idx_lip + 1, idx_prot);
    String prot_1g = cad.substring(idx_prot + 1, idx_kcal);
    String kcal_1g = cad.substring(idx_kcal + 1);

    SerialPC.println("Código: " + barcode);
    SerialPC.println("Nombre: " + nombre_producto);
    SerialPC.println("Carb_1g: " + carb_1g);
    SerialPC.println("Lip_1g: " + lip_1g);
    SerialPC.println("Prot_1g: " + prot_1g);
    SerialPC.println("Kcal_1g: " + kcal_1g);
}