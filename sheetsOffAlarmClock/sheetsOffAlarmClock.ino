// Syren motor driver library Copyright (c) 2012 Dimension Engineering LLC
// RTC Lbrary Copyright (c) Adafruit

#include <SoftwareSerial.h>
#include <SyRenSimplified.h>

SoftwareSerial SWSerial1(NOT_A_PIN, 1); // RX on no pin (unused), TX on pin 1 (to S1).
SyRenSimplified SR1(SWSerial1); // Use SWSerial as the serial port.

//motor constants
bool motorEnable = true;
float motorEffort = 150; //from -126 to 126

//const float EFFORTRANGE = 126.0;
const float EFFORTRANGE = 1.0;
float effortP = 0.2;


//all input buttons have a pull down resistor
const int firePin = 2;
const int ignitionPin = 50;
bool fire, ignition;



//print values
float timeLast = millis();
float timeCurr = timeLast;
const float printTime = 300;

void setup()
{
  Serial.begin(115200);
  
  //enable serial
  SWSerial1.begin(9600);
  

}

void loop()
{
  //////////////////////////////////////////
  ////////        read new data       //////
  //////////////////////////////////////////

  //switches
  ignition = digitalRead(ignitionPin);
  fire = digitalRead(firePin);

  //////////////////////////////////////////
  ////////  ignition state machine    //////
  //////////////////////////////////////////
  if(ignition && fire) motorEnable = true;
  else if(!ignition) motorEnable = false;
  

  //////////////////////////////////////////
  ////////        calculations        //////
  //////////////////////////////////////////
  

  //////////////////////////////////////////
  ////////        set efforts         //////
  //////////////////////////////////////////
//  leftEffort = leftEffort * effortP;
//  rightEffort = rightEffort * effortP;
  
  if(motorEnable){
    SR1.motor(motorEffort);
  } else {
    SR1.motor(0);
  }
  

  //////////////////////////////////////////
  ////////         Telemetry          //////
  //////////////////////////////////////////
  timeCurr = millis();
  if(timeCurr - timeLast > printTime){ //prints every printTime ms

    //switches
    Serial.print("\tIgnition?: ");
    Serial.print(ignition);
    Serial.print("\tFire Missiles?: ");
    Serial.print(fire);
    Serial.print("\tmotor enable?: ");
    Serial.print(motorEnable);
    Serial.print("\n");

    //values
    Serial.print("\tmotor effort: ");
    Serial.print(motorEffort);
    
    Serial.print("\n\n\n");
    timeLast = timeCurr;
  }
  
}
