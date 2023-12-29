#include <util/atomic.h> // For the ATOMIC_BLOCK macro

#define ENC1A 2// YELLOW
#define ENC1B 4 // GREEN
#define ENC2A 3// YELLOW
#define ENC2B 7 // GREEN
int M1A = 5;
int M1B = 6;
int M2A = 9;
int M2B = 10;
volatile int state = 0;
volatile bool reach1 = 0;
volatile bool reach2 = 0;

volatile float setpoint = 0;
float err1 = 0;
float err2 = 0;
float priorerr1 = 0;
float priorerr2 = 0;
float pidsc1 = 0;
float pidsc2 = 0;
float integral1 = 0;
float integral2 = 0;

volatile float pos1 = 0; //Volatile means any reference of the variable updates the value
volatile float pos2 = 0;

void setup() {
  Serial.begin(57600);
  pinMode(ENC1A,INPUT);
  pinMode(ENC1B,INPUT);
  pinMode(M1A, OUTPUT);
  pinMode(M1B, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENC1A),readEncoder1,RISING);
  pinMode(ENC2A,INPUT);
  pinMode(ENC2B,INPUT);
  pinMode(M2A, OUTPUT);
  pinMode(M2B, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENC2A),readEncoder2,RISING);
  Serial.println("Setup Complete");
}

void loop() {
  Serial.print(1100); Serial.print(",");
  Serial.print(-100); Serial.print(",");
  Serial.print("State: ");
  Serial.println(state);

  //state = 99;
  switch (state) {
    case 0:
      //Report pos1tion
      while(Serial.available() == 0) {
        Serial.print("Enter pos1tion: ");
        Serial.print(pos1);
        Serial.print("   -   ");
        Serial.println(pos2);
      }
      setpoint = Serial.parseFloat();
      if(setpoint != 0){
        reach1 = 0;
        reach2 = 0;
        state = 1;
        Serial.print("Setpoint: ");
        Serial.println(setpoint);
      }
      break;
    case 1: //move motors to input position
      Serial.println(pos1);
      Serial.println(" ");

      if(reach1 == 1){
        analogWrite(M1A, 0);
        analogWrite(M1B, 0);
      }
      else{
        movemotor1();
      }
      if(reach2 == 1){
        analogWrite(M2A, 0);
        analogWrite(M2B, 0);
      }
      else{
        movemotor2();
      }

      if(reach1==1 && reach2==1){
        //change this to check if reach1 and reach2 are == 1
        delay(10);
        if(abs(setpoint-pos1)<50 && abs(setpoint-pos2)<50){
          integral1 = 0;
          analogWrite (M1A, 0);
          analogWrite (M1B, 0);
          analogWrite (M2A, 0);
          analogWrite (M2B, 0);
          state = 2;
        }
      }
      break;
    case 2://just stop after the destination is reached
      Serial.println("BOTH POSITIONS REACHED");
      delay(1000);
      state = 0;
      break;
    default:
      // statements
      break;
  }
}

void readEncoder1(){
  int b = digitalRead(ENC1B);
  if(b > 0){
    pos1++;
  }
  else{
    pos1--;
  }

  if(abs(setpoint-pos1)<50){
    reach1 = 1;
  }
}

void readEncoder2(){
  int b = digitalRead(ENC2B);
  if(b > 0){
    pos2++;
  }
  else{
    pos2--;
  }

  if(abs(setpoint-pos2)<50){
    reach2 = 1;
  }
}

void movemotor1(){
  float Kp = 0.3;
  float Kd = 0.0001;
  float Ki = 0.01;

  err1 = setpoint-pos1;

  pidsc1 = abs(Kp*err1+Kd*(err1-priorerr1)+Ki*integral1);
  Serial.print("error: ");
  Serial.println(err1);
  Serial.print("pidsc1: ");
  Serial.println(pidsc1);
  if(abs(err1) < 150){
    integral1 += err1;
  }
  if(abs(setpoint-pos1)<30){
    analogWrite (M1A, 0);
    analogWrite (M1B, 0);
    reach1 = 1;
    return;
  }
  float lowerv = 125;
  float lowersc = 50;
  float uppersc = 200;
  float m = (255-lowerv)/(uppersc-lowersc);
  float b = lowerv-m*lowersc;

  pidsc1 = m*pidsc1+b;

  if(abs(pidsc1)>254){
    pidsc1 = 255;
  }

  if(pos1>setpoint){
    analogWrite (M1A, 0);
    analogWrite (M1B, pidsc1);
  }
  else{
    analogWrite (M1A, pidsc1);
    analogWrite (M1B, 0);
  }

  if(abs(setpoint-pos1)<30){
    analogWrite (M1A, 0);
    analogWrite (M1B, 0);
    reach1 = 1;
    return;
  }
  priorerr1 = err1;
}

void movemotor2(){
  float Kp = 0.3;
  float Kd = 0.0001;
  float Ki = 0.01;

  err2 = setpoint-pos2;

  pidsc2 = abs(Kp*err2+Kd*(err2-priorerr2)+Ki*integral2);
  Serial.print("error: ");
  Serial.println(err2);
  Serial.print("pidsc2: ");
  Serial.println(pidsc2);
  if(abs(err2) < 150){
    integral2 += err2;
  }
  if(abs(setpoint-pos2)<30){
    analogWrite (M2A, 0);
    analogWrite (M2B, 0);
    reach2 = 1;
    return;
  }
  float lowerv = 125;
  float lowersc = 50;
  float uppersc = 200;
  float m = (255-lowerv)/(uppersc-lowersc);
  float b = lowerv-m*lowersc;

  pidsc2 = m*pidsc2+b;

  if(abs(pidsc2)>254){
    pidsc2 = 255;
  }

  if(pos2>setpoint){
    analogWrite (M2A, 0);
    analogWrite (M2B, pidsc2);
  }
  else{
    analogWrite (M2A, pidsc2);
    analogWrite (M2B, 0);
  }

  if(abs(setpoint-pos2)<30){
    analogWrite (M2A, 0);
    analogWrite (M2B, 0);
    reach2 = 1;
    return;
  }
  priorerr2 = err1;
}