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

int* DateAndTime();

//motor constants
float motorEffort = 150; //from -126 to 126

//const float EFFORTRANGE = 126.0;
const float EFFORTRANGE = 1.0;
float effortP = 0.2;

//Numpad Setup
const byte ROWS = 4;
const byte COLS = 4;

char hexaKeys[ROWS][COLS] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'}
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
long timeLast = millis();
long timeCurr = timeLast;
const float printTime = 300;

//time constants for clock update
long clockTimeLast = millis();

//misc variables
char lastKeyPress;
bool enteringState = true;

char daysOfTheWeek[7][12] = {
  "S",
  "M",
  "T",
  "W",
  "T",
  "F",
  "U"
};

const char monthInWords[13][4] = {" ", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", 
                                         "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};

int alarmTime[4] = {99, 99, 99, 99};
int newAlarmTime[4];
int hourMinuteAlarmTime[2] = {25, 70};

//typedef enum {
//  IDLING,
//  SETALARM,
//  ALARM
//} State;
//
//State state = IDLING;

int state = 0;

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

  //rtc.adjust(DateTime(2023, 1, 15, 1, 40, 0)); //only use once sets to Jan 15, 2023, 01:40:00

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
  fire = digitalRead(firePin);
  char numKey = numKeypad.getKey();
  if(numKey) lastKeyPress = numKey; //store last numberpad key;

  
  //////////////////////////////////////////
  ////////       state machine        //////
  //////////////////////////////////////////

//  switch (state){ //case switch borked on the arduino it seems
//    case 0:
  if(state == 0){

      if(millis() - clockTimeLast > 1000){
        clockTimeLast = millis();
        
        DateTime now = rtc.now();
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(now.month());
        lcd.print("/");
        lcd.print(now.day());
//        lcd.print("/");
//        lcd.print(now.year());
        lcd.print(" ");
        lcd.print(daysOfTheWeek[now.dayOfTheWeek()]);
        lcd.print(" ");
        lcd.print(now.hour());
        lcd.print(":");
        lcd.print(now.minute());
        lcd.print(":");
        if(now.second() < 10) lcd.print("0");
        lcd.print(now.second());
        
        
        lcd.setCursor(0, 1);
        lcd.print("# to set alarm");

        if(hourMinuteAlarmTime[0] <= (int(now.hour())) && hourMinuteAlarmTime[1] <= (int(now.minute()))) //check to see if go alarm
            state = 2; 

        Serial.print(hourMinuteAlarmTime[0]);
        Serial.print(int(now.hour()));
        Serial.print(hourMinuteAlarmTime[0] <= (int(now.hour())));
        Serial.print("\t");
        Serial.print(hourMinuteAlarmTime[1]);
        Serial.print(int(now.minute()));
        Serial.print(hourMinuteAlarmTime[1] <= (int(now.minute())));
        
      }

      if (lastKeyPress == '#') {
        lastKeyPress = ' ';
        state = 1;
        enteringState = true;
      }


//      int* currTime = DateAndTime();
//      if(alarmTime[0] >= currTime[3] && alarmTime[1] >= currTime[4] && alarmTime[2] >= currTime[5]) state = 2;
//      for(int f = 0; f <= 2; f++) {
//        int x = 3;
//        Serial.print("f: " + alarmTime[f]);
//        Serial.println(" and x: " + currTime[x]);
//        //Serial.print(alarmTime[f]);
//        //Serial.println(currTime[x]);
//        x++;
//      }
    
  } else if (state == 1){    ////////////set alarm state///////////////////////
//    case 1: //set alarm
   if(enteringState){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Set alarm");
      lcd.setCursor(0, 1);
      lcd.print("in military time");
      enteringState = false;
      delay(500);
   }

    for (int i = 0; i <= 3; i++){
      char numKey = numKeypad.getKey();
      while(!numKey) numKey = numKeypad.getKey();
      int intNumKey = numKey - 48;
//      Serial.print(numKey);
//      Serial.print("\t");
//      Serial.print(intNumKey);
//      Serial.print("\t");
      
      alarmTime[i] = intNumKey;
      Serial.print("\n");
      Serial.print(alarmTime[i]);
      Serial.print("\t");
      Serial.print(alarmTime[0]);
      Serial.print(alarmTime[1]);
      Serial.print(":");
      Serial.print(alarmTime[2]);
      Serial.print(alarmTime[3]);
      Serial.print("\t");
      Serial.print(hourMinuteAlarmTime[0]);
      Serial.print(":");
      Serial.print(hourMinuteAlarmTime[1]);

      if(i == 1) hourMinuteAlarmTime[0] = alarmTime[0]*10 + alarmTime[1]; //hour
      if(i == 3) hourMinuteAlarmTime[1] = alarmTime[2]*10 + alarmTime[3]; //minute

      Serial.print("\t");
      Serial.print(hourMinuteAlarmTime[0]);
      Serial.print(":");
      Serial.print(hourMinuteAlarmTime[1]);
     
//      Serial.print(alarmTime[i]);
//
//      Serial.print("\t");
//      Serial.print(alarmTime[0]);
//      Serial.print("\t");
//      Serial.print(alarmTime[1]);
//      Serial.print("\t");
//      Serial.print(alarmTime[2]);
//      Serial.print("\t");
//      Serial.print(alarmTime[3]);
      delay(500);
    }
  
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("alarm set for:");
    lcd.setCursor(0,1);
    lcd.print(alarmTime[0]);
    lcd.print(alarmTime[1]);
    lcd.print(":");
    lcd.print(alarmTime[2]);
    lcd.print(alarmTime[3]);
    state = 0;
    enteringState = true;
    delay(5000);
      
  } else if (state == 2) {   //////////////alarm state//////////////////

    
//    case 2: //alarm
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ALARM TIME");
      
      SR1.motor(100); //-126 to 126
      delay(1000);
      SR1.motor(0);
      delay(1000);

      state = 0;
    }
    

  

  //////////////////////////////////////////
  ////////         Telemetry          //////
  //////////////////////////////////////////
  
  timeCurr = millis();
  if(timeCurr - timeLast > printTime){ //prints every printTime ms

    //switches
    Serial.print("\tFire Missiles?: ");
    Serial.print(fire);
    Serial.print("\tstate: ");
    Serial.print(state);
    Serial.print("\talarm: ");
    Serial.print(alarmTime[0]);
    Serial.print(alarmTime[1]);
    Serial.print(":");
    Serial.print(alarmTime[2]);
    Serial.print(alarmTime[3]);
    Serial.print("\n");
    Serial.print("\tLast Keypress: ");
    Serial.print(lastKeyPress);

    
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

int* DateAndTime() {
  DateTime now = rtc.now();
//  Serial.print("Date & Time: " + now.year() + '/' + now.month() + 
//    '/' + now.day() + " (" + daysOfTheWeek[now.dayOfTheWeek()] + 
//      ") " + now.hour() + ':' + now.minute() + ':' + now.second());
  static int r[6];
  r[0] = now.year();
  r[1] = now.month();
  r[2] = now.day();
//  r[3] = daysOfTheWeek[now.dayOfTheWeek()];
  r[3] = now.hour();
  r[4] = now.minute();
  r[5] = now.second();
  return r;
}
