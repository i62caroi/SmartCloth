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
                3.1) Cancelar leer código de barras (para que el ESP32 no siga esperándolo si se cancela la acción):
                    "CANCEL-BARCODE"

            4) Buscar producto:
                "GET-PRODUCT:<barcode>"



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


#include "json_functions.h" // incluye "wifi_functions.h"
#include "Serial_functions.h" // incluye debug.h
#include "barcode.h"



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

        // ------ Comprobar Wifi y avisar al Due ---------
        if (msgFromDue == "CHECK-WIFI") checkWiFi(); // Comprobar si hay WiFi y responder al Due (WIFI-OK o NO-WIFI)
        // -----------------------------------------------

        // ------ Procesar líneas y generar JSON ---------
        else if (msgFromDue == "SAVE") processJSON_OnePerMeal(); // Autentica esp32, recibe líneas y genera JSON por comida
        // -----------------------------------------------

        // Se ha dividido en dos partes para que no se mezclen los mensajes:
        //      1) Leer código de barras cuando se reciba "GET-BARCODE"
        //      2) Buscar producto en OpenFoodFacts cuando se reciba "GET-PRODUCT:<barcode>"
        // Así no hay problemas de que al Due le llegue "PRODUCT:<barcode>;<nombre_producto>;<carb_1g>;<lip_1g>;<prot_1g>;<kcal_1g>" sin antes haber recibido "BARCODE:<barcode>",
        // como ocurría a veces.

        // ------ Intentar obtener Barcode ---------------
        else if (msgFromDue == "GET-BARCODE") tryGetBarcode(); // Si hay WiFi, leer barcode
        // -----------------------------------------------

        // ------ Intentar buscar producto ---------------
        else if (msgFromDue.startsWith("GET-PRODUCT:")) tryGetProduct(msgFromDue); // Buscar producto en OpenFoodFacts
        // -----------------------------------------------

        else{ 
            // Otras (p.ej. BARCODE)
            #if defined(SM_DEBUG)
                SerialPC.println(F("Comando desconocido"));
            #endif
        }
    }

}



