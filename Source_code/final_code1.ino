#include <Wire.h> //I2C Arduino Library

#define address 0x1E //0011110b, I2C 7bit address of HMC5883 
#define trigPin 3
#define echoPin 2  

const int motorPin =4;
int Gpsdata;             // for incoming serial data
unsigned int finish =0;  // indicate end of message
unsigned int pos_cnt=0;  // position counter
unsigned int lat_cnt=0;  // latitude data counter
unsigned int log_cnt=0;  // longitude data counter
unsigned int flg    =0;  // GPS flag
unsigned int com_cnt=0;  // comma counter
char lat[20];            // latitude array
char lg[20];             // longitude array
double LAT = 8.4567;     //From user
double LOG = 76.93;      //From user
double dlat,dlog;        //From gps
int flag=0;
void Receive_GPS_Data();

void setup()
{
  pinMode(trigPin,OUTPUT);
  pinMode(echoPin,INPUT);
  Wire.begin();
  Serial.begin(9600);   // opens serial port, sets data rate to 9600 bps
  Serial1.begin(9600);  //GPS
  Wire.beginTransmission(address); //open communication with HMC5883
  Wire.write(0x02); //select mode register
  Wire.write(0x00); //continuous measurement mode
  Wire.endTransmission();

}


void loop()
{
  Receive_GPS_Data();           
  Serial.print("Latitude : ");
  Serial.println(lat);
  Serial.print("Longitude : ");
  Serial.println(lg);
  finish = 0;
  pos_cnt = 0;
  String Lat(lat);
  String Log(lg);
  dlat = Lat.toFloat()-LAT;
  dlog = Log.toFloat()-LOG;
  if(dlat<0) 
  {
    Serial.print("to NORTH");
  }
  else Serial.print("to SOUTH");
  if(dlog<0)
  {
    Serial.print("-to EAST\n");
  }
  else Serial.print("-to WEST\n");
  
  /*COMPASS*/
  
  int x,y,z;
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();

 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
  x = Wire.read()<<8; //X msb
  x |= Wire.read(); //X lsb
  z = Wire.read()<<8; //Z msb
  z |= Wire.read(); //Z lsb
  y = Wire.read()<<8; //Y msb
  y |= Wire.read(); //Y lsb
 }

float angle = ((atan2(x,y)/3.14)*180);

//Print out values of each axis

/*Serial.print("x: ");
  Serial.print(x);
  Serial.print("  y: ");
  Serial.print(y);
  Serial.print("  z: ");
  Serial.println(z);
  Serial.print("  angle: ");
  Serial.println(angle);*/

  Serial.print("Angle: ");
  Serial.print(angle);
  Serial.print(" deg\n");
  if(angle>10) 
  Serial.print("*STURN LEFT#\n");
  else if(angle<-10) 
  Serial.print("*STURN RIGHT#\n");
  else 
  Serial.print("*SGO STRAIGHT#\n");

  /* ULTRASONIC SENSOR FOR OBJECT DETECTION*/
  
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  digitalWrite(trigPin,LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  long distance = pulseIn(echoPin,HIGH);
  distance = distance*0.034/2;
  Serial.print("Distance : ");
  Serial.println(distance);
  
/*CODE FOR PROVIDING ALERT USING VIBRATION MOTOR*/
   if(distance<20)
   {
     digitalWrite(motorPin, HIGH);
     delay(800);
   }
     digitalWrite(motorPin, LOW);
     delay(1000);

  }





/*******************************************************************************************

  Function    : Receive_GPS_Data()

  Description : finding Latitudse and longitude from GPRMC message

 

*******************************************************************************************/

  void Receive_GPS_Data()

  {

    while(finish==0){

      while(Serial1.available()>0){         // Check GPS data

        Gpsdata = Serial1.read();

        flg = 1;

       if( Gpsdata=='$' && pos_cnt == 0)   // finding GPRMC header

         pos_cnt=1;

       if( Gpsdata=='G' && pos_cnt == 1)

         pos_cnt=2;

       if( Gpsdata=='P' && pos_cnt == 2)

         pos_cnt=3;

       if( Gpsdata=='R' && pos_cnt == 3)

         pos_cnt=4;

       if( Gpsdata=='M' && pos_cnt == 4)

         pos_cnt=5;

       if( Gpsdata=='C' && pos_cnt==5 )

         pos_cnt=6;

       if(pos_cnt==6 &&  Gpsdata ==','){   // count commas in message

         com_cnt++;

         flg=0;

       }

 

       if(com_cnt==3 && flg==1){

        lat[lat_cnt++] =  Gpsdata;         // latitude

        flg=0;

       }

 

       if(com_cnt==5 && flg==1){

         lg[log_cnt++] =  Gpsdata;         // Longitude

         flg=0;

       }

 

       if( Gpsdata == '*' && com_cnt >= 5){

         com_cnt = 0;                      // end of GPRMC message

         lat_cnt = 0;

         log_cnt = 0;

         flg     = 0;

         finish  = 1;

 

      }

    }

 }

}
