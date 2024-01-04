#include <util/atomic.h> // For the ATOMIC_BLOCK macro
#include <math.h>

#define ENC1A 19// YELLOW
#define ENC1B 25// GREEN
#define ENC2A 18// YELLOW
#define ENC2B 24 // GREEN
#define ENC3A 21// YELLOW
#define ENC3B 23 // GREEN
#define ENC4A 20// YELLOW
#define ENC4B 22 // GREEN

#define PI 3.1415926535897932384626433832795
float A = 32100;

int M1A = 6;
int M1B = 7;
int M2A = 8;
int M2B = 9;
int M3A = 2;
int M3B = 3;
int M4A = 4;
int M4B = 5;

volatile int state = 0;
volatile bool reach1 = 0;
int move = 0;
int move2 = 0;

int destinationm1234[4] = {10000, 10000, 10000, 10000};
int errorm1234[4] = {0, 0, 0, 0};
int reachm1234[4] = {0, 0, 0, 0}; //0 means NOT reached
int allreached = 0;

volatile float destinationy = 0;
volatile float destinationz = 0;
float pidsc1 = 0;
float pidsc2 = 0;
float pidsc3 = 0;
float pidsc4 = 0;

float radius1 = 0;
float radius2 = 0;

volatile float pos1 = 1; //Volatile means any reference of the variable updates the value
volatile float pos2 = 1;
volatile float pos3 = 1; 
volatile float pos4 = 1;

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
  pinMode(ENC3A,INPUT);
  pinMode(ENC3B,INPUT);
  pinMode(M3A, OUTPUT);
  pinMode(M3B, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENC3A),readEncoder3,RISING);
  pinMode(ENC4A,INPUT);
  pinMode(ENC4B,INPUT);
  pinMode(M4A, OUTPUT);
  pinMode(M4B, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENC4A),readEncoder4,RISING);

  Serial.println("Setup Complete");
  delay(1000);
  state = 11;
}

void loop() {
  Serial.println(state);
  delay(100);

  switch (state) {
    case 3:
      Serial.print(pos1);
      Serial.print("   -   ");
      Serial.print(pos2);
      Serial.print("   -   ");
      Serial.print(pos3);
      Serial.print("   -   ");
      Serial.println(pos4);
      allreached = reachm1234[0]+reachm1234[1]+reachm1234[2]+reachm1234[3];
      if (allreached == 4){
        state = 11;
        Serial.println("Reached");
        reachm1234[0] = 0;
        reachm1234[1] = 0;
        reachm1234[2] = 0;
        reachm1234[3] = 0;
        delay(10000);
      }
      else{
        Serial.println("Move Motors");
        delay(100);
        movemotors2();
      }
      break;

    case 11:
      Serial.println("entered case 11");
      while(Serial.available() == 0) {
        Serial.println("move motors: ");
      }
      move = Serial.parseInt();
      while (Serial.available() > 0) {
        Serial.read();
      }

      switch(move){
        case 1://pull in
          analogWrite (M1A, 0);
          analogWrite (M1B, 255);
          break;
        case 2: 
          analogWrite (M1A, 255);
          analogWrite (M1B, 0);
          break;
        case 3://pull in
          analogWrite (M2A, 255);
          analogWrite (M2B, 0);
          break;
        case 4:
          analogWrite (M2A, 0);
          analogWrite (M2B, 255);
          break;
        case 5://pull in
          analogWrite (M3A, 0);
          analogWrite (M3B, 255);
          break;
        case 6: 
          analogWrite (M3A, 255);
          analogWrite (M3B, 0);
          break;
        case 7://pull in
          analogWrite (M4A, 255);
          analogWrite (M4B, 0);
          break;
        case 8:
          analogWrite (M4A, 0);
          analogWrite (M4B, 255);
          break;
        case 99:
          state = 12;
          break;
        case 77:
          state = 3;
          break;
        default:
          Serial.println("default---------------------------");
          analogWrite (M1A, 0);
          analogWrite (M1B, 0);
          analogWrite (M2A, 0);
          analogWrite (M2B, 0);
          analogWrite (M3A, 0);
          analogWrite (M3B, 0);
          analogWrite (M4A, 0);
          analogWrite (M4B, 0);
          break;
      }
      break;

    case 12:
      Serial.flush();
      delay(10);
      while(Serial.available() == 0) {
        Serial.println("At which motor is the module? ");
      }
      move2 = Serial.parseFloat();
      while (Serial.available() > 0) {
        Serial.read();
      }
      switch(move2){
        case 1: //next to motor 1
          pos1 = 10;
          state = 11;
          Serial.flush();
          delay(100);
          break;
        case 2: //next to motor 2
          pos2 = 10;
          state = 11;
          Serial.flush();
          delay(100);
          break;
        case 3: //next to motor 3
          pos3 = 10;
          state = 11;
          Serial.flush();
          delay(100);
          break;
        case 4: //next to motor 4
          pos4 = 10;
          state = 11;
          Serial.flush();
          delay(100);
          break;
        default:
          break;
      }
      Serial.flush();
      break;
    
    default:
      Serial.println("Default");
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
}

void readEncoder2(){
  int b = digitalRead(ENC2B);
  if(b > 0){
    pos2--;
  }
  else{
    pos2++;
  }
}

void readEncoder3(){
  int b = digitalRead(ENC3B);
  if(b > 0){
    pos3++;
  }
  else{
    pos3--;
  }
}

void readEncoder4(){
  int b = digitalRead(ENC4B);
  if(b > 0){
    pos4--;
  }
  else{
    pos4++;
  }
}



void movemotors2(){
  errorm1234[0] = abs(pos1) - destinationm1234[0]; //if greater than zero, pull in
  errorm1234[1] = abs(pos2) - destinationm1234[1];
  errorm1234[2] = abs(pos3) - destinationm1234[2];
  errorm1234[3] = abs(pos4) - destinationm1234[3];

  //if error is small, its reached
  pidsc1 = (errorm1234[0])/abs(errorm1234[0])*250;
  pidsc2 = (errorm1234[1])/abs(errorm1234[1])*250;
  pidsc3 = (errorm1234[2])/abs(errorm1234[2])*250;
  pidsc4 = (errorm1234[3])/abs(errorm1234[3])*250;

  if (abs(errorm1234[0]) < 50 || reachm1234[0] == 1){
    reachm1234[0] = 1;
    pidsc1 = 0;
  }
  if (abs(errorm1234[1]) < 50 || reachm1234[1] == 1){
    reachm1234[1] = 1;
    pidsc2 = 0;
  }
  if (abs(errorm1234[2]) < 50 || reachm1234[2] == 1){
    reachm1234[2] = 1;
    pidsc3 = 0;
  }
  if (abs(errorm1234[3]) < 50 || reachm1234[3] == 1){
    reachm1234[3] = 1;
    pidsc4 = 0;
  }
  Serial.print(errorm1234[0]);
  Serial.print("   -   ");
  Serial.print(errorm1234[1]);
  Serial.print("   -   ");
  Serial.print(errorm1234[2]);
  Serial.print("   -   ");
  Serial.println(errorm1234[3]);

  Serial.print(pidsc1);
  Serial.print("   -   ");
  Serial.print(pidsc2);
  Serial.print("   -   ");
  Serial.print(pidsc3);
  Serial.print("   -   ");
  Serial.println(pidsc4);

  if(pidsc1>0){
    delay(20000);
  }

  runm1();
  runm2();
  runm3();
  runm4();

}

void runm1(){//motor1
  if(pidsc1<0){//let out
    analogWrite (M1A, abs(pidsc1));
    analogWrite (M1B, 0);
  }
  else{//pull in
    analogWrite (M1A, 0);
    analogWrite (M1B, abs(pidsc1));
  }
}

void runm2(){//motor2
  if(pidsc2<0){//let out
    analogWrite (M2A, 0);
    analogWrite (M2B, abs(pidsc2));
  }
  else{//pull in
    analogWrite (M2A, abs(pidsc2));
    analogWrite (M2B, 0);
  }
}

void runm3(){//motor3
  if(pidsc3<0){//let out
    analogWrite (M3A, abs(pidsc3));
    analogWrite (M3B, 0);
  }
  else{//pull in
    analogWrite (M3A, 0);
    analogWrite (M3B, abs(pidsc3));
  }
}

void runm4(){//motor4
  if(pidsc4<0){//let out
    analogWrite (M4A, 0);
    analogWrite (M4B, abs(pidsc4));
  }
  else{//pull in
    analogWrite (M4A, abs(pidsc4));
    analogWrite (M4B, 0);
  }
}