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

#define SM_DEBUG // Descomentar para habilitar mensajes de depuración entre Due y PC

#if defined(SM_DEBUG)
#define SerialPC Serial
#endif

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




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    hayConexionWiFi();  // Comprobar si hay conexión WiFi
void    connectToWiFi();    // Conectar a la red WiFi

void    sendJsonToDatabase(DynamicJsonDocument& JSONdoc);    // Enviar el JSON generado al servidor

void    sendJsonToDatabase_fullProcess(DynamicJsonDocument& JSONdoc);  // Enviar el JSON generado al servidor
void    fetchTokenFromServer(String &bearerToken);                                  // 1. Pedir token
void    uploadJSONtoServer(DynamicJsonDocument& JSONdoc, String &bearerToken);      // 2. Subir JSON
void    logoutFromServer(String &bearerToken);                                      // 3. Cerrar sesión
/*-----------------------------------------------------------------------------*/




/*-----------------------------------------------------------------------------*/
/**
 * @brief Comprueba si hay conexión a la red WiFi.
 */
/*-----------------------------------------------------------------------------*/
bool hayConexionWiFi(){ return (WiFi.status()== WL_CONNECTED); }



/*-----------------------------------------------------------------------------*/
/**
 * @brief Hace varios intentos de conexión a WiFi, uno cada 30 segundos, 
 *        hasta que pasan 120 segundos.
 */
/*-----------------------------------------------------------------------------*/
/*void connectToWiFi() 
{
    #if defined(SM_DEBUG)
    SerialPC.println();
    #endif

    // Inicializar startTime
    unsigned long startTime = millis();

    // Esperar hasta que se establezca la conexión o se agote el tiempo
    unsigned long timeout_Reintento = 120000; // 120 segundos

    while ((!hayConexionWiFi()) && (millis() - startTime < timeout_Reintento)) 
    {
        #if defined(SM_DEBUG)
        SerialPC.print(F("Conectando a WiFi..."));
        #endif
        WiFi.begin(ssid, password);

        // Esperar hasta que se establezca la conexión o se agote el tiempo
        unsigned long timeout_waitConexion = 30000; // 30 segundos
        unsigned long auxStartTime = millis();
        while ((!hayConexionWiFi()) && (millis() - auxStartTime < timeout_waitConexion)) 
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

            return; // Salir del bucle de reintento
        } 
        else 
        {
            #if defined(SM_DEBUG)
            SerialPC.println(F("\nNo se pudo establecer la conexion. Reintentando..."));
            #endif
            // Eliminar esta linea en el programa final:
            //SerialESP32Due.println("Fallo en conexión WiFi. Reintentando..."); // No hace falta, se pregunta después
        }
    }

    // Si tras 30 segundos, reintentándolo cada 10s, no se ha establecido la conexión:
    #if defined(SM_DEBUG)
    SerialPC.println(F("Unable to establish WiFi connection.\n"));
    #endif
    //SerialESP32Due.println(F("NO-WIFI")); // No hace falta, se pregunta después

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
 */
/*-----------------------------------------------------------------------------*/
void sendJsonToDatabase(DynamicJsonDocument& JSONdoc)
{
    if(hayConexionWiFi())
    {
        // Convertir el documento JSON en una cadena
        String jsonString;
        serializeJson(JSONdoc, jsonString);

        // Configurar la petición HTTP
        HTTPClient http;
        http.begin(post_testServerName);
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
            #if defined(SM_DEBUG)
            SerialPC.println();
            SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
            #endif
            // No se puede poner directamente SerialPC.println("\n" + httpResponseCode); porque se imprimen cosas raras

            String response = http.getString(); // Obtener la respuesta del servidor
            #if defined(SM_DEBUG)
            SerialPC.println(response);         // Imprimir la respuesta del servidor
            #endif

            if((httpResponseCode >= 200) && (httpResponseCode < 300)){
                SerialESP32Due.println(F("SAVED-OK"));
            }
            else{
                String errorResponse = "ERROR-HTTP: " + String(httpResponseCode);
                SerialESP32Due.println(errorResponse); 
            }
        }
        else
        {
            #if defined(SM_DEBUG)
            SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
            #endif
            String errorResponse = "ERROR-HTTP: " + String(httpResponseCode);
            SerialESP32Due.println(errorResponse);
        }

        // Cerrar la conexión
        http.end();
    }
    else
    {
        #if defined(SM_DEBUG)
        SerialPC.println(F("WiFi desconectado"));
        #endif
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

        fetchTokenFromServer(bearerToken);          // 1. Pedir token de autenticación
        uploadJSONtoServer(JSONdoc,bearerToken);    // 2. Enviar JSON
        logoutFromServer(bearerToken);              // 3. Cerrar sesión
    }
    else
    {
        #if defined(SM_DEBUG)
        SerialPC.println(F("No tengo wifi"));
        #endif
        SerialESP32Due.println(F("NO-WIFI"));
    }
}



/*-----------------------------------------------------------------------------*/
/**
 * Realiza una petición HTTP POST al servidor para obtener un token de autenticación.
 * 
 * @param bearerToken Referencia a una cadena de caracteres donde se almacenará el token obtenido.
 */
/*-----------------------------------------------------------------------------*/
void fetchTokenFromServer(String &bearerToken)
{
    #if defined(SM_DEBUG)
    SerialPC.println("\n1. Pidiendo token...");
    #endif

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
        //#if defined(SM_DEBUG)
        //SerialPC.println();
        //SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
        //#endif

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
            #if defined(SM_DEBUG)
            SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
            #endif

            SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode); 
            
            bearerToken = "";
        }
    }
    else
    {
        #if defined(SM_DEBUG)
        SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
        #endif
        
        SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode); 

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
void uploadJSONtoServer(DynamicJsonDocument& JSONdoc, String &bearerToken)
{
    #if defined(SM_DEBUG)
    SerialPC.println("\n2. Subiendo JSON...");
    #endif

    // Convertir el documento JSON en una cadena
    String jsonString;
    serializeJson(JSONdoc, jsonString);

    // Configurar la petición HTTP: un POST con la info de la comida en el body y el token en el header
    HTTPClient http;
    //http.begin(post_testServerName);
    http.begin(post_ComidaServerName);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + bearerToken); // Añadir el token de autenticación

    // Enviar la petición HTTP
    int httpResponseCode = http.POST(jsonString);

    // Comprobar el código de respuesta HTTP
    if(httpResponseCode > 0)
    {
        //#if defined(SM_DEBUG)
        //SerialPC.println();
        //SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
        //#endif

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
        #if defined(SM_DEBUG)
        SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
        #endif

        SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode);
    }

    // Cerrar la conexión
    http.end();
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
    #if defined(SM_DEBUG)
    SerialPC.println("\n3. Cerrando sesion...");
    #endif

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
            bool status = doc["status"];
            String message = doc["message"];     
            
            #if defined(SM_DEBUG)
            if(status && message == "User logged out successfully") SerialPC.println("Cierre de sesion exitoso");
            else  SerialPC.println("Error al cerrar la sesion\n");
            #endif
        }
        else{
            #if defined(SM_DEBUG)
            SerialPC.print(F("\nError en la petición HTTP GET: ")); SerialPC.println(httpResponseCode);
            #endif
            
            SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode); 
        }
        
    }
    else {
        #if defined(SM_DEBUG)
        SerialPC.print(F("\nError en la petición HTTP GET: ")); SerialPC.println(httpResponseCode);
        #endif

        SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode);
    }
    http.end();
}



#endif