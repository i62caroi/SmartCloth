#include "esp_http_server.h"
#include "esp_camera.h"
#include "fb_gfx.h"
#include <WiFi.h>

#define RESET_GPIO_NUM   5
#define XCLK_GPIO_NUM    15
#define SIOD_GPIO_NUM    22
#define SIOC_GPIO_NUM    23

#define D0_GPIO_NUM      2    // Y2
#define D1_GPIO_NUM      14   // Y3
#define D2_GPIO_NUM      35   // Y4
#define D3_GPIO_NUM      12   // Y5
#define D4_GPIO_NUM      27   // Y6
#define D5_GPIO_NUM      33   // Y7
#define D6_GPIO_NUM      34   // Y8
#define D7_GPIO_NUM      39   // Y9

#define VSYNC_GPIO_NUM   18
#define HREF_GPIO_NUM    36
#define PCLK_GPIO_NUM    26

#define LED_GPIO_NUM 25

#define PART_BOUNDARY "123456789000000000000987654321"
static const char *_STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char *_STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char *_STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\nX-Timestamp: %d.%06d\r\n\r\n";

static esp_err_t frameHandler(httpd_req_t *req);
static esp_err_t streamHandler(httpd_req_t *req);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Start");
  pinMode(LED_GPIO_NUM, OUTPUT);
  digitalWrite(19, 0);

  WiFi.softAP("Upahead-Cam", "12345678");

  camera_config_t config;

  config.pin_d0 = D0_GPIO_NUM;
  config.pin_d1 = D1_GPIO_NUM;
  config.pin_d2 = D2_GPIO_NUM;
  config.pin_d3 = D3_GPIO_NUM;
  config.pin_d4 = D4_GPIO_NUM;
  config.pin_d5 = D5_GPIO_NUM;
  config.pin_d6 = D6_GPIO_NUM;
  config.pin_d7 = D7_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = -1;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;

  config.frame_size = FRAMESIZE_VGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 12;
  config.fb_count = 1;

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
   s->set_vflip(s, 1);

  Serial.println("Camera init OK.");

  httpd_handle_t server;
  httpd_config_t http_options = HTTPD_DEFAULT_CONFIG();
  err = httpd_start(&server, &http_options);
  if (ESP_OK != err)
  {
    Serial.println("Error");
    for (;;) delay(10000);
  }

  Serial.println("Httpd Ok");
  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  Serial.println("' to connect");

  httpd_uri_t uriFrameHandler = {
    .uri = "/frame",
    .method = HTTP_GET,
    .handler = frameHandler,
    .user_ctx = NULL
  };

  
  httpd_uri_t uriStreamHandler = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = streamHandler,
    .user_ctx = NULL
  };
  
  httpd_register_uri_handler(server, &uriFrameHandler);
  httpd_register_uri_handler(server, &uriStreamHandler);
}

static esp_err_t frameHandler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  esp_err_t res = ESP_OK;
  int64_t fr_start = esp_timer_get_time();
  fb = esp_camera_fb_get();

  if (!fb) {
    ESP_LOGE(TAG, "Camera capture failed");
    httpd_resp_send_500(req);
    return ESP_FAIL;
  }

  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  
  httpd_resp_send(req, (const char *)fb->buf, fb->len);
  
  esp_camera_fb_return(fb);
  
  return ESP_OK;
}

static esp_err_t streamHandler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  struct timeval _timestamp;
  esp_err_t res = ESP_OK;
  char *part_buf[128];

  static int64_t last_frame = 0;
  if (!last_frame) {
    last_frame = esp_timer_get_time();
  }
  
  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) {
    return res;
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  httpd_resp_set_hdr(req, "X-Framerate", "60");
  
  while (true) {
    fb = esp_camera_fb_get();

    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      _timestamp.tv_sec = fb->timestamp.tv_sec;
      _timestamp.tv_usec = fb->timestamp.tv_usec;
      if (res == ESP_OK)
        res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
      if (res == ESP_OK) {
        size_t hlen = snprintf((char *)part_buf, 128, _STREAM_PART, fb->len, _timestamp.tv_sec, _timestamp.tv_usec);
        res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
      }
      if (res == ESP_OK)
        res = httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
      if (fb) {
        esp_camera_fb_return(fb);
        fb = NULL;
      }
    }
  }

  return res;
    
}

void loop() {
  delay(500);
  pinMode(LED_GPIO_NUM, INPUT_PULLDOWN);
  delay(500);
  pinMode(LED_GPIO_NUM, INPUT);
  Serial.println("App running");
}
