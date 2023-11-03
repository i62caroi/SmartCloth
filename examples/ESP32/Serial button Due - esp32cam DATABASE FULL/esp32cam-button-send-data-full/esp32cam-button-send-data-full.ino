/*
  Irene Casares Rodríguez
  30/10/23

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
      "SAVE-&carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X"
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
      "BARCODE-<codigo_barras_leido>:<carb>;<prot>;<kcal>"
 
 */

 

#include <WiFi.h>
#include <HTTPClient.h>

//#include <ArduinoJson.h> // Enviar datos en formato JSON en lugar de Formulario

#define RXD1 14
#define TXD1 15

#define SerialESP32Due Serial1


// Replace with your network credentials
const char* ssid     = "Irene";
const char* password = "icradeba5050";


// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://smartcloth.site/post-esp-data.php"; 


// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "apiKeySmartCloth";




struct Message {
  String command;
  String data;
};


// ----- FUNCIONES ------
Message getQuery(String msg);
void connectToWiFi();
void enviarPeticion(String comidaValues);



void setup() {
    // --- Serial esp32-PC ---
    Serial.begin(115200);
    while (!Serial);
    
    // --- Serial esp32-Due ---
    SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
    while (!SerialESP32Due);

    // Intentar conectarse a la red WiFi
    connectToWiFi();

    Serial.println("\n");
  
}



void loop() {
  
    // --- SE HA RECIBIDO MENSAJE ---
    if (SerialESP32Due.available() > 0) {
        // --- MENSAJE RECIBIDO ---
        String msgFromDue = SerialESP32Due.readStringUntil('\n');
        Serial.print("Mensaje recibido: "); Serial.println(msgFromDue); Serial.println();

        Message msgReceived = getQuery(msgFromDue);

        // --- RECIBIDO "SAVE" ---
        if (msgReceived.command == "SAVE-") {

            // --- HAY CONEXION WIFI ---
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("Enviando petición POST...");
                enviarPeticion(msgReceived.data); // Enviar valores de la comida
            } 

            // --- FALLO EN LA CONEXION WIFI ---
            else { 
                // A lo mejor no se puso el wifi al encender SM y se pone justo
                // antes de guardar, entonces habría que hacer un intento de conexión
                // por si acaso:
                //connectToWiFi();
                Serial.println("WiFi connection lost.");
                SerialESP32Due.println("NO-WIFI");
            }


        }
        // --- RECIBIDO OTRO MENSAJE --- 
        else{
          Serial.println("Comando no gestionado");
          SerialESP32Due.println("Comando no gestionado");
        }
        
    }

}






Message getQuery(String msg){
    // Objeto 'Message'
    Message myMessage;
    
    // Busca el carácter '-' en el mensaje
    int delimiterIndex = msg.indexOf('-');
    
    if (delimiterIndex != -1) { // Si existe el delimitador en la cadena
        myMessage.command = msg.substring(0, delimiterIndex + 1); // "SAVE-"
        myMessage.data = msg.substring(delimiterIndex + 1); // "&carb=X&carb_R=X&lip=X&lip_R=X&prot=X&prot_R=X&kcal=X&peso=X"
        
        Serial.print("Comando recibido: "); Serial.println(myMessage.command);
        Serial.print("Datos recibidos: "); Serial.println(myMessage.data);
        Serial.println();
    }
    /*else {
        myMessage.command = "NO";
        myMessage.data = "msg";
    }*/

    return myMessage;
}







void connectToWiFi() {
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
    // Send HTTP POST request
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
