/**
 * @file wifi_functions.h
 * @brief Este archivo contiene funciones para enviar el JSON a la database.
 * 
 * @author Irene Casares Rodríguez
 * @date 13/01/2024
 */

#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#include <HTTPClient.h>
#include <ArduinoJson.h>    // Trabajar con JSON devuelto por API

#define SerialPC Serial
#define SerialESP32Due Serial1

const char* ssid = "UCOTEAM";
const char* password = "-polonio210alfileres-";

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
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
void    connectToWiFi();                // Conectar a la red WiFi
bool    hayConexionWiFi();              // Comprobar si hay conexión WiFi
void    getFoodData(String &barcode);   // Obtener los datos de un alimento
/*-----------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si hay conexión a la red WiFi.
 */
/*-----------------------------------------------------------------------------*/
bool hayConexionWiFi(){ return (WiFi.status()== WL_CONNECTED); }





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
        //SerialESP32Due.println(F("WIFI")); // No hace falta, se pregunta después
    } 
    else 
    {
        // Si tras 10 segundos no se ha establecido la conexión:
        SerialPC.println(F("\nNo se pudo establecer la conexion WiFi."));
        //SerialESP32Due.println(F("NO-WIFI")); // No hace falta, se pregunta después
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
void getFoodData(String &barcode) {
  
  // Comprobamos si hay conexión a internet antes de leer el código. No nos sirve de nada el código
  // si no podemos buscar su información en OpenFoodFacts.

  //if(hayConexionWiFi()) {   //Comprueba si la conexión WiFi sigue activa
        HTTPClient http;  

        String serverPath = openFoodFacts_server + barcode + openFoodFacts_fields;

        http.begin(serverPath.c_str()); // Inicializar URL de la API

        // En las operaciones de lectura (obtener info de un producto) solo hace falta el User-Agent customizado
        // En las operaciones de escritura hace falta más autenticación (credenciales), pero no nos afecta porque 
        // no vamos a modificar productos.
        http.setUserAgent("SmartCloth/1.0 (irene.casares@imibic.org)"); // Establece el User-Agent personalizado

        int httpResponseCode = http.GET();  // Método de petición HTTP

        if(httpResponseCode>0){
            String payload = http.getString();  //Obtiene la respuesta
            //SerialPC.println(httpResponseCode);   //Imprime el código de estado HTTP
            //SerialPC.println(payload);            //Imprime la respuesta del servidor

            // Crear un objeto DynamicJsonDocument para almacenar el JSON analizado
            DynamicJsonDocument doc(JSON_SIZE_LIMIT); // 512 bytes

            // Analizar la respuesta en el objeto DynamicJsonDocument
            deserializeJson(doc, payload);

            // Comprobar si el estado del JSON es 1, lo que significa que se encontró el producto
            if (doc["status"] == 1) {
                // Imprimir la información requerida del producto
                String barcode = doc["code"].as<String>();
                SerialPC.println("\n\nBarcode: " + barcode);

                String nombreProducto;
                // Preferencia del nombre en español frente al general (seguramente en inglés)
                if (doc["product"]["product_name_es"]) nombreProducto = doc["product"]["product_name_es"].as<String>();
                else if (doc["product"]["product_name"]) nombreProducto = doc["product"]["product_name"].as<String>();
                SerialPC.println("\nNombre: " + nombreProducto);

                // Hay productos sin marconutrientes (como el agua), así que si no se obtienen eso valores, se asumen 0.0
                // Si se toman valores 0.0, entonces los valores por 1gr también son 0.0. Si no, entonces se divide entre 100.0
                float carb_100g = doc["product"]["carbohydrates_100g"] ? doc["product"]["carbohydrates_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
                float carb_1g = carb_100g != 0.0 ? carb_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 si 
                SerialPC.println("\nCarb_100g: " + String(carb_100g));
                SerialPC.println("Carb_1g: " + String(carb_1g));

                float lip_100g = doc["product"]["fat_100g"] ? doc["product"]["fat_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
                float lip_1g = lip_100g != 0.0 ? lip_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 si 
                SerialPC.println("\nLip_100g: " + String(lip_100g));
                SerialPC.println("Lip_1g: " + String(lip_1g));

                float prot_100g = doc["product"]["proteins_100g"] ? doc["product"]["proteins_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
                float prot_1g = prot_100g != 0.0 ? prot_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 si 
                SerialPC.println("\nProt_100g: " + String(prot_100g));
                SerialPC.println("Prot_1g: " + String(prot_1g));

                float kcal_100g = doc["product"]["energy-kcal_100g"] ? doc["product"]["energy-kcal_100g"].as<float>() : 0.0; // Si está, su valor. Si no, a 0.0 
                float kcal_1g = kcal_100g != 0.0 ? kcal_100g / 100.0 : 0.0; // Si está, tomar por gramo. Si no, a 0.0 si 
                SerialPC.println("\nKcal_100g: " + String(kcal_100g));
                SerialPC.println("Kcal_1g: " + String(kcal_1g));
            }
            else{
                SerialPC.println("Producto no encontrado!!");
            }
        }
        else {
            Serial.print("Error en la solicitud: ");
            Serial.println(httpResponseCode);
        }
        http.end();   //Cierra la conexión
    //}
    //else {
    //    Serial.println("Error en la conexión WiFi");
    //}
}


#endif