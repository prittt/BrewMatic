
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(19, INPUT);
  attachInterrupt(digitalPinToInterrupt(19), bottone, RISING);
}

unsigned long cnt = 0; 
unsigned long time_start = millis();
void loop() {
  //loop
}

void bottone(){
  Serial.print("SPINTO ");
  Serial.println(++cnt);  
}

