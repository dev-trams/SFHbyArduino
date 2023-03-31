#include <NTPClient.h>//와이파이 실시간
#include <WiFiUdp.h>//와이파이 실시간
#include <WiFiManager.h>//와이파이 매니저 
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h>
#include <DHT.h>//온습도 센서
#include <Wire.h>
#include <SoftwareSerial.h>//소프트웨어 통신
#include <Adafruit_MLX90614.h>
//와이파이매니저
WiFiManager wifiManager;
//와이파이 실시간
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 3600 * 9, 60000);
String realtime;
#define RX D6                                      // 소프트웨어 RX포트로 사용할 핀 번호 정의
#define TX D7                                      // 소프트웨어 TX포트로 사용할 핀 번호 정의
//온습도 센서
#define DHTTYPE DHT22
#define DHTPIN  D4
#define RETRY_LIMIT  20
DHT dht(DHTPIN, DHTTYPE);
Adafruit_MLX90614 mlx = Adafruit_MLX90614();    // 비접촉 온도센서 라이브러리 생성

SoftwareSerial Arduino_to_Radar_Serial(RX, TX);   // 소프트웨어 시리얼 객체명 선언(설정 보드의 RX 핀, 설정보드의 TX 핀)
int Radar_Signal_Read_Pin = D5;                    // Radar 모듈 신호선(3번핀)과 아두이노에 연결된 핀번호

int val;                                      // Read한 Radar 모듈 신호선의 값을 저장할 변수
int state;
float spd;
float tb;
bool flag_Radar = 1;
bool flag = 1;
String Receive_Char;

//Your Domain name with URL path or IP address with path
String serverNameSend = "http://192.168.0.2/testCRUD/reSFH/SFHIO.php?mod=input&tb=data";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 1000;

void setup() {
  Serial.begin(115200);
  dht.begin();//온습도

  timeClient.begin();//NTPClient

  mlx.begin();

  pinMode(Radar_Signal_Read_Pin, INPUT); // 디지털 7번핀을 GPIO INPUT으로 설정
  pinMode(LED_BUILTIN, OUTPUT); // LED_BUILTIN는 아두이노 보드에 자체적으로 달린 LED이며, 이를 Signal Out핀으로 설정

  Arduino_to_Radar_Serial.begin(9600); // Arduino_to_Radar_Serial 객체는 Arduino와 Radar 모듈과의 시리얼 통신을 담당하며, Baud Rate = 9600 설정


  wifiManager.autoConnect("ESP8266_AP");//WiFiManager
  Serial.println("Connected to Wi-Fi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  // put your setup code here, to run once:
}

void loop() {
  // Send an HTTP POST request depending on timerDelayint rtl = RETRY_LIMIT;

  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      String POSTJson = "{}";

      float humi =  dht.readHumidity();// Read Humidity from DHT11 sensor
      float temp = dht.readTemperature(); // Read Temperature from DHT11 sensor
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

      tb = mlx.readObjectTempC();
      while(tb > 100 ){
        tb = mlx.readObjectTempC();
      }
      if (!isnan(tb) ) {
        Serial.print("Body Temperature: ");
        Serial.print(tb);
        Serial.println(" *C");
      }

      if (Serial.available())
      {
        // read the incoming byte:
        char Transmit_Char = Serial.read(); // 시리얼 모니터를 통해 전송할 데이터를 1바이트씩 읽고 Transmit_Char 변수에 저장
        Arduino_to_Radar_Serial.write(Transmit_Char); // Transmit_Char 변수에 저장된 1바이트 값을 Radar 모듈로 송신
        Serial.print(Transmit_Char); // 아두이노 사용자가 자신이 전송한 데이터를 알 수 있도록 Transmit_Char 변수에 저장된 1바이트 값을 시리얼 모니터에 표시
      }

      // Radar 모듈 -> 아두이노 -> 시리얼 모니터
      Arduino_to_Radar_Serial.flush();
      if (Arduino_to_Radar_Serial.available())
      {
        Receive_Char = Arduino_to_Radar_Serial.readStringUntil(0x0A);
        // read the incoming byte:
        Serial.println(Receive_Char); // Radar 모듈이 보낸 데이터를 아두이노 사용자가 알 수 있도록 Receive_Char 변수에 저장된 1바이트 값을 시리얼 모니터에 표시
        // Radar 모듈이 보낸 데이터를 1바이트씩 읽고 Receive_Char 변수에 저장
        if (flag_Radar != 1) {
          Arduino_to_Radar_Serial.write("MotionDataEnable 1");
          flag_Radar = 0;
        }
        if (Receive_Char.length() < 37) {
          String removechar[] = {"MotionData", "+", "-"};
          for (int i = 0; i < 3; i++) {
            Receive_Char.replace(removechar[i], "");
          }
          const char* ch = Receive_Char.c_str();
          sscanf(ch, " %d %f %d", &val, &spd, &state);
          //Serial.println(Receive_Char);
        }
        Serial.print(val);
        Serial.print("/");
        Serial.print(spd);
        Serial.print("/");
        Serial.print(state);
        Serial.println("/");
        flag = 1;
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
      POSTJson = "{}";
      Serial.print("습도");
      Serial.print(humi);
      Serial.print("  온도");
      Serial.print(temp);
      Serial.print("  체온");
      Serial.print(tb);
      Serial.print("  운동량");
      Serial.print(val);
      Serial.print("  속도");
      Serial.print(spd);
      Serial.print("  움직여부");
      Serial.print(state);
      httpPOSTSendJson(serverNameSend, POSTJson );

    }
  }
  // put your main code here, to run repeatedly:

}
