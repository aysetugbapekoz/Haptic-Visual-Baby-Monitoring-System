/*
  Baby Unit Code
  This code reads sound and motion data from sensors.
  If crying sound or movement is detected,
  an alarm signal is sent to the light unit and wearable unit
  using ESP-NOW communication.
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>

// Receiver MAC Addresses
// Light unit MAC address
uint8_t isikMAC[] = {0x24, 0x4C, 0xAB, 0x6D, 0x14, 0x78};

// Wearable bracelet MAC address
uint8_t bileklikMAC[] = {0xC8, 0x2B, 0x96, 0x1C, 0x22, 0x68};

// Sensor pin definitions
int sesSensorPin = A0;     
const int MPU_ADDR = 0x68;

// Motion threshold value
int hareketEsikDegeri = 4000;

// Real sound threshold value
int gercekSesEsigi = 300;

// Accelerometer values
int16_t accX, accY, accZ;
int16_t oncekiAccX, oncekiAccY, oncekiAccZ;

// Startup waiting period
unsigned long baslangicZamani = 0;
unsigned long baslangicBeklemeSuresi = 5000;

// Alarm state tracking
bool oncekiAlarmDurumu = false;
unsigned long sonGonderimZamani = 0;
unsigned long gonderimAraligi = 1000;

// Data packet structure
typedef struct veriPaketi {
  bool alarmDurumu;
} veriPaketi;

veriPaketi gonderilecekVeri;

// Function to read MPU6050 motion sensor
bool mpuOku() {

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B);

  byte hata = Wire.endTransmission(false);

  if (hata != 0) return false;

  Wire.requestFrom((uint8_t)MPU_ADDR, (size_t)6, true);

  accX = Wire.read() << 8 | Wire.read();
  accY = Wire.read() << 8 | Wire.read();
  accZ = Wire.read() << 8 | Wire.read();

  return true;
}

// Function to send alarm signal
void alarmGonder(bool alarmDurumu) {

  gonderilecekVeri.alarmDurumu = alarmDurumu;

  // Send signal to light unit
  esp_now_send(
    isikMAC,
    (uint8_t *) &gonderilecekVeri,
    sizeof(gonderilecekVeri)
  );

  // Send signal to wearable bracelet
  esp_now_send(
    bileklikMAC,
    (uint8_t *) &gonderilecekVeri,
    sizeof(gonderilecekVeri)
  );
}

void setup() {

  // Start serial communication
  Serial.begin(115200);

  delay(2000);

  // Initialize MPU6050 sensor
  Wire.begin(D2, D1);

  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B);
  Wire.write(0);
  Wire.endTransmission(true);

  delay(500);

  // Read initial accelerometer values
  if (mpuOku()) {

    oncekiAccX = accX;
    oncekiAccY = accY;
    oncekiAccZ = accZ;
  }

  baslangicZamani = millis();

  // Enable Wi-Fi station mode
  WiFi.mode(WIFI_STA);

  // Start ESP-NOW communication
  if (esp_now_init() != 0) {
    return;
  }

  // Set this device as controller (sender)
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Add receivers
  esp_now_add_peer(isikMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_add_peer(bileklikMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Send initial safe state
  alarmGonder(false);

  sonGonderimZamani = millis();

  Serial.println("Sistem Hazir.");
}

void loop() {

  // Wait for startup delay
  if (millis() - baslangicZamani < baslangicBeklemeSuresi) {

    delay(1000);
    return;
  }

  // ----- REAL SOUND DETECTION -----

  unsigned long baslama = millis();

  unsigned int sesMax = 0;
  unsigned int sesMin = 1024;

  // Listen for 50 ms
  while (millis() - baslama < 50) {

    unsigned int okunanDeger = analogRead(sesSensorPin);

    if (okunanDeger < 1024) {

      if (okunanDeger > sesMax)
        sesMax = okunanDeger;

      if (okunanDeger < sesMin)
        sesMin = okunanDeger;
    }
  }

  // Calculate sound intensity
  int gercekSesSiddeti = sesMax - sesMin;

  bool yuksekSesVar =
      (gercekSesSiddeti > gercekSesEsigi);

  // ----- MOTION DETECTION -----

  mpuOku();

  long farkX = abs(accX - oncekiAccX);
  long farkY = abs(accY - oncekiAccY);
  long farkZ = abs(accZ - oncekiAccZ);

  long toplamFark =
      farkX + farkY + farkZ;

  bool hareketVar =
      (toplamFark > hareketEsikDegeri);

  // ----- ALARM DECISION -----

  bool alarmVar =
      (yuksekSesVar || hareketVar);

  // Print sensor status
  Serial.print("Gercek Ses Hacmi: ");
  Serial.print(gercekSesSiddeti);

  Serial.print(" | Hareket: ");
  Serial.print(toplamFark);

  Serial.print(" | Durum: ");
  Serial.println(
      alarmVar ? "ALARM!" : "Sakin"
  );

  // Send data if state changes
  if (
      alarmVar != oncekiAlarmDurumu ||
      millis() - sonGonderimZamani >= gonderimAraligi
  ) {

    alarmGonder(alarmVar);

    oncekiAlarmDurumu = alarmVar;

    sonGonderimZamani = millis();
  }

  // Update previous motion values
  oncekiAccX = accX;
  oncekiAccY = accY;
  oncekiAccZ = accZ;

  delay(100);
}