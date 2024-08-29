/**
 * @brief Activa el Serial entre el ESP32 y el lector para recibir el código de barras que lea el lector, 
 *        continuamente activo, y busca la información del producto en OpenFoodFacts con una petición HTTP POST 
 *        con el EAN del producto leído y la muestra en terminal.
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

*/

/*
  [27/05/24 12:37] 
    El entorno de staging de OpenFoodFacts (https://world.openfoodfacts.net) está fallando, devuelve un error 500.
    En la documentación (https://openfoodfacts.github.io/openfoodfacts-server/api/) dicen que usemos el staging si no
    estamos en producción, pero como no funciona, vamos a usar el entorno de producción (https://world.openfoodfacts.org).
*/



#include "Serial_functions.h"
#include "wifi_functions.h"

bool bufferCleaned = false; // Bandera para saber si se ha limpiado el buffer del lector de códigos de barras

void setup() 
{
    // --- COMUNICACIÓN SERIAL ---
    setupAllSerial(); // ESP32-PC y ESP32-BarcodeReader

    SerialPC.println("\nHOLA PC DESDE ESP32");
    // ---------------------------
    
    // --- CONEXIÓN WIFI ---------
    setupWiFi();  // Configurar módulo WiFi y conectar a red
    // ---------------------------

    
    // --- COMPROBAR FUNCIONES DE VALIDACION DE CODIGOS ------------------
    String barEAN13 = "9780201379624"; // EAN13 de un producto válido
    SerialPC.println("\nBarcode EAN-13: " + barEAN13);
    if(isValidEAN13(barEAN13)) SerialPC.println(" EAN13 válido");
    else SerialPC.println(" EAN13 NO válido");

    barEAN13 = "9780201379625"; // EAN13 de un producto no válido
    SerialPC.println("\nBarcode EAN-13: " + barEAN13);
    if(isValidEAN13(barEAN13)) SerialPC.println(" EAN13 válido");
    else SerialPC.println(" EAN13 NO válido");

    // -----

    String barUPCA = "725272730706"; // UPCA de un producto válido
    SerialPC.println("\n\nBarcode UPC-A: " + barUPCA);
    if(isValidUPCA(barUPCA)) SerialPC.println(" UPCA válido");
    else SerialPC.println(" UPCA NO válido");

    barUPCA = "725272730707"; // UPCA de un producto no válido
    SerialPC.println("\nBarcode UPC-A: " + barUPCA);
    if(isValidUPCA(barUPCA)) SerialPC.println(" UPCA válido");
    else SerialPC.println(" UPCA NO válido");

    // ----

    String barEAN8 = "90311017"; // EAN8 de un producto válido
    SerialPC.println("\n\nBarcode EAN-8: " + barEAN8);
    if(isValidEAN8(barEAN8)) SerialPC.println(" EAN8 válido");
    else SerialPC.println(" EAN8 NO válido");

    barEAN8 = "90311018"; // EAN8 de un producto no válido
    SerialPC.println("\nBarcode EAN-8: " + barEAN8);
    if(isValidEAN8(barEAN8)) SerialPC.println(" EAN8 válido");
    else SerialPC.println(" EAN8 NO válido");
    // -------------------------------------------------------------------
    
}


void loop() 
{
  /*
    // ----- LIMPIAR BUFFER ANTES DE LEER --------------
    // A veces, al leer el Serial se lee el código de barras anterior o dos códigos concatenados. 
    // Limpiar el buffer podría acabar eliminando el código de barras que se está leyendo ahora, 
    // pero eso es mejor que enviar datos extraños en la petición a OpenFoodFacts. Si se borra sin 
    // querer lo leído ahora, que el usuario lo vuelva a escanear.
    if(!bufferCleaned)
    {
        limpiarBufferBR();
        bufferCleaned = true;
    }
    // -------------------------------------------------


    // ----- PRODUCTO DETECTADO ------------------------
    if (hayMsgFromBR()) // Cámara leyó algo
    {
        // ---- OBTENER CÓDIGO DE BARRAS -----------
        String barcode;
        readMsgFromSerialBR(barcode); 
        SerialPC.println("\n\nBarcode: " + barcode); 
        // -----------------------------------------

        // ---- COMPROBAR VALIDEZ CODIGO -----------
        // Se comprueba el contenido del buffer por si se ha concatenado el código con residuos
        // de lecturas previas. Si es así, se toma el último código leído, validando si es 
        // un EAN-13, UPC-A o EAN-8 válido, los más comunes.
        
        SerialPC.println("Comprobando concatenación y validando código");
        if(extractAndValidateBarcode(barcode)) // Comprobar si en la parte final de lo leído en el buffer hay un código válido (EAN-13, UPC-A o EAN-8)
        {
            // ---- BUSCAR PRODUCTO EN OPENFOODFACTS ---
            getFoodData(barcode); // Obtener información del 'barcode' en OpenFoodFacts
            // -----------------------------------------
        }
        else
        {
            SerialPC.println("Código no válido");
        }
        // -----------------------------------------

    }
    // --------------------------------------------------

*/
    delay(200);

}












