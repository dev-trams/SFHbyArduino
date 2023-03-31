String LEDBarn(float h ,float hs ,float t , float ts) {
  String result;
  if (h < hs - 2 || t > ts + 5) {
    result = "Y";
  } else {
    result = "N";
  }
  return result;
}
String PANBarn(float h,float hs ,float t , float ts, int PANYN) {
  String result;
  if (h > hs + 2 || t > ts + 5 || PANYN == 1) {
    result = "Y";
  } else {
    result = "N";
  }
  return result;
}
