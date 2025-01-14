/**
 * @file wifi_functions.h
 * @brief Este archivo contiene funciones para buscar un producto en OpenFoodFacts mediante
 *        su EAN y obtener su nombre y valores nutricionales por 100gr (kcal, proteinas, lipidos y carb).
 * 
 * @author Irene Casares Rodríguez
 * @date 09/05/2024
 */

#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#include <HTTPClient.h>
#include <ArduinoJson.h>    // Trabajar con JSON devuelto por API

#include "Serial_functions.h"

// ------- CREDENCIALES -------------
// Credenciales conexión red WiFi
// Laboratorio:
const char* ssid = "UCOTEAM";
const char* password = "-polonio210alfileres-";
// Casa:
//const char* ssid = "MOVISTAR_FB23_EXT";
//const char* password = "DP6BUuEtuFvRw3mHmFoG";

// Testeos:
//const char* ssid = "Irene";
//const char* password = "icradeba5050";

// Post testeos:
//const char* ssid = "SmartCloth";
//const char* password = "SM-pass24/";
// -----------------------------------

/*
  [27/05/24 12:37] 
    El entorno de staging de OpenFoodFacts (https://world.openfoodfacts.net) está fallando, devuelve un error 500.
    En la documentación (https://openfoodfacts.github.io/openfoodfacts-server/api/) dicen que usemos el staging si no
    estamos en producción, pero como no funciona, vamos a usar el entorno de producción (https://world.openfoodfacts.org).
*/

//const char* openFoodFacts_server = "https://world.openfoodfacts.net/api/v2/product/"; // Staging environment
const char* openFoodFacts_server = "https://world.openfoodfacts.org/api/v2/product/";   // Production environment
const char* openFoodFacts_fields = "?fields=product_name,product_name_es,carbohydrates_100g,energy-kcal_100g,fat_100g,proteins_100g"; // Campos requeridos

#define JSON_SIZE_LIMIT 512 // El JSON devuelto suele de ser de 200 bytes, pero ponemos 512 por segurarnos




/*-----------------------------------------------------------------------------
                           DECLARACIÓN FUNCIONES
-----------------------------------------------------------------------------*/
inline bool    hayConexionWiFi(){ return (WiFi.status()== WL_CONNECTED); };  // Comprobar si hay conexión a la red WiFi

void    setupWiFi();                    // Configurar modo del WiFi y conectar a la red, si se puede
void    connectToWiFi();                // Conectar a la red WiFi
void    searchBarcode(String barcode);    // Obtener los datos de un alimento
void    getProductInfo(const String &payload, String &productInfo); // Obtener la información del producto a partir de un JSON
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
    // Mostrar identificador del esp32 (MAC) para ver si está en database
    SerialPC.print("MAC: "); SerialPC.println(WiFi.macAddress());
    SerialPC.println();
    // -------------------------------
}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Hace un solo intento de conexión a WiFi, con una espera máxima
 *        de 5 segundos.
 */
/*-----------------------------------------------------------------------------*/
void connectToWiFi() 
{
    SerialPC.println();

    // Inicializar startTime
    unsigned long startTime = millis();

    // Esperar hasta que se establezca la conexión o se agote el tiempo
    unsigned long timeout_waitConexion = 10000; // 10 segundos

    SerialPC.print(F("Conectando a WiFi..."));
    WiFi.begin(ssid, password);

    // Mientras no se haya conectado a WiFi y mientras no hayan pasado 5 segundos.
    // Si se conecta o si pasan los 5 segundos, sale del while.
    while ((!hayConexionWiFi()) && (millis() - startTime < timeout_waitConexion)) 
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
        // Si tras 10 segundos no se ha establecido la conexión:
        SerialPC.println(F("\nNo se pudo establecer la conexion WiFi."));
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
void searchBarcode(String barcode) 
{
  
  // Comprobamos si hay conexión a internet antes de leer el código. No nos sirve de nada el código
  // si no podemos buscar su información en OpenFoodFacts.

  if(hayConexionWiFi())    //Comprueba si la conexión WiFi sigue activa
  {
        SerialPC.println("Obteniendo información del producto...");
        
        // --- CONFIGURAR PETICIÓN HTTP ---
        // Configurar la petición HTTP: un GET a la URL de la API de OpenFoodFacts con el código de barras
        // y los campos que se quieren obtener
        HTTPClient http;  
        String serverPath = openFoodFacts_server + barcode + openFoodFacts_fields; // Conformar URL de la API
        http.begin(serverPath.c_str()); // Inicializar URL de la API
        http.setTimeout(5000); // Establecer 5 segundos de espera para la respuesta del servidor OpenFoodFacts

        // En las operaciones de lectura (obtener info de un producto) solo hace falta el User-Agent customizado
        // En las operaciones de escritura hace falta más autenticación (credenciales), pero no nos afecta porque 
        // no vamos a modificar productos.
        http.setUserAgent("SmartCloth/1.0 (i62caroi@uco.es)"); // Establece el User-Agent personalizado
        // --------------------------------

        // --- ENVIAR PETICIÓN HTTP -------
        // Enviar la petición HTTP
        int httpResponseCode = http.GET();  // Método de petición HTTP
        // --------------------------------

        // --- PROCESAR RESPUESTA HTTP -------------------------------
        // Comprobar el código de respuesta HTTP

        // ----- RESPUESTA DE OPENFOODFACTS ---------------------
        if(httpResponseCode>0)
        {
            // ------ PRODUCTO ENCONTRADO -----------------
            if((httpResponseCode >= 200) && (httpResponseCode < 300)) // Se encontró la info del producto
            {
                // --- OBTENER INFO DEL PRODUCTO ---
                String payload = http.getString();  // Obtener la respuesta del servidor de OpenFoodFacts
                String productInfo;
                getProductInfo(payload, productInfo); // Obtener la información del producto del JSON
                // --------------------------------
            }
            // --------------------------------------------
            // ------- ERROR EN PETICION ------------------
            else // No se encontró la info del producto
            {
                // ----- PRODUCTO NO ENCONTRADO -----
                if(httpResponseCode == 404) 
                {
                    // Si no se encuentra el producto, se obtiene código 404 (Not Found) y un JSON como este:
                    // { "code": "8422114932702", "status": 0, "status_verbose": "product not found" }
                    SerialPC.println("Producto no encontrado.\n");
                }
                // ----------------------------------
                // ----- ERROR EN PETICION ----------
                else 
                {
                    SerialPC.println("Error en la solicitud: " + httpResponseCode);
                }
                // ----------------------------------
            }
            // --------------------------------------------
        }
        // ------------------------------------------------------
        // ----- TIMEOUT DE OPENFOODFACTS -----------------------
        else if(httpResponseCode==-1) 
        {
            SerialPC.println("Tiempo de espera agotado. OpenFoodFacts no responde.");
        }
        // ------------------------------------------------------
        // --------- OTROS ERRORES ------------------------------
        else 
        { // Creo que nunca llegaría a esta condición
            SerialPC.println("Error en la solicitud: " + httpResponseCode);
        }
        // ------------------------------------------------------
        // -----------------------------------------------------------

        // --- CERRAR CONEXIÓN HTTP -------
        http.end();   //Cierra la conexión
        // --------------------------------
    }
    else 
    {
        SerialPC.println("Error en la conexión WiFi");
    }
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
    // Crear un objeto DynamicJsonDocument para almacenar el JSON analizado
    DynamicJsonDocument doc(JSON_SIZE_LIMIT); // 512 bytes
    deserializeJson(doc, payload);

    // --- CODIGO DEL PRODUCTO ------
    String barcode = doc["code"].as<String>();
    //SerialPC.println("\n\nBarcode: " + barcode);
    // ------------------------------

    // ------ INFORMACION DEL PRODUCTO --------------
    // --- NOMBRE DEL PRODUCTO ------
    // Preferencia del nombre en español frente al general (seguramente en inglés)
    String nombreProducto;
    if (doc["product"]["product_name_es"]) nombreProducto = doc["product"]["product_name_es"].as<String>();
    else if (doc["product"]["product_name"]) nombreProducto = doc["product"]["product_name"].as<String>();
    SerialPC.println("\nNombre: " + nombreProducto);
    // ------------------------------

    // Hay productos sin marconutrientes (como el agua), así que si no se obtienen esos valores, se asumen 0.0
    // Si se toman valores 0.0, entonces los valores por 1gr también son 0.0. Si no, entonces se divide entre 100.0
    
    // --- CARBOHIDRATOS ------------
    float carb_100g = doc["product"]["carbohydrates_100g"] ? doc["product"]["carbohydrates_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float carb_1g = carb_100g != 0.0 ? carb_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 si 
    SerialPC.println("\nCarb_100g: " + String(carb_100g));
    SerialPC.println("Carb_1g: " + String(carb_1g));
    // ------------------------------

    // --- LIPIDOS ------------------
    float lip_100g = doc["product"]["fat_100g"] ? doc["product"]["fat_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float lip_1g = lip_100g != 0.0 ? lip_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 si 
    SerialPC.println("\nLip_100g: " + String(lip_100g));
    SerialPC.println("Lip_1g: " + String(lip_1g));
    // ------------------------------

    // --- PROTEINAS ----------------
    float prot_100g = doc["product"]["proteins_100g"] ? doc["product"]["proteins_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float prot_1g = prot_100g != 0.0 ? prot_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 si 
    SerialPC.println("\nProt_100g: " + String(prot_100g));
    SerialPC.println("Prot_1g: " + String(prot_1g));
    // ------------------------------

    // --- KILOCALORIAS -------------
    float kcal_100g = doc["product"]["energy-kcal_100g"] ? doc["product"]["energy-kcal_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
    float kcal_1g = kcal_100g != 0.0 ? kcal_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 si 
    SerialPC.println("\nKcal_100g: " + String(kcal_100g));
    SerialPC.println("Kcal_1g: " + String(kcal_1g));
    // ------------------------------
    // ----------------------------------------------
}


#endif