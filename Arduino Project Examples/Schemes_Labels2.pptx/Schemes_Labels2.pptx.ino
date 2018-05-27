
#define encoderPin1 3
#define encoderPin2 2
#define SW 18

volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastencoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;

void setup() {
  Serial.begin (9600);
  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(2), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(18), bottone, LOW);
}

unsigned long time_start = millis();
void loop(){
  Serial.println(encoderValue);
}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
  lastEncoded = encoded; //store this value for next time
}

void bottone(){
  // put your main code here, to run repeatedly: 
  Serial.println("SPINTO");
}
