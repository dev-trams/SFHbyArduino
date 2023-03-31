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

  return payload;
}

void parseJSON(String Readings, float ReadingsArr[]) {
  JSONVar myObject = JSON.parse(Readings);
  if (JSON.typeof(myObject) == "undefined") {
    Serial.println("Parsing input failed!");
    return;
  }
  for (int i = 0; i < 5; i++) {
    String key = "value" + String(i + 1);
    JSONVar value = myObject[key];
    ReadingsArr[i] = double(value);
  }
}
