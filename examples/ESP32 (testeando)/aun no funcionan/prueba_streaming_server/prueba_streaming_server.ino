

// hello from esp32!


#include "esp_camera.h"
#include <WiFi.h>
#include <WebServer.h>
#include "soc/soc.h" //disable brownout problems
#include "soc/rtc_cntl_reg.h"  //disable brownout problems

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22


//Replace with your network credentials
const char* ssid = "Irene";
const char* password = "icradeba5050";

WebServer server(80);


void setupCamera(){
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG; 
    //config.pixel_format = PIXFORMAT_GRAYSCALE;

    if(psramFound()){
      //Serial.println("Tiene psram");
      //config.frame_size = FRAMESIZE_UXGA;
      //config.frame_size = FRAMESIZE_SVGA;
      config.frame_size = FRAMESIZE_VGA;
      config.jpeg_quality = 10;
      //config.jpeg_quality = 8;
      config.fb_count = 2;
    } else {
      config.frame_size = FRAMESIZE_SVGA;
      config.jpeg_quality = 12;
      config.fb_count = 1;
    }

    // Camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
      Serial.printf("Camera init failed with error 0x%x", err);
      return;
    }
}


void handleRoot() {
  // Incluye una etiqueta HTML para mostrar el video de la cámara
  String html = "<html><body>";
  html += "<img src=\"/video\" width=\"640\" height=\"480\">"; // Ajusta el tamaño del video
  html += "</body></html>";

  server.send(200, "text/html", html);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}



void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector
 
    Serial.begin(115200);
    Serial.setDebugOutput(false);

    setupCamera();

    // Wi-Fi connection
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());


    // Servidor
    server.on("/", handleRoot);

    // Manejar la solicitud de video
    server.on("/video", HTTP_GET, [](){
      server.sendHeader("Content-Type", "multipart/x-mixed-replace; boundary=frame");
      while (true) {
        camera_fb_t *fb = esp_camera_fb_get();
        if (!fb) {
          break;
        }
        /*server.sendHeader("Content-Length", String(fb->len));
        server.sendHeader("Content-Disposition", "inline; filename=capture.jpg");
        server.send(200, "image/jpeg", false, (const char *)fb->buf, fb->len);*/
        server.sendContent_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
        esp_camera_fb_return(fb);
      }
    });

    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");

}




void loop() {
    server.handleClient();
    delay(2);  // Permitir que la CPU realice otras tareas

}
