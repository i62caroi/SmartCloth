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

#include "debug.h" // SM_DEBUG --> SerialPC

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
const char* ssid = "UCOTEAM";
const char* password = "-polonio210alfileres-";


// URL del servidor donde enviar el JSON
const char* fetchTokenServerName = "https://smartclothweb.org/api/mac";
const char* comidaServerName = "https://smartclothweb.org/api/comidas";
const char* logOutServerName = "https://smartclothweb.org/api/logout_mac";




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    hayConexionWiFi();  // Comprobar si hay conexión WiFi
void    connectToWiFi();    // Conectar a la red WiFi

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
    unsigned long timeout_waitConexion = 5000; // 5 segundos

    #if defined(SM_DEBUG)
        SerialPC.print(F("Conectando a WiFi..."));
    #endif
    WiFi.begin(ssid, password);

    // Mientras no se haya conectado a WiFi y mientras no hayan pasado 5 segundos.
    // Si se conecta o si pasan los 5 segundos, sale del while.
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
    } 
    else 
    {
        // Si tras 10 segundos no se ha establecido la conexión:
        #if defined(SM_DEBUG)
            SerialPC.println(F("\nNo se pudo establecer la conexion WiFi."));
        #endif
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
    http.begin(fetchTokenServerName);
    http.addHeader("Content-Type", "application/json");

    // JSON con MAC del esp32
    String macAddress = WiFi.macAddress();
    String requestBody = "{\"mac\":\"" + macAddress + "\"}";

    // Enviar la petición HTTP
    int httpResponseCode = http.POST(requestBody);

    // Comprobar el código de respuesta HTTP
    if(httpResponseCode > 0)
    {
        #if defined(SM_DEBUG)
            SerialPC.print("Respuesta HTTP: "); SerialPC.println(httpResponseCode); 
        #endif

        if((httpResponseCode >= 200) && (httpResponseCode < 300)){ // Petición exitosa
            // Obtener la respuesta del servidor
            String response = http.getString(); 

            // Extraer token de la respuesta recibida:
            DynamicJsonDocument doc(1024);
            deserializeJson(doc, response);
            bearerToken = doc["token"].as<String>();
            #if defined(SM_DEBUG)
                SerialPC.println("\nTOKEN: " + bearerToken + "\n");     
            #endif
        }
        else{
            #if defined(SM_DEBUG)
                SerialPC.print(F("A. Error pidiendo token: ")); SerialPC.println(httpResponseCode);
            #endif

            SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode); 
            
            bearerToken = "";
        }
    }
    else
    {
        #if defined(SM_DEBUG)
            SerialPC.print(F("B. Error pidiendo token: ")); SerialPC.println(httpResponseCode);
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
    http.begin(comidaServerName);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + bearerToken); // Añadir el token de autenticación

    // Enviar la petición HTTP
    int httpResponseCode = http.POST(jsonString);

    // Comprobar el código de respuesta HTTP
    if(httpResponseCode > 0)
    {
        #if defined(SM_DEBUG)
            SerialPC.print("Respuesta HTTP: "); SerialPC.println(httpResponseCode); 
        #endif

        String response = http.getString(); // Obtener la respuesta del servidor
        //SerialPC.println(response);         // Imprimir la respuesta del servidor

        if((httpResponseCode >= 200) && (httpResponseCode < 300)){
            #if defined(SM_DEBUG)
                SerialPC.print(F("Comida subida")); 
            #endif
            SerialESP32Due.println(F("SAVED-OK"));
        }
        else{
            #if defined(SM_DEBUG)
                SerialPC.print(F("A. Error subiendo comida: ")); SerialPC.println(httpResponseCode);
            #endif
            SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode); 
        }
    }
    else
    {
        #if defined(SM_DEBUG)
            SerialPC.print(F("B. Error subiendo comida: ")); SerialPC.println(httpResponseCode);
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
    http.begin(logOutServerName);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Authorization", "Bearer " + bearerToken);

    // Enviar la petición HTTP
    int httpResponseCode = http.POST(""); // Enviar un POST vacío con el token en el header

    // Comprobar el código de respuesta HTTP
    if(httpResponseCode>0)
    {
        #if defined(SM_DEBUG)
            SerialPC.print("Respuesta HTTP: "); SerialPC.println(httpResponseCode); 
        #endif

        if((httpResponseCode >= 200) && (httpResponseCode < 300)){ // Petición exitosa
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
            
            SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode); 
        }
        
    }
    else {
        #if defined(SM_DEBUG)
            SerialPC.print(F("B. Error cerrando sesión: ")); SerialPC.println(httpResponseCode);
        #endif

        SerialESP32Due.print(F("ERROR-HTTP: ")); SerialESP32Due.println(httpResponseCode);
    }
    http.end();
}



#endif