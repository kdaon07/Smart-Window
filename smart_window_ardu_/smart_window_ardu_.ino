#include <DFRobot_DHT11.h>
#include <Servo.h>
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#define WIFI_SSID "bssm_free"
#define WIFI_PASSWORD "bssm_free"
#define API_KEY "AIzaSyAlUkKCjlnXXI15SUrLCWEaMPnHPkgg6sA"
#define DATABASE_URL "https://smart-window-72fb6-default-rtdb.firebaseio.com/"
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
Servo myservo;
const int servoPin = D2;    
DFRobot_DHT11 DHT;
#define DHT11_PIN D3
int pos = 0;                                               // 모터 위치를 확인하기 위해 변수를 선언
void setup() {
  Serial.begin(115200);
  pinMode (servoPin, OUTPUT);           // 모터 제어핀을 출력으로 설정
  myservo.attach(servoPin);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);   
}
int temp, humi;
void loop() {
  temp = DHT.temperature;
  humi = DHT.humidity;
  DHT.read(DHT11_PIN);
  Serial.print(temp);
  Serial.print("  ");
  Serial.println(humi);
  delay(1000);
  if (Firebase.RTDB.setString(&fbdo, "/sensor/temp", String(temp))) {
    Serial.println("PASSED");
  }
  if (Firebase.RTDB.setString(&fbdo, "/sensor/humi", String(humi))) {
    Serial.println("PASSED");
  }
  if (Firebase.ready()) {
  if(Firebase.RTDB.getString(&fbdo, "/set/window") == 1)
      myservo.write(160);
  else
      myservo.attach(servoPin);
  }
}
