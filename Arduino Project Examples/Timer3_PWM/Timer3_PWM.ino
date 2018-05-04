/*
 *  Timer1 library example
 *  June 2008 | jesse dot tane at gmail dot com
 */
 
#include "TimerOne.h"

bool print_message = false;
unsigned long long timer = millis();
 
void setup()
{
  Serial.begin(9600);
  pinMode(10, OUTPUT);
  Timer1.initialize(5000000);          // initialize timer1, and set a 1/2 second period
  Timer1.pwm(9, 820);                // setup pwm on pin 9, 50% duty cycle
  Timer1.attachInterrupt(CallbackFunction);  // attaches callback() as a timer overflow interrupt
}

unsigned long long display_time = millis();
void loop()
{
  if(millis() - display_time > 500){
    Serial.println(digitalRead(9));
    display_time = millis();
  }
}


//#include <Timer3.h>
//
//bool print_message = false;
//unsigned long long timer = millis();
//
//void setup() {
//  // put your setup code here, to run once:
//  pinMode(10, OUTPUT);
//  Serial.begin(9600);
//  TimerThree.initialize(10000000); // uSeconds
//  Timer3.pwm(2,1023);
//  Timer3.attachInterrupt(CallbackFunction);
//  Serial.println(F_CPU);
//}
//
//
//void loop() {
//  // put your main code here, to run repeatedly:
//}

void CallbackFunction(){
  digitalWrite(10, digitalRead(10)^1);

  noInterrupts();
  double supp = (millis() - timer)/(double) 1000;
  timer = millis();

  //Serial.println(digitalRead(10));
  Serial.print("\n\nLast Change: ");
  Serial.print(supp);
  Serial.print(" s\n\n");

  interrupts();
}
