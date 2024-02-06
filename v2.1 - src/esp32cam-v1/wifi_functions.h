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
//const char* ssid = "Irene";               // Nombre red
//const char* password =  "icradeba5050";   // Contraseña

const char* ssid = "UCOTEAM";
const char* password = "-polonio210alfileres-";


// URL del servidor donde enviar el JSON
const char* serverName = "http://smartcloth.site/post-esp32-data-json.php"; 
//const char* serverName = "https://smartclothweb.org/api/comidas"; // URL del servidor smartcloth.web oficial




/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
bool    hayConexionWiFi();  // Comprobar si hay conexión WiFi
void    connectToWiFi();    // Conectar a la red WiFi

void    sendJsonToDatabase(DynamicJsonDocument& JSONdoc);    // Enviar el JSON generado al servidor
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
/*-----------------------------------------------------------------------------*/
void connectToWiFi() 
{
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
    SerialPC.println(F("Unable to establish WiFi connection."));
    #endif
    //SerialESP32Due.println(F("NO-WIFI")); // No hace falta, se pregunta después

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
        http.begin(serverName);
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


#endif