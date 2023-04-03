# SFH V2 축사 시스템 by Arduino

### 이 코드는 축사의 온도와 습도를 측정하여, 축사의 LED 조명 및 팬을 자동으로 제어하는 코드입니다.

## [HTTP.ino](https://github.com/dev-trams/SFHbyArduino/blob/master/SFHBarn_V2/HTTP.ino)

이 코드는 Arduino와 같은 임베디드 시스템에서 HTTP 요청을 보내고 JSON 데이터를 파싱하는 데 사용됩니다. 함수는 WiFiClient 및 HTTPClient 라이브러리를 사용하여 작성되었습니다.

- httpPOSTSendJson 함수는 serverName과 Json 매개 변수를 받습니다. 이 함수는 HTTP POST 요청을 보내고 응답을 받아오는데, 이 때 Content-Type을 application/json으로 설정하여 JSON 형식으로 데이터를 보냅니다. 응답으로 받은 데이터는 문자열 형태의 payload 변수에 저장되며, 이 변수가 반환됩니다.

- httpGETRequest 함수는 serverName 매개 변수를 받습니다. 이 함수는 HTTP GET 요청을 보내고 응답을 받아옵니다. 만약 요청이 성공적으로 이루어졌다면, 응답 데이터는 문자열 형태의 payload 변수에 저장되며, 이 변수가 반환됩니다. 그렇지 않다면, 에러 코드가 시리얼 모니터에 출력됩니다.

- parseJSON 함수는 Readings 문자열과 ReadingsArr 배열을 매개 변수로 받습니다. 이 함수는 JSON.parse() 함수를 사용하여 JSON 문자열을 객체로 변환합니다. 변환된 객체를 사용하여 ReadingsArr 배열에 데이터를 저장합니다. 이 때, value1, value2 등과 같이 key값을 사용하여 JSON 객체의 값을 읽어오며, 이를 double 형식으로 변환하여 배열에 저장합니다.


## [SFHBarn.ino](https://github.com/dev-trams/SFHbyArduino/blob/master/SFHBarn_V2/SFHBarn.ino)

이 코드는 SFH의 메인 컨트롤러중 하나인 축사 컨트롤러의 ESP8266 마이크로컨트롤러를 사용하여 다양한 축사 시스템에 내장된 센서들을 활용하고, 와이파이를 통해 웹서버와 통신하는 IoT 기기의 메인 코드입니다.
코드의 주요 기능은 다음과 같습니다.

- WiFiManager: 이 기능은 ESP8266 기기가 처음 시작될 때 자동으로 AP (Access Point)를 생성하고, 스마트폰과 같은 다른 기기를 사용하여 웹브라우저를 열어 와이파이 네트워크에 연결할 수 있도록 해줍니다.

- NTPClient: 이 기능은 ESP8266 기기에서 내부적으로 시간을 동기화하도록 해주는 기능입니다. NTP 프로토콜을 사용하여 인터넷에서 시간 정보를 가져와 시간 동기화를 합니다.

- Adafruit_ADS1115: 이 기능은 4개의 아날로그 입력 핀 (A0 ~ A3)을 제공하는 I2C 인터페이스를 가진 확장보드인 ADS1115을 사용하여 아날로그 값을 읽어올 수 있습니다.

- DHT: 이 기능은 DHT22와 같은 온습도 센서에서 온도와 습도를 읽어올 수 있도록 해줍니다.

- 초음파 센서: 이 코드는 초음파 센서를 사용하여 물체까지의 거리를 측정할 수 있습니다.

- 네오픽셀 라이브러리: 이 기능은 네오픽셀 LED를 제어하기 위해 사용됩니다.

- HTTPClient: 이 기능은 HTTP 프로토콜을 사용하여 웹서버와 통신할 수 있도록 해줍니다.

- JSON 라이브러리: 이 기능은 JSON 형식의 데이터를 파싱하고 생성할 수 있도록 해줍니다.

- digitalWrite: 이 함수는 LED와 같은 디지털 출력 핀을 제어할 수 있도록 해줍니다.

### 기능

코드 구성 요소

1. void setup() <br> Arduino 보드에서 처음 한번 실행되며, 초기 설정을 수행합니다.

2. void loop() <br> setup() 함수 실행 후, 계속해서 반복적으로 실행됩니다.
온도와 습도를 측정하고, LED와 팬을 제어하는 코드를 포함합니다.

3. float readDHTTemperature() <br> DHT11 센서를 통해 온도를 측정하는 함수입니다. <br> 반환 값은 측정된 온도값입니다.

4. float readDHTHumidity() <br> DHT11 센서를 통해 습도를 측정하는 함수입니다. <br> 반환 값은 측정된 습도값입니다.

5. String LEDBarn(float h ,float hs ,float t , float ts) <br> LED 조명의 On/Off 여부를 결정하는 함수입니다. <br> h : 현재 습도 값, hs : 습도 센서에서 정한 경계 값, t : 현재 온도 값, ts : 온도 센서에서 정한 경계 값 <br> 습도 값이 정한 경계 값보다 작으면 LED가 켜집니다. <br> 온도 값이 정한 경계 값보다 크면 LED가 켜집니다. <br> 반환 값은 LED 조명 On/Off 여부를 나타내는 문자열입니다.

6. String PANBarn(float h,float hs ,float t , float ts, int PANYN) <br> 팬의 On/Off 여부를 결정하는 함수입니다. <br> h : 현재 습도 값, hs : 습도 센서에서 정한 경계 값, t : 현재 온도 값, <br> ts : 온도 센서에서 정한 경계 값, PANYN : 팬 On/Off 여부를 나타내는 변수 (1 : On, 0 : Off) <br> 습도 값이 정한 경계 값보다 크면 팬이 켜집니다. <br> 온도 값이 정한 경계 값보다 크면 팬이 켜집니다. <br> PANYN 변수가 1이면 팬이 항상 켜집니다. <br> 반환 값은 팬 On/Off 여부를 나타내는 문자열입니다.

## [WHERE.ino](https://github.com/dev-trams/SFHbyArduino/blob/master/SFHBarn_V2/WHERE.ino)

- LEDBarn() <br> 함수는 h, hs, t, ts 네 개의 실수형(float) 변수를 입력받아서 문자열(String) 형태의 결과 값을 반환합니다. 이 함수는 LEDBarn이라는 이름의 장소에서 온도(t)와 습도(h)를 입력받아, hs(습도 최소치)보다 낮은 습도이거나 ts(온도 최대치)보다 높은 온도일 경우 "Y"라는 문자열을 반환하고, 그 외의 경우에는 "N"을 반환합니다.

- PANBarn() <br> 함수도 LEDBarn 함수와 마찬가지로 h, hs, t, ts 네 개의 실수형(float) 변수를 입력받아서 문자열(String) 형태의 결과 값을 반환합니다. 이 함수는 PANBarn이라는 이름의 장소에서 온도(t), 습도(h)와 PANYN이라는 정수형(int) 변수를 입력받아, hs(습도 최대치)보다 높은 습도이거나 ts(온도 최대치)보다 높은 온도, 또는 PANYN이 1일 경우에는 "Y"라는 문자열을 반환하고, 그 외의 경우에는 "N"을 반환합니다.
