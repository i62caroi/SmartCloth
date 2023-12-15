/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-esp8266-mysql-database-php/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

*/


/*
  Irene Casares Rodríguez
  17/10/23

  Este programa genera valores aleatorios de macronutrientes, kcal y peso, y los envía
  mediante petición HTTP POST a un servidor encargado de obtener los valores de la query
  e insertarlos en la base de datos. 
  El programa realiza un envío cada 5 minutos.

*/

#include <WiFi.h>
#include <HTTPClient.h>
//#include <WiFiClientSecure.h> // HTTPS

// Replace with your network credentials
const char* ssid     = "Irene";
const char* password = "icradeba5050";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://smartcloth.site/post-esp-data.php"; 
//const char* serverName = "https://smartcloth.site/post-esp-data.php"; // HTTPS

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "apiKeySmartCloth";


float carb, carb_R, lip, lip_R, prot, prot_R, kcal, peso;


void setup() {
  Serial.begin(115200);
  while (!Serial);
  delay(1000); 
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

}

void loop() {
  // Generar valores aleatorios para guardar en la database
  carb = random(5,30) + 0.5; // + 0.5 para ver si se muestran decimales, porque con .0 se trunca el valor
  carb_R = round(carb/10);
  lip = random(5,30) + 0.5;
  lip_R = round(lip/10);
  prot = random(5,30) + 0.5;
  prot_R = round(prot/10);
  kcal = (carb * 4) + (lip * 9) + (prot * 4);
  peso = random(50, 150) + 0.5;

  Serial.println();
  Serial.print(carb); Serial.print(" "); Serial.print(carb_R); Serial.print(" ");
  Serial.print(lip); Serial.print(" "); Serial.print(lip_R); Serial.print(" ");
  Serial.print(prot); Serial.print(" "); Serial.print(prot_R); Serial.print(" ");
  Serial.print(kcal); Serial.print(" "); Serial.print(peso); Serial.println("\n");


  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    /* // HTTPS
    WiFiClientSecure *client = new WiFiClientSecure;
    client->setInsecure(); //don't use SSL certificate
    HTTPClient https;
    */

    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    // https.begin(*client, serverName); // HTTPS
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    //https.addHeader("Content-Type", "application/x-www-form-urlencoded"); // HTTPS
    

    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&carb=" + String(carb) + "&carb_R=" + String(carb_R)
                          + "&lip=" + String(lip) + "&lip_R=" + String(lip_R) 
                          + "&prot=" + String(prot) + "&prot_R=" + String(prot_R) 
                          + "&kcal=" + String(kcal) + "&peso=" + String(peso) + "";
    
    //Serial.print("httpRequestData: "); Serial.println(httpRequestData);
    
    // You can comment the httpRequestData variable above
    // then, use the httpRequestData variable below (for testing purposes)
    //String httpRequestData = "api_key=apiKeySmartCloth&carb=17&carb_R=2&lip=32&lip_R=3&prot=45&prot_R=4&kcal=176&peso=256";

    Serial.print("httpRequestData: "); Serial.println(httpRequestData);


    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    //int httpResponseCode = https.POST(httpRequestData); // HTTPS
     
    // If you need an HTTP request with a content type: text/plain
    //http.addHeader("Content-Type", "text/plain"); // HTTP
    //int httpResponseCode = http.POST("Hello, World!"); // HTTP
    //https.addHeader("Content-Type", "text/plain"); // HTTPS
    //int httpResponseCode = https.POST("Hello, World!"); // HTTPS
    
    // If you need an HTTP request with a content type: application/json, use the following:
    //http.addHeader("Content-Type", "application/json"); // HTTP
    //int httpResponseCode = http.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}"); // HTTP
    //https.addHeader("Content-Type", "application/json"); // HTTPS
    //int httpResponseCode = https.POST("{\"value1\":\"19\",\"value2\":\"67\",\"value3\":\"78\"}"); // HTTPS
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    //https.end(); // HTTPS
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 5 minutes
  delay(300000);  
}