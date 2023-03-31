#include <NTPClient.h>  //와이파이 실시간
#include <WiFiUdp.h> //와이파이 실시간
#include <WiFiManager.h> //와이파이 매니저 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <Arduino_JSON.h> //JSON 라이브러리
#include <DHT.h>//온습도 센서
#include <Wire.h>
#include <Adafruit_ADS1X15.h>  //확장보드
#include <Adafruit_NeoPixel.h>  //네오픽셀 라이브러리

#define LED_PIN D3     //네오픽셀에 신호를 줄 핀번호
#define LED_COUNT 20  //아두이노에 연결된 네오픽셀의 개수
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
WiFiManager wifiManager;  //와이파이매니저
WiFiUDP ntpUDP;  //와이파이 실시간
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600 * 9, 60000);
String realtime;
Adafruit_ADS1115 ads;  // 확장보드 ADS1115 객체를 생성합니다.
#define DHTTYPE DHT22 //온습도 센서
#define DHTPIN  D4
#define RETRY_LIMIT  20
DHT dht(DHTPIN, DHTTYPE);
#define TRIG D7  //초음파 센서
#define ECHO D8
#define in1Pin D5
#define in4Pin D6
#define RelayPin D0

//Your Domain name with URL path or IP address with path
String serverNameSend = "http://211.255.23.65/resfh/SFHIO.php?mod=input&tb=data";
String serverNameRequest1 = "http://211.255.23.65/resfh/SFHIO.php?mod=output&tb=io&app_loca=barn";
String parsing1 = "";
String serverNameRequest2 = "http://211.255.23.65/resfh/SFHIO.php?mod=output&tb=setup&app_loca=barn";
String parsing2 = "";
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;
String sensorReadings;
float YNReadingsArr[5];
float SETReadingsArr[5];

void setup() {
  dht.begin();
  ads.setGain(GAIN_ONE); // ADS1115를 초기화합니다.
  ads.begin();  // ADS1115를 초기화합니다.
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // 네오픽셀에 빛을 출력하기 위한 것인데 여기서는 모든 네오픽셀을 OFF하기 위해서 사용한다.
  strip.setBrightness(50); // 네오픽셀의 밝기 설정(최대 255까지 가능)
  pinMode(TRIG, OUTPUT);  // 초음파 센서 설정
  pinMode(ECHO, INPUT);
  pinMode(LED_PIN, OUTPUT);  // 출력 설정
  pinMode(in1Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);
  pinMode(RelayPin, OUTPUT);
  timeClient.begin(); //NTPClient 실시간 타임
  Serial.begin(115200);
  wifiManager.autoConnect("ESP8266_AP");//WiFiManager
  Serial.println("Connected to Wi-Fi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  int adc0, adc1, adc2, adc3;
  adc0 = ads.readADC_SingleEnded(0);  // A0 핀에서 ADC 값을 읽습니다.
  adc1 = ads.readADC_SingleEnded(1);  // A1 핀에서 ADC 값을 읽습니다.
  adc2 = ads.readADC_SingleEnded(2);  // A2 핀에서 ADC 값을 읽습니다.
  adc3 = ads.readADC_SingleEnded(3);  // A3 핀에서 ADC 값을 읽습니다.
  adc0 = map(adc0, 0, 32767, 0, 1024); // ADC 값을 0에서 1024 사이의 값으로 변환합니다
  adc1 = map(adc1, 0, 32767, 0, 1024);
  adc2 = map(adc2, 0, 32767, 0, 1024);
  adc3 = map(adc3, 0, 32767, 0, 1024);
  if ((millis() - lastTime) > timerDelay) {
    if (WiFi.status() == WL_CONNECTED) {
      String POSTJson = "{}";
      int rtl = RETRY_LIMIT; //온습도 측정
      float humi =  dht.readHumidity();// Read Humidity from DHT11 sensor
      float temp = dht.readTemperature(); // Read Temperature from DHT11 sensor
      delay(100);
      while (isnan(temp) || isnan(humi)) {
        Serial.println("Check sensor again - " + rtl);
        humi =  dht.readHumidity();
        temp = dht.readTemperature();
        delay(100);
      }
      long duration, Ultr; //초음파 측정
      digitalWrite(TRIG, LOW);
      delayMicroseconds(2);
      digitalWrite(TRIG, HIGH);
      delayMicroseconds(10);
      digitalWrite(TRIG, LOW);
      duration = pulseIn (ECHO, HIGH);
      Ultr = duration * 17 / 1000;
      delay(100);
      int Water = adc0;  //수위 측정
      int Gas = adc1; //가스 측정
      timeClient.update();  //실시간
      realtime = timeClient.getFormattedDate();
      String Seting[] = {"testbarn", "barn", "arduino", "Y"};
      /*POSTJson = "{\"app_code\":\"" + Seting[0] + "\",\"app_loca\":\"" + Seting[1] +
                 "\",\"app_nm\":\"" + Seting[2] + "\",\"temp\":\"" + String(temp) +
                 "\",\"humi\":\"" + String(humi) + "\",\"ultr\":\"" + String(Ultr) +
                 "\",\"water\":\"" + String(Water) + "\",\"gas\":\"" + String(Gas) +
                 "\",\"yn\":\"" + Seting[3] + "\",\"realtime\":\"" + realtime + "\"}";*/
      JSONVar SendObject;
      SendObject["app_code"] = Seting[0]; SendObject["app_loca"] = Seting[1];
      SendObject["app_nm"] = Seting[2]; SendObject["temp"] = String(temp);
      SendObject["humi"] = String(humi); SendObject["ultr"] = String(Ultr);
      SendObject["water"] = String(Water); SendObject["gas"] = String(Gas);
      SendObject["yn"] = Seting[3]; SendObject["realtime"] = realtime;
      Serial.println(SendObject);
      POSTJson = JSON.stringify(SendObject);;
      Serial.print(" 습도 ");
      Serial.print(humi);
      Serial.print(" 온도 ");
      Serial.print(temp);
      Serial.print(" 가스 ");
      Serial.print(Gas);
      Serial.print(" 초음파 ");
      Serial.print(Ultr);
      Serial.print(" 수위 ");
      Serial.println(Water);
      httpPOSTSendJson(serverNameSend, POSTJson );

      sensorReadings = httpGETRequest(serverNameRequest1);
      while (sensorReadings == "{}") {
        sensorReadings = httpGETRequest(serverNameRequest1);
        Serial.println(sensorReadings);
      }
      Serial.println(sensorReadings);
      
      if (parsing1 != sensorReadings) {
        parsing1 = sensorReadings;
        parseJSON(sensorReadings, YNReadingsArr);
      }

      sensorReadings = httpGETRequest(serverNameRequest2);
      while (sensorReadings == "{}") {
        sensorReadings = httpGETRequest(serverNameRequest2);
        Serial.println(sensorReadings);
      }
      Serial.println(sensorReadings);

      if (parsing2 != sensorReadings) {
        parsing2 = sensorReadings;
        parseJSON(sensorReadings, SETReadingsArr);
      }
      float tempseting = SETReadingsArr[0];
      float humiseting = SETReadingsArr[1];
      int PanYN = int(YNReadingsArr[0]);
      int PumpYN = int(YNReadingsArr[1]);
      int MoterYN = int(YNReadingsArr[2]);
      String resLED = LEDBarn(humi, humiseting , temp , tempseting );
      String resPan = PANBarn(humi, humiseting , temp , tempseting , PanYN);
      if (PumpYN == 1) { //물 공급
        digitalWrite(RelayPin, HIGH);        //펌프
        delay(500);
        digitalWrite(RelayPin, LOW);
        delay(100);
      } else {
        digitalWrite(RelayPin, LOW);
        delay(100);
        Serial.println("펌프 : 작동 멈춤");
      }
      if (MoterYN == 1) {  //사료 모터
        int sp = 255;
        digitalWrite(in1Pin, HIGH);
        analogWrite(in1Pin, sp);
        delay(500);
        sp = 0;
        digitalWrite(in1Pin, LOW);
        analogWrite(in1Pin, sp);
        delay(100);
      } else {
        int sp = 0;
        digitalWrite(in1Pin, HIGH);
        analogWrite(in1Pin, sp);
        delay(100);
        Serial.println("모터 : 작동 멈춤");
      }
      if (resLED == "Y") {  //LED
        for (int i = 0; i < 60; i++)
        {
          strip.setPixelColor(i, strip.Color(255,   75,  0)); // Red
          strip.show();
        }
      } else {
        strip.clear();
        strip.show();
        Serial.println("LED : 작동 멈춤");
      }
      if (resPan == "Y") {   //환기 팬
        int sp = 255;
        digitalWrite(in4Pin, HIGH);
        analogWrite(in4Pin, sp);
        delay(500);
        sp = 0;
      } else {
        int sp = 0;
        digitalWrite(in4Pin, LOW);
        analogWrite(in4Pin, sp);
        delay(100);
        Serial.println("모터 : 작동 멈춤");
      }

    }
    delay(1000);
  }
}
