/* Code sourced from Krishna Pattabiraman's how to set up a keypad on arduni
 *  https://www.circuitbasics.com/how-to-set-up-a-keypad-on-an-arduino/
 */

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;

//char hexaKeys[ROWS][COLS] = {
//  {'1', '2', '3', 'A'},
//  {'4', '5', '6', 'B'},
//  {'7', '8', '9', 'C'},
//  {'*', '0', '#', 'D'}
//};

char hexaKeys[ROWS][COLS] = {
  {'D', 'C', 'B', 'A'},
  {'#', '9', '6', '3'},
  {'0', '8', '5', '2'},
  {'*', '7', '4', '1'}
};

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);  

//print values
float timeLast = millis();
float timeCurr = timeLast;
const float printTime = 300;

void setup(){
  Serial.begin(115200);
  lcd.init();
  lcd.backlight(); 
}

void loop(){
  char customKey = customKeypad.getKey();
  if (customKey){
    lcd.clear();
    lcd.setCursor(0, 0); 
    lcd.print(customKey);
  }

  timeCurr = millis();
  if(timeCurr - timeLast > printTime){ //prints every printTime ms
//    Serial.print("\tKey Press: ");
//    Serial.print(customKey);
    
    Serial.print("\n\n\n");
    timeLast = timeCurr;
  }
  
}
