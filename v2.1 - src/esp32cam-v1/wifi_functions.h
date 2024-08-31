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

/*
    Las credenciales se podrían guardar en la memoria flash no volátil del ESP32 en lugar de
    usar variables globales, aunque a efectos prácticos es lo mismo:

    #include <Preferences.h>

    // Crear instancia de la clase Preferences
    Preferences preferences; 

    void setup() {
        // ...
        // Abrir espacio de nombres donde almacenar las credenciales
        preferences.begin("my_namespace", false);
        preferences.putString("ssid", "your_SSID");
        preferences.putString("password", "your_PASSWORD");
        // ...
    }
*/

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

// Post testeos:
//const char* ssid = "SmartCloth";
//const char* password = "SM-pass24/";
// -----------------------------------


// ------- DATABASE SMARTCLOTH -------
// URL del servidor donde enviar el JSON
const char* fetchTokenServerName = "https://smartclothweb.org/api/mac";
const char* comidaServerName = "https://smartclothweb.org/api/comidas";
const char* logOutServerName = "https://smartclothweb.org/api/logout_mac";
// ----------------------------------


// ------- OPEN FOOD FACTS ----------
/*  [27/05/24 12:37] 
    El entorno de staging de OpenFoodFacts (https://world.openfoodfacts.net) está fallando, devuelve un error 500.
    En la documentación (https://openfoodfacts.github.io/openfoodfacts-server/api/) dicen que usemos el staging si no
    estamos en producción, pero como no funciona, vamos a usar el entorno de producción (https://world.openfoodfacts.org).*/

// URL de la API de OpenFoodFacts
const char* openFoodFacts_server = "https://world.openfoodfacts.net/api/v2/product/"; // Staging environment
//const char* openFoodFacts_server = "https://world.openfoodfacts.org/api/v2/product/";   // Production environment
const char* openFoodFacts_fields = "?fields=product_name,product_name_es,carbohydrates_100g,energy-kcal_100g,fat_100g,proteins_100g"; // Campos requeridos

#define JSON_SIZE_LIMIT_BARCODE 512 // El JSON devuelto suele de ser de 200 bytes, pero ponemos 512 por segurarnos
// ----------------------------------




/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-----------------------------------------------------------------------------*/
inline bool    hayConexionWiFi(){ return (WiFi.status()== WL_CONNECTED); };  // Comprobar si hay conexión a la red WiFi

void    setupWiFi();        // Configurar módulo WiFi y conectar a la red
void    connectToWiFi();    // Conectar a la red WiFi
void    checkWiFi();        // Comprobar si hay WiFi e indicarlo al Due

bool    fetchTokenFromServer(String &bearerToken);                                  // 1. Pedir token
void    uploadJSONtoServer(DynamicJsonDocument& JSONdoc, String &bearerToken);      // 2. Subir JSON
void    logoutFromServer(String &bearerToken);                                      // 3. Cerrar sesión

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
    #if defined(SM_DEBUG)
        SerialPC.print("MAC: "); SerialPC.println(WiFi.macAddress());
        SerialPC.println();
    #endif
    // -------------------------------
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Hace un solo intento de conexión a WiFi, con una espera máxima
 *        de 10 segundos.
 */
/*-----------------------------------------------------------------------------*/
void connectToWiFi() 
{
    #if defined(SM_DEBUG)
        SerialPC.println();
    #endif

    // Inicializar startTime
    unsigned long startTime = millis();

    // Esperar hasta que se establezca la conexión o se agote el tiempo
    unsigned long timeout_waitConexion = 15000; // 15 segundos

    #if defined(SM_DEBUG)
        SerialPC.print(F("Conectando a WiFi..."));
    #endif
    WiFi.begin(ssid, password);

    // Mientras no se haya conectado a WiFi y mientras no hayan pasado 15 segundos.
    // Si se conecta o si pasan los 15 segundos, sale del while.
    while(!hayConexionWiFi() && !isTimeoutExceeded(startTime, timeout_waitConexion))
    {
        delay(500);
        #if defined(SM_DEBUG)
            SerialPC.print(F("."));
        #endif
    }

    // Comprobar si se estableció la conexión
    if (hayConexionWiFi()) 
    {
        #if defined(SM_DEBUG)
            SerialPC.println();
            SerialPC.print(F("Conectado a red WiFi con IP: ")); SerialPC.println(WiFi.localIP());
            SerialPC.println();
        #endif
    } 
    else 
    {
        // Si tras 15 segundos no se ha establecido la conexión:
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nNo se pudo establecer la conexion WiFi."));
        #endif
    }

}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Verifica si hay conexión WiFi y envía un mensaje al dispositivo ESP32 Due.
 * 
 * Esta función verifica si hay una conexión WiFi disponible. Si hay conexión, 
 * se envía un mensaje al dispositivo ESP32 Due indicando que hay WiFi. Si no 
 * hay conexión, se envía un mensaje indicando que no hay WiFi.
 */
/*-----------------------------------------------------------------------------*/
void checkWiFi()
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    limpiarBufferBR();
    // ---------------------------------------------------------

    if(hayConexionWiFi())
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nTengo Wifi"));
        #endif

        sendMsgToDue(F("WIFI-OK"));
    }
    else 
    { 
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nNo tengo wifi"));
        #endif

        sendMsgToDue(F("NO-WIFI"));
    }
}





/*-----------------------------------------------------------------------------*/
/**
 * Realiza una petición HTTP POST al servidor para obtener un token de autenticación.
 * 
 * @param bearerToken Referencia a una cadena de caracteres donde se almacenará el token obtenido.
 */
/*-----------------------------------------------------------------------------*/
bool fetchTokenFromServer(String &bearerToken)
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    limpiarBufferBR();
    // ---------------------------------------------------------

    // Pide el token si sigue teniendo conexión
    if(hayConexionWiFi())
    {

        #if defined(SM_DEBUG)
            SerialPC.println("\n1. Pidiendo token...");
        #endif

        // --- CONFIGURAR PETICIÓN HTTP ---
        // Configurar la petición HTTP: un POST con la mac para obtener el token
        HTTPClient http;
        http.begin(fetchTokenServerName);
        http.addHeader("Content-Type", "application/json");

        // JSON con MAC del esp32
        String macAddress = WiFi.macAddress();
        String requestBody = "{\"mac\":\"" + macAddress + "\"}";
        // --------------------------------

        // --- ENVIAR PETICIÓN HTTP -------
        // Enviar la petición HTTP
        int httpResponseCode = http.POST(requestBody);
        // --------------------------------


        // --- PROCESAR RESPUESTA HTTP -----
        // Comprobar el código de respuesta HTTP
        if(httpResponseCode > 0)
        {
            #if defined(SM_DEBUG)
                SerialPC.println();
                SerialPC.print("Respuesta pedir token: "); SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP 
            #endif

            if((httpResponseCode >= 200) && (httpResponseCode < 300)) // Petición exitosa
            {
                // ---- TOKEN ------------------------------
                // Obtener la respuesta del servidor
                String response = http.getString(); 

                // Extraer token de la respuesta recibida:
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, response);
                bearerToken = doc["token"].as<String>(); // Token asignado por el servidor
                // -----------------------------------------

                #if defined(SM_DEBUG)
                    SerialPC.println("\nTOKEN: " + bearerToken + "\n");     
                #endif

                return true; // Token obtenido
            }
            else
            {
                #if defined(SM_DEBUG)
                    SerialPC.print(F("A. Error pidiendo token: ")); SerialPC.println(httpResponseCode);
                #endif

                // -- RESPUESTA AL DUE ---
                sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode)); // Error en la petición HTTP
                // -----------------------

                bearerToken = ""; // Token vacío por error en autenticación

                return false; // No se ha podido obtener el token
            }
        }
        else
        {
            #if defined(SM_DEBUG)
                SerialPC.print(F("B. Error pidiendo token: ")); SerialPC.println(httpResponseCode);
            #endif
            
            // -- RESPUESTA AL DUE ---
            sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode)); // Error en la petición HTTP
            // -----------------------

            bearerToken = ""; // Token vacío por error en autenticación

            return false; // No se ha podido obtener el token
        }
        // --------------------------------

        // --- CERRAR CONEXIÓN HTTP -------
        http.end(); // Cierra la conexión
        // --------------------------------

    }
    else
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nNo se puede PEDIR TOKEN porque ha perdido la conexion a Internet"));
        #endif

        // -- RESPUESTA AL DUE ---
        sendMsgToDue("NO-WIFI"); // No hay conexión WiFi
        // -----------------------

        bearerToken = ""; // Token vacío por error en autenticación

        return false; // No se ha podido obtener el token
    }

}


/*-----------------------------------------------------------------------------*/
/**
 * Sube un documento JSON al servidor mediante una petición HTTP POST.
 * 
 * @param JSONdoc El documento JSON a subir.
 * @param bearerToken El token de autenticación para la petición HTTP.
 */
void uploadJSONtoServer(DynamicJsonDocument& JSONdoc, String &bearerToken)
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    limpiarBufferBR();
    // ---------------------------------------------------------

    // Sube la comida si sigue teniendo conexión
    if(hayConexionWiFi())
    {
        #if defined(SM_DEBUG)
            SerialPC.println("\n2. Subiendo JSON...");
        #endif

        // --- CONFIGURAR PETICIÓN HTTP ---
        // Convertir el documento JSON en una cadena
        String jsonString;
        serializeJson(JSONdoc, jsonString);

        // Configurar la petición HTTP: un POST con la info de la comida en el body y el token en el header
        HTTPClient http;
        http.begin(comidaServerName);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer " + bearerToken); // Añadir el token de autenticación
        // --------------------------------

        // --- ENVIAR PETICIÓN HTTP -------
        // Enviar la petición HTTP
        int httpResponseCode = http.POST(jsonString);
        // --------------------------------

        // --- PROCESAR RESPUESTA HTTP -----
        // Comprobar el código de respuesta HTTP
        if(httpResponseCode > 0)
        {
            #if defined(SM_DEBUG)
                SerialPC.print("Respuesta HTTP: "); SerialPC.println(httpResponseCode); 
            #endif

            String response = http.getString(); // Obtener la respuesta del servidor
            //SerialPC.println(response);         // Imprimir la respuesta del servidor

            if((httpResponseCode >= 200) && (httpResponseCode < 300))
            {
                #if defined(SM_DEBUG)
                    SerialPC.print(F("Comida subida")); 
                #endif

                // -- RESPUESTA AL DUE ---
                sendMsgToDue(F("SAVED-OK"));
                // -----------------------
            }
            else
            {
                #if defined(SM_DEBUG)
                    SerialPC.print(F("A. Error subiendo comida: ")); SerialPC.println(httpResponseCode);
                #endif

                // -- RESPUESTA AL DUE ---
                sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode)); // Error en la petición HTTP
                // -----------------------
            }
        }
        else
        {
            #if defined(SM_DEBUG)
                SerialPC.print(F("B. Error subiendo comida: ")); SerialPC.println(httpResponseCode);
            #endif
    
            // -- RESPUESTA AL DUE ---
            sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode)); // Error en la petición HTTP
            // -----------------------
        }
        // --------------------------------

        // --- CERRAR CONEXIÓN HTTP -------
        http.end(); // Cierra la conexión
        // --------------------------------

    }
    else
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nNo se puede SUBIR LA COMIDA porque ha perdido la conexion a Internet"));
        #endif

        // -- RESPUESTA AL DUE ---
        sendMsgToDue("NO-WIFI"); // No hay conexión WiFi
        // -----------------------
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * Realiza una solicitud de cierre de sesión al servidor utilizando el token de autorización proporcionado.
 * 
 * @param bearerToken El token de autorización utilizado para autenticar la solicitud.
 */
/*-----------------------------------------------------------------------------*/
void logoutFromServer(String &bearerToken)
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    //limpiarBufferBR();
    // ---------------------------------------------------------

    // Cierra sesión si sigue teniendo conexión
    //
    // Creo que no pasa nada si se queda abierta, se cerrará a la media hora por si fallara el cerrar sesión y 
    // pocos minutos después se quisiera guardar más comidas. Lo que no sé es que si se queda abierta, si se
    // puede volver a pedir token con la misma MAC. 
    if(hayConexionWiFi())
    {

        #if defined(SM_DEBUG)
            SerialPC.println("\n3. Cerrando sesion...");
            SerialPC.println("\nTOKEN: " + bearerToken + "\n"); 
        #endif

        // --- CONFIGURAR PETICIÓN HTTP ---
        // Configurar la petición HTTP: un GET con el token en el header y el body vacío
        HTTPClient http;
        http.begin(logOutServerName);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer " + bearerToken);
        // --------------------------------

        // --- ENVIAR PETICIÓN HTTP -------
        // Enviar la petición HTTP
        int httpResponseCode = http.POST(""); // Enviar un POST vacío con el token en el header
        // --------------------------------

        // --- PROCESAR RESPUESTA HTTP -----
        // Comprobar el código de respuesta HTTP
        if(httpResponseCode>0)
        {
            #if defined(SM_DEBUG)
                SerialPC.println();
                SerialPC.print("Respuesta logout: "); SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
            #endif

            if((httpResponseCode >= 200) && (httpResponseCode < 300)) // Petición exitosa
            {
                #if defined(SM_DEBUG)
                    // Obtener la respuesta del servidor
                    String response = http.getString(); 

                    // Extraer mensaje de la respuesta recibida:
                    DynamicJsonDocument doc(1024);
                    deserializeJson(doc, response);
                    bool success = doc["success"];
                    String message = doc["message"];     
                    
                    
                    if(success && message == "User logged out successfully") SerialPC.println("Cierre de sesion exitoso");
                    else{ 
                        SerialPC.println("Error al cerrar la sesion");
                    }
                #endif
            }
            else{
                #if defined(SM_DEBUG)
                    SerialPC.print(F("A. Error cerrando sesión: ")); SerialPC.println(httpResponseCode);
                #endif
                
                //sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode));
                // No hace falta avisar al Due. Si no se puede cerrar la sesión, debería cerrarse automáticamente a la media hora.
            }
            
        }
        else {
            #if defined(SM_DEBUG)
                SerialPC.print(F("B. Error cerrando sesión: ")); SerialPC.println(httpResponseCode);
            #endif

            //sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode));
            // No hace falta avisar al Due. Si no se puede cerrar la sesión, debería cerrarse automáticamente a la media hora.
        }
        // --------------------------------

        // --- CERRAR CONEXIÓN HTTP -------
        http.end(); // Cierra la conexión
        // --------------------------------

    }
    else
    {
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nNo se puede CERRAR SESION porque ha perdido la conexion a Internet"));
        #endif

        //sendMsgToDue(F("NO-WIFI"));
        // No hace falta avisar al Due. Si no se puede cerrar la sesión, debería cerrarse automáticamente a la media hora.
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
 */
/*-----------------------------------------------------------------------------*/
void getFoodData(String barcode) 
{
    // ---- LIMPIAR BUFFER -------------------------------------
    // Se limpia el buffer de recepción (Rx) antes de enviar para asegurar que se procesa la respuesta 
    // al mensaje que se va a enviar y no otros enviados anteriormente
    limpiarBufferBR();
    // ---------------------------------------------------------


    // --- COMPROBAR SI HAY CONEXIÓN A INTERNET -----
    // Si no hay conexión a Internet, no merece la pena escanear el código de barras porque no se podrá buscar su información en OpenFoodFacts
    // Creo que esto ya se hace en Due antes de pedir leer el barcode, pero por si acaso.

    // --- HAY INTERNET ---
    if(hayConexionWiFi())    //Comprueba si la conexión WiFi sigue activa
    {
        #if defined(SM_DEBUG)
            SerialPC.println("\nObteniendo información del producto...");
        #endif
        
        // --- CONFIGURAR PETICIÓN HTTP ---
        // Configurar la petición HTTP: un GET a la URL de la API de OpenFoodFacts con el código de barras y los campos que se quieren obtener
        HTTPClient http;  
        String serverPath = openFoodFacts_server + barcode + openFoodFacts_fields; // Conformar URL de la API

        // Por ejemplo, para tostas de trigo:
        // "https://world.openfoodfacts.org/api/v2/product/5601560111905?fields=product_name,product_name_es,carbohydrates_100g,energy-kcal_100g,fat_100g,proteins_100g"

        http.begin(serverPath.c_str()); // Inicializar URL de la API
        http.setTimeout(5000);          // Establecer 5 segundos de espera para la respuesta del servidor OpenFoodFacts

        // En las operaciones de lectura (obtener info de un producto) solo hace falta el User-Agent customizado
        // En las operaciones de escritura hace falta más autenticación (credenciales), pero no nos afecta porque 
        // no vamos a modificar productos.
        http.setUserAgent("SmartCloth/1.0 (irene.casares@imibic.org)"); // Establece el User-Agent personalizado
        // --------------------------------

        // --- ENVIAR PETICIÓN HTTP -------
        // Enviar la petición HTTP
        int httpResponseCode = http.GET();  // Método de petición HTTP
        // --------------------------------

        // --- PROCESAR RESPUESTA HTTP -----------------------
        // Comprobar el código de respuesta HTTP
        if(httpResponseCode>0)
        {
            // --- PRODUCTO ENCONTRADO --------------
            if((httpResponseCode >= 200) && (httpResponseCode < 300)) // Se encontró la info del producto
            {
                // --- OBTENER INFO DEL PRODUCTO ---
                String payload = http.getString();  // Obtener la respuesta del servidor de OpenFoodFacts
                String productInfo;
                getProductInfo(payload, productInfo); // Obtener la información del producto del 'payload' y almacenarla en 'productInfo' con la estuctura adecuada:
                                                      //   "PRODUCT:barcode;nombreProducto;carb_1g;lip_1g;prot_1g;kcal_1g"
                // --------------------------------

                // --- ENVIAR INFO AL DUE ---------
                sendMsgToDue(productInfo);
                // --------------------------------
            }
            // --------------------------------------
            // --- PRODUCTO NO ENCONTRADO -----------
            else // No se encontró la info del producto porque no está en database o por error en la petición
            {
                if(httpResponseCode == 404) 
                {
                    // Si no se encuentra el producto, se obtiene código 404 (Not Found) y un JSON como este:
                    // { "code": "8422114932702", "status": 0, "status_verbose": "product not found" }
                    #if defined(SM_DEBUG)
                        SerialPC.println("Producto no encontrado.");
                    #endif

                    // -- RESPUESTA AL DUE ---
                    sendMsgToDue(F("NO-PRODUCT"));
                    // -----------------------
                }
                else 
                {
                    #if defined(SM_DEBUG)
                        SerialPC.println("Error en la solicitud: " + httpResponseCode);
                    #endif

                    // -- RESPUESTA AL DUE ---
                    sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode)); // Error en la petición HTTP
                    // -----------------------
                
                }
            }
            // --------------------------------------
        }
        else if(httpResponseCode==-1) 
        {
            #if defined(SM_DEBUG)
                SerialPC.println("Tiempo de espera agotado. OpenFoodFacts no responde.");
            #endif

            // -- RESPUESTA AL DUE ---
            sendMsgToDue(F("PRODUCT-TIMEOUT"));
            // -----------------------
        }
        else 
        { // Creo que nunca llegaría a esta condición
            #if defined(SM_DEBUG)
                SerialPC.println("Error en la solicitud: " + httpResponseCode);
            #endif

            // -- RESPUESTA AL DUE ---
            sendMsgToDue("HTTP-ERROR:" + String(httpResponseCode)); // Error en la petición HTTP
            // -----------------------
        }
        // ---------------------------------------------------

        // --- CERRAR CONEXIÓN HTTP --------------------------
        http.end();   //Cierra la conexión
        // ---------------------------------------------------
    }
    // --- FIN HAY INTERNET --

    // --- NO HAY INTERNET ---
    else 
    {
        #if defined(SM_DEBUG)
            SerialPC.println("No se puede obtener la información del producto porque ha perdido la conexión a Internet");
        #endif
        sendMsgToDue(F("NO-WIFI"));
    }
    // --- FIN NO HAY INTERNET ---
    // ---- FIN CHEQUEO INTERNET --------------------
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Obtiene la información del producto a partir de un JSON y la almacena en una cadena de texto.
 * 
 * @param payload El JSON que contiene la información del producto.
 * @param productInfo La cadena de texto donde se almacenará la información del producto.
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
    #if defined(SM_DEBUG)
        SerialPC.println("\n\nBarcode: " + barcode);
    #endif
    // -----------------------

    // --- NOMBRE PRODUCTO ---
    // Preferencia del nombre en español frente al general (seguramente en inglés)
    String nombreProducto;
    if (doc["product"]["product_name_es"]) nombreProducto = doc["product"]["product_name_es"].as<String>();
    else if (doc["product"]["product_name"]) nombreProducto = doc["product"]["product_name"].as<String>();
    #if defined(SM_DEBUG)
        SerialPC.println("\nNombre: " + nombreProducto);
    #endif
    // ----------------------

    // ------ MACRONUTRIENTES -------
    // Hay productos sin marconutrientes (como el agua), así que si no se obtienen esos valores, se asumen 0.0
    // Si se obtienen valores 0.0, entonces los valores por 1gr también son 0.0. Si no, se dividen entre 100.0
    
    // --- CARBOHIDRATOS ---
    float carb_100g = doc["product"]["carbohydrates_100g"] ? doc["product"]["carbohydrates_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float carb_1g = carb_100g != 0.0 ? carb_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 
    #if defined(SM_DEBUG)
        SerialPC.println("\nCarb_100g: " + String(carb_100g));
        SerialPC.println("Carb_1g: " + String(carb_1g));
    #endif
    // ---------------------

    // --- LÍPIDOS ---------
    float lip_100g = doc["product"]["fat_100g"] ? doc["product"]["fat_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float lip_1g = lip_100g != 0.0 ? lip_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 
    #if defined(SM_DEBUG)
        SerialPC.println("\nLip_100g: " + String(lip_100g));
        SerialPC.println("Lip_1g: " + String(lip_1g));
    #endif
    // ---------------------

    // --- PROTEÍNAS -------
    float prot_100g = doc["product"]["proteins_100g"] ? doc["product"]["proteins_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float prot_1g = prot_100g != 0.0 ? prot_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 
    #if defined(SM_DEBUG)
        SerialPC.println("\nProt_100g: " + String(prot_100g));
        SerialPC.println("Prot_1g: " + String(prot_1g));
    #endif
    // ---------------------

    // --- KILOCALORÍAS -----
    float kcal_100g = doc["product"]["energy-kcal_100g"] ? doc["product"]["energy-kcal_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float kcal_1g = kcal_100g != 0.0 ? kcal_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 
    #if defined(SM_DEBUG)
        SerialPC.println("\nKcal_100g: " + String(kcal_100g));
        SerialPC.println("Kcal_1g: " + String(kcal_1g));
    #endif
    // ---------------------
    // ------------------------------
    // --------------------------------------------------------------


    // --- CONSTRUIR CADENA DE TEXTO --------------------------------
    // Construir la cadena de texto con los datos
    productInfo = "PRODUCT:" + barcode + ";" + nombreProducto + ";" + String(carb_1g) + ";" + String(lip_1g) + ";" + String(prot_1g) + ";" + String(kcal_1g);
    #if defined(SM_DEBUG)
        SerialPC.println("\n\n" + productInfo);
    #endif   
    // --------------------------------------------------------------
    
}



#endif