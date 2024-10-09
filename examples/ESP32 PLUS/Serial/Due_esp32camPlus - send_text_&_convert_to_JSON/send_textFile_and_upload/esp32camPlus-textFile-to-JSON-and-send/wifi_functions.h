/**
 * @file wifi_functions.h
 * @brief Este archivo contiene funciones para enviar el JSON a la database.
 * 
 * @author Irene Casares Rodríguez
 * @date 13/01/2024
 */

#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

//#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>      // Para serializar el JSON y enviarlo
#include "Serial_functions.h" // Funciones de interacción con el Serial ESP32-Due



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
const char* ssid = "MOVISTAR_FB23";
const char* password = "DP6BUuEtuFvRw3mHmFoG";

// Testeos:
//const char* ssid = "Irene";
//const char* password = "icradeba5050";

// Post testeos:
//const char* ssid = "SmartCloth";
//const char* password = "SM-pass24/";
// -----------------------------------


// URL del servidor donde enviar el JSON
// Servidor de testeo
const char* post_testServerName = "http://smartcloth.site/post-esp32-data-json.php"; 

// ------- DATABASE SMARTCLOTH -------
// URL del servidor donde enviar el JSON
const char* fetchTokenServerName = "https://smartclothweb.org/api/mac";
const char* comidaServerName = "https://smartclothweb.org/api/comidas";
const char* logOutServerName = "https://smartclothweb.org/api/logout_mac";
// ----------------------------------


/*-----------------------------------------------------------------------------
                           DEFINICIONES FUNCIONES
-----------------------------------------------------------------------------*/
inline bool    hayConexionWiFi(){ return (WiFi.status()== WL_CONNECTED); };  // Comprobar si hay conexión a la red WiFi

void    setupWiFi();                    // Configurar modo del WiFi y conectar a la red, si se puede
void    connectToWiFi();    // Conectar a la red WiFi
void    checkWiFi();        // Comprobar si hay WiFi e indicarlo al Due

void    sendJsonToDatabase_fullProcess(DynamicJsonDocument& JSONdoc);           // Enviar el JSON generado al servidor
bool    fetchTokenFromServer(String &bearerToken);                              // 1. Pedir token
void    uploadJSONtoServer(DynamicJsonDocument& JSONdoc, String &bearerToken);  // 2. Subir JSON
void    logoutFromServer(String &bearerToken);                                  // 3. Cerrar sesión


void    uploadJSONtoServer_testServer(DynamicJsonDocument& JSONdoc);  // Subir JSON
/*-----------------------------------------------------------------------------*/

void scanNetworks() {
    Serial.println("Escaneando redes WiFi...");
    int n = WiFi.scanNetworks();
    if (n == 0) {
        Serial.println("No se encontraron redes");
    } else {
        Serial.println("Redes encontradas:");
        for (int i = 0; i < n; ++i) {
            Serial.printf("%d: %s, Señal: %d dBm\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
            delay(10);
        }
    }
}

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
            SerialDue.println(F("WIFI"));

            return; // Salir del bucle de reintento
        } 
        else 
        {
            SerialPC.println(F("\nNo se pudo establecer la conexion. Reintentando..."));
            // Eliminar esta linea en el programa final:
            SerialDue.println("Fallo en conexión WiFi. Reintentando...");
        }
    }

    // Si tras 30 segundos, reintentándolo cada 10s, no se ha establecido la conexión:
    SerialPC.println(F("Unable to establish WiFi connection."));
    SerialDue.println(F("NO-WIFI"));

    SerialPC.println();

}*/


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
    unsigned long timeout_waitConexion = 30000; // 30 segundos

    SerialPC.print("Conectando a red "); SerialPC.println(ssid);
    SerialPC.print(F("Conectando a WiFi..."));
    WiFi.begin(ssid, password);

    // Mientras no se haya conectado a WiFi y mientras no hayan pasado 5 segundos.
    // Si se conecta o si pasan los 5 segundos, sale del while.
    //while ((!hayConexionWiFi()) && (millis() - startTime < timeout_waitConexion)) 
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
        // No hace falta indicar al Due que se tiene WiFi, se pregunta después
    } 
    else 
    {
        // Si tras 10 segundos no se ha establecido la conexión:
        SerialPC.println(F("\nNo se pudo establecer la conexion WiFi."));
        SerialPC.print("Estado de WiFi: "); SerialPC.println(WiFi.status());
        // No hace falta indicar al Due que NO se tiene WiFi, se pregunta después
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
    if(hayConexionWiFi())
    {
        SerialPC.println(F("\nTengo Wifi"));
        sendMsgToDue(F("WIFI-OK"));
    }
    else 
    { 
        SerialPC.println(F("\nNo tengo wifi"));
        sendMsgToDue(F("NO-WIFI"));
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
        // Si falla la obtención de token, indica el error HTTP y no intenta subir la data

        if(bearerToken != ""){ // Se ha obtenido token
            uploadJSONtoServer(JSONdoc,bearerToken);    // 2. Enviar JSON
            // Si se devuelve SAVED-OK, da igual que falle el logout
            logoutFromServer(bearerToken);              // 3. Cerrar sesión
        }
        else{
            SerialPC.println("Error al pedir token");
        }
    }
    else
    {
        SerialPC.println(F("No tengo wifi"));
        
        //SerialDue.println(F("NO-WIFI"));
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
    // Pide el token si sigue teniendo conexión
    if(hayConexionWiFi())
    {
        SerialPC.println("\n1. Pidiendo token...");

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
            SerialPC.println();
            SerialPC.print("Respuesta pedir token: "); SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP

            if((httpResponseCode >= 200) && (httpResponseCode < 300)){ // Petición exitosa
                // Obtener la respuesta del servidor
                String response = http.getString(); 

                // Extraer token de la respuesta recibida:
                DynamicJsonDocument doc(1024);
                deserializeJson(doc, response);
                bearerToken = doc["token"].as<String>();
                SerialPC.println("\nTOKEN: " + bearerToken + "\n");    
            }
            else{
                SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
                
                //SerialDue.print(F("ERROR-HTTP:")); SerialDue.println(httpResponseCode); 
                //String errorHttp = "ERROR-HTTP:" + httpResponseCode;
                //sendMsgToDue(errorHttp);
                sendMsgToDue("ERROR-HTTP:" + String(httpResponseCode));

                bearerToken = "";
            }
        }
        else
        {
            SerialPC.print(F("\nError en la petición HTTP GET: ")); SerialPC.println(httpResponseCode);
            
            //SerialDue.print(F("ERROR-HTTP:")); SerialDue.println(httpResponseCode); 
            //String errorHttp = "ERROR-HTTP:" + httpResponseCode;
            //sendMsgToDue(errorHttp);
            sendMsgToDue("ERROR-HTTP:" + String(httpResponseCode));

            bearerToken = "";
        }

        // Cerrar la conexión
        http.end();

    }
    else{
        SerialPC.println(F("\nNo se puede PEDIR TOKEN porque ha perdido la conexion a Internet"));
        
        //SerialDue.println(F("NO-WIFI"));
        
        bearerToken = "";
    }


    // Si se ha obtenido el token, devolver true
    if(bearerToken != ""){ 
        SerialPC.println("Token obtenido.");
        return true;
    }
    else{
        SerialPC.println("Falla token. No se continua.");
        return false;
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
    // Sube la comida si sigue teniendo conexión
    if(hayConexionWiFi())
    {
        SerialPC.println("\n2. Subiendo JSON...");
        SerialPC.println("\nTOKEN: " + bearerToken + "\n"); 

        // Convertir el documento JSON en una cadena
        String jsonString;
        serializeJson(JSONdoc, jsonString);

        // Configurar la petición HTTP: un POST con la info de la comida en el body y el token en el header
        HTTPClient http;
        //http.begin(post_testServerName);
        http.begin(comidaServerName);
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
            SerialPC.println();
            SerialPC.print("Respuesta subir comida: "); SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
            // No se puede poner directamente SerialPC.println("\n" + httpResponseCode); porque se imprimen cosas raras

            //String response = http.getString(); // Obtener la respuesta del servidor
            //SerialPC.println(response);         // Imprimir la respuesta del servidor

            if((httpResponseCode >= 200) && (httpResponseCode < 300)){
                SerialPC.println("Comida subida\n");

                //SerialDue.println(F("SAVED-OK"));
                sendMsgToDue(F("SAVED-OK"));
            }
            else{
                SerialPC.println("Error en subir comida");

                //SerialDue.print(F("ERROR-HTTP:")); SerialDue.println(httpResponseCode); 
                //String errorHttp = "ERROR-HTTP:" + httpResponseCode;
                //sendMsgToDue(errorHttp);
                sendMsgToDue("ERROR-HTTP:" + String(httpResponseCode));
            }
        }
        else
        {
            SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);

            //SerialDue.print(F("ERROR-HTTP:")); SerialDue.println(httpResponseCode); 
            //String errorHttp = "ERROR-HTTP:" + httpResponseCode;
            //sendMsgToDue(errorHttp);
            sendMsgToDue("ERROR-HTTP:" + String(httpResponseCode));
        }

        // Cerrar la conexión
        http.end();

    }
    else{
        SerialPC.println(F("\nNo se puede SUBIR LA COMIDA porque ha perdido la conexion a Internet"));
        
        //SerialDue.println(F("NO-WIFI"));
        sendMsgToDue(F("NO-WIFI"));
    }

}


void uploadJSONtoServer_testServer(DynamicJsonDocument& JSONdoc)
{
    // Sube la comida si sigue teniendo conexión
    if(hayConexionWiFi())
    {
        SerialPC.println("\n2. Subiendo JSON...");

        // Convertir el documento JSON en una cadena
        String jsonString;
        serializeJson(JSONdoc, jsonString);

        // Configurar la petición HTTP: un POST con la info de la comida en el body y el token en el header
        HTTPClient http;
        http.begin(post_testServerName); // "http://smartcloth.site/post-esp32-data-json.php"
        http.addHeader("Content-Type", "application/json");

        // Enviar la petición HTTP
        int httpResponseCode = http.POST(jsonString);

        // Comprobar el código de respuesta HTTP
        if(httpResponseCode > 0)
        {
            SerialPC.println();
            SerialPC.print("Respuesta subir comida: "); SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP
            // No se puede poner directamente SerialPC.println("\n" + httpResponseCode); porque se imprimen cosas raras

            //String response = http.getString(); // Obtener la respuesta del servidor
            //SerialPC.println(response);         // Imprimir la respuesta del servidor

            if((httpResponseCode >= 200) && (httpResponseCode < 300)){
                SerialPC.println("Comida subida\n");
                sendMsgToDue(F("SAVED-OK"));
            }
            else{
                SerialPC.println("Error en subir comida");
                sendMsgToDue("ERROR-HTTP:" + String(httpResponseCode));
            }
        }
        else
        {
            SerialPC.print(F("\nError en la petición HTTP POST: ")); SerialPC.println(httpResponseCode);
            sendMsgToDue("ERROR-HTTP:" + String(httpResponseCode));
        }

        // Cerrar la conexión
        http.end();

    }
    else{
        SerialPC.println(F("\nNo se puede SUBIR LA COMIDA porque ha perdido la conexion a Internet"));
        sendMsgToDue(F("NO-WIFI"));
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
    // Cierra sesión si sigue teniendo conexión
    //
    // Creo que no pasa nada si se queda abierta, se cerrará a la media hora por si fallara el cerrar sesión y 
    // pocos minutos después se quisiera guardar más comidas. Lo que no sé es que si se queda abierta, si se
    // puede volver a pedir token con la misma MAC. 
    if(hayConexionWiFi())
    {
        SerialPC.println("\n3. Cerrando sesión...");
        SerialPC.println("\nTOKEN: " + bearerToken + "\n"); 

        // Configurar la petición HTTP: un GET con el token en el header y el body vacío
        HTTPClient http;
        http.begin(logOutServerName);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "Bearer " + bearerToken);

        // Enviar la petición HTTP
        int httpResponseCode = http.POST(""); // Enviar un POST vacío con el token en el header

        // Comprobar el código de respuesta HTTP
        if(httpResponseCode > 0)
        {
            SerialPC.println();
            SerialPC.print("Respuesta logout: "); SerialPC.println(httpResponseCode); // Imprimir el código de respuesta HTTP

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
                
                //SerialDue.print(F("ERROR-HTTP:")); SerialDue.println(httpResponseCode); 
                //String errorHttp = "ERROR-HTTP:" + httpResponseCode;
                //sendMsgToDue(errorHttp);
                sendMsgToDue("ERROR-HTTP:" + String(httpResponseCode));
            }
            
        }
        else {
            SerialPC.print(F("\nError en la petición HTTP GET: ")); SerialPC.println(httpResponseCode);
            
            //SerialDue.print(F("ERROR-HTTP:")); SerialDue.println(httpResponseCode); 
            //String errorHttp = "ERROR-HTTP:" + httpResponseCode;
            //sendMsgToDue(errorHttp);
            sendMsgToDue("ERROR-HTTP:" + String(httpResponseCode));
        }
        http.end();

    }
    else{
        SerialPC.println(F("\nNo se puede CERRAR SESION porque ha perdido la conexion a Internet"));
        
        //SerialDue.println(F("NO-WIFI"));
        sendMsgToDue(F("NO-WIFI"));
    }

}







#endif