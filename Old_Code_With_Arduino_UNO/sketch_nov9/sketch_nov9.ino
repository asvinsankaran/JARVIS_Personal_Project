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
float Y = 0;
float Z = 0;
volatile int state = 0;
volatile bool reach1 = 0;
int move = -1;
int move2 = -1;
int move3 = -1;


volatile float destinationy = 0;
volatile float destinationz = 0;
float pidsc1 = 0;
float pidsc2 = 0;
float bump = 100;
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
}

void loop() {
  Serial.println("----------------------****");
  Serial.println(state);
  Serial.print(pos1);
  Serial.print("   -   ");
  Serial.println(pos2);
  Serial.print(destinationy);
  Serial.print("   -   ");
  Serial.println(destinationz);
  
  switch (state) {
    case 1:
      while(Serial.available() == 0) {
        Serial.println("Direction to move: ");
      }
      move3 = Serial.parseInt();
      printposition();
      switch(move3){
        case 1://move left
          destinationy = Y+bump;
          destinationz = Z;
          movemotors();
          break;
        case 2://move down
          destinationy = Y;
          destinationz = Z+bump;
          movemotors();
          break;
        case 3://move right
          destinationy = Y-bump;
          destinationz = Z;
          movemotors();
          break;
        case 5://move up
          destinationy = Y;
          destinationz = Z-bump;
          movemotors();
          break;
        default://stop moving
          analogWrite (M1A, 0);
          analogWrite (M1B, 0);
          analogWrite (M2A, 0);
          analogWrite (M2B, 0);
          destinationy = Y;
          destinationz = Z;
          break;
      }
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
          pos1 = 100;
          pos2 = A-100;
          state = 1;
          printposition();
          destinationy = Y;
          destinationz = Z;
          Serial.flush();
          delay(1000);
          break;
        case 2: //next to motor 2
          pos1 = A-100;
          pos2 = 100;
          state = 1;
          printposition();
          destinationy = Y;
          destinationz = Z;
          Serial.flush();
          delay(1000);
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

void printposition(){//Updates Y and Z
  /*float gamma = acos(float((A*A+pos1*pos1-pos2*pos2)/(2*pos1*A)));
  Serial.print("---->>>>>>>>>>>--------------");
  Serial.println((float((A*A+pos1*pos1-pos2*pos2)/(2*pos1*A))), 8);
  Y = pos1*cos(gamma);
  Z = pos1*sin(gamma);*/

  Y = (sq(pos1)+sq(A)-sq(pos2))/(2*A);
  Z = sqrt(sq(pos1)-sq(Y));

  Serial.print("Y:  ");
  Serial.println(Y);
  Serial.print("Z:  ");
  Serial.println(Z);
}

void movemotors(){
  /*float gamma = acos(float((A*A+pos1*pos1-pos2*pos2)/(2*pos1*A)));
  Serial.print("---->>>>>>>>>>>--------------");
  Serial.println((float((A*A+pos1*pos1-pos2*pos2)/(2*pos1*A))), 8);
  Y = pos1*cos(gamma);
  Z = pos1*sin(gamma);*/

  float P1[2] = {0-Y, 0-Z};
  float P2[2] = {A-Y, 0-Z};
  float lenp1 = sqrt(sq(P1[0])+sq(P1[1]));
  float lenp2 = sqrt(sq(P2[0])+sq(P2[1]));
  P1[0] = P1[0]/lenp1;
  P1[1] = P1[1]/lenp1;
  P2[0] = P2[0]/lenp2;
  P2[1] = P2[1]/lenp2;

  float a[2] = {destinationy-Y, destinationz-Z};
  Serial.println("---------------");

  float c = a[0]*P1[0]+a[1]*P1[1];//this has some positive/negative sign but cant figure it out
  float b = a[0]*P2[0]+a[1]*P2[1]; //reverting next few lines just to try and debug
  Serial.println(a[0]);
  Serial.println(a[1]);
  Serial.println(P1[0]);
  Serial.println(P1[1]);
  

  float lena = sqrt(sq(a[0])+sq(a[1]));

  /*float theta1 = acos(c/(lena*lenp1));//with c and b not being absolute signs, then it may have affected performance!
  float theta2 = acos(b/(lena*lenp2));

  if (theta1 > PI/2){
    c = -1*c;
  }
  if (theta2 < PI/2){
    b = -1*b;
  }*/
  c = -1*c;
  b = -1*b;
  Serial.println(c, 8);
  Serial.println(b, 8);

  //use square error to add a ramp down as it approaches
  float minimumpwm = 50;
  float ramp = 255-minimumpwm;
  float squareerror = lena;

  if (squareerror<50){
    reach1 = 1;
    ramp = ramp*squareerror/50;
  }

  //if b or c is <0, that means pull in
  if(abs(b) > abs(c)){
    //run motor2 at max
    pidsc1 = ramp*(abs(c)/abs(b))*(c/abs(c))+(c/abs(c))*minimumpwm;
    pidsc2 = ramp*(b/abs(b))+(b/abs(b))*minimumpwm;
  }
  else{
    //run motor1 at max
    pidsc1 = ramp*(c/abs(c))+(c/abs(c))*minimumpwm;
    pidsc2 = ramp*(abs(b)/abs(c))*(b/abs(b))+(b/abs(b))*minimumpwm;
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
  reach1 = 0;
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
