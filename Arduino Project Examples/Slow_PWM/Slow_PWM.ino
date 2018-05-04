#include "SlowPWM.h"

// 1 Led PWM.

SlowPWM myLedPWM(5000, A0, 13); // Creates a SlowPWM object.

void setup() {
  Serial.begin(9600);
  pinMode(A0, INPUT);
  pinMode(13, OUTPUT);
}

unsigned long long timer = millis();

void loop() {
  analogWrite(A0, 125); 
  myLedPWM.on();

  if(millis() - timer > 500){
    timer = millis();
    Serial.println(digitalRead(13));
  }  
}
