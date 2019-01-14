
char key;
String data = "";
String runningS,xS,yS,zS;
double x,y,z;
int startInd, ind1, ind2, ind3, endInd;

double pos[2];

double frontRight[] =  {260,260};
double frontLeft[] = {-260,260};
double rearLeft[] = {-260,-260};
double rearRight[] =  {260,-260};

double FRDist;
double FLDist;
double RLDist;
double RRDist;

double FRScale;
double FLScale;
double RLScale;
double RRScale;

double FROut;
double FLOut;
double RLOut;
double RROut;

int FRPin = 5;
int FLPin = 6;
int RLPin = 9;
int RRPin = 10;

void setup() {
  Serial.begin(9600);
  delay(1000);

  analogWrite(FRPin, 0);
  analogWrite(FLPin, 0);
  analogWrite(RLPin, 0);
  analogWrite(RRPin, 0);
}

void loop() {
//Input Allocation---------------------------------------------
/*
 * Input Format
 * <motorsRunning/xxx/yyy/zzz>
 * ex. 
 *  <1/255/255/255>
 *  <0/0/0/0>
 */





  while(Serial.available()){
    key = Serial.read();
    data.concat(key);
    
    if(key == '>'){ // if end of message received
      startInd = data.indexOf('<');
      ind1 = data.indexOf('/');
      ind2 = data.indexOf('/',ind1+1);
      ind3 = data.indexOf('/',ind2+1);
      endInd = data.indexOf('>');

      runningS = data.substring(startInd+1,ind1);
      xS = data.substring(ind1+1,ind2);
      yS = data.substring(ind2+1,ind3);
      zS = data.substring(ind3+1,endInd);

      Serial.println(startInd);Serial.println(ind1);Serial.println(ind2);Serial.println(ind3);Serial.println(endInd);
      Serial.println("Data: "+data);
      Serial.println("x: "+xS);
      Serial.println("y: "+yS);
      Serial.println("z: "+zS);


        x = xS.toDouble();
        y = yS.toDouble();
        z = zS.toDouble();
      
        xS = "";
        yS = "";
        zS = "";
        
        pos[0] = x;
        pos[1] = y;
      
        FRDist = sqrt(sq(x-frontRight[0]) + sq(y-frontRight[1]));
        FLDist = sqrt(sq(x-frontLeft[0]) + sq(y-frontLeft[1]));
        RLDist = sqrt(sq(x-rearLeft[0]) + sq(y-rearLeft[1]));
        RRDist = sqrt(sq(x-rearRight[0]) + sq(y-rearRight[1])); 
        
        Serial.println(FRDist);
        Serial.println(FLDist);
        Serial.println(RLDist);
        Serial.println(RRDist);
        
        
        //Front Right
        if(FRDist<1)FRDist = 1;
        FRScale = FRDist/735.39;
        FROut = 255*FRScale;
        if(FROut < 75) FROut = 75;
        
        //Front Left
        if(FLDist<1)FLDist = 1;
        FLScale = FLDist/735.39;
        FLOut = 255*FLScale;
        if(FLOut < 75) FLOut = 75;

        //Rear Left
        if(RLDist<1)RLDist = 1;
        RLScale = RLDist/735.39;
        RLOut = 255*RLScale;
        if(RLOut < 75) RLOut = 75;
        
        //Rear Right
        if(RRDist<1)RRDist = 1;
        RRScale = RRDist/735.39;
        RROut = 255*RRScale;
        if(RROut < 75) RROut = 75;
        /*
        Serial.println(FRScale); 
`       */

        Serial.println("Outputs");
        Serial.println("---------");
        Serial.println(runningS);
        Serial.println(FROut);
        Serial.println(FLOut);
        Serial.println(RLOut);
        Serial.println(RROut);
        Serial.println("---------");
  
        if(runningS == "1"){
          Serial.println("Outputs sent");
          analogWrite(FRPin, FROut);
          analogWrite(FLPin, FLOut);
          analogWrite(RLPin, RLOut);
          analogWrite(RRPin, RROut);
          delay(20);
        }else Serial.println("No outputs sent");

       runningS == "0"; 
       data = "";
     }      
  }
  
}
