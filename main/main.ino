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
#define D5 14     //Fan Speed number 1
#define D6 12     //Fan Speed number 2
#define D7 13     //Fan Speed number 3

#define DHTTYPE DHT11
// #define DHTTYPE DHT22

//Temperature Sensor
DHT dht(DHTPIN, DHTTYPE);

//LCD I2C Display
LiquidCrystal_I2C lcd(0x3f, 16, 2);

//Timer
BlynkTimer timer;

int nowSpeed = 0;
int fanMode = 0;   //0 = manual mode

int relayPin[] = {
  D5, //D5
  D6, //D6
  D7  //D7
};

//function declearation
float getTemperature();
void myFunction();
void resetSpeed();
void setFanSpeed(int number);

BLYNK_WRITE(V0) {
  int val = param.asInt();
  if (val == 1) {
    fanMode = 1;
    Blynk.virtualWrite(V2, 0);
    Blynk.virtualWrite(V3, 0);
    Blynk.virtualWrite(V4, 0);
    Serial.println("Auto Mode");
  }else {
    fanMode = 0;
  }
}

//when click fan speed number 1
BLYNK_WRITE(V2) {
  int val = param.asInt();
  if(!fanMode) {
    if (val == 1) {
      setFanSpeed(1);
      Blynk.virtualWrite(V3, 0);
      Blynk.virtualWrite(V4, 0);
      Serial.println("Number 1");
    }else {
      resetSpeed();
    }
  }else {

  }
}

//when click fan speed number 2
BLYNK_WRITE(V3) {
  int val = param.asInt();
  if(!fanMode) {
    if (val == 1) {
      setFanSpeed(2);
      Blynk.virtualWrite(V2, 0);
      Blynk.virtualWrite(V4, 0);
      Serial.println("Number 2");
    }else {
      resetSpeed();
    }
  }
}
//when click fan speed number 3
BLYNK_WRITE(V4) {
  int val = param.asInt();
  if(!fanMode) {
    if (val == 1) {
      setFanSpeed(3);
      Blynk.virtualWrite(V2, 0);
      Blynk.virtualWrite(V3, 0);
      Serial.println("Number 3");
    }else {
      resetSpeed();
    }
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
  timer.setInterval(1000L, myFunction);
}

void loop() {
  BlynkEdgent.run();
  timer.run();
}

void myFunction() {
  float temp = getTemperature();
  if(temp == -1) {
    Serial.println(F("Failed to read from DHT sensor!"));
    // Blynk.virtualWrite(V1, 0);
    // return;
  }
  Serial.println(temp);
  Blynk.virtualWrite(V1, temp);


  if(fanMode) {     //when auto mode
    if(temp <= 30.00) {
      resetSpeed();
    }else if(temp > 30 && temp <= 33){
      setFanSpeed(1);
      Blynk.virtualWrite(V2, 1);
    }else if(temp > 33 && temp <= 35) {
      setFanSpeed(2);
      Blynk.virtualWrite(V3, 1);
    }else {
      setFanSpeed(3);
      Blynk.virtualWrite(V4, 1);
    }
  }

  lcd.clear();
  lcd.cursor();
  lcd.print("Temp : ");
  lcd.setCursor(8, 0);
  lcd.print(temp);
  lcd.setCursor(0, 1);
  lcd.print("Speed Now is : ");
  lcd.print(nowSpeed);

}

float getTemperature() {
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

void resetSpeed() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(relayPin[i], 1);
  }
  Serial.println("Reset fan speed");
}

void setFanSpeed(int number) {
  nowSpeed = number;
  resetSpeed();
  if(number >= 1 && number <= 3) {
    digitalWrite(relayPin[number - 1], 0);
  }
  Serial.print("now is number : ");
  Serial.println(number);
} 

