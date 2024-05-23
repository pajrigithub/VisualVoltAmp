#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include "Wire.h"
#include "Adafruit_INA219.h"

#define FIREBASE_HOST "https://visualvoltamp-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "yjQwglb6zOZJEDIn3GPhE2ea0kvbbpZxd10nw6NT"
#define WIFI_SSID "KONSIKI LT1"
#define WIFI_PASSWORD "KonsIKI1"

#define push_button 23
#define dip_switch 7

Adafruit_INA219 ina219;

const float calibrationFactor = 0.7586;
float current_mA = 0;
float voltage = 0;
float jenis_pengukuran;
int analogValue, x = 0, i = 0;
String data[] = { "data1", "data2", "data3", "data4", "data5", "data6", "data7", "data8", "data9", "data10" };
String mode[] = { "Voltase/", "Ampere/" };


void baca_arus() {
  current_mA = ina219.getCurrent_mA();
  Serial.println("Nilai Arus: " + String(current_mA) + "mA");
}

//pembacaan voltase
void baca_voltase() {
  analogValue = analogRead(17);
  voltage = analogValue / 4096.0 * 3.3 * (1 + 100000.0 / 16000.0) * calibrationFactor;
  Serial.println("Nilai voltage: " + String(voltage) + " V       ||       Nilai analog: " + String(analogValue));
}
//mode pengukuran
void mode_pengukuran() {
  if (digitalRead(dip_switch) == LOW) {  //diswitch ke 1 = LOW || ke 0  = HIGH
    if (x = 1) {
      i = 0;
    }
    x = 0;
    jenis_pengukuran = voltage;
  } else {
    if (x = 0) {
      i = 0;
    }
    x = 1;
    jenis_pengukuran = current_mA;
  }
}

void setup() {
  Serial.begin(9600);
  ina219.begin();

  pinMode(push_button, INPUT_PULLUP);
  pinMode(dip_swuitch, INPUT_PULLUP);

  // connect to wifi.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

void loop() {
  baca_arus();
  baca_voltase();
  mode_pengukuran();
  if (digitalRead(push_button) == LOW) {
    Firebase.setFloat(mode[x] + data[i], jenis_pengukuran);
    i += 1;
    if (i == 9) {
      i = 0;
    } else {
    }
  } else {
    baca_arus();
    baca_voltase();
  }

  delay(1000);
}
