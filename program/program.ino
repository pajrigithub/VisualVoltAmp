#include <WiFi.h>
#include <IOXhop_FirebaseESP32.h>
#include "Wire.h"
#include "Adafruit_INA219.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#define i2c_Address 0x3c

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define FIREBASE_HOST "https://visual-volt-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "lAvtj5J9phZqbYGXRNvjUj5CKU7XE0FSoCvTN1YQ"
#define WIFI_SSID "KONSIKI LT1"
#define WIFI_PASSWORD "KonsIKI1"

#define push_button 38
#define dip_switch 12
#include "gambar.h"

Adafruit_INA219 ina219;

const float calibrationFactor = 0.7586;
float voltage = 0;
float current_mA = 0;

float jenis_pengukuran;
int analogValue, x = 0, i = 0;
String data[] = { "data1", "data2", "data3", "data4", "data5", "data6", "data7", "data8", "data9", "data10" };
String mode[] = { "voltase/", "ampere/" };

unsigned long waktuSebelumnya = 0;
const long selangWaktu = 1000;  // selang waktu dalam milidetik

void baca_arus() {
  current_mA = ina219.getCurrent_mA();
  Serial.println("Nilai Arus: " + String(current_mA) + "mA");
  if (current_mA < 0) {
    current_mA = 0;
  } 
  display.clearDisplay();
  displayDisplayCenter(String(current_mA) + " mA");
  printText("Data ke " + String(i + 1) , 40, 50);
}


//pembacaan voltase
void baca_voltase() {
  analogValue = analogRead(17);
  voltage = analogValue / 4096.0 * 3.3 * (1 + 100000.0 / 16000.0) * calibrationFactor;
  if (voltage < 0) {
    voltage = 0;
  }
  Serial.println("Nilai voltage: " + String(voltage) + " V       ||       Nilai analog: " + String(analogValue));
  display.clearDisplay();
  displayDisplayCenter(String(voltage) + " V");
  printText("Data ke " + String(i + 1), 40, 50);
}

//mode pengukuran
void mode_pengukuran() {
  if (digitalRead(dip_switch) == LOW) {  //diswitch ke 1 = LOW || ke 0  = HIGH
    baca_voltase();
    jenis_pengukuran = voltage;
    x = 0;
  } else {
    baca_arus();
    jenis_pengukuran = current_mA;
    x = 1;
  }
}

//Untuk mengatur ke posisi tengah
void displayDisplayCenter(String text) {
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.getTextBounds(text, 0, 0, &x1, &y1, &width, &height);
  display.clearDisplay();
  display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
  display.println(text);  // text to display
  display.display();
}


void printText(String pesan, int x2, int y2) {
  display.setTextSize(0.2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(x2, y2);
  display.println(pesan);
  display.display();
}

void setup() {
  Serial.begin(9600);
  display.begin(i2c_Address, true);
  display.display();
  ina219.begin();

  pinMode(push_button, INPUT_PULLUP);
  pinMode(dip_switch, INPUT_PULLUP);
  display.clearDisplay();
  display.drawBitmap(0, 0, merk, 128, 64, 1);
  display.display();
  delay(3000);

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
  unsigned long waktuSekarang = millis();

  if (waktuSekarang - waktuSebelumnya >= selangWaktu) {
    waktuSebelumnya = waktuSekarang;
    mode_pengukuran();
  }
  if (digitalRead(push_button) == LOW) {
    Firebase.setFloat(mode[x] + data[i], jenis_pengukuran);
    Serial.print("data ");
    Serial.print(i);
    Serial.println(" terkirim");
    i += 1;
    if (i == 10) {
      i = 0;
    }
  }
  delay(2000);
}
