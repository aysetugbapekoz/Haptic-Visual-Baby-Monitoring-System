#include <ESP8266WiFi.h>
#include <espnow.h>

// LED pin definitions
int anneLedPin = D1;  
int besikLedPin = D2;

// Data packet structure
typedef struct veriPaketi {
  bool alarmDurumu;
} veriPaketi;

veriPaketi gelenVeri;

// Function triggered when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {

  // Copy incoming data
  memcpy(&gelenVeri, incomingData, sizeof(gelenVeri));

  // If alarm is active
  if (gelenVeri.alarmDurumu == true) {

    Serial.println("Alarm geldi! Isiklar yaniyor.");

    // Turn on parent room LED instantly
    digitalWrite(anneLedPin, HIGH);

    // Slowly brighten cradle LED
    for(int i = 0; i <= 255; i++){
      analogWrite(besikLedPin, i);
      delay(15);
    }
  } 
  else {

    // Turn off LEDs
    Serial.println("Bebek sakin. Isiklar kapali.");

    digitalWrite(anneLedPin, LOW);
    analogWrite(besikLedPin, 0);
  }
}

void setup() {

  Serial.begin(115200);

  // Configure LED pins
  pinMode(anneLedPin, OUTPUT);
  pinMode(besikLedPin, OUTPUT);

  digitalWrite(anneLedPin, LOW);
  analogWrite(besikLedPin, 0);

  // Enable Wi-Fi station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("ESP-NOW Baslatilamadi!");
    return;
  }

  // Device role is receiver
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);

  // Register receive callback
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {}
}
