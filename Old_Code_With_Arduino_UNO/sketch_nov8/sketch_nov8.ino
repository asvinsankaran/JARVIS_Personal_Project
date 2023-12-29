#include <util/atomic.h> // For the ATOMIC_BLOCK macro
#include <math.h>

#define ENC1A 2// YELLOW
#define ENC1B 4 // GREEN
#define ENC2A 3// YELLOW
#define ENC2B 7 // GREEN
#define PI 3.1415926535897932384626433832795
float A = 32100;

int M1A = 5;
int M1B = 6;
int M2A = 9;
int M2B = 10;
volatile int state = 0;
volatile bool reach1 = 0;
int move = -1;
int move2 = -1;


volatile float destinationy = 0;
volatile float destinationz = 0;
float pidsc1 = 0;
float pidsc2 = 0;
volatile float pos1 = 1; //Volatile means any reference of the variable updates the value
volatile float pos2 = 1;

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
  delay(1000);
  state = 11;

  destinationy = 10000; 
  destinationz = 10000;
}

void loop() {
  Serial.println("----------------------");
  Serial.println(state);
  Serial.print(pos1);
  Serial.print("   -   ");
  Serial.println(pos2);
  Serial.print(destinationy);
  Serial.print("   -   ");
  Serial.println(destinationz);
  
  switch (state) {
    case 1: 
      if(reach1 == 1){
        analogWrite(M1A, 0);
        analogWrite(M1B, 0);
        analogWrite(M2A, 0);
        analogWrite(M2B, 0);
        state = 2;
      }
      else{
        printposition();
        movemotors();
      }
      break;

    case 2://Stop after destination is reached
      Serial.println("BOTH POSITIONS REACHED");
      delay(10000);
      reach1 = 0;
      state = 1;

      destinationy = 30000; 
      destinationz = 10000;
      break;

    case 11:
      while(Serial.available() == 0) {
        Serial.println("move motors: ");
      }
      move = Serial.parseInt();
      switch(move){
        case 1: //pull in for m1
          analogWrite (M1A, 0);
          analogWrite (M1B, 255);
          break;
        case 2: //let out for m1
          analogWrite (M1A, 255);
          analogWrite (M1B, 0);
          break;
        case 3://pull in other motor
          analogWrite (M2A, 255);
          analogWrite (M2B, 0);
          break;
        case 4://let out other motor
          analogWrite (M2A, 0);
          analogWrite (M2B, 255);
          break;
        case 99:
          state = 12;
          break;
        default:
          analogWrite (M1A, 0);
          analogWrite (M1B, 0);
          analogWrite (M2A, 0);
          analogWrite (M2B, 0);
          break;
      }
      Serial.flush();
      break;

    case 12:
      while(Serial.available() == 0) {
        Serial.println("where is the module: ");
      }
      move2 = Serial.parseFloat();
      switch(move2){
        case 1: //next to motor 1
          pos1 = 10;
          pos2 = A-10;
          state = 1;
          Serial.flush();
          delay(1000);
          break;
        case 2: //next to motor 2
          pos1 = A-10;
          pos2 = 10;
          state = 1;
          Serial.flush();
          delay(1000);
          break;
      }      
      Serial.flush();
      break;
    case 13:
      while(Serial.available() == 0) {
        Serial.println("move motors: ");
      }
      move = Serial.parseInt();
      switch(move){
        case 1: //pull in for m1
          analogWrite (M1A, 0);
          analogWrite (M1B, 255);
          break;
        case 2: //let out for m1
          analogWrite (M1A, 255);
          analogWrite (M1B, 0);
          break;
        case 3://pull in other motor
          analogWrite (M2A, 255);
          analogWrite (M2B, 0);
          break;
        case 4://let out other motor
          analogWrite (M2A, 0);
          analogWrite (M2B, 255);
          break;
        case 99:
          state = 12;
          break;
        default:
          analogWrite (M1A, 0);
          analogWrite (M1B, 0);
          analogWrite (M2A, 0);
          analogWrite (M2B, 0);
          break;
      }
      Serial.flush();
      printposition();
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

void printposition(){//This is working fine
  float gamma = acos(float((A*A+pos1*pos1-pos2*pos2)/(2*pos1*A)));
  Serial.print("---->>>>>>>>>>>--------------");
  Serial.println((float((A*A+pos1*pos1-pos2*pos2)/(2*pos1*A))), 8);
  float Y = pos1*cos(gamma);
  float Z = pos1*sin(gamma);

  Serial.print("Y:  ");
  Serial.println(Y);
  Serial.print("Z:  ");
  Serial.println(Z);
}

void movemotors(){
  float gamma = acos(float((A*A+pos1*pos1-pos2*pos2)/(2*pos1*A)));
  Serial.print("---->>>>>>>>>>>--------------");
  Serial.println((float((A*A+pos1*pos1-pos2*pos2)/(2*pos1*A))), 8);
  float Y = pos1*cos(gamma);
  float Z = pos1*sin(gamma);

  float P1[2] = {0-Y, 0-Z};
  float P2[2] = {A-Y, 0-Z};
  float lenp1 = sqrt(sq(P1[0])+sq(P1[1]));
  float lenp2 = sqrt(sq(P2[0])+sq(P2[1]));
  P1[0] = P1[0]/lenp1;
  P1[1] = P1[1]/lenp1;
  P2[0] = P2[0]/lenp2;
  P2[1] = P2[1]/lenp2;

  float a[2] = {destinationy-Y, destinationz-Z};

  float c = abs(a[0]*P1[0]+a[1]*P1[1]);//this has some positive/negative sign but cant figure it out
  float b = abs(a[0]*P2[0]+a[1]*P2[1]); //reverting next few lines just to try and debug

  float lena = sqrt(sq(a[0])+sq(a[1]));

  float theta1 = acos(c/(lena*lenp1));//with c and b not being absolute signs, then it may have affected performance!
  float theta2 = acos(b/(lena*lenp2));

  if (theta1 > PI/2){
    c = -c;
  }
  if (theta2 < PI/2){
    b = -b;
  }

  //find the square error
  float squareerror = lena;

  if(squareerror<30){
    analogWrite (M1A, 0);
    analogWrite (M1B, 0);
    analogWrite (M2A, 0);
    analogWrite (M2B, 0);
    reach1 = 1;
    return;
  }

  //use square error to add a ramp down as it approaches
  float ramp = 255;
  if (squareerror<150){
    ramp = ramp*squareerror/150;
  }

  //if b or c is <0, that means pull in
  if(abs(b) > abs(c)){
    //run motor2 at max
    pidsc1 = ramp*(abs(c)/abs(b))*(c/abs(c));
    pidsc2 = ramp*(b/abs(b));
  }
  else{
    //run motor1 at max
    pidsc1 = ramp*(c/abs(c));
    pidsc2 = ramp*(abs(b)/abs(c))*(b/abs(b));
  }

  Serial.print("PIDSC1");
  Serial.print("   :   ");
  Serial.println(pidsc1);

  Serial.print("PIDSC2");
  Serial.print("   :   ");
  Serial.println(pidsc2);

  if(reach1==0){
    runm1();
    runm2();
  }
}

void runm1(){//motor1
  if(pidsc1>0){//let out
    analogWrite (M1A, abs(pidsc1));
    analogWrite (M1B, 0);
  }
  else{//pull in
    analogWrite (M1A, 0);
    analogWrite (M1B, abs(pidsc1));
  }
}

void runm2(){//motor2
  if(pidsc2>0){//let out
    analogWrite (M2A, 0);
    analogWrite (M2B, abs(pidsc2));
  }
  else{//pull in
    analogWrite (M2A, abs(pidsc2));
    analogWrite (M2B, 0);
  }
}
