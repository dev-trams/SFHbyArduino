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
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
//와이파이매니저
WiFiManager wifiManager;
//와이파이 실시간
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600 * 9, 60000);
String realtime;
//확장보드
Adafruit_ADS1115 ads;  // ADS1115 객체를 생성합니다.
//온습도 센서
#define DHTTYPE DHT11
#define DHTPIN  D4
#define RETRY_LIMIT  20
DHT dht(DHTPIN, DHTTYPE);
//초음파 센서
#define in1Pin D5
#define in2Pin D6
#define in3Pin D7
#define in4Pin D8
//Your Domain name with URL path or IP address with path
String serverNameSend = "http://211.255.23.65/resfh/SFHIO.php?mod=input&tb=data";
String serverNameRequest1 = "http://211.255.23.65/resfh/SFHIO.php?mod=output&tb=io&app_loca=farm";
String parsing1 = "";
String serverNameRequest2 = "http://211.255.23.65/resfh/SFHIO.php?mod=output&tb=setup&app_loca=farm";
String parsing2 = "";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 1000;

String sensorReadings;
float YNReadingsArr[5];
float SETReadingsArr[5];

void setup() {
  ads.setGain(GAIN_ONE); // ADS1115를 초기화합니다.
  ads.begin();  // ADS1115를 초기화합니다.

  dht.begin();  //온습도 설정

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // 네오픽셀에 빛을 출력하기 위한 것인데 여기서는 모든 네오픽셀을 OFF하기 위해서 사용한다.
  strip.setBrightness(50); // 네오픽셀의 밝기 설정(최대 255까지 가능)

  // 출력 설정 
  pinMode(in1Pin, OUTPUT); // 모터드라이버 
  pinMode(in2Pin, OUTPUT);
  pinMode(in3Pin, OUTPUT);
  pinMode(in4Pin, OUTPUT);

  timeClient.begin();//NTPClient
  Serial.begin(115200);

  wifiManager.autoConnect("ESP8266_AP");//WiFiManager
  Serial.println("Connected to Wi-Fi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // put your setup code here, to run once:
}

void loop() {
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

  // Send an HTTP POST request depending on timerDelay
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String POSTJson = "{}";
      //온습도 측정
      int rtl = RETRY_LIMIT;
      float humi =  dht.readHumidity();// Read Humidity from DHT11 sensor
      float temp = dht.readTemperature(); // Read Temperature from DHT11 sensor
      delay(100);
      while (isnan(temp) || isnan(humi)) {
        Serial.println("Check sensor again - " + rtl);
        humi =  dht.readHumidity();
        temp = dht.readTemperature();
        delay(100);
      }
      //수위 측정
      int Water = adc0;
      //가스 측정
      int Gas = adc1;
      //토양 측정
      int Soil = adc2;
      //실시간 현재 시간 파싱
      int Hour = 0;
      int dump[5];
      timeClient.update();
      realtime = timeClient.getFormattedDate();
      const char* datatime = realtime.c_str();
      sscanf(datatime, "%d-%d-%d %d:%d:%d", &dump[0],&dump[1] ,&dump[2] ,&Hour ,&dump[3] , &dump[4]);
      Serial.print(dump[0]);
      Serial.print(dump[1]);
      Serial.print(dump[2]);
      /* JSON형식 "{\"app_code\":\"testbarn\"" +
        ",\"app_loca\":\" barn\"" +
        ",\"app_nm\": \"아두이노 \"" +
        ",\"temp\":" + String(temp) +
        ",\"humi\":" + String(humi) +
        ",\"ultr\":" + String(Ultr) +
        ",\"water\":" + String(Water) +
        ",\"gas\":" + String(Gas) + "}"
      */
      // Json으로 서버에 보내기  
      String Seting[] = {"testfarm", "farm", "arduino", "Y"};
      POSTJson = "{\"app_code\":\"" + Seting[0] +
                 "\",\"app_loca\":\"" + Seting[1] +
                 "\",\"app_nm\":\"" + Seting[2] +
                 "\",\"temp\":\"" + String(temp) +
                 "\",\"humi\":\"" + String(humi) +
                 "\",\"soil\":\"" + String(Soil) +
                 "\",\"water\":\"" + String(Water) +
                 "\",\"gas\":\"" + String(Gas) +
                 "\",\"yn\":\"" + Seting[3] +
                 "\",\"realtime\":\"" + realtime + "\"}";
      //POSTJson = "{\"name\":\"kim\",\"temp\":\""+String(temp)+"\"}";
      Serial.print(" 습도 ");
      Serial.print(humi);
      Serial.print(" 온도 ");
      Serial.print(temp);
      Serial.print(" 가스 ");
      Serial.print(Gas);
      Serial.print(" 토양 ");
      Serial.print(Soil);
      Serial.print(" 수위 ");
      Serial.println(Water);
      httpPOSTSendJson(serverNameSend, POSTJson );

      //서버에서 Json으로 가져오기 및 파싱
      sensorReadings = httpGETRequest(serverNameRequest1);
      Serial.println(sensorReadings);
      if (parsing1 != sensorReadings) {
        parsing1 = sensorReadings;
        JSONVar myObject = JSON.parse(sensorReadings);
        // JSON.typeof(jsonVar) can be used to get the type of the var
        if (JSON.typeof(myObject) == "undefined") {
          Serial.println("Parsing input failed!");
          return;
        }
        //Serial.print("JSON object = ");
        //Serial.println(myObject);
        for (int i = 0; i < 5; i++) {
          String key = "value" + String(i + 1);
          JSONVar value = myObject[key];
          /*Serial.print(key);
          Serial.print(" = ");
          Serial.println(value);*/
          YNReadingsArr[i] = double(value);
        }
      }
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
      
      // 파싱한 값과 센서 값으로 조건문 
      float Soilseting = 50.0;
      float tempseting = SETReadingsArr[0];
      float humiseting = SETReadingsArr[1];
      int timestart = int(SETReadingsArr[2]);
      int timeend = int(SETReadingsArr[3]);
      int LedYN = int(YNReadingsArr[0]);
      int PanYN = int(YNReadingsArr[1]);
      int PumpYN = int(YNReadingsArr[2]);
      String resLED = LEDfarm(Hour, timestart, timeend, LedYN);
      String resPUMP = PUMPfarm(Soil, Soilseting, PumpYN);
      String resFAN = PANfarm(humi, humiseting, temp, tempseting, PanYN);
      //팬
      if (resFAN == "Y") {
        digitalWrite(in1Pin, HIGH);
        digitalWrite(in2Pin, LOW);
      } else {
        digitalWrite(in1Pin, LOW);
        digitalWrite(in2Pin, LOW);
      }
      //펌프
      if (resPUMP == "Y") {
        digitalWrite(in3Pin, HIGH);
        digitalWrite(in4Pin, LOW);
      } else {
        digitalWrite(in3Pin, LOW);
        digitalWrite(in4Pin, LOW);
      }
      //
      if (resLED == "Y") {
        for (int i = 0; i < 60; i++)
        {
          strip.setPixelColor(i, strip.Color(255,   75,  0)); // Red
          strip.show();
        }
      } else {
        strip.clear();
        strip.show();
      }
    }
  }
  delay(1000);
  // put your main code here, to run repeatedly:
  

}
