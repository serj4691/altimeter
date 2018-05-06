#include "BMP280.h"
#include "Wire.h"
#include <SPI.h>
#include <SD.h>
#include "ds1302.h"

//#define BMP280_CONFIG_REG                    (0xF5)  /*Configuration Register */
//#define BMP280_FILTER_COEFF_16                (0x04)
//#define BMP280_CONFIG_REG_FILTER__POS              (2)
//#define BMP280_CONFIG_REG_FILTER__MSK              (0x1C)
//#define BMP280_CONFIG_REG_FILTER__LEN              (3)
//#define BMP280_CONFIG_REG_FILTER__REG              (BMP280_CONFIG_REG)
//#define BMP280_SET_BITSLICE(regvar, bitname, val)\
//  ((regvar & ~bitname##__MSK) | ((val<<bitname##__POS)&bitname##__MSK))

#include <SoftwareSerial.h>
SoftwareSerial mySerial(2, 3); // указываем пины RX и TX
int  LED = 10;


#define P0 1013.25
const int chipSelect = 4;
int hh=0;
BMP280 bmp;
char DataFile[]="97.txt";
//char dfile[]="dt.txt";
char hold[]="98";
const int kCePin   = 8; //12;  // Chip Enable
const int kIoPin   = 9;//11;  // Input/Output
const int kSclkPin = 7; //13;  // Serial Clock
double T,P,B;
double val1=0,val2=0,val3=0;
//int s=0,sz=0;
//char fg[]=" ";
char cc[3];


//Sd2Card card;
DS1302 rtc(kCePin, kIoPin, kSclkPin);

Time  t = rtc.time();

//------------------------------------------------------------------------------
// call back for file timestamps
void cb_dateTime(uint16_t* date, uint16_t* time) {
  t = rtc.time();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(t.yr, t.mon, t.date);
  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(t.hr, t.min, t.sec);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/*
void ChangeMode()
{
  cli();  
  if (mySerial.available())
    {
      mySerial.write(hh);
    }
    for (int i=0;i<1000;i++)
    {
      
    }
    digitalWrite(LED, LOW);
  sei();
}*/
//------------------------------------------------------------------------------


void setup()
{
//  attachInterrupt(PCINT2, ChangeMode, RISING);
  pinMode(2,INPUT);
pinMode(3,OUTPUT);
  pinMode(LED, OUTPUT);

mySerial.begin(9600);

  Serial.begin(9600);
   if(!bmp.begin()){
    Serial.println("BMP init failed!");
    while(1);
  }
  else Serial.println("BMP init success!");
  
  bmp.setOversampling(4);   //Number from 0 to 4 
  //4 being the slowest and most accurate
  //0 being the fastest
  Serial.println("Logging has Begun");
  Serial.println (t.hr);
        Serial.println(":");
        Serial.println(t.min);
        Serial.println(":");
       Serial.println(t.sec);
  
   SdFile::dateTimeCallback(cb_dateTime);
     delay(50);
    char result = bmp.startMeasurment();
    delay(100);
    bmp.getTemperatureAndPressure(T,P);
       delay(50);
    B = bmp.altitude(P,P0);
  //  hh=(B/100)*100;



  
  Serial.print("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
 //if (!card.init(SPI_HALF_SPEED, chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  Serial.println("card initialized.");
  //So you can find different trials easier//
  while(SD.exists((char*)DataFile)){
    hold[1]+=1;
    if(hold[1]>9+48){
      hold[0]+=1;
      hold[1]=(0+48);
    }
    if(hold[0]>9+48){
      hold[0]=(0+48);
    }
    DataFile[1]=hold[1];
    DataFile[0]=hold[0];
  }
 /* if (SD.exists(dfile))
    {
       File Rocket_Logi = SD.open((const char*)dfile, FILE_READ); 
        File Rocket_Logo = SD.open((const char*)DataFile, FILE_WRITE);
       sz=Rocket_Logi.size();
    while( s<sz)
       {
        Rocket_Logo.write(Rocket_Logi.read());
        s++;
       }
        
    Rocket_Logi.close();
    Rocket_Logo.close();
    }
    SD.remove(dfile);
  strcpy(DataFile,dfile);*/
  
  Serial.print("Launch file: ");
 // Serial.print("*");
  for(int i=0;i<6;i++)
    Serial.print(DataFile[i]);
 // Serial.println("*");
  File Rocket_Log = SD.open((const char*)DataFile, FILE_WRITE);
  if (Rocket_Log){
    Rocket_Log.print("Rocket Launch #");
    Rocket_Log.print(DataFile[1],DEC);
    Rocket_Log.println(DataFile[0],DEC);
    Rocket_Log.close();

   // sei();
   hh=0;
  }
 
    /*
unsigned char data[2];
data[0] =BMP280_CONFIG_REG;
   char res = bmp.readBytes(data, 2);
   Serial.println("---");
     Serial.println(data[0]);*/
}
void loop()
{
  //double T,P;
  char result = bmp.startMeasurment();

  

  if(result!=0){
    delay(result);
    result = bmp.getTemperatureAndPressure(T,P);

    if(result!=0)
    {
      double A = bmp.altitude(P,P0);


val3=val2;
val2=val1;
val1=A-B;
if(val1>hh)
  hh=(val1*100)/100;
  
      File Rocket_Log = SD.open((const char*)DataFile, FILE_WRITE);
      if (Rocket_Log) {
      //  Rocket_Log.print("Altitude(m)-");
        Rocket_Log.print((val1+val2+val3)/3);//*3.28084);
        Rocket_Log.print(" , ");
      //  Rocket_Log.print("Time-");
        t = rtc.time();
        Rocket_Log.print(t.hr);
        Rocket_Log.print(":");
        Rocket_Log.print(t.min);
        Rocket_Log.print(":");
        Rocket_Log.print(t.sec);
         Rocket_Log.print("\n");
        Rocket_Log.close();
        delay(100);
      }
    }
    else {
      Serial.println("Error.");
    }
  }
  else {
    Serial.println("Error.");
  }

if (mySerial.available())
{
char c = mySerial.read(); // читаем из software-порта
//Serial.write(c); // пишем в hardware-порт
    if(c == '1') {
     //  digitalWrite(LED, HIGH); // если 0, то включаем LED
       itoa(hh,cc,10);
     //  strcpy(jj,fg);
     // strcat(jj,cc);
        mySerial.write(cc);
   //     mySerial.write(c);
        Serial.write(cc);
      //  Serial.write(jj);
    }
    else digitalWrite(LED, LOW);
}

  
}

