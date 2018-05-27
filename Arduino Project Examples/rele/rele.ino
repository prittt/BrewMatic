#define PINA 7
#define PINB 6

void setup() {
  // put your setup code here, to run once:
  pinMode(PINA, OUTPUT);
  pinMode(PINB, OUTPUT);
  digitalWrite(PINA, LOW);
  digitalWrite(PINB, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PINA, LOW);
  digitalWrite(PINB, LOW);
  delay(2000);
  digitalWrite(PINA, HIGH);
  digitalWrite(PINB, LOW);
  delay(2000);
  digitalWrite(PINA, LOW);
  digitalWrite(PINB, HIGH);
  delay(2000);
}
