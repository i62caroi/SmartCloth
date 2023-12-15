#ifndef WIFI_FUNCTIONS_H
#define WIFI_FUNCTIONS_H

#include <WiFi.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid     = "Irene";
const char* password = "icradeba5050";


// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://smartcloth.site/post-esp-data.php"; 


// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "apiKeySmartCloth";


#endif