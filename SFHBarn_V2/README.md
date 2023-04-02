# SFH V2 축사 시스템 by Arduino

## HTTP.ino

이 코드는 Arduino와 같은 임베디드 시스템에서 HTTP 요청을 보내고 JSON 데이터를 파싱하는 데 사용됩니다. 함수는 WiFiClient 및 HTTPClient 라이브러리를 사용하여 작성되었습니다.

- httpPOSTSendJson 함수는 serverName과 Json 매개 변수를 받습니다. 이 함수는 HTTP POST 요청을 보내고 응답을 받아오는데, 이 때 Content-Type을 application/json으로 설정하여 JSON 형식으로 데이터를 보냅니다. 응답으로 받은 데이터는 문자열 형태의 payload 변수에 저장되며, 이 변수가 반환됩니다.

- httpGETRequest 함수는 serverName 매개 변수를 받습니다. 이 함수는 HTTP GET 요청을 보내고 응답을 받아옵니다. 만약 요청이 성공적으로 이루어졌다면, 응답 데이터는 문자열 형태의 payload 변수에 저장되며, 이 변수가 반환됩니다. 그렇지 않다면, 에러 코드가 시리얼 모니터에 출력됩니다.

- parseJSON 함수는 Readings 문자열과 ReadingsArr 배열을 매개 변수로 받습니다. 이 함수는 JSON.parse() 함수를 사용하여 JSON 문자열을 객체로 변환합니다. 변환된 객체를 사용하여 ReadingsArr 배열에 데이터를 저장합니다. 이 때, value1, value2 등과 같이 key값을 사용하여 JSON 객체의 값을 읽어오며, 이를 double 형식으로 변환하여 배열에 저장합니다.
