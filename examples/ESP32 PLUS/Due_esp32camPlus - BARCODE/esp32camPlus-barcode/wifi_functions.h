/**
 * @file wifi_functions.h
 * @brief Este archivo contiene funciones para enviar el JSON a la database.
 * 
 * @author Irene Casares Rodríguez
 * @date 13/01/2024
 */

#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> // Para serializar el JSON y enviarlo

#include "Serial_functions.h" // incluye debug.h


// ------- CREDENCIALES -------------
// Credenciales conexión red WiFi
// Laboratorio:
//const char* ssid = "UCOTEAM";
//const char* password = "-polonio210alfileres-";
// Casa:
//const char* ssid = "MOVISTAR_FB23_EXT";
//const char* password = "DP6BUuEtuFvRw3mHmFoG";
// Testeos:
const char* ssid = "Irene";
const char* password = "icradeba5050";

// Definitivo:
//const char* ssid = "SmartCloth";
//const char* password = "SM-pass24/";
// -----------------------------------



// ------- OPEN FOOD FACTS ----------
/*  [27/05/24 12:37] 
    El entorno de staging de OpenFoodFacts (https://world.openfoodfacts.net) está fallando, devuelve un error 500.

    [04/10/24 01:08] 
    El entorno de staging de OpenFoodFacts (https://world.openfoodfacts.net) vuelve a fallar, devuelve un error 502.

    En la documentación (https://openfoodfacts.github.io/openfoodfacts-server/api/) dicen que usemos el staging si no
    estamos en producción, pero como no funciona, vamos a usar el entorno de producción (https://world.openfoodfacts.org).
*/

// URL de la API de OpenFoodFacts
//const char* openFoodFacts_server = "https://world.openfoodfacts.net/api/v2/product/"; // Staging environment
const char* openFoodFacts_server = "https://world.openfoodfacts.org/api/v2/product/";   // Production environment
const char* openFoodFacts_fields = "?fields=product_name,product_name_es,carbohydrates_100g,energy-kcal_100g,fat_100g,proteins_100g"; // Campos requeridos

#define JSON_SIZE_LIMIT_BARCODE 512 // El JSON devuelto por OpenFoodFacts suele de ser de 200 bytes, pero ponemos 512 por segurarnos
// ----------------------------------




/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-----------------------------------------------------------------------------*/
void    setupWiFi();        // Configurar módulo WiFi y conectar a la red
void    connectToWiFi();    // Conectar a la red WiFi
void    checkWiFi();        // Comprobar si hay WiFi e indicarlo al Due
inline bool    hayConexionWiFi(){ return (WiFi.status()== WL_CONNECTED); };  // Comprobar si hay conexión a la red WiFi                                    // 3. Cerrar sesión

// Barcode
void    getFoodData(String barcode);                                    // Obtener los datos de un alimento
void    getProductInfo(const String &payload, String &productInfo);     // Obtener la información del producto a partir de un JSON
/*-----------------------------------------------------------------------------*/






/*-----------------------------------------------------------------------------*/
/**
 * @brief Configura el módulo WiFi del ESP32 y lo conecta a la red, si es posible
 */
/*-----------------------------------------------------------------------------*/
void setupWiFi()
{
    // --- CONFIGURAR MÓDULO WIFI ----
    // Configurar el módulo WiFi en modo estación (STA) para que pueda conectarse a una red WiFi
    // existente, pero no pueda aceptar conexiones entrantes como punto de acceso. Esto es necesario
    // para obtener la MAC correcta para este modo.
    WiFi.mode(WIFI_MODE_STA);
    // -------------------------------

    // --- CONECTAR A RED ------------
    // Intentar conectarse a la red WiFi
    connectToWiFi();
    // -------------------------------

    // --- OBTENER MAC ---------------
    // Mostrar identificador del esp32 (MAC)
    SerialPC.print("MAC: "); SerialPC.println(WiFi.macAddress());
    SerialPC.println();
    // -------------------------------
}





/*-----------------------------------------------------------------------------*/
/**
 * @brief Conecta el dispositivo a una red WiFi.
 * 
 * Esta función intenta conectar el dispositivo a una red WiFi utilizando las credenciales
 * proporcionadas. La conexión se intentará durante un máximo de 15 segundos. 
 * 
 */
/*-----------------------------------------------------------------------------*/
void connectToWiFi() 
{
    SerialPC.println();

    // Inicializar startTime
    unsigned long startTime = millis();

    // Esperar hasta que se establezca la conexión o se agote el tiempo
    unsigned long timeout_waitConexion = 15000; // 15 segundos

    SerialPC.print(F("Conectando a WiFi..."));
    WiFi.begin(ssid, password);

    // Mientras no se haya conectado a WiFi y mientras no hayan pasado 15 segundos.
    // Si se conecta o si pasan los 15 segundos, sale del while.
    while(!hayConexionWiFi() && !isTimeoutExceeded(startTime, timeout_waitConexion))
    {
        delay(500);
        SerialPC.print(F("."));
    }

    // Comprobar si se estableció la conexión
    if (hayConexionWiFi()) 
    {
        SerialPC.println();
        SerialPC.print(F("Conectado a red WiFi con IP: ")); SerialPC.println(WiFi.localIP());
        SerialPC.println();
    } 
    else 
    {
        // Si tras 15 segundos no se ha establecido la conexión:
        SerialPC.println(F("\nNo se pudo establecer la conexion WiFi."));
    }

}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Verifica si hay conexión WiFi y envía un mensaje al dispositivo ESP32 Due.
 * 
 * Esta función verifica si hay una conexión WiFi disponible. Si hay conexión, 
 * se envía un mensaje al Due indicando que hay WiFi. Si no hay conexión, se
 * envía un mensaje indicando que no hay WiFi.
 */
/*-----------------------------------------------------------------------------*/
void checkWiFi()
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    //limpiarBufferDue();
    // ---------------------------------------------------------

    if(hayConexionWiFi())
    {
        SerialPC.println(F("\nTengo Wifi"));

        sendMsgToDue("WIFI-OK");
    }
    else 
    { 
        SerialPC.println(F("\nNo tengo wifi"));

        sendMsgToDue("NO-WIFI");
    }
}







/*-----------------------------------------------------------------------------*/
/**
 * @brief Obtiene los datos de un alimento a través de un código de barras.
 * 
 * Esta función realiza una solicitud HTTP a la API de Open Food Facts para obtener los datos de un alimento
 * utilizando el código de barras proporcionado. Los datos obtenidos incluyen el nombre del producto, los
 * carbohidratos por cada 100 gramos, la energía en kilocalorías por cada 100 gramos, la grasa por cada 100 gramos
 * y las proteínas por cada 100 gramos.
 *
 * [27/05/24 12:37] El entorno de staging de OpenFoodFacts (https://world.openfoodfacts.net) está fallando, devuelve un error 500.
 *  En la documentación (https://openfoodfacts.github.io/openfoodfacts-server/api/) dicen que usemos el staging si no
 *  estamos en producción, pero como no funciona, vamos a usar el entorno de producción (https://world.openfoodfacts.org).
 *
 *  Ejemplo: https://world.openfoodfacts.org/api/v2/product/3017624010701?fields=product_name,carbohydrates_100g,energy-kcal_100g,fat_100g,proteins_100g
 * 
 * @param barcode El código de barras del alimento.
 * 
 * @note No comprueba conexión a Internet porque se supone que se ha comprobado antes de llamar a esta función.
 */
/*-----------------------------------------------------------------------------*/
void getFoodData(String barcode) 
{
    SerialPC.println("\nObteniendo información del producto " + barcode);
    
    // --- CONFIGURAR PETICIÓN HTTP ---
    // Configurar la petición HTTP: un GET a la URL de la API de OpenFoodFacts con el código de barras y los campos que se quieren obtener
    HTTPClient http;  
    String serverPath = openFoodFacts_server + barcode + openFoodFacts_fields; // Conformar URL de la API
    // Por ejemplo, para tostas de trigo:
    // "https://world.openfoodfacts.org/api/v2/product/5601560111905?fields=product_name,product_name_es,carbohydrates_100g,energy-kcal_100g,fat_100g,proteins_100g"

    http.begin(serverPath.c_str());  // Inicializar URL de la API. Se hace c_str() para obtener un const char* en lugar de un String
    http.setTimeout(10000);          // Establecer 10 segundos de espera para la respuesta del servidor OpenFoodFacts

    // En las operaciones de lectura (obtener info de un producto) solo hace falta el User-Agent customizado
    // En las operaciones de escritura hace falta más autenticación (credenciales), pero no nos afecta porque 
    // no vamos a modificar productos.
    //http.setUserAgent("SmartCloth/1.0 (irene.casares@imibic.org)"); // Establece el User-Agent personalizado
    http.setUserAgent("SmartCloth/1.0 (i62caroi@uco.es)"); // Establece el User-Agent personalizado
    // --------------------------------

    // --- ENVIAR PETICIÓN HTTP -------
    // Enviar la petición HTTP
    int httpResponseCode = http.GET();  // Método de petición HTTP
    // --------------------------------

    // --- PROCESAR RESPUESTA HTTP -----------------------
    // Comprobar el código de respuesta HTTP
    // ------ SOLICITUD EXITOSA -----------------
    if(httpResponseCode > 0)
    {
        // --- PRODUCTO ENCONTRADO --------------
        if((httpResponseCode >= HTTP_CODE_OK) && (httpResponseCode < HTTP_CODE_MULTIPLE_CHOICES)) // Se encontró la info del producto. Código [200, 300)
        {
            // --- OBTENER INFO DEL PRODUCTO ---
            String payload = http.getString();  // Obtener la respuesta del servidor de OpenFoodFacts
            String productInfo;
            getProductInfo(payload, productInfo); // Obtener la información del producto del 'payload' y almacenarla en 'productInfo' con la estuctura adecuada:
                                                    //   "PRODUCT:barcode;nombreProducto;carb_1g;lip_1g;prot_1g;kcal_1g"
            // --------------------------------

            // --- ENVIAR INFO AL DUE ---------
            SerialPC.print("Enviando info del producto al Due: "); SerialPC.print("\"" + productInfo); SerialPC.println("\"");
            sendMsgToDue(productInfo); // Envía "PRODUCT:barcode;nombreProducto;carb_1g;lip_1g;prot_1g;kcal_1g" al Due
            // --------------------------------
        }
        // --------------------------------------
        // --- PRODUCTO NO ENCONTRADO -----------
        else // No se encontró la info del producto porque no está en database o por error en la petición
        {
            if(httpResponseCode == HTTP_CODE_NOT_FOUND) // Código 404 (Not Found)
            {
                // Si no se encuentra el producto, se obtiene código 404 (Not Found) y un JSON como este:
                // { "code": "8422114932702", "status": 0, "status_verbose": "product not found" }
                SerialPC.println("Producto no encontrado.");

                // -- RESPUESTA AL DUE ---
                sendMsgToDue("NO-PRODUCT");
                // -----------------------
            }
            else 
            {
                SerialPC.println("Error en la solicitud: " + httpResponseCode);

                // -- RESPUESTA AL DUE ---
                sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode)); // Error en la petición HTTP
                // -----------------------
            }
        }
        // --------------------------------------
    }
    // ---- FIN SOLICITUD EXITOSA ---------------

    // ------ TIMEOUT DE OPENFOODFACTS ----------
    //else if(httpResponseCode == -1) 
    else if(httpResponseCode == HTTPC_ERROR_READ_TIMEOUT) // Tiempo de espera agotado (código -11)
    {
        SerialPC.println("Tiempo de espera agotado. OpenFoodFacts no responde.");

        // -- RESPUESTA AL DUE ---
        sendMsgToDue("PRODUCT-TIMEOUT");
        // -----------------------
    }
    // ---- FIN TIMEOUT DE OPENFOODFACTS --------

    // ------- ERROR EN SOLICITUD ---------------
    else 
    { // Creo que nunca llegaría a esta condición
        SerialPC.println("Error en la solicitud: " + httpResponseCode);

        // -- RESPUESTA AL DUE ---
        sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode)); // Error en la petición HTTP
        // -----------------------
    }
    // ---- FIN ERROR EN SOLICITUD ---------------
    // ---- FIN PROCESAR SOLICITUD HTTP ------------------


    // --- CERRAR CONEXIÓN HTTP --------------------------
    http.end();   //Cierra la conexión
    // ---------------------------------------------------
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Obtiene la información del producto a partir de un JSON recibido de OpenFoodFacts y la construye en una cadena de texto.
 * 
 * Esta función analiza un JSON proporcionado en el payload para extraer información del producto,
 * incluyendo el código de barras, nombre del producto, y macronutrientes (carbohidratos, lípidos, proteínas y kilocalorías).
 * La información extraída se almacena en la referencia de cadena de texto productInfo.
 * 
 * @param payload El JSON en formato de cadena de texto que contiene la información del producto.
 * @param productInfo Referencia a una cadena de texto donde se almacenará la información del producto construida.
 * 
 * @note No comprueba conexión a Internet porque se supone que se ha comprobado antes de llamar a esta función.
 */
/*-----------------------------------------------------------------------------*/
void getProductInfo(const String &payload, String &productInfo)
{
    // ---- OBTENER INFO DEL JSON -----------------------------------
    // Crear un objeto DynamicJsonDocument para almacenar el JSON analizado
    DynamicJsonDocument doc(JSON_SIZE_LIMIT_BARCODE); // 512 bytes
    deserializeJson(doc, payload);
    // --------------------------------------------------------------

    // ---- OBTENER DATOS DEL PRODUCTO ------------------------------
    // --- BARCODE -----------
    // Código de barras
    String barcode = doc["code"].as<String>();
    SerialPC.println("\n\nBarcode: " + barcode);
    // -----------------------

    // --- NOMBRE PRODUCTO ---
    // Preferencia del nombre en español frente al general (seguramente en inglés)
    String nombreProducto;
    if (doc["product"]["product_name_es"]) nombreProducto = doc["product"]["product_name_es"].as<String>();
    else if (doc["product"]["product_name"]) nombreProducto = doc["product"]["product_name"].as<String>();
    SerialPC.println("\nNombre: " + nombreProducto);
    // ----------------------

    // ------ MACRONUTRIENTES -------
    // Hay productos sin marconutrientes (como el agua), así que si no se obtienen esos valores, se asumen 0.0
    // Si se obtienen valores 0.0, entonces los valores por 1gr también son 0.0. Si no, se dividen entre 100.0
    
    // --- CARBOHIDRATOS ---
    float carb_100g = doc["product"]["carbohydrates_100g"] ? doc["product"]["carbohydrates_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float carb_1g = carb_100g != 0.0 ? carb_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 
    SerialPC.println("\nCarb_100g: " + String(carb_100g));
    SerialPC.println("Carb_1g: " + String(carb_1g));
    // ---------------------

    // --- LÍPIDOS ---------
    float lip_100g = doc["product"]["fat_100g"] ? doc["product"]["fat_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float lip_1g = lip_100g != 0.0 ? lip_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 
    SerialPC.println("\nLip_100g: " + String(lip_100g));
    SerialPC.println("Lip_1g: " + String(lip_1g));
    // ---------------------

    // --- PROTEÍNAS -------
    float prot_100g = doc["product"]["proteins_100g"] ? doc["product"]["proteins_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float prot_1g = prot_100g != 0.0 ? prot_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 
    SerialPC.println("\nProt_100g: " + String(prot_100g));
    SerialPC.println("Prot_1g: " + String(prot_1g));
    // ---------------------

    // --- KILOCALORÍAS -----
    float kcal_100g = doc["product"]["energy-kcal_100g"] ? doc["product"]["energy-kcal_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float kcal_1g = kcal_100g != 0.0 ? kcal_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 
    SerialPC.println("\nKcal_100g: " + String(kcal_100g));
    SerialPC.println("Kcal_1g: " + String(kcal_1g));
    // ---------------------
    // ------------------------------
    // --------------------------------------------------------------


    // --- CONSTRUIR CADENA DE TEXTO --------------------------------
    // Construir la cadena de texto con los datos
    productInfo = "PRODUCT:" + barcode + ";" + nombreProducto + ";" + String(carb_1g) + ";" + String(lip_1g) + ";" + String(prot_1g) + ";" + String(kcal_1g);
    SerialPC.println("\n\n" + productInfo);
    // --------------------------------------------------------------
    
}



#endif