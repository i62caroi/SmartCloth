/**
 * @file wifi_functions.h
 * @brief Este archivo contiene funciones auxiliares para convertir enviar el JSON a la database.
 * 
 * @author Irene Casares Rodríguez
 * @date 13/01/2024
 */

#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#define SerialPC Serial
#define SerialESP32Due Serial1

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

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


// Variable global para almacenar la dirección MAC
String macAddress;


// URL del servidor donde enviar el JSON
const char* serverName = "http://smartcloth.site/post-esp32-data-json.php"; 



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
        SerialPC.print("Conectando a WiFi...");
        WiFi.begin(ssid, password);

        // Esperar hasta que se establezca la conexión o se agote el tiempo
        unsigned long timeout_waitConexion = 30000; // 30 segundos
        unsigned long auxStartTime = millis();
        while ((!hayConexionWiFi()) && (millis() - auxStartTime < timeout_waitConexion)) 
        {
            delay(500);
            SerialPC.print(".");
        }

        // Comprobar si se estableció la conexión
        if (hayConexionWiFi()) 
        {
            SerialPC.println("");
            SerialPC.print("Conectado a red WiFi con IP: ");
            SerialPC.println(WiFi.localIP());
            SerialESP32Due.println("Conexion WiFi correcta");

            return; // Salir del bucle de reintento
        } 
        else 
        {
            SerialPC.println("No se pudo establecer la conexion. Reintentando...");
            SerialESP32Due.println("Fallo en conexión WiFi. Reintentando...");
        }
    }

    // Si tras 30 segundos, reintentándolo cada 10s, no se ha establecido la conexión:
    SerialPC.println("Unable to establish WiFi connection.");
    SerialESP32Due.println("NO WIFI");

}



/*-----------------------------------------------------------------------------*/
/**
 * @brief Convierte a String el JSON generado y lo envía a la base de datos.
 * @param JSONdoc - documento JSON a enviar
 */
/*-----------------------------------------------------------------------------*/
void sendJsonDocToDatabase(DynamicJsonDocument& JSONdoc)
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
            String response = http.getString(); // Obtener la respuesta del servidor
            SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
            SerialPC.println(response);         // Imprimir la respuesta del servidor

            if((httpResponseCode >= 200) && (httpResponseCode < 300)){
                SerialESP32Due.println("JSON enviado correctamente");
            }
            else{
                SerialESP32Due.println("Error al enviar el JSON (servidor)");
            }
        }
        else
        {
            SerialPC.print("Error en la petición HTTP POST: ");
            SerialPC.println(httpResponseCode);

            SerialESP32Due.print("Error en la petición HTTP POST: ");
            SerialESP32Due.println(httpResponseCode);
        }

        // Cerrar la conexión
        http.end();
    }
    else
    {
        SerialPC.println("WiFi desconectado");
        SerialESP32Due.println("WiFi desconectado");
    }
}





#endif