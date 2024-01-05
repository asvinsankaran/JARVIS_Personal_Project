#include <util/atomic.h> // For the ATOMIC_BLOCK macro
#include <math.h>
#include <Arduino.h>

#define ENC1A 19// YELLOW
#define ENC1B 25// GREEN
#define ENC2A 18// YELLOW
#define ENC2B 24 // GREEN
#define ENC3A 21// YELLOW
#define ENC3B 23 // GREEN
#define ENC4A 20// YELLOW
#define ENC4B 22 // GREEN

long int NS = 37370;
long int EW = 30590;

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

long int destinationm1234[4] = {10000, 10000, 10000, 10000};
long int errorm1234[4] = {0, 0, 0, 0};
int reachm1234[4] = {0, 0, 0, 0}; //0 means NOT reached
int allreached = 0;

float alpha = 0.1;
int maxIterations = 100;
float epsilon = 0.001;
//initial guess
float moduleLocation[3] = {100.0, 100.0, 100.0};//This is the xyz position of the module


float cornerLocations[4][3] = {{0,EW,0},{0,0,0},{NS,EW,0},{NS,0,0}};  // Coordinates of each corner
volatile float wireRadii[4] = {1, 1, 1, 1};  

float pidsc[4] = {0, 0, 0,0}
// float pidsc[0] = 0;
// float pidsc[1] = 0;
// float pidsc[2] = 0;
// float pidsc[3] = 0;

long int x = 0;
long int y = 0;
long int z = 0;

// volatile long int wireRadii[0] = 1; //Volatile means any reference of the variable updates the value
// volatile long int wireRadii[1] = 1;
// volatile long int wireRadii[2] = 1; 
// volatile long int wireRadii[3] = 1;

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
  x = 10000;
  y = 10000;
  z = 5000;
}

void loop() {
  Serial.println(state);

  switch (state) {
    case 3:
      Serial.print(wireRadii[0]);
      Serial.print("   -   ");
      Serial.print(wireRadii[1]);
      Serial.print("   -   ");
      Serial.print(wireRadii[2]);
      Serial.print("   -   ");
      Serial.println(wireRadii[3]);
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
        movemotors2();
      }
      break;

    case 11:
      Serial.println("entered case 11");
      while(Serial.available() == 0) {
        Serial.println("move motors: ");
        Serial.print(wireRadii[0]);
        Serial.print("   -   ");
        Serial.print(wireRadii[1]);
        Serial.print("   -   ");
        Serial.print(wireRadii[2]);
        Serial.print("   -   ");
        Serial.println(wireRadii[3]);
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
          state = 13;
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
          wireRadii[0] = 10;
          state = 11;
          Serial.flush();
          delay(100);
          break;
        case 2: //next to motor 2
          wireRadii[1] = 10;
          state = 11;
          Serial.flush();
          delay(100);
          break;
        case 3: //next to motor 3
          wireRadii[2] = 10;
          state = 11;
          Serial.flush();
          delay(100);
          break;
        case 4: //next to motor 4
          wireRadii[3] = 10;
          state = 11;
          Serial.flush();
          delay(100);
          break;
        case 5:
          Serial.println("Writing position by hand");
          // wireRadii[0] = 25056;
          // wireRadii[1] = 17307;
          // wireRadii[2] = 35721;
          // wireRadii[3] = 35713;
          wireRadii[0] = 17933;
          wireRadii[1] = 190;
          wireRadii[2] = 38132;
          wireRadii[3] = 27614;
          state = 14;
          break;
        default:
          break;
      }
      Serial.flush();
      break;
    
    case 13:
      destinationm1234[0] = sqrt(sq(abs(x-0))+sq(abs(y-EW))+sq(abs(z-0)));
      destinationm1234[1] = sqrt(sq(abs(x-0))+sq(abs(y-0))+sq(abs(z-0)));
      destinationm1234[2] = sqrt(sq(abs(x-NS))+sq(abs(y-EW))+sq(abs(z-0)));
      destinationm1234[3] = sqrt(sq(abs(x-NS))+sq(abs(y-EW))+sq(abs(z-0)));

      Serial.print(destinationm1234[0]);
      Serial.print("   -   ");
      Serial.print(destinationm1234[1]);
      Serial.print("   -   ");
      Serial.print(destinationm1234[2]);
      Serial.print("   -   ");
      Serial.println(destinationm1234[3]);
      delay(1000);
      state = 3;
      break;
    case 14:
      getXYZPosition();
      break;
    default:
      Serial.println("Default");
      break;

  }
}

void readEncoder1(){
  int b = digitalRead(ENC1B);
  if(b > 0){
    wireRadii[0]--;
  }
  else{
    wireRadii[0]++;
  }
}

void readEncoder2(){
  int b = digitalRead(ENC2B);
  if(b > 0){
    wireRadii[1]++;
  }
  else{
    wireRadii[1]--;
  }
}

void readEncoder3(){
  int b = digitalRead(ENC3B);
  if(b > 0){
    wireRadii[2]--;
  }
  else{
    wireRadii[2]++;
  }
}

void readEncoder4(){
  int b = digitalRead(ENC4B);
  if(b > 0){
    wireRadii[3]++;
  }
  else{
    wireRadii[3]--;
  }
}

void movemotors2(){
  errorm1234[0] = abs(wireRadii[0]) - destinationm1234[0]; //if greater than zero, pull in
  errorm1234[1] = abs(wireRadii[1]) - destinationm1234[1];
  errorm1234[2] = abs(wireRadii[2]) - destinationm1234[2];
  errorm1234[3] = abs(wireRadii[3]) - destinationm1234[3];

  //if error is small, its reached
  pidsc[0] = (errorm1234[0])/abs(errorm1234[0])*250;
  pidsc[1] = (errorm1234[1])/abs(errorm1234[1])*250;
  pidsc[2] = (errorm1234[2])/abs(errorm1234[2])*250;
  pidsc[3] = (errorm1234[3])/abs(errorm1234[3])*250;

  if (abs(errorm1234[0]) < 100 || reachm1234[0] == 1){
    reachm1234[0] = 1;
    pidsc[0] = 0;
  }
  if (abs(errorm1234[1]) < 100 || reachm1234[1] == 1){
    reachm1234[1] = 1;
    pidsc[1] = 0;
  }
  if (abs(errorm1234[2]) < 100 || reachm1234[2] == 1){
    reachm1234[2] = 1;
    pidsc[2] = 0;
  }
  if (abs(errorm1234[3]) < 100 || reachm1234[3] == 1){
    reachm1234[3] = 1;
    pidsc[3] = 0;
  }
  Serial.print(errorm1234[0]);
  Serial.print("   -   ");
  Serial.print(errorm1234[1]);
  Serial.print("   -   ");
  Serial.print(errorm1234[2]);
  Serial.print("   -   ");
  Serial.println(errorm1234[3]);

  Serial.print(pidsc[0]);
  Serial.print("   -   ");
  Serial.print(pidsc[1]);
  Serial.print("   -   ");
  Serial.print(pidsc[2]);
  Serial.print("   -   ");
  Serial.println(pidsc[3]);

  if(pidsc[0]>0){
    delay(20000);
  }

  runm1();
  runm2();
  runm3();
  runm4();

}

void runm1(){//motor1
  if(pidsc[0]<0){//let out
    analogWrite (M1A, abs(pidsc[0]));
    analogWrite (M1B, 0);
  }
  else{//pull in
    analogWrite (M1A, 0);
    analogWrite (M1B, abs(pidsc[0]));
  }
}

void runm2(){//motor2
  if(pidsc[1]<0){//let out
    analogWrite (M2A, 0);
    analogWrite (M2B, abs(pidsc[1]));
  }
  else{//pull in
    analogWrite (M2A, abs(pidsc[1]));
    analogWrite (M2B, 0);
  }
}

void runm3(){//motor3
  if(pidsc[2]<0){//let out
    analogWrite (M3A, abs(pidsc[2]));
    analogWrite (M3B, 0);
  }
  else{//pull in
    analogWrite (M3A, 0);
    analogWrite (M3B, abs(pidsc[2]));
  }
}

void runm4(){//motor4
  if(pidsc[3]<0){//let out
    analogWrite (M4A, 0);
    analogWrite (M4B, abs(pidsc[3]));
  }
  else{//pull in
    analogWrite (M4A, abs(pidsc[3]));
    analogWrite (M4B, 0);
  }
}

void getXYZPosition(){
  gradientDescent(moduleLocation, alpha, maxIterations, epsilon);

  Serial.print("Module Location: ");
  Serial.print(moduleLocation[0]);
  Serial.print(", ");
  Serial.print(moduleLocation[1]);
  Serial.print(", ");
  Serial.println(moduleLocation[2]);
}