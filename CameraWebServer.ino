#include "esp_camera.h"
#include <WiFi.h>
#define CAMERA_MODEL_XIAO_ESP32S3
#include "camera_pins.h"
#include "AsyncUDP.h"
#define PWM_MAX 255

const char* ssid_AP = "ap_ssid";
const char* password_AP = "ap_password";

AsyncUDP udp;

void startCameraServer();

int f = 0;
void setup() {
  Serial.begin(115200);
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
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_QVGA;
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_LATEST;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 10;
  config.fb_count = 2;
  
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  pinMode(D3, OUTPUT);
  pinMode(D4, OUTPUT);

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  // sensor_t * s = esp_camera_sensor_get();

  WiFi.mode(WIFI_AP);  
  WiFi.softAP(ssid_AP, password_AP);
  Serial.println(WiFi.softAPIP());
  WiFi.setSleep(false);
  if (udp.listen(8888)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    
    udp.onPacket([](AsyncUDPPacket packet) {
        char x[packet.length()];
        strcpy(x, (char*)packet.data());
        int z = atoi(x);
        //>101 lub <-101 to lewo
        f = z;
        
        
    });
  }
  startCameraServer();
}

void leftMotor(int V){
  if (V>0){
    V -= 100;
    V = map(V, 0, 100, 0, PWM_MAX);
    digitalWrite(D1, LOW);
    digitalWrite(D2, HIGH);
    analogWrite(D0, V);
  }else{
    V += 100;
    V = abs(V);
    V = map(V, 0, 100, 0, PWM_MAX);
    digitalWrite(D1, HIGH);
    digitalWrite(D2, LOW);
    analogWrite(D0, V);
  }
}

void rightMotor(int V){
  if (V>0){
    V = map(V, 0, 100, 0, PWM_MAX);
    digitalWrite(D3, HIGH);
    digitalWrite(D4, LOW);
    analogWrite(D5, V);
  }else{
    V = abs(V);
    V = map(V, 0, 100, 0, PWM_MAX);
    digitalWrite(D3, LOW);
    digitalWrite(D4, HIGH);
    analogWrite(D5, V); 
  }
}
void loop() {
  if(f<200 && f>-200){
        if(f>=101){
          leftMotor(f);
          rightMotor(50);
          Serial.println("Lewo");
          
        }else if(f<=-101){
          leftMotor(f);
          rightMotor(-50);
          Serial.println("Tył Lewo");
        } else if(f<0){
          rightMotor(f);
          leftMotor(-150);
          Serial.println("Tył prawo");
        } else if(f>0){
          rightMotor(f);
          leftMotor(150);
          Serial.println("Prawo");
        } else{
          analogWrite(D0, 0);
          analogWrite(D5, 0);
        }
    }
}
