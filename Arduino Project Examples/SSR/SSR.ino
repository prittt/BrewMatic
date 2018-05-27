#define PIN A11

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN, OUTPUT);
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("PIN - HIGH");
  digitalWrite(PIN, HIGH);
  delay(2000);
  Serial.println("PIN - LOW");
  digitalWrite(PIN, LOW);
  delay(2000);
}
