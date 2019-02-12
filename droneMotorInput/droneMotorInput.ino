#include <RH_ASK.h>
#include <SPI.h>
/*
 * RADIO/ELEVATION BRANCH
 * 
 * Pitch & Roll Controll From Power Glove
 * Pitch & Roll:
 *        Impplemented to map drone to exactly glove orientation from behind, imverted scale
 *        Pitch & Roll based off slowing adjacent motors
 *        
 * Yaw:  NOTE ~may need to swap f1 and f2 motor reading~
 *        Yaw control NOT implemented, glove needs to be Mk. III for finger control then yaw can be input
 *        Yaw based off of slowing on pair of diagonal motors & speeding up the other pair of diagional motors
 *       
 * Elevation:
 *        Likely to replace yaw with elevation control on flex sensors 1 and 2 on glove
 *        
 * Issues:        
 *        Can NOT use PWM pin 9 when using 433 radio comm. Writing to it causes the loop to hold still nnot sure why
 *        
 * Informational Link on Drone Design
 * https://www.dronezon.com/learn-about-drones-quadcopters/how-a-quadcopter-works-with-propellers-and-motors-direction-design-explained/
 */
RH_ASK driver;
char key;
String data = "";
int startInd, ind1, ind2, ind3, ind4, ind5, endInd;
String runningS,xS,yS,zS;
String f1S,f2S;

double x,y,z;
double f1,f2;

double pos[2];

//Based off values of 260
//rCOS(deg) for x
//rSIN(deg) for y
double frontRight[] =  {183,183}; //f2
double frontLeft[] = {-183,183};  //f1
double rearLeft[] = {-183,-183}; //f2
double rearRight[] =  {183,-183}; //f1

double FRDist;
double FLDist;
double RLDist;
double RRDist;

double FRScale;
double FLScale;
double RLScale;
double RRScale;

double f1Scale;
double f2Scale;

double FROut;
double FLOut;
double RLOut;
double RROut;

int FRPin = 10;
int FLPin = 6;
int RLPin = 5;
int RRPin = 3;

 

void setup() {

  Serial.begin(9600);
 
  if (!driver.init())
         Serial.println("init failed");
  delay(1000);  

  pinMode(FRPin, OUTPUT);
  pinMode(FLPin, OUTPUT);
  pinMode(RLPin, OUTPUT);
  pinMode(RRPin, OUTPUT);
  
  analogWrite(FRPin, 0);
  analogWrite(FLPin, 0);
  analogWrite(RLPin, 0);
  analogWrite(RRPin, 0);
}

void loop() {
//Input Allocation---------------------------------------------
/*
 * Input Format
 * 
 * <runningS/x/y/z/f1/f2>
 * 
 * ex.   ~neither of these states are possible, just to show max value~
 *  <1/255/255/255/255/255> 
 *  <0/0/0/0/0/0>
 */



  uint8_t buf[27];
    uint8_t buflen = sizeof(buf);
    while (driver.recv(buf, &buflen)) // Non-blocking
    {
      int i;
      Serial.println((char*)buf); 
      data = buf;        
    
    
      startInd = data.indexOf('<');
      ind1 = data.indexOf('/');
      ind2 = data.indexOf('/',ind1+1);
      ind3 = data.indexOf('/',ind2+1);
      ind4 = data.indexOf('/',ind3+1);
      ind5 = data.indexOf('/',ind4+1);
      endInd = data.indexOf('>');

      runningS = data.substring(startInd+1,ind1);
      xS = data.substring(ind1+1,ind2);
      yS = data.substring(ind2+1,ind3);
      zS = data.substring(ind3+1,ind4);

      f1S = data.substring(ind4+1,ind5);
      f2S = data.substring(ind5+1,endInd);
      
      Serial.println("Data: "+data);
      Serial.println("x: "+xS);
      Serial.println("y: "+yS);
      Serial.println("z: "+zS);
      Serial.println("f1: "+f1S);
      Serial.println("f2: "+f2S);

        //Convert string data to #s
        x = xS.toDouble();
        y = yS.toDouble();
        z = zS.toDouble();

        f1 = f1S.toDouble();
        f2 = f2S.toDouble();
        
        

        //Map distances on power map
        pos[0] = x;
        pos[1] = y;
        
        FRDist = sqrt(sq(x-frontRight[0]) + sq(y-frontRight[1]));
        FLDist = sqrt(sq(x-frontLeft[0]) + sq(y-frontLeft[1]));
        RLDist = sqrt(sq(x-rearLeft[0]) + sq(y-rearLeft[1]));
        RRDist = sqrt(sq(x-rearRight[0]) + sq(y-rearRight[1])); 
        
        //Serial.println(FRDist);
        //Serial.println(FLDist);
        //Serial.println(RLDist);
        //Serial.println(RRDist);
        
        //Build Power Scale from 0 to 1, map to motor out from 0 to 255

        f1Scale = f1/255;
        f2Scale = f2/255;
        
        //Front Right
        if(FRDist<1)FRDist = 1;
        FRScale = FRDist/735.39;
        FROut = 115 + 75*FRScale + 32.5*f1Scale * - 32.5*f2Scale;
        
        //Front Left
        if(FLDist<1)FLDist = 1;
        FLScale = FLDist/735.39;
        FLOut = 115 + 75*FLScale + 32.5*f1Scale - 32.5*f2Scale;

        //Rear Left
        if(RLDist<1)RLDist = 1;
        RLScale = RLDist/735.39;
        RLOut = 115 + 75*RLScale + 32.5*f1Scale - 32.5*f2Scale;
        
        //Rear Right
        if(RRDist<1)RRDist = 1;
        RRScale = RRDist/735.39;
        RROut = 115 + 75*RRScale + 32.5*f1Scale - 32.5*f2Scale;
        
        //Outputs 
        Serial.println("Outputs");
        Serial.println("---------");
        Serial.println(runningS);
        Serial.println(FROut);
        Serial.println(FLOut);
        Serial.println(RLOut);
        Serial.println(RROut);
        Serial.println("---------");

        //PWM Control
        if(runningS == "1"){
          Serial.println("Outputs sent");
          analogWrite(FRPin, FROut);
          analogWrite(FLPin, FLOut);
          analogWrite(RLPin, RLOut);
          analogWrite(RRPin, RROut);
          //delay(20);
        }else Serial.println("No outputs sent");


       //runningS == "0"; 
       data = "";
           
    } 
  
}
