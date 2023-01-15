/*  Syren motor driver library Copyright (c) 2012 Dimension Engineering LLC
 *  RTC Lbrary Copyright (c) Adafruit
 *  Keypad and LCD Code sourced from Krishna Pattabiraman's how to set up a keypad on ardunio
 *  located at https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
 */

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <SoftwareSerial.h>
#include <SyRenSimplified.h>
#include <RTClib.h>

RTC_DS1307 rtc;
SoftwareSerial SWSerial1(NOT_A_PIN, 1); // RX on no pin (unused), TX on pin 1 (to S1).
SyRenSimplified SR1(SWSerial1); // Use SWSerial as the serial port.

//motor constants
float motorEffort = 150; //from -126 to 126

//const float EFFORTRANGE = 126.0;
const float EFFORTRANGE = 1.0;
float effortP = 0.2;

//Numpad Setup
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {9, 8, 7, 6}; 
byte colPins[COLS] = {5, 4, 3, 2}; 

Keypad numKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//LCD Setup
LiquidCrystal_I2C lcd(0x27, 16, 2);  

//inputs
const int firePin = 12;
const int motorEngagedPin = 13;
bool fire, motorEnable;

//print values
float timeLast = millis();
float timeCurr = timeLast;
const float printTime = 300;

//misc variables
char lastKeyPress;

char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

const char monthInWords[13][4] = {" ", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", 
                                         "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

void setup()
{
  Serial.begin(115200);
  
  lcd.init();
  lcd.backlight();

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1);
  }

  //rtc.adjust(DateTime(2023, 1, 14, 1, 18, 0)); //only use once sets to Jan 14, 2023, 00:10:00

  pinMode(motorEngagedPin, INPUT);
  pinMode(firePin, INPUT);

  //enable serial
  SWSerial1.begin(9600);

}

void loop()
{
  //////////////////////////////////////////
  ////////        read new data       //////
  //////////////////////////////////////////

  //switches
  motorEnable = digitalRead(motorEngagedPin);
  fire = digitalRead(firePin);
  char numKey = numKeypad.getKey();
  if(numKey) lastKeyPress = numKey; //store last numberpad key;
  //////////////////////////////////////////
  ////////       state machine        //////
  //////////////////////////////////////////
  if(!motorEnable){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Motor Disabled");
    lcd.setCursor(0, 1);
    lcd.print("Reenable to Operate");
  }
  

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
    Serial.print("\tMotor Disabled??: ");
    Serial.print("");
    Serial.print("\tFire Missiles?: ");
    Serial.print(fire);
    Serial.print("\tmotor enable?: ");
    Serial.print(motorEnable);
    Serial.print("\n");
    Serial.print("\tLast Keypress: ");
    Serial.print(lastKeyPress);

    //values
    Serial.print("\tmotor effort: ");
    Serial.print(motorEffort);
    
    Serial.print("\n\n\n");
    timeLast = timeCurr;
  }
  
}

// returns array that is formatted [year, month, day, day of week, hour, minute, second]
//struct DateAndTime(){
//  DateTime now = rtc.now();
////  Serial.print("Date & Time: " + now.year() + '/' + now.month() + 
////    '/' + now.day() + " (" + daysOfTheWeek[now.dayOfTheWeek()] + 
////      ") " + now.hour() + ':' + now.minute() + ':' + now.second());
//
//  return([now.year(), now.month(), now.day(), daysOfTheWeek[now.dayOfTheWeek()], 
//    now.hour(), now.minute(), now.second()]);
//}

int * DateAndTime() {
  DateTime now = rtc.now();
//  Serial.print("Date & Time: " + now.year() + '/' + now.month() + 
//    '/' + now.day() + " (" + daysOfTheWeek[now.dayOfTheWeek()] + 
//      ") " + now.hour() + ':' + now.minute() + ':' + now.second());
  static int r[6];
  r[0] = now.year();
  r[1] = now.month();
  r[2] = now.day();
  r[3] = daysOfTheWeek[now.dayOfTheWeek()];
  r[4] = now.hour();
  r[5] = now.minute();
  r[6] = now.second();
//  return([now.year(), now.month(), now.day(), daysOfTheWeek[now.dayOfTheWeek()], 
//    now.hour(), now.minute(), now.second()]);
return r;
}
