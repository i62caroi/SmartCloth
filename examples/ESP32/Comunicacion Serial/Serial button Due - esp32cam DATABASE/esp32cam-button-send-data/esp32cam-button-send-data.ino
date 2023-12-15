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


float carb, carb_R, lip, lip_R, prot, prot_R, kcal, peso;




void setup() {
    // --- Serial esp32-PC ---
    Serial.begin(115200);
    while (!Serial);
    
    // --- Serial esp32-Due ---
    SerialESP32Due.begin(115200, SERIAL_8N1, RXD1, TXD1);
    while (!SerialESP32Due);

    // Intentar conectarse a la red WiFi
    connectToWiFi();
  
}



void loop() {
  
    // --- SE HA RECIBIDO MENSAJE ---
    if (SerialESP32Due.available() > 0) {
        // --- MENSAJE RECIBIDO ---
        String command = SerialESP32Due.readStringUntil('\n');
        Serial.print("Mensaje recibido: "); Serial.println(command);

        // --- RECIBIDO "SAVE" ---
        if (command == "save") {

            // --- HAY CONEXION WIFI ---
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("Generando valores aleatorios...");
                generarValores();
                Serial.println("Enviando petición POST...");
                enviarPeticion();
                //msgToSend = false; //Peticion enviada
            } 

            // --- FALLO EN LA CONEXION WIFI ---
            else { // WiFi.status() != WL_CONNECTED
                /*Serial.println("WiFi connection lost. Retrying...");
                SerialESP32Due.println("Se ha perdido la conexión WiFi. Reintentando...");
                connectToWiFi();*/
                Serial.println("WiFi connection lost.");
                SerialESP32Due.println("Se ha perdido la conexión WiFi.");
            }


        }
        // --- RECIBIDO OTRO MENSAJE --- (no debería ocurrir)
        else{
          Serial.println("Command not valid");
          SerialESP32Due.println("Command not valid");
        }
        
    }

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
    //SerialESP32Due.println("No es posible establecer la conexión WiFi");

}



void generarValores(){
    // --- GENERAR VALORES ALEATORIOS ---
    // Generar valores aleatorios para guardar en la database
    carb = random(5,30) + 0.5; // + 0.5 para ver si se muestran decimales, porque con .0 se trunca el valor
    carb_R = round(carb/10);
    lip = random(5,30) + 0.5;
    lip_R = round(lip/10);
    prot = random(5,30) + 0.5;
    prot_R = round(prot/10);
    kcal = (carb * 4) + (lip * 9) + (prot * 4);
    peso = random(50, 150) + 0.5;

    // ----- 1. FORMATO DEL CONTENIDO: FORMULARIO -----
    Serial.println();
    Serial.print(carb); Serial.print(" "); Serial.print(carb_R); Serial.print(" ");
    Serial.print(lip); Serial.print(" "); Serial.print(lip_R); Serial.print(" ");
    Serial.print(prot); Serial.print(" "); Serial.print(prot_R); Serial.print(" ");
    Serial.print(kcal); Serial.print(" "); Serial.print(peso); Serial.println("\n");
    // --- fin FORMULARIO ---

    /*
    // ----- 2. FORMATO DEL CONTENIDO: JSON -----
    // Crear un objeto JSON para almacenar los datos
    DynamicJsonDocument jsonDoc(200);  // Ajusta el tamaño según tus necesidades

    // Agregar datos al objeto JSON
    jsonDoc["api_key"] = apiKeyValue;
    jsonDoc["carb"] = carb;
    jsonDoc["carb_R"] = carb_R;
    jsonDoc["lip"] = lip;
    jsonDoc["lip_R"] = lip_R;
    jsonDoc["prot"] = prot;
    jsonDoc["prot_R"] = prot_R;
    jsonDoc["kcal"] = kcal;
    jsonDoc["peso"] = peso;

    // Serializar el objeto JSON en una cadena
    String jsonString;
    serializeJson(jsonDoc, jsonString);
    */// --- fin JSON ---
}


void enviarPeticion(){
    // --- CLIENTES WIFI Y HTTP ----
    WiFiClient client;
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);

    // 1. FORMATO DEL CONTENIDO: FORMULARIO 
    // Specify content-type header --> Formulario
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // 2. FORMATO DEL CONTENIDO: JSON 
    // Specify content-type header --> JSON
    //http.addHeader("Content-Type", "application/json");


    // ------------ PETICION HTTP POST ------------------------
    // ----- 1. FORMATO DEL CONTENIDO: FORMULARIO -----
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&carb=" + String(carb) + "&carb_R=" + String(carb_R)
                          + "&lip=" + String(lip) + "&lip_R=" + String(lip_R) 
                          + "&prot=" + String(prot) + "&prot_R=" + String(prot_R) 
                          + "&kcal=" + String(kcal) + "&peso=" + String(peso) + "";

    Serial.print("httpRequestData: "); Serial.println(httpRequestData);
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    // --- fin FORMULARIO ---

    // ----- 2. FORMATO DEL CONTENIDO: JSON -----
    // Enviar la solicitud HTTP con el cuerpo de JSON
    //int httpResponseCode = http.POST(jsonString);
    // --- fin JSON ---
    // -------------- FIN PETICION -----------------------------

      // --- RESPUESTA DEL SERVIDOR A LA PETICION ---
      if (httpResponseCode>0) {
          Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
          SerialESP32Due.print("HTTP Response code: "); SerialESP32Due.println(httpResponseCode);
      }
      else {
          Serial.print("Error code: "); Serial.println(httpResponseCode);
          SerialESP32Due.print("Error code: "); SerialESP32Due.println(httpResponseCode);
      }

      // Free resources
      http.end();
}
