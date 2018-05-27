
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(A15, INPUT);
}

unsigned long cnt = 0; 
unsigned long time_start = millis();
void loop() {
  if(digitalRead(A15)==HIGH){
    if(millis() - time_start > 1000){
        bottone();
    }  
  }else{
      time_start = millis();  
  }
}

void bottone(){
  Serial.print("SPINTO ");
  Serial.println(++cnt);  
}

