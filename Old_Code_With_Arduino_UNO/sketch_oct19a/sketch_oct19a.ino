#include <util/atomic.h> // For the ATOMIC_BLOCK macro
#include <math.h>

#define ENC1A 2// YELLOW
#define ENC1B 4 // GREEN
#define ENC2A 3// YELLOW
#define ENC2B 7 // GREEN
float A = 32100;

int M1A = 5;
int M1B = 6;
int M2A = 9;
int M2B = 10;
volatile int state = 0;
volatile bool reach1 = 0;
volatile bool reach2 = 0;
int move = 0;
int move2 = 0;


volatile float destination1 = 0;
volatile float destination2 = 0;
float err1 = 0;
float err2 = 0;
float priorerr1 = 0;
float priorerr2 = 0;
float pidsc1 = 0;
float pidsc2 = 0;
float integral1 = 0;
float integral2 = 0;
float radius1 = 0;
float radius2 = 0;

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
  state = 11;
}

void loop() {
  Serial.println(state);
  Serial.print(pos1);
  Serial.print("   -   ");
  Serial.println(pos2);
  pos2 = 12000;
  
  switch (state) {

    case 0:
      Serial.flush();
      Serial.println(destination1);
      while(Serial.available() == 0) {
        Serial.print("Enter state1: ");
        Serial.print(pos1);
      }
      destination1 = Serial.parseFloat();
      if(destination1 != 0){
        reach1 = 0;
        reach2 = 0;
        Serial.print("destination1: ");
        Serial.println(destination1);
      }
      Serial.flush();
      while(Serial.available() == 0) {
        Serial.print("Enter state2: ");
        Serial.println(pos2);
      }
      destination2 = Serial.parseFloat();
      if(destination2 != 0){
        reach1 = 0;
        reach2 = 0;
        state = 1;
        Serial.print("destination2: ");
        Serial.println(destination2);
      }
      break;
    //----------------------------------------------------------------------------------------------------------------------------------
    case 1: 
      Serial.print(radius1);
      Serial.print("   -   ");
      Serial.println(radius2);

      if(reach1 == 1){
        analogWrite(M1A, 0);
        analogWrite(M1B, 0);
        analogWrite(M2A, 0);
        analogWrite(M2B, 0);
        state = 2;
      }
      else{
        movemotors();
      }
      break;
    case 2://Stop after destination is reached
      Serial.println("BOTH POSITIONS REACHED");
      delay(1000);
      reach1 = 0;
      state = 11;
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
        case 2: //pull in for m1
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
      break;
    case 12:
      Serial.println("ayo");
      Serial.flush();
      delay(10);
      while(Serial.available() == 0) {
        Serial.println("where is the module: ");
      }
      move2 = Serial.parseFloat();
      switch(move2){
        case 1: //next to motor 1
          pos1 = 0;
          pos2 = A;
          state = 96;
          Serial.flush();
          delay(1000);
          break;
        case 2: //next to motor 2
          pos1 = A;
          pos2 = 0;
          state = 96;
          Serial.flush();
          delay(1000);
          break;
        default:
          break;
      }      
      Serial.flush();
      break;


    case 96:

      destination1 = 20000; 
      destination2 = 10000;

      radius1 = sqrt(sq(destination1)+sq(destination2)); 
      radius2 = sqrt(sq(A-destination1)+sq(destination2));
      Serial.println("case 95");
      state = 1;
      break;

    case 69:
      Serial.print("------------------------------------------------------");
      pos2 = A;
      float thiss = acos(float((A*A-pos1*pos1-pos2*pos2)/(-2*pos1*pos2)));
      Serial.println(thiss);
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

  if(abs(destination1-pos1)<50){
    reach1 = 1;
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

  if(abs(destination2-pos2)<50){
    reach2 = 1;
  }
}

void movemotors(){
  float runningtheta = acos(float((A*A+pos1*pos1-pos2*pos2)/(2*pos1*A)));
  if(isnan(runningtheta)){
    runningtheta = 0.0;
  }
  float Y = pos1*cos(runningtheta);
  float Z = pos1*sin(runningtheta);

  float ma = (destination2-Z)/(destination1-Y);
  float mb = (0-Z)/(A-Y);
  float mc = (0-Z)/(0-Y);

  float theta1 = atan(abs((ma-mb)/(0+ma*mb)));
  float theta2 = atan(abs((ma-mc)/(0+ma*mc)));
  float theta3 = 180-theta1-theta2;

  //float a = 1;
  float b = 1*sin(theta2)/sin(theta3);
  float c = 1*sin(theta1)/sin(theta3);

  //find the square error
  float squareerror = sqrt(sq(destination1-Y)+sq(destination2-Z));


  //use square error to add a ramp down as it approaches
  float ramp = 1;
  if (squareerror<100){
    ramp = squareerror/100;
  }

  if(b>c){
    //run m2 at max
    pidsc1 = ramp*255*(c/b);
    pidsc2 = ramp*255;
  }
  else{
    //run m1 at max
    pidsc1 = ramp*255;
    pidsc2 = ramp*255*(b/c);
  }

  if(reach1==0){
    runm1();
    runm2();
  }

  Serial.println(pidsc1);
  Serial.println(pidsc2);
  delay(1000);

  squareerror = sqrt(sq(destination1-Y)+sq(destination2-Z));

  if(squareerror<30){
    analogWrite (M1A, 0);
    analogWrite (M1B, 0);
    analogWrite (M2A, 0);
    analogWrite (M2B, 0);
    reach1 = 1;
    return;
  }
}

void runm1(){
  if(pos1>radius1){
    analogWrite (M1A, 0);
    analogWrite (M1B, pidsc1);
  }
  else{
    analogWrite (M1A, pidsc1);
    analogWrite (M1B, 0);
  }
}

void runm2(){
  if(pos2>radius2){
    analogWrite (M2A, pidsc2);
    analogWrite (M2B, 0);
  }
  else{
    analogWrite (M2A, 0);
    analogWrite (M2B, pidsc2);
  }
}