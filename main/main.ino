//#define BLYNK_TEMPLATE_ID           "TMPxxxxxx"
//#define BLYNK_TEMPLATE_NAME         "Device"

#define BLYNK_FIRMWARE_VERSION        "0.1.0"

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG

#define APP_DEBUG

#include "BlynkEdgent.h"
#include <DHT.h>
#include <TimeLib.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//define PIN use on ESP8266 only
#define DHTPIN 0  //D3
#define D5 14     //Fan Speed number 1  (D5)
#define D6 12     //Fan Speed number 2  (D6)
#define D7 13     //Fan Speed number 3  (D7)

#define DHTTYPE DHT11
// #define DHTTYPE DHT22

//Temperature Sensor
DHT dht(DHTPIN, DHTTYPE);

//LCD I2C Display
LiquidCrystal_I2C lcd(0x3f, 16, 2);

//Timer
BlynkTimer timer;

int nowSpeed = 0;   //fan speed number
int fanMode = 0;    //0 = manual mode
int lastSpeed = 0;  //for optimize

int relayPin[] = {
  D5, //D5
  D6, //D6
  D7  //D7
};

//function declearation
inline float getTemperature();
void myLoop();
inline void lcdDisplay(float temp, int _nowSpeed)
inline void resetFanSpeed();
void resetButton();
void setButton(int number);
inline void setFanSpeed(int number);

BLYNK_WRITE(V0) {
  fanMode = param.asInt();
}

//when click fan speed number 1
BLYNK_WRITE(V2) {
  int val = param.asInt();
  if(fanMode) {   //auto mode
    resetButton();
    setButton(nowSpeed);
  }else {         //manual mode
    nowSpeed = val ? 1 : 0;
  }
}

//when click fan speed number 2
BLYNK_WRITE(V3) {
  int val = param.asInt();
  if(fanMode) {   //auto mode
    resetButton();
    setButton(nowSpeed);
  }else {         //manual mode
    nowSpeed = val ? 2 : 0;
  }
}
//when click fan speed number 3
BLYNK_WRITE(V4) {
  int val = param.asInt();
  if(fanMode) {   //auto mode
    resetButton();
    setButton(nowSpeed);
  }else {         //manual mode
    nowSpeed = val ? 3 : 0;
  }
}

void setup()
{
  lcd.begin();
  lcd.backlight();
  lcd.cursor();

  dht.begin();

  Serial.begin(115200);

  delay(100);

  for(int i = 0; i < 3; i++) {
    pinMode(relayPin[i], OUTPUT);
    digitalWrite(relayPin[i], 1);
  }

  BlynkEdgent.begin();
  timer.setInterval(1000L, myLoop);
}

void loop() {
  BlynkEdgent.run();
  timer.run();
}

void myLoop() {
  float temp = getTemperature();
  if(temp == -1) {
    Serial.println(F("Failed to read from DHT sensor!"));
  }
  Serial.println(temp);
  Blynk.virtualWrite(V1, temp);

  if(fanMode) {     //when auto mode
    if(temp <= 30.00) {
      nowSpeed = 0;
    }else if(temp > 30 && temp <= 33){
      nowSpeed = 1;
    }else if(temp > 33 && temp <= 35) {
      nowSpeed = 2;
    }else {
      nowSpeed = 3;
    }
  }

  setFanSpeed(nowSpeed);
  lcdDisplay(temp, nowSpeed)
}

inline void lcdDisplay(float temp, int _nowSpeed) {
  lcd.clear();
  lcd.cursor();
  lcd.print("Temp : ");
  lcd.setCursor(8, 0);
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print("Speed Now is : ");
  lcd.print(nowSpeed);
}

inline float getTemperature() {
  //temperature get values
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  //check for ready
  if (isnan(h) || isnan(t) || isnan(f)) {
    return -1;
  }
  return dht.computeHeatIndex(t, h, false);
}

inline void resetFanSpeed() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(relayPin[i], 1);
  }
  Serial.println("Reset fan speed");
}

void resetButton() {
  Blynk.virtualWrite(V2, 0);
  Blynk.virtualWrite(V3, 0);
  Blynk.virtualWrite(V4, 0);
}

void setButton(int number) {
  switch (number) {
    case 1:
      Blynk.virtualWrite(V2, 1);
      break;
    case 2:
      Blynk.virtualWrite(V3, 1);
      break;
    case 3:
      Blynk.virtualWrite(V4, 1);
      break;
    default:
      break;
  }
}

inline void setFanSpeed(int number) {
  if(lastSpeed != number) {
    lastSpeed = number;
    resetFanSpeed();
    resetButton();
    if(number >= 1 && number <= 3) {
      digitalWrite(relayPin[number - 1], 0);
      setButton(number)
    }
  }
  Serial.print("Speed now is : ");
  Serial.println(number);
}