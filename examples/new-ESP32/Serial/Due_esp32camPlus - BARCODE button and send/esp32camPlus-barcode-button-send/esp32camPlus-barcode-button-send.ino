/**
 * @file esp32camPlus-barcode-button-send.ino
 * @brief Si recibe "GET-BARCODE" desde el Arduino Due, "activa" el lector.
 *        En realidad, el lector siempre está encendido y leyendo, pero el esp32 lo ignora hasta que 
 *        se le indique que atienda al código leído. 
 * 
 *        En cuanto lee un código de barras, envía una petición HTTP a la API de OpenFoodFacts con el código
 *        y recibe la información del producto.
 * 
 * @author Irene Casares Rodríguez
 * @date 09/05/2024
 *
 * Se ha utilizado el módulo ESP32-CAM PLUS:
 *    SPI Flash: default 32Mbit
 *    RAM: built-in 520 KB+external 4MPSRAM
 *    Bluetooth: Bluetooth 4.2 BR/EDR and BLE standards 
 *    Wi-Fi: 802.11 b/g/n de hasta 150 Mbps
 *    Support Interface: UART, SPI, I2C, PWM
 *    Camera: OV5640 autofocus
 *    Image Output Format: JXGA
 *    Power supply range: 5V
 *
 * @see ESP32-CAM PLUS (Aliexpress): https://acortar.link/Aubtwq
 *
 * Como lector de códigos de barras se ha utilizado el "Barcode Scanner Module (D)" de Waveshare:
 *    Resolución: 640x480
 *    Ángulos de Escaneo: Horizontal 70°, Vertical 55°, Rotación 360°
 *    Modos de Escaneo: Sensado, Continuo
 *    Precisión: Código 1D ≥ 5mil, Código 2D ≥ 10mil
 *    Interfaz de Comunicación: UART (9600 8N1 por defecto)
 *    Indicador LED: Azul (escaneo), Verde (decodificado exitoso)
 *    Alimentación: 3.3V, 120mA
 *    Protección: ESD (contacto 6 kV, aire ±12 kV), IP54 (frontal)
 *    Temperatura: Operación -30°C a +70°C, Almacenamiento -40°C a +80°C
 *    Humedad: 5%RH ~ 95%RH (no condensante)
 *
 * @see https://www.waveshare.com/wiki/Barcode_Scanner_Module_(D)
 * 
 * 
 */

/*

      ------------------------------------------------------
      |    ESP32-CAM PLUS        |  Arduino Due (Serial1)  |
      ------------------------------------------------------
      |    IO17 (Tx1) (4º izq)   |      pin 19 (Rx1)       |    
      |    IO16 (Rx1) (5º izq)   |      pin 18 (Tx1)       |
      |      3V3                 |        3V3              |
      |      GND                 |        GND              | 
      ------------------------------------------------------


      ------------------------------------------------------
      |    Lector Barcode     |      ESP32-CAM PLUS        |
      ------------------------------------------------------
      |      Rx (azul)        |    pin 21 (Tx software)    |
      |      Tx (naranja)     |    pin 19 (Rx software)    |    
      |      3V3 (marron)     |        3V3                 |
      |      GND (gris)       |        GND                 | 
      ------------------------------------------------------

      ¡¡¡¡¡ IMPORTANTE !!!!!
      USAR ARDUINO IDE. EN VSCODE NO FUNCIONA LA TERMINAL.
      
      ARDUINO BOARD CONFIGURATION:
      - Board: "ESP32 Dev Module"
      - Partition Scheme: "Huge APP (3MB No OTA/1MB SPIFFS)"
      - Flash Mode: "DIO"
      - Upload Speed: "460800"

      Con eso ya se puede subir el programa a la placa sin problemas y 
      sin tener que presionar el botón de reset ni GIO0.

  **********************************************************************

    -------- MENSAJES ARDUINO -> ESP32 --------------
        ----- COMPROBAR COMUNICACIÓN --
        1) Comprobar conexión:
            "PING"

        ----- CONEXIÓN A INTERNET -----
        2) Check wifi:
            "CHECK-WIFI"

        ----- INFO COMIDAS ------------
        3) Guardar info en base de datos al inicio:
            3.1. Indicar guardado:
                "SAVE"
            3.2. Mandar datos a guardar, línea a línea:
                "INICIO-COMIDA" "INICIO-PLATO" "ALIMENTO,9,54.35" ... --> sendFileToEsp32MealByMeal()

        ----- BARCODE ----------------
        4) Leer código de barras:
            "GET-BARCODE"



    -------- MENSAJES ESP32 -> ARDUINO -------------- 
        ----- COMPROBAR COMUNICACIÓN --
        1) Comprobación de conexión:
            "PONG"

        ----- CONEXIÓN A INTERNET -----
        2) Hay wifi:
            "WIFI-OK"

        3) No hay wifi:
            "NO-WIFI"
        
        ----- INFO COMIDAS ------------
        4) Esperando datos a subir:
            "WAITING-FOR-DATA"

        5) JSON guardado correctamente:
            "SAVED-OK"

        6) Error en el guardado de la comida (petición HTTP POST):
            "ERROR-HTTP:<codigo_error>"

        ----- BARCODE ----------------
            ----- LEER BARCODE -----
            7) Código de barras leído. Buscando información del producto:
                "BARCODE:<barcode>"
        
            8) No se ha podido leer el código de barras (no se ve bien o el lector no responde):
                "NO-BARCODE"
    
            ----- BUSCAR PRODUCTO -----
            9) Información del producto:
                "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>"
            
            10) No se ha encontrado el producto en OpenFoodFacts (error HTTP 404 not found):
                "NO-PRODUCT"

            11) Error al buscar producto (diferente a no encontrado):
                "ERROR-HTTP:<codigo_error>"

            12) El servidor de OpenFoodFacts no responde:
                "PRODUCT-TIMEOUT"

*/

/*
  [27/05/24 12:37] 
    El entorno de staging de OpenFoodFacts (https://world.openfoodfacts.net) está fallando, devuelve un error 500.
    En la documentación (https://openfoodfacts.github.io/openfoodfacts-server/api/) dicen que usemos el staging si no
    estamos en producción, pero como no funciona, vamos a usar el entorno de producción (https://world.openfoodfacts.org).
*/



#include "wifi_functions.h"
#include "Serial_functions.h" // Funciones de interación con Seriales ESP32-PC, ESP32-Due y ESP32-Barcode_Reader





void setup() 
{

    // --- COMUNICACIÓN SERIAL ---
    setupAllSerial();

    SerialPC.println("HOLA PC DESDE ESP32");
    //sendMsgToDue("HOLA DUE");
    // ---------------------------


    // --- CONEXIÓN WIFI ---------
    setupWiFi();
    // ---------------------------

}


void loop() 
{
    // ------- DUE --> ESP32 -----------------------------
    //if (SerialESP32Due.available() > 0) { // Due envió algo
    if(hayMsgFromDue()) 
    {
        String msgFromDue;
        readMsgFromSerialDue(msgFromDue);

        SerialPC.print("\nMensaje recibido: |"); SerialPC.print(msgFromDue); SerialPC.println("|\n");

        // ------ Comprobar comunicación Serial ----------
        if (msgFromDue == "PING") ackDue(); // Responder al Due (PONG) indicando comunicación establecida
        // -----------------------------------------------

        // ------ Intentar obtener Barcode ---------------
        else if (msgFromDue == "GET-BARCODE") tryGetBarcode(); // Si hay WiFi, leer barcode
        // -----------------------------------------------

        // ------ Comando no válido ----------------------
        else{
          SerialPC.println("Command not valid");
          sendMsgToDue(F("Command not valid"));
        }
        // -----------------------------------------------
    }
    // ---------------------------------------------------


    delay(200);

}
