/**
 * @file esp32camPlus-barcode-button-send.ino
 * @brief Si recibe "GET-BARCODE" desde el Arduino Due, "activa" el lector.
 *        En realidad, el lector siempre está encendido y leyendo, pero el esp32 lo ignora hasta que 
 *        se le indique que atienda al código leído. 
 * 
 *        En cuanto lee un código de barras, lo envía al Due para que compruebe si ya tenemos su info
 *        en la SD porque es un producto recurrente. Si no se encuentra la info del producto en la SD,
 *        el Due indica al ESP32 que lo busque en OpenFoodFacts, para lo que envía una petición HTTP a 
 *        su API de OpenFoodFacts con el código y recibe la información del producto, que reenvía al Due
 *        para que sepa el producto escogido y lo guarde en la SD para la próxima vez.
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

/*
  [27/05/24 12:37] 
    El entorno de staging de OpenFoodFacts (https://world.openfoodfacts.net) está fallando, devuelve un error 500.
    En la documentación (https://openfoodfacts.github.io/openfoodfacts-server/api/) dicen que usemos el staging si no
    estamos en producción, pero como no funciona, vamos a usar el entorno de producción (https://world.openfoodfacts.org).
*/



#include "wifi_functions.h"
#include "Serial_functions.h" // Funciones de interación con Seriales ESP32-PC, ESP32-Due y ESP32-Barcode_Reader
#include "barcode.h"




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

        // Se ha dividido en dos partes para que no se mezclen los mensajes:
        //      1) Leer código de barras cuando se reciba "GET-BARCODE"
        //      2) Buscar producto en OpenFoodFacts cuando se reciba "GET-PRODUCT:<barcode>"
        // Así no hay problemas de que al Due le llegue "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>" sin antes 
        // haber recibido "BARCODE:<barcode>", como ocurría a veces.

        // ------ Intentar obtener Barcode ---------------
        if (msgFromDue == "GET-BARCODE") getBarcode(); // Leer barcode
        // -----------------------------------------------

        // ------ Comprobar Wifi y avisar al Due ---------
        else if (msgFromDue == "CHECK-WIFI") checkWiFi(); // Comprobar si hay WiFi y responder al Due (WIFI-OK o NO-WIFI)
        // -----------------------------------------------


        // ------ Intentar buscar producto ---------------
        else if (msgFromDue.startsWith("GET-PRODUCT:"))
        { 
            String barcode = msgFromDue.substring(12);  // Extraer la parte de <barcode> de la cadena "GET-PRODUCT:<barcode>"
            getFoodData(barcode);                       // Buscar producto en OpenFoodFacts
        }
        // -----------------------------------------------

        else{ 
            // Otras (p.ej. BARCODE)
            SerialPC.println("Command not valid");
            sendMsgToDue(F("Command not valid"));
        }
    }
    // ---------------------------------------------------


    delay(200);

}
