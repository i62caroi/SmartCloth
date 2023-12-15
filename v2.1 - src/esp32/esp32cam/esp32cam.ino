/*
  Irene Casares Rodríguez
  11/12/23

  Cuando recibe "save" desde el Arduino Due, hace lo siguiente:
  1 - Comprueba si hay conexión Wifi
  1.1 - Si no hay, responde con "No hay conexión WiFi"
  2 - Conforma la query a mandar por petición HTTP POST
  3 - Envía la query
  4 - Envía al Arduino Due la respuesta HTTP recibida del servidor.
  
*/



/*
   -------- MENSAJES ARDUINO -> ESP32 --------------
  
  1. Guardar info en base de datos:
      "SAVE:&carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X"
      Esta opción ya está lista para la petición POST

      Proxima opción: "SAVE:<grupoX>-<peso>;<grupoY>-<peso>;<grupoZ>-<peso>;...." 
          Con esta opción se guardarían los pares 'grupoX-peso' en la base de datos y se calcularían los valores 
          de macronutrientes en el servidor al hacer GET. De esta forma, se podría llevar una trazabilidad de
          lo comido exactamente (a nivel grupo).

  2. Activar cámara y leer código de barras:
      "GET-BARCODE"

  
  -------- MENSAJES ESP32 -> ARDUINO -------------- 

  1. Guardado correctamente:
      "SAVED-OK"
  
  2. Error en el guardado (petición HTTP POST):
      "ERROR-HTTP:<codigo_error>"
  
  3. No hay wifi:
      "NO-WIFI"

  4. Código de barras leído (se consulta base de datos de alimentos y se obtienen sus valores de carb, lip, prot y kcal por gramo):
      "BARCODE:<codigo_barras_leido>:<carb>;<prot>;<kcal>"
 
 */

 





#define RXD1 14
#define TXD1 15




#include "wifi_functions.h"
#include "functions.h"
#include <ArduinoJson.h> 

#define SerialPC Serial



// ----- FUNCIONES ------
/*
void connectToWiFi();
void enviarPeticion(String comidaValues);
*/



void setup() {
    // --- Serial esp32-PC ---
    SerialPC.begin(115200);
    while (!SerialPC);
    
    // --- Serial esp32-Due ---
    SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
    while (!SerialESP32Due);

    // Intentar conectarse a la red WiFi
    //connectToWiFi();

    SerialPC.println("\n");
  
}



void loop() {
  
    // --- SE HA RECIBIDO MENSAJE ---
    if (SerialESP32Due.available() > 0) {
        // --- MENSAJE RECIBIDO ---
        String msgFromDue = SerialESP32Due.readStringUntil('\n');
        msgFromDue.trim();
        SerialPC.print("Mensaje recibido: "); SerialPC.println(msgFromDue); SerialPC.println();


        // --- RECIBIDO "SAVE" --- 
        if (msgFromDue.startsWith("SAVE-INICIO")) {
            createJson(msgFromDue);
        }
        else if (msgFromDue.startsWith("SAVE-FIN")) {
            serializeJsonPretty(doc, SerialPC);
        }
        else if (msgFromDue.startsWith("GET-BARCODE")) {
            // Activar la cámara y leer el código de barras
            SerialPC.println("Comando barcode\n");
        }
        // --- RECIBIDO OTRO MENSAJE --- 
        else{
          SerialPC.println("Comando no gestionado");
          SerialESP32Due.println("Comando no gestionado");
        }
        
    }

}








/*void connectToWiFi() {
    // Inicializar startTime
    unsigned long startTime = millis();

    // Esperar hasta que se establezca la conexión o se agote el tiempo
    unsigned long timeout_Reintento = 30000; // 30 segundos

    while ((WiFi.status() != WL_CONNECTED) && (millis() - startTime < timeout_Reintento)) {
        Serial.print("Connecting to WiFi...");
        WiFi.begin(ssid, password);

        // Esperar hasta que se establezca la conexión o se agote el tiempo
        unsigned long timeout_waitConexion = 10000; // 10 segundos
        while ((WiFi.status() != WL_CONNECTED) && (millis() - startTime < timeout_waitConexion)) {
            delay(500);
            Serial.print(".");
        }

        // Comprobar si se estableció la conexión
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("");
            Serial.print("Connected to WiFi network with IP Address: ");
            Serial.println(WiFi.localIP());
            //SerialESP32Due.println("Conexion WiFi correcta");
            return; // Salir del bucle de reintento
        } else {
            Serial.println("Unable to establish WiFi connection. Retrying...");
            //SerialESP32Due.println("Fallo en conexión WiFi. Reintentando...");
        }
    }

    // Si tras 30 segundos, reintentándolo cada 10s, no se ha establecido la conexión:
    Serial.println("Unable to establish WiFi connection.");
    //SerialESP32Due.println("NO WIFI");

}







void enviarPeticion(String comidaValues){
    // --- CLIENTES WIFI Y HTTP ----
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // Specify content-type header --> Formulario
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");


    // ------------ PETICION HTTP POST ------------------------
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + comidaValues + "";

    Serial.print("httpRequestData: "); Serial.println(httpRequestData); Serial.println();
    // Send HTTP POST request and obtein response
    int httpResponseCode = http.POST(httpRequestData);
    Serial.print("httpResponseCode: "); Serial.println(httpResponseCode); Serial.println();

      // --- RESPUESTA DEL SERVIDOR A LA PETICION ---
      if((httpResponseCode >= 200) and (httpResponseCode < 300)){// Todo OK 
          Serial.println("Guardado correcto\n");
          SerialESP32Due.println("SAVED-OK");
      }
      else {
          Serial.print("Error code: "); Serial.println(httpResponseCode); Serial.println();
          SerialESP32Due.print("ERROR-HTTP:"); SerialESP32Due.println(httpResponseCode);
      }

      // Free resources
      http.end();
}
*/