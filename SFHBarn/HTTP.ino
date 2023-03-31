//서버에 Json형식으로 보내는 함수 

String httpPOSTSendJson(String serverName, String Json) {
  WiFiClient client;
  HTTPClient http;

  // Your Domain name with URL path or IP address with path
  http.begin(client, serverName);

  // If you need an HTTP request with a content type: application/json, use the following:
  http.addHeader("Content-Type", "application/json");
  int httpResponseCode = http.POST(Json);

  String payload = "";

  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  payload = http.getString();
  Serial.println(payload);

  // Free resources
  http.end();

  return payload;
}

//서버에서 Json으로 받아롸서
String httpGETRequest(String serverName) {
  WiFiClient client;
  HTTPClient http;

  // Your IP address with path or Domain name with URL path
  http.begin(client, serverName);

  // If you need Node-RED/server authentication, insert user and password below
  //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

  // Send HTTP POST request
  int httpResponseCode = http.GET();

  String payload = "{}";

  if (httpResponseCode > 0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload; //못받으면 {} 받으면 파싱값
}
