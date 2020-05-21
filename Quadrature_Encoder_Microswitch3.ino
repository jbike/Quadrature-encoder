
//https://www.rs-online.com/designspark/detecting-quadrature-encoders-with-arduino-uno
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SparkFunMiniMoto.h>
#include <EEPROM.h>

#define BACKLIGHT_PIN     12
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
const int buttonPin = A1;     // the number of the pushbutton pin
const int buttonPin2 = A2;
const int ledPin =  13;      // the number of the LED pin for testing
const int BUTTON1 = 1;
 const int BUTTON2 = 2;
 const int BUTTON3 = 3;
 const int BUTTON4 = 4;
 const int BUTTON5 = 5;
 
 const int BUTTON1LOW = 500;
 const int BUTTON1HIGH = 530;
 const int BUTTON2LOW = 400;
 const int BUTTON2HIGH = 460;
 const int BUTTON3LOW = 725;
 const int BUTTON3HIGH = 780;
 const int BUTTON4LOW = 600;
 const int BUTTON4HIGH = 650;
 const int BUTTON5LOW = 200; 
 const int BUTTON5HIGH = 360;
 
 // Variables will change:
 int tmpButtonState = LOW;             // the current reading from the input pin
 int ledState = HIGH;         // the current state of the output pin
 int buttonState;             // the current reading from the input pin
 int lastButtonState = LOW;   // the previous reading from the input pin
 int set=0; // variable for flag to go left or right
 int SetGoR=0; // saves to flash right, or goes to right saved position
 int SetGoL=0; // saves to flash left, or goes to right saved position
  
 // the following variables are long's because the time, measured in miliseconds,
 // will quickly become a bigger number than can be stored in an int.
 long lastDebounceTime = 0;  // the last time the output pin was toggled
  long debounceDelay = 50;    // the debounce time; increase if the output flickers

volatile int Pos1 = 0; //Actual Position
long PosL = 0; // saved left position
int Left =0; // compare values against Pos1
int Right =0;
int PosR = 0; //saved right position
const int encoder1PinA = 2;
const int encoder1PinB = 3;

const int limit = 6;
const int limitGnd = 7;

MiniMoto motor1(0xD0); // A1 = 1, A0 = 1 (default)

void setup(){
  pinMode(A1, INPUT); // for buttons 1 to 5
  pinMode(limit, INPUT_PULLUP);
  pinMode(limitGnd,OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  digitalWrite(limitGnd, LOW);
  lcd.begin(16,2);
  lcd.backlight();
  lcd.setCursor (0,0);
  lcd.print("Encoder Tester");  // Print a message to the LCD.
  while(digitalRead(limit)==LOW){
    motor1.drive(-100);
    }
  motor1.brake();
  delay(40);
  Pos1=0;
  delay(1000);
  lcd.setCursor (0,0);
  lcd.print("                "); // erase first line
  attachInterrupt(0, runEncoder1, RISING );
}
void loop(){

lcd.setCursor (0,1);        // go to the 2nd line
lcdGo(); // display Pos1

ckbtn();
switch(buttonState){
case BUTTON1:
SetGoR = 1;
break;
case BUTTON2: 
SetGoL = 1;
break;
case BUTTON3:
set = 1;
break;
case BUTTON4:
motor1.drive(-100);
break;
case BUTTON5:
motor1.drive(100);
break;
default:
motor1.brake();
break;
}
if (set == 1){
if (SetGoL == 1){
  int eeAddress = 0;
  Left=Pos1;
  EEPROM.put(eeAddress,Left);
lcd.setCursor (0,0);        // go to the 1st line
lcd.print("       ");
lcd.print(Left, DEC);

  SetGoL=0;
  set = 0;
  delay(100);
}
}
if (set == 1){
if (SetGoR == 1){
int eeAddress = 2;
  Right=Pos1; 
  EEPROM.put(eeAddress,Right);
lcd.setCursor (10,0);        // go to the 1st line
lcd.print("       ");
lcd.print(Right, DEC);
  SetGoR=0;
  set=0;
  delay(100);
}
}
if (set == 0 && SetGoL == 1){
 EEPROM.get(0,Left);
lcd.setCursor (0,0);        // 1st line
lcd.print("     ");
delay(100);
lcd.setCursor (0,0);        // 1st line
lcd.print(Left, DEC);
delay(100); 
while((Pos1 - Left) > 2){   // rather than 0 due to overshoot
  motor1.drive(-50); 
  lcd.setCursor (0,1);        // go to the 2nd line
lcdGo();
}
while ((Pos1 - Left) < -2){  // rather than 0 due to overshoot
   motor1.drive(50);
   lcd.setCursor (0,1);        // go to the 2nd line
lcdGo();
}
 motor1.brake();
 SetGoL=0;

 lcd.setCursor (0,1);        // go to the 2nd line
lcdGo(); 
}
if (set == 0 && SetGoR == 1){
int  eeAddress = 2;  // 0 & 1 first integer, 2 & 3 second integer
 EEPROM.get(eeAddress,Right);
lcd.setCursor (0,0);        // 1st line
lcd.print("     ");
delay(100);
lcd.setCursor (0,0);        // 1st line
lcd.print(Right, DEC);
delay(100);
while ((Pos1 - Right) > 2){  // rather than 0 due to overshoot
   motor1.drive(-50);
 lcd.setCursor (0,1);        // go to the 2nd line
lcdGo();  
}
while ((Pos1 - Right) < -2){  // rather than 0 due to overshoot
   motor1.drive(50);
   lcd.setCursor (0,1);        // go to the 2nd line
lcdGo();
}
motor1.brake();
SetGoR=0;

lcd.setCursor (0,1);        // go to the 2nd line
lcdGo();
}
}


void runEncoder1()
{
 if (digitalRead(encoder1PinA) && ! digitalRead(encoder1PinB))   
 Pos1++; 
 if (digitalRead(encoder1PinA) &&  digitalRead(encoder1PinB))                               
 Pos1--;
}
void lcdGo(){
lcd.print(Pos1, DEC);
if (Pos1 < 10) lcd.print("             ");
if (Pos1 < 100 && Pos1 > 10) lcd.print("            ");
if (Pos1 < 1000 && Pos1 > 100) lcd.print("           ");
if (Pos1 < 10000 && Pos1 > 1000) lcd.print("          ");
if (Pos1 < 100000 && Pos1 > 10000) lcd.print("         ");
}
void ckbtn(){
int reading = analogRead(buttonPin); 
//lcd.setCursor (8,1);        // go to the 2nd line
//lcd.print(reading, DEC); 
//lcd.print("  ");

 tmpButtonState = LOW;             // the current reading from the input pin
  
   if(reading>BUTTON5LOW && reading<BUTTON5HIGH){
     //Read switch 5
     tmpButtonState = BUTTON5;
   }else if(reading>BUTTON4LOW && reading<BUTTON4HIGH){
     //Read switch 4
     tmpButtonState = BUTTON4;
   }else if(reading>BUTTON3LOW && reading<BUTTON3HIGH){
     //Read switch 3
     tmpButtonState = BUTTON3;
   }else if(reading>BUTTON2LOW && reading<BUTTON2HIGH){
     //Read switch 2
     tmpButtonState = BUTTON2;
   }else if(reading>BUTTON1LOW && reading<BUTTON1HIGH){
     //Read switch 1
     tmpButtonState = BUTTON1;
   }else{
     //No button is pressed;
     tmpButtonState = LOW;
   }

   // check to see if you just pressed the button 
   // (i.e. the input went from LOW to a buttonState),  and you've waited 
   // long enough since the last press to ignore any noise:  

   // If the switch changed, due to noise or pressing:
   if (tmpButtonState != lastButtonState) {
     // reset the debouncing timer
     lastDebounceTime = millis();
   } 
   if ((millis() - lastDebounceTime) > debounceDelay) {
     // whatever the reading is at, it's been there for longer
     // than the debounce delay, so take it as the actual current state:
     buttonState = tmpButtonState;
     //Serial.println(buttonState);
 //    lcd.setCursor(13,1);
 //    lcd.print(buttonState);
     }
   // save the reading.  Next time through the loop,
   // it'll be the lastButtonState:
   lastButtonState = tmpButtonState;
   
   // set the LED using the state of the button for testing:
   switch(buttonState){
     case BUTTON1:
     digitalWrite(ledPin, buttonState>0);
     digitalWrite(ledPin, LOW);
     break;
     case BUTTON2:
     digitalWrite(ledPin, buttonState>0);
     digitalWrite(ledPin, LOW);
     break;
     case BUTTON3:
     digitalWrite(ledPin, buttonState>0);
     digitalWrite(ledPin, LOW);
     break;
     case BUTTON4:
     digitalWrite(ledPin, buttonState>0);
     digitalWrite(ledPin, LOW);
     break;
     case BUTTON5:
     digitalWrite(ledPin, buttonState>0);
     digitalWrite(ledPin, LOW);
     break;
   }
}
