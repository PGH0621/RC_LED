#include "PinChangeInterrupt.h"
#include <math.h>

#define NEUTRAL_THROTTLE 1500
#define pinRC1 A2                 // CH1: RGB 색상 제어 (조이스틱 수평)
#define pinRC3 A0                 // CH3: 밝기 조절 (조이스틱 수직)
#define pinRC5 A1                 // CH5: LED ON/OFF 스위치
#define pinLED_PWM 9              // 일반 LED (밝기 조절)
#define pinLED_ONOFF 6            // ON/OFF 제어 핀

#define pinLED_R 3                // RGB - 빨강
#define pinLED_G 5                // RGB - 초록
#define pinLED_B 10               // RGB - 파랑

volatile int nRC1PulseWidth = 1500;              
volatile int nRC3PulseWidth = NEUTRAL_THROTTLE;  
volatile int nRC5PulseWidth = 1000;

volatile unsigned long ulRC1StartHigh = 0;       
volatile unsigned long ulRC3StartHigh = 0;       
volatile unsigned long ulRC5StartHigh = 0;       

volatile boolean bNewRC1Pulse = false;           
volatile boolean bNewRC3Pulse = false;           
volatile boolean bNewRC5Pulse = false;           

int prevRC3PulseWidth = NEUTRAL_THROTTLE;
bool ledEnabled = false;

void pwmRC1();
void pwmRC3();
void pwmRC5();
void hsvToRgb(float h, float s, float v, int& r, int& g, int& b);

void setup() {
  pinMode(pinRC1, INPUT_PULLUP);
  pinMode(pinRC3, INPUT_PULLUP);
  pinMode(pinRC5, INPUT_PULLUP);

  attachPCINT(digitalPinToPCINT(pinRC1), pwmRC1, CHANGE);
  attachPCINT(digitalPinToPCINT(pinRC3), pwmRC3, CHANGE);
  attachPCINT(digitalPinToPCINT(pinRC5), pwmRC5, CHANGE);

  pinMode(pinLED_PWM, OUTPUT);
  pinMode(pinLED_ONOFF, OUTPUT);
  pinMode(pinLED_R, OUTPUT);
  pinMode(pinLED_G, OUTPUT);
  pinMode(pinLED_B, OUTPUT);

  analogWrite(pinLED_PWM, 0);
  digitalWrite(pinLED_ONOFF, LOW);
  digitalWrite(pinLED_R, LOW);
  digitalWrite(pinLED_G, LOW);
  digitalWrite(pinLED_B, LOW);

  Serial.begin(9600);
  Serial.println("CH1(A2): RGB 색상 | CH3(A0): 밝기 | CH5(A1): LED ON/OFF");
}

void loop() {
  if (bNewRC5Pulse) {
    if (nRC5PulseWidth > 1500) {
      ledEnabled = true;
      digitalWrite(pinLED_ONOFF, HIGH);

      // ✅ 밝기 즉시 복원
      int brightness = map(nRC3PulseWidth, 1000, 2000, 255, 0);
      brightness = constrain(brightness, 0, 255);
      analogWrite(pinLED_PWM, brightness);

      Serial.println("CH5 A Switch: ON → LEDs ENABLED (Brightness Restored)");
    } else {
      ledEnabled = false;
      digitalWrite(pinLED_ONOFF, LOW);
      analogWrite(pinLED_PWM, 0);
      digitalWrite(pinLED_R, LOW);
      digitalWrite(pinLED_G, LOW);
      digitalWrite(pinLED_B, LOW);
    }
    bNewRC5Pulse = false;
  }

  if (ledEnabled && bNewRC3Pulse) {
    if (abs(prevRC3PulseWidth - nRC3PulseWidth) > 10) {
      int brightness = map(nRC3PulseWidth, 1000, 2000, 255, 0);
      brightness = constrain(brightness, 0, 255);
      analogWrite(pinLED_PWM, brightness);
      Serial.print("CH3 Brightness: ");
      Serial.println(brightness);
      prevRC3PulseWidth = nRC3PulseWidth;
    }
    bNewRC3Pulse = false;
  }

  if (ledEnabled && bNewRC1Pulse) {
    float hue = map(nRC1PulseWidth, 1000, 2000, 0, 360);
    int r, g, b;
    hsvToRgb(hue, 1.0, 1.0, r, g, b);
    analogWrite(pinLED_R, r);
    analogWrite(pinLED_G, g);
    analogWrite(pinLED_B, b);

    Serial.print("CH1 Hue: ");
    Serial.print(hue);
    Serial.print(" → RGB(");
    Serial.print(r); Serial.print(", ");
    Serial.print(g); Serial.print(", ");
    Serial.print(b); Serial.println(")");
    
    bNewRC1Pulse = false;
  }
}

void pwmRC1() {
  if (digitalRead(pinRC1) == HIGH) {
    ulRC1StartHigh = micros();
  } else {
    if (ulRC1StartHigh && !bNewRC1Pulse) {
      nRC1PulseWidth = (int)(micros() - ulRC1StartHigh);
      ulRC1StartHigh = 0;
      bNewRC1Pulse = true;
    }
  }
}

void pwmRC3() {
  if (digitalRead(pinRC3) == HIGH) {
    ulRC3StartHigh = micros();
  } else {
    if (ulRC3StartHigh && !bNewRC3Pulse) {
      nRC3PulseWidth = (int)(micros() - ulRC3StartHigh);
      ulRC3StartHigh = 0;
      bNewRC3Pulse = true;
    }
  }
}

void pwmRC5() {
  if (digitalRead(pinRC5) == HIGH) {
    ulRC5StartHigh = micros();
  } else {
    if (ulRC5StartHigh && !bNewRC5Pulse) {
      nRC5PulseWidth = (int)(micros() - ulRC5StartHigh);
      ulRC5StartHigh = 0;
      bNewRC5Pulse = true;
    }
  }
}

void hsvToRgb(float h, float s, float v, int& r, int& g, int& b) {
  float c = v * s;
  float x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
  float m = v - c;
  float r1, g1, b1;

  if (h < 60) {
    r1 = c; g1 = x; b1 = 0;
  } else if (h < 120) {
    r1 = x; g1 = c; b1 = 0;
  } else if (h < 180) {
    r1 = 0; g1 = c; b1 = x;
  } else if (h < 240) {
    r1 = 0; g1 = x; b1 = c;
  } else if (h < 300) {
    r1 = x; g1 = 0; b1 = c;
  } else {
    r1 = c; g1 = 0; b1 = x;
  }

  r = (r1 + m) * 255;
  g = (g1 + m) * 255;
  b = (b1 + m) * 255; 
}
