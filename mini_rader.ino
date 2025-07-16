#include <TM1637Display.h>
#include <ESP32Servo.h>
#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

#define CLK 18
#define DIO 13
TM1637Display display(CLK, DIO);

Servo myservo;
int servopin = 14;

int trig = 27;
int echo = 35;
long dura;
int distance = 0;

int buzzerpin = 32;
bool systemOn = false;  // State flag

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32_radar");  // Bluetooth device name

  pinMode(buzzerpin, OUTPUT);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);

  display.setBrightness(7);
  myservo.attach(servopin);
}

void loop() {
  if (SerialBT.available()) {
    char incoming = SerialBT.read();

    if (incoming == '1') {
      systemOn = true;
    } else if (incoming == '0') {
      systemOn = false;
      digitalWrite(buzzerpin, LOW);
    }
  }

  if (systemOn) {
    for (int i = 0; i <= 180; i += 5) {
      myservo.write(i);
      delay(50);

      digitalWrite(trig, LOW);
      delayMicroseconds(2);
      digitalWrite(trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig, LOW);

      dura = pulseIn(echo, HIGH, 30000);  // 30 ms timeout
      distance = dura * 0.034 / 2;

      Serial.println(distance);

      if (distance > 0 && distance <= 9999) {
        display.showNumberDec(distance, true, 4, 0);
      } else {
        display.showNumberDec(0);
      }

      if (distance > 0 && distance < 50) {
        for (int q = 0; q <= 5; q++) {
          digitalWrite(buzzerpin, HIGH);
          delay(50);
          digitalWrite(buzzerpin, LOW);
          delay(50);
        }
      } else {
        digitalWrite(buzzerpin, LOW);
      }

      if (!systemOn) break;  // exit early if '0' is received
    }

    for (int i = 180; i >= 0; i -= 5) {
      myservo.write(i);
      delay(50);

      digitalWrite(trig, LOW);
      delayMicroseconds(2);
      digitalWrite(trig, HIGH);
      delayMicroseconds(10);
      digitalWrite(trig, LOW);

      dura = pulseIn(echo, HIGH, 30000);
      distance = dura * 0.034 / 2;

      Serial.println(distance);

      if (distance > 0 && distance <= 9999) {
        display.showNumberDec(distance, true, 4, 0);
      } else {
        display.showNumberDec(0);
      }

      if (distance > 0 && distance < 50) {
        for (int q = 0; q <= 5; q++) {
          digitalWrite(buzzerpin, HIGH);
          delay(50);
          digitalWrite(buzzerpin, LOW);
          delay(50);
        }
      } else {
        digitalWrite(buzzerpin, LOW);
      }

      if (!systemOn) break;
    }
  }
}
