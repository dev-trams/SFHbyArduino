#include <NTPClient.h>//와이파이 실시간
#include <WiFiUdp.h>//와이파이 실시간
#include <WiFiManager.h>//와이파이 매니저 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <Arduino_JSON.h>//JSON 라이브러리
#include <DHT.h>//온습도 센서
#include <Wire.h>
#include <Adafruit_ADS1X15.h>//확장보드
#include <Adafruit_NeoPixel.h>//네오픽셀 라이브러리
#define LED_PIN D3     //네오픽셀에 신호를 줄 핀번호
#define LED_COUNT 20  //아두이노에 연결된 네오픽셀의 개수
#define RX D5// 미세먼지
#define TX D6// 미세먼지
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
#include <SoftwareSerial.h>//소프트웨어 통신
#include <PMS.h>//미세먼지 라이브러리
#include <Servo.h>//서브 모터
Servo servo1 ;
Servo servo2 ;

//와이파이매니저
WiFiManager wifiManager;
//와이파이 실시간
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600 * 9, 60000);
String realtime;
//확장보드
//Adafruit_ADS1115 ads;  // ADS1115 객체를 생성합니다.
//온습도 센서
#define DHTTYPE DHT22
#define DHTPIN  D4
#define RETRY_LIMIT  20
DHT dht(DHTPIN, DHTTYPE);
//적외선 근접센서
#define ViciPin D7
#define GasPin A0
//미세먼지 센서 및 설정
SoftwareSerial pms_seiral(RX, TX);
PMS pms(pms_seiral);
PMS::DATA data;

//Your Domain name with URL path or IP address with path
String serverNameSend = "http://211.255.23.65/resfh/SFHIO.php?mod=input&tb=data";
String serverNameRequest1 = "http://211.255.23.65/resfh/SFHIO.php?mod=output&tb=io&app_loca=home";
String parsing1 = "";
String serverNameRequest2 = "http://211.255.23.65/resfh/SFHIO.php?mod=output&tb=setup&app_loca=home";
String parsing2 = "";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 500;

String sensorReadings;
float YNReadingsArr[5];
float SETReadingsArr[5];

void setup() {
  /*
    ads.setGain(GAIN_ONE); // ADS1115를 초기화합니다.
    ads.begin();  // ADS1115를 초기화합니다.
  */

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // 네오픽셀에 빛을 출력하기 위한 것인데 여기서는 모든 네오픽셀을 OFF하기 위해서 사용한다.
  strip.setBrightness(50); // 네오픽셀의 밝기 설정(최대 255까지 가능)

  dht.begin();//온습도

  // 입력 설정
  //적외선근접센서
  pinMode(ViciPin, INPUT);
  //미세먼지 센서
  pms_seiral.begin(9600);

  // 출력 설정
  pinMode(LED_PIN, OUTPUT);
  pinMode(D1, OUTPUT);
  pinMode(D2, OUTPUT);
  servo1.attach(D0);
  servo2.attach(D8);

  timeClient.begin();//NTPClient
  Serial.begin(115200);

  wifiManager.autoConnect("ESP8266_AP");//WiFiManager
  Serial.println("Connected to Wi-Fi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // put your setup code here, to run once:
}

void loop() {
  /*
    int adc0, adc1, adc2, adc3;

    adc0 = ads.readADC_SingleEnded(0);  // A0 핀에서 ADC 값을 읽습니다.
    adc1 = ads.readADC_SingleEnded(1);  // A1 핀에서 ADC 값을 읽습니다.
    adc2 = ads.readADC_SingleEnded(2);  // A2 핀에서 ADC 값을 읽습니다.
    adc3 = ads.readADC_SingleEnded(3);  // A3 핀에서 ADC 값을 읽습니다.

    // ADC 값을 0에서 1024 사이의 값으로 변환합니다.
    adc0 = map(adc0, 0, 32767, 0, 1024);
    adc1 = map(adc1, 0, 32767, 0, 1024);
    adc2 = map(adc2, 0, 32767, 0, 1024);
    adc3 = map(adc3, 0, 32767, 0, 1024);
  */
  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String POSTJson = "{}";
      //온습도 측정
      int rtl = RETRY_LIMIT;
      float humi =  dht.readHumidity();// Read Humidity from DHT11 sensor
      float temp = dht.readTemperature(); // Read Temperature from DHT11 sensor
      /*
      if (!isnan(humi)) {//h 내용 처리
        Serial.print("Humidity: ");
        Serial.print(humi);
        Serial.print(" %\t");
        //return;
      }
      if ( !isnan(temp)) {//t 내용 처리
        Serial.print("Temperature: ");
        Serial.print(temp);
        Serial.println(" *C");
        //return;
      }
      */
      int Gas = analogRead(GasPin);
      int Vici = digitalRead(ViciPin);
      float dust = 0;
      while (dust == 0) {
        if (pms.read(data)) {
          dust = data.PM_AE_UG_2_5;// 미세먼지 값
        }
      }
      timeClient.update();
      realtime = timeClient.getFormattedDate();
      /* JSON형식 "{\"app_code\":\"testbarn\"" +
        ",\"app_loca\":\" barn\"" +
        ",\"app_nm\": \"아두이노 \"" +
        ",\"temp\":" + String(temp) +
        ",\"humi\":" + String(humi) +
        ",\"ultr\":" + String(Ultr) +
        ",\"water\":" + String(Water) +
        ",\"gas\":" + String(Gas) + "}"
      */

      String Seting[] = {"testbarn", "home", "Arduino", "Y"};
      JSONVar SendObject;
      SendObject["app_code"] = Seting[0]; SendObject["app_loca"] = Seting[1];
      SendObject["app_nm"] = Seting[2]; SendObject["temp"] = String(temp);
      SendObject["humi"] = String(humi); SendObject["dust"] = String(dust);
      SendObject["vici"] = String(Vici); SendObject["gas"] = String(Gas);
      SendObject["yn"] = Seting[3]; SendObject["realtime"] = realtime;
      Serial.println(SendObject);
      POSTJson = JSON.stringify(SendObject);;
      Serial.print("습도");
      Serial.println(humi);
      Serial.print("온도");
      Serial.println(temp);
      Serial.print("가스");
      Serial.println(Gas);
      Serial.print("미세먼지");
      Serial.println(dust);
      Serial.print("적외선");
      Serial.println(Vici);
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
      /*
        sensorReadings = httpGETRequest(serverNameRequest2);
        Serial.println(sensorReadings);
        if (parsing2 != sensorReadings) {
        parsing2 = sensorReadings;
        JSONVar myObject = JSON.parse(sensorReadings);
        // JSON.typeof(jsonVar) can be used to get the type of the var
        if (JSON.typeof(myObject) == "undefined") {
          Serial.println("Parsing input failed!");
          return;
        }
        Serial.print("JSON object = ");
        Serial.println(myObject);
        for (int i = 0; i < 5; i++) {
          String key = "value" + String(i + 1);
          JSONVar value = myObject[key];
          Serial.print(key);
          Serial.print(" = ");
          Serial.println(value);
          SETReadingsArr[i] = double(value);
        }
        }
      */
      //float humiseting = SETReadingsArr[0];
      //float tempseting = SETReadingsArr[1];
      int LEDYN = int(YNReadingsArr[0]);
      int DoorYN = int(YNReadingsArr[1]);
      int ValveYN = int(YNReadingsArr[2]);

      if (LEDYN == 1) {
        for (int i = 0; i < 10; i++)
        {
          strip.setPixelColor(i, strip.Color(255,   255,  255)); // Red
          strip.show();
        }
      } else {
        strip.clear();
        strip.show();
      }
      if (DoorYN == 1) {
        servo1.write(180);
        delay(30);
      } else {
        servo1.write(0);
        delay(15);
      }
      if (ValveYN == 1 || Gas >= 350) {
        servo2.write(90);
        delay(15);
      } else {
        servo2.write(0);
        delay(15);
      }
    }
    
  }
  delay(1000);
  // put your main code here, to run repeatedly:

}
