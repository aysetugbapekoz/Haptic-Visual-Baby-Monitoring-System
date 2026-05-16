#include <ESP8266WiFi.h>

void setup(){
  Serial.begin(115200);
  Serial.println();
  Serial.print("Bu Kartin MAC Adresi: ");
  Serial.println(WiFi.macAddress());
}

void loop(){
  // Bu kod sadece adres bulmak içindir
}

