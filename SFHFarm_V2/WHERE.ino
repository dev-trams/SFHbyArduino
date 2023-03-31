String LEDfarm(int t,int Start,int End,int LEDYN) {
  Serial.println(t);
  String result;
  if ((t > Start && t < End) || LEDYN == 1 ) {
    result = "Y";
  } else {
    result = "N";
  }
  return result;
}
String PANfarm(float h, float hs, float t, float ts, int PANYN) {
  String result;
  if (h > hs + 2 || t > ts + 5 || PANYN == 1) {
    result = "Y";
  } else {
    result = "N";
  }
  return result;
}
String PUMPfarm(int h, float hs, int PUMPYN) {
  String result;
  if (h < hs*10 + 2 || PUMPYN == 1) {
    result = "Y";
  } else {
    result = "N";
  }
  return result;
}
