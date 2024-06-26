/**
 * @file  esp32cam_v1.ino
 * @brief Programa principal del ESP32
 *
 * @author Irene Casares Rodríguez
 * @date 12/01/2024
 * @version 1.0
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
    **********************************************************************

      -------------------------------------------------
      |    ESP32-CAM PLUS   |  Arduino Due (Serial1)  |
      -------------------------------------------------
      |    IO17 (Tx1)       |      pin 19 (Rx1)       |    
      |    IO16 (Rx1)       |      pin 18 (Tx1)       |
      |      3V3            |        3V3              |
      |      GND            |        GND              | 
      -------------------------------------------------

      ------------------------------------------------------------
      |    ESP32-CAM PLUS              |    Barcode Reader       |
      ------------------------------------------------------------
      |  IO21 (Tx2-Software) (4º izq)  |  cable azul (Rx)        |    
      |  IO19 (Rx2-Software) (1º izq)  |  cable naranja (Tx)     |
      |     3V3                        |        3V3              |
      |     GND                        |        GND              | 
      ------------------------------------------------------------

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

    -----------------------------------------------------------------------------------
    1. Recibe "CHECK-WIFI" del Due
    2. Comprueba la conexión WiFi:
        2.1. Hay conexión, responde "WIFI"
        2.2. No hay conexión, responde "NO-WIFI"

    3. Si hay conexión, recibe "SAVE" del Due indicando a va a enviar data del TXT
    4. Responde al Due con "WAITING-FOR-DATA", quedando a la espera de ese data
    5. Cada línea recibida la procesa y modifica el JSON según su contenido
    6. Cuando recibe "FIN-COMIDA", cierra el JSON y lo envía al servidor.
    7. Repite pasos 5 y 6 hasta recibir "FIN-TRANSMISION"
    -----------------------------------------------------------------------------------

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


#include "json_functions.h" // incluye "wifi_functions.h"
#include "Serial_functions.h" // incluye debug.h



void setup() 
{
    // --- COMUNICACIÓN SERIAL ---
    setupAllSerial();
    // ---------------------------


    // --- CONEXIÓN WIFI ---------
    setupWiFi();
    // ---------------------------
  
}



void loop() 
{  
    if(hayMsgFromDue()) // Recibido algo del Due
    {
        // ------ Leer mensaje del Due ---------
        String msgFromDue;
        readMsgFromSerialDue(msgFromDue); // Leer mensaje del Serial

        #if defined(SM_DEBUG)
            SerialPC.println("\nMensaje recibido: " + msgFromDue); 
        #endif
        // -------------------------------------

        // ------ Comprobar comunicación Serial ----------
        //if (msgFromDue == "PING") ackDue(); // Responder al Due (PONG) indicando comunicación establecida
        // -----------------------------------------------

        // ------ Comprobar Wifi y avisar al Due ---------
        if (msgFromDue == "CHECK-WIFI") checkWiFi(); // Comprobar si hay WiFi y responder al Due (WIFI-OK o NO-WIFI)
        // -----------------------------------------------

        // ------ Procesar líneas y generar JSON ---------
        else if (msgFromDue == "SAVE") processJSON_OnePerMeal(); // Autentica esp32, recibe líneas y genera JSON por comida
        // -----------------------------------------------

        // ------ Intentar obtener Barcode ---------------
        else if (msgFromDue == "GET-BARCODE") tryGetBarcode(); // Si hay WiFi, leer barcode
        // -----------------------------------------------

        else{ 
            // Otras (p.ej. BARCODE)
            #if defined(SM_DEBUG)
                SerialPC.println(F("Comando desconocido"));
            #endif
        }
    }

}



