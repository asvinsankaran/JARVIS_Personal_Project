int M3A = 2;
int M3B = 3;
int M4A = 4;
int M4B = 5;



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(1, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  Serial.println("Setup Complete");
}

void loop() {
  // put your main code here, to run repeatedly:
  ///pwm pins from 2 to 9

  Serial.println("idk");
  int i = 2;
  while (i < 10){
    analogWrite(i, 150);
    i++;
  }
  Serial.println("pwm applied");
  delay(10000);

}
