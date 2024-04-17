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

#define SerialPC Serial
#define SerialESP32Due Serial1

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

// Credenciales conexión red WiFi
const char* ssid = "Irene";               // Nombre red
const char* password =  "icradeba5050";   // Contraseña

//const char* ssid = "UCOTEAM";
//const char* password = "-polonio210alfileres-";


// URL del servidor donde enviar el JSON
// Servidor de testeo
const char* post_testServerName = "http://smartcloth.site/post-esp32-data-json.php"; 

// URLs del servidor oficial
const char* post_fetchTokenServerName = "https://smartclothweb.org/api/mac";
const char* post_ComidaServerName = "https://smartclothweb.org/api/comidas";
const char* get_logOutServerName = "https://smartclothweb.org/api/logout";


// Token de autenticación pedido al servidor para poder subir información
String bearerToken;


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    hayConexionWiFi();  // Comprobar si hay conexión WiFi
void    connectToWiFi();    // Conectar a la red WiFi

void    sendJsonToDatabase(DynamicJsonDocument& JSONdoc);   // Enviar el JSON generado al servidor
bool    fetchToken();                                       // Pedir token de autenticación al servidor
void    logoutFromServer();                                 // Cerrar sesión en el servidor tras subir info


void    sendJsonToDatabase_fullProcess(DynamicJsonDocument& JSONdoc);  // Enviar el JSON generado al servidor
void    fetchToken_part1(String &bearerToken);                                  // 1. Pedir token
void    uploadJSON_part2(DynamicJsonDocument& JSONdoc, String &bearerToken);    // 2. Subir JSON
void    logout_part3(String &bearerToken);                                      // 3. Cerrar sesión
/*-----------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si hay conexión a la red WiFi.
 */
/*-----------------------------------------------------------------------------*/
bool hayConexionWiFi(){ return (WiFi.status()== WL_CONNECTED); }



/*-----------------------------------------------------------------------------*/
/**
 * @brief Hace varios intentos de conexión a WiFi, uno cada 10 segundos, 
 *        hasta que pasan 30 segundos.
 */
/*void connectToWiFi() 
{
    SerialPC.println();

    // Inicializar startTime
    unsigned long startTime = millis();

    // Esperar hasta que se establezca la conexión o se agote el tiempo
    unsigned long timeout_Reintento = 120000; // 120 segundos

    while ((!hayConexionWiFi()) && (millis() - startTime < timeout_Reintento)) 
    {
        SerialPC.print(F("Conectando a WiFi..."));
        WiFi.begin(ssid, password);

        // Esperar hasta que se establezca la conexión o se agote el tiempo
        unsigned long timeout_waitConexion = 30000; // 30 segundos
        unsigned long auxStartTime = millis();
        while ((!hayConexionWiFi()) && (millis() - auxStartTime < timeout_waitConexion)) 
        {
            delay(500);
            SerialPC.print(F("."));
        }

        // Comprobar si se estableció la conexión
        if (hayConexionWiFi()) 
        {
            SerialPC.println();
            SerialPC.print(F("Conectado a red WiFi con IP: ")); SerialPC.println(WiFi.localIP());
            SerialESP32Due.println(F("WIFI"));

            return; // Salir del bucle de reintento
        } 
        else 
        {
            SerialPC.println(F("\nNo se pudo establecer la conexion. Reintentando..."));
            // Eliminar esta linea en el programa final:
            SerialESP32Due.println("Fallo en conexión WiFi. Reintentando...");
        }
    }

    // Si tras 30 segundos, reintentándolo cada 10s, no se ha establecido la conexión:
    SerialPC.println(F("Unable to establish WiFi connection."));
    SerialESP32Due.println(F("NO-WIFI"));

    SerialPC.println();

}*/


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
    unsigned long timeout_waitConexion = 10000; // 10 segundos

    #if defined(SM_DEBUG)
    SerialPC.print(F("Conectando a WiFi..."));
    #endif
    WiFi.begin(ssid, password);

    // Mientras no se haya conectado a WiFi y mientras no hayan pasado 10 segundos.
    // Si se conecta o si pasan los 10 segundos, sale del while.
    while ((!hayConexionWiFi()) && (millis() - startTime < timeout_waitConexion)) 
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
        //SerialESP32Due.println(F("WIFI")); // No hace falta, se pregunta después
    } 
    else 
    {
        // Si tras 10 segundos no se ha establecido la conexión:
        #if defined(SM_DEBUG)
        SerialPC.println(F("\nNo se pudo establecer la conexion WiFi."));
        #endif
        //SerialESP32Due.println(F("NO-WIFI")); // No hace falta, se pregunta después
    }

}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte a String el JSON generado y lo envía a la base de datos.
 * @param JSONdoc - documento JSON a enviar
 * 
 * @note Esta función utiliza las variables globales post_ComidaServerName y bearerToken.
 */
void sendJsonToDatabase(DynamicJsonDocument& JSONdoc)
{
    if(hayConexionWiFi())
    {
        // Convertir el documento JSON en una cadena
        String jsonString;
        serializeJson(JSONdoc, jsonString);

        // Configurar la petición HTTP: un POST con la info de la comida en el body y el token en el header
        HTTPClient http;
        //http.begin(post_testServerName);
        http.begin(post_ComidaServerName);
        http.addHeader("Content-Type", "application/json");
        /*  // Si se quisiera configurar opciones específicas de la conexión, se haría con un cliente WiFi
            // y se pasaría como parámetro al crear la conexión:
                WiFiClient client;
                // ...configurar 'client'...
                http.begin(client, serverName);
            // Si no se configura explícitamente, al hacer 'http.begin(serverName)' la biblioteca HTTPClient
            // ya se encargará de crear un cliente por defecto
        */

        // Enviar la petición HTTP
        int httpResponseCode = http.POST(jsonString);

        // Comprobar el código de respuesta HTTP
        if(httpResponseCode > 0)
        {
            SerialPC.println();
            SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
            // No se puede poner directamente SerialPC.println("\n" + httpResponseCode); porque se imprimen cosas raras

            String response = http.getString(); // Obtener la respuesta del servidor
            SerialPC.println(response);         // Imprimir la respuesta del servidor

            if((httpResponseCode >= 200) && (httpResponseCode < 300)){
                SerialESP32Due.println(F("SAVED-OK"));
            }
            else{
                SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode); 
            }
        }
        else
        {
            SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);

            SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode);
        }

        // Cerrar la conexión
        http.end();
    }
    else
    {
        SerialPC.println(F("No tengo wifi"));
        SerialESP32Due.println(F("NO-WIFI"));
    }
}






/*-----------------------------------------------------------------------------*/
/**
 * @brief Envía un objeto JSON a la base de datos.
 *  1. Pedir token de autenticación
 *  2. Subir JSON
 *  3. Cerrar sesión
 * 
 * @param JSONdoc - documento JSON a enviar
 * 
 */
/*-----------------------------------------------------------------------------*/
void sendJsonToDatabase_fullProcess(DynamicJsonDocument& JSONdoc)
{
    if(hayConexionWiFi())
    {
        String bearerToken;

        fetchToken_part1(bearerToken);          // 1. Pedir token de autenticación
        uploadJSON_part2(JSONdoc,bearerToken);  // 2. Enviar JSON
        logout_part3(bearerToken);              // 3. Cerrar sesión
    }
    else
    {
        SerialPC.println(F("No tengo wifi"));
        SerialESP32Due.println(F("NO-WIFI"));
    }
}



/*void sendJsonToDatabase_justSend(DynamicJsonDocument& JSONdoc, String &bearerToken)
{
    if(hayConexionWiFi())
    {
        uploadJSON_part2(JSONdoc,bearerToken);  // 2. Enviar JSON
    }
    else
    {
        SerialPC.println(F("No tengo wifi"));
        SerialESP32Due.println(F("NO-WIFI"));
    }
}*/


bool sendJsonPerMealToDatabase_fullProcess(DynamicJsonDocument& JSONdoc)
{
    bool mealUploaded;

    if(hayConexionWiFi())
    {
        String bearerToken;

        fetchToken_part1(bearerToken);          // 1. Pedir token de autenticación
        mealUploaded = uploadJSONPerMeal_part2(JSONdoc,bearerToken);  // 2. Enviar JSON
        logout_part3(bearerToken);              // 3. Cerrar sesión
    }
    else
    {
        SerialPC.println(F("No tengo wifi"));
        SerialESP32Due.println(F("NO-WIFI"));

        mealUploaded = false;
    }

    return mealUploaded;
}


/*-----------------------------------------------------------------------------*/
/**
 * Realiza una petición HTTP POST al servidor para obtener un token de autenticación.
 * 
 * @param bearerToken Referencia a una cadena de caracteres donde se almacenará el token obtenido.
 */
/*-----------------------------------------------------------------------------*/
void fetchToken_part1(String &bearerToken)
{
    SerialPC.println("\n1. Pidiendo token...");

    // Configurar la petición HTTP: un POST con la mac para obtener el token
    HTTPClient http;
    http.begin(post_fetchTokenServerName);
    http.addHeader("Content-Type", "application/json");

    // JSON con MAC del esp32
    String macAddress = WiFi.macAddress();
    String requestBody = "{\"mac\":\"" + macAddress + "\"}";

    // Enviar la petición HTTP
    int httpResponseCode = http.POST(requestBody);

    // Comprobar el código de respuesta HTTP
    if(httpResponseCode > 0)
    {
        //SerialPC.println();
        //SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP

        if((httpResponseCode >= 200) && (httpResponseCode < 300)){ // Petición exitosa
            // Obtener la respuesta del servidor
            String response = http.getString(); 

            // Extraer token de la respuesta recibida:
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, response);
            bearerToken = doc["token"].as<String>();
            SerialPC.println("token: " + bearerToken);    
        }
        else{
            SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
            bearerToken = "";
        }
    }
    else
    {
        SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
        bearerToken = "";
    }

    // Cerrar la conexión
    http.end();

}


/*-----------------------------------------------------------------------------*/
/**
 * Sube un documento JSON al servidor mediante una petición HTTP POST.
 * 
 * @param JSONdoc El documento JSON a subir.
 * @param bearerToken El token de autenticación para la petición HTTP.
 */
void uploadJSON_part2(DynamicJsonDocument& JSONdoc, String &bearerToken)
{
    SerialPC.println("\n2. Subiendo JSON...");

    // Convertir el documento JSON en una cadena
    String jsonString;
    serializeJson(JSONdoc, jsonString);

    // Configurar la petición HTTP: un POST con la info de la comida en el body y el token en el header
    HTTPClient http;
    //http.begin(post_testServerName);
    http.begin(post_ComidaServerName);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + bearerToken); // Añadir el token de autenticación
    /*  // Si se quisiera configurar opciones específicas de la conexión, se haría con un cliente WiFi
        // y se pasaría como parámetro al crear la conexión:
            WiFiClient client;
            // ...configurar 'client'...
            http.begin(client, serverName);
        // Si no se configura explícitamente, al hacer 'http.begin(serverName)' la biblioteca HTTPClient
        // ya se encargará de crear un cliente por defecto
    */

    // Enviar la petición HTTP
    int httpResponseCode = http.POST(jsonString);

    // Comprobar el código de respuesta HTTP
    if(httpResponseCode > 0)
    {
        //SerialPC.println();
        //SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
        // No se puede poner directamente SerialPC.println("\n" + httpResponseCode); porque se imprimen cosas raras

        String response = http.getString(); // Obtener la respuesta del servidor
        //SerialPC.println(response);         // Imprimir la respuesta del servidor

        if((httpResponseCode >= 200) && (httpResponseCode < 300)){
            SerialESP32Due.println(F("SAVED-OK"));
        }
        else{
            SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode); 
        }
    }
    else
    {
        SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);

        SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode);
    }

    // Cerrar la conexión
    http.end();
}


bool uploadJSONPerMeal_part2(DynamicJsonDocument& JSONdoc, String &bearerToken)
{
    SerialPC.println("\n2. Subiendo JSON...");

    bool mealUploaded;

    // Convertir el documento JSON en una cadena
    String jsonString;
    serializeJson(JSONdoc, jsonString);

    // Configurar la petición HTTP: un POST con la info de la comida en el body y el token en el header
    HTTPClient http;
    //http.begin(post_testServerName);
    http.begin(post_ComidaServerName);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + bearerToken); // Añadir el token de autenticación
    /*  // Si se quisiera configurar opciones específicas de la conexión, se haría con un cliente WiFi
        // y se pasaría como parámetro al crear la conexión:
            WiFiClient client;
            // ...configurar 'client'...
            http.begin(client, serverName);
        // Si no se configura explícitamente, al hacer 'http.begin(serverName)' la biblioteca HTTPClient
        // ya se encargará de crear un cliente por defecto
    */

    // Enviar la petición HTTP
    int httpResponseCode = http.POST(jsonString);

    // Comprobar el código de respuesta HTTP
    if(httpResponseCode > 0)
    {
        //SerialPC.println();
        //SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
        // No se puede poner directamente SerialPC.println("\n" + httpResponseCode); porque se imprimen cosas raras

        String response = http.getString(); // Obtener la respuesta del servidor
        //SerialPC.println(response);         // Imprimir la respuesta del servidor

        if((httpResponseCode >= 200) && (httpResponseCode < 300)){
            //SerialESP32Due.println(F("SAVED-OK"));
            mealUploaded = true;
        }
        else{
            //SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode); 
            mealUploaded = false;
        }
    }
    else
    {
        SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);

        //SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode);

        mealUploaded = false;
    }

    // Cerrar la conexión
    http.end();

    return mealUploaded; // Si se ha subido o no la comida
}


/*-----------------------------------------------------------------------------*/
/**
 * Realiza una solicitud de cierre de sesión al servidor utilizando el token de autorización proporcionado.
 * 
 * @param bearerToken El token de autorización utilizado para autenticar la solicitud.
 */
/*-----------------------------------------------------------------------------*/
void logout_part3(String &bearerToken)
{
    SerialPC.println("\n3. Cerrando sesión...");

    // Configurar la petición HTTP: un GET con el token en el header y el body vacío
    HTTPClient http;
    http.begin(get_logOutServerName);
    http.addHeader("Authorization", "Bearer " + bearerToken);

    // Enviar la petición HTTP
    int httpResponseCode = http.GET();

    // Comprobar el código de respuesta HTTP
    if(httpResponseCode>0){
        //SerialPC.println();
        //SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP

        if((httpResponseCode >= 200) && (httpResponseCode < 300)){ // Petición exitosa
            // Obtener la respuesta del servidor
            String response = http.getString(); 

            // Extraer mensaje de la respuesta recibida:
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, response);
            bool success = doc["success"];
            String message = doc["message"];     

            if(success && message == "User logged out successfully"){
                SerialPC.println("Cierre de sesión exitoso");
            }
            else {
                SerialPC.println("Error al cerrar la sesión\n");
            }
        }
        else{
            SerialPC.print(F("\nError en la petición HTTP GET: ")); SerialPC.println(httpResponseCode);
        }
        
    }
    else {
        SerialPC.print("ERROR-HTTP: ");
        SerialPC.println(httpResponseCode);
    }
    http.end();
}




/*-----------------------------------------------------------------------------*/
/**
 * @brief Realiza una petición HTTP para obtener un token de autenticación que se 
 * utilizará para realizar operaciones de autenticación en futuras solicitudes.
 * 
 * @note Esta función utiliza las variables globales post_fetchTokenServerName y bearerToken.
 * 
 * @return true si se obtuvo el token correctamente, false en caso contrario.
 */
/*-----------------------------------------------------------------------------*/
bool fetchToken()
{
    bearerToken = ""; // Reiniciar token para saber si se actualiza o no

    if(hayConexionWiFi()){
        // Configurar la petición HTTP: un POST con la mac para obtener el token
        HTTPClient http;
        http.begin(post_fetchTokenServerName);
        http.addHeader("Content-Type", "application/json");

        // JSON con MAC del esp32
        String macAddress = WiFi.macAddress();
        String requestBody = "{\"mac\":\"" + macAddress + "\"}";

        // Enviar la petición HTTP
        int httpResponseCode = http.POST(requestBody);

        // Comprobar el código de respuesta HTTP
        if(httpResponseCode > 0)
        {
            SerialPC.println();
            SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP

            if((httpResponseCode >= 200) && (httpResponseCode < 300)){ // Petición exitosa
                // Obtener la respuesta del servidor
                String response = http.getString(); 

                // Extraer token de la respuesta recibida:
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, response);
                bearerToken = doc["token"].as<String>();
                SerialPC.println("token: " + bearerToken);    
                SerialPC.println();     
            }
            else{
                SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
            }
        }
        else
        {
            SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
        }

        // Cerrar la conexión
        http.end();
    }
    // No hace falta, se hace luego si esta función devuelve false
    /*else { 
        SerialPC.println(F("No tengo wifi"));
        SerialESP32Due.println("NO-WIFI");
    }*/

    if(bearerToken == "") return false; // Hubo error y no se actualizó (devolvió) token
    else return true;

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Cierra la sesión del usuario en el servidor.
 * 
 * Esta función realiza una petición HTTP GET al servidor para cerrar la sesión del usuario.
 * Verifica si hay conexión WiFi antes de realizar la petición.
 * 
 * @note Esta función utiliza las variables globales get_logOutServerName y bearerToken.
 */
/*-----------------------------------------------------------------------------*/
void logoutFromServer() 
{
    if(hayConexionWiFi())
    {
        // Configurar la petición HTTP: un GET con el token en el header y el body vacío
        HTTPClient http;
        http.begin(get_logOutServerName);
        http.addHeader("Authorization", "Bearer " + bearerToken);

        // Enviar la petición HTTP
        int httpResponseCode = http.GET();

        // Comprobar el código de respuesta HTTP
        if(httpResponseCode>0){
            SerialPC.println();
            SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP

            if((httpResponseCode >= 200) && (httpResponseCode < 300)){ // Petición exitosa
                // Obtener la respuesta del servidor
                String response = http.getString(); 

                // Extraer mensaje de la respuesta recibida:
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, response);
                bool success = doc["success"];
                String message = doc["message"];     

                if(success && message == "User logged out successfully"){
                    SerialPC.println("Cierre de sesión exitoso");
                }
                else {
                    SerialPC.println("Error al cerrar la sesión");
                }
            }
            else{
                SerialPC.print(F("\nError en la petición HTTP GET: ")); SerialPC.println(httpResponseCode);
            }
            
        }
        else {
            SerialPC.print("ERROR-HTTP: ");
            SerialPC.println(httpResponseCode);
        }
        http.end();
    }
    else { 
        SerialPC.println(F("No tengo wifi"));
        //SerialESP32Due.println("NO-WIFI");
    }
}



#endif