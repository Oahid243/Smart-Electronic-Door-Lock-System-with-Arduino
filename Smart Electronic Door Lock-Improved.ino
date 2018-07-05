#include <LiquidCrystal.h>
#include<SoftwareSerial.h>
#include <Servo.h>
#include <Keypad.h>

SoftwareSerial BT(10,11);
int a=0;

//Outputs
const int lock = 6;
const int doorLamp = 5;
const int emargencyLamp = 9;
const int buzzer = 12;
const int lcdPower = 3;
const int ledRed = 44;
const int ledGreen = 42;
const int rs = 51, en = 49, d4 = 45, d5 = 43, d6 = 41, d7 = 39; //Display

//Inputs
const int doorBtn = 2;
const int pir = 4;
const int lockUnlockButton = 8;
const byte ROWS = 4; //four keypad rows
const byte COLS = 4; //four keypad columns

//Values
int lockState = 1;
int passLen = 0;
int errorCount = 0;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {36, 34, 32, 30}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {28, 26, 24, 22}; //connect to the column pinouts of the keypad
String pass1 = "A4032", pass2 = "A3671", pass3 = "A3487", pass4 = "A3773", pass5 = "A3516", pass6 = "B1234", pass7 = "B1122", pass8 = "C3666";
String inputPass = "";
char keyPressed;
int doorState;
int indoorLight;
int outdoorLight;
int isMotion;
int temp=0;
int buttonPressed;
int passMatch;

//Objects
Servo servo;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );



void lcdControl()
{
  lcd.setCursor(5,0);
  lcd.print("Welcome");
  lcd.setCursor(0,1);
  lcd.print("Enter Your Password");
  lcd.setCursor(8,2);
  lcd.print("or");
  lcd.setCursor(0,3);
  lcd.print("Press D for Doorbell");
}

void beep(int c)
{
  if(c==1){
    int i;
    for(i=0; i<2; i++){
      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(100);

      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(100);

      digitalWrite(buzzer, HIGH);
      delay(200);
      digitalWrite(buzzer, LOW);
      delay(100);

      digitalWrite(buzzer, HIGH);
      delay(800);
      digitalWrite(buzzer, LOW);
      delay(1000);
    }
  }
  else if(c==2){
    int b;
    for(b=0; b<60; b++){
      digitalWrite(buzzer, HIGH);
      delay(400);
      digitalWrite(buzzer, LOW);
      delay(100);
    }
  }
  else if(c==3){
    digitalWrite(buzzer, HIGH);
    delay(500);
    digitalWrite(buzzer, LOW);
    delay(500);
  }
  else{
    return;
  }
}

int passCheck()
{
  if(inputPass==pass1 || inputPass==pass2 || inputPass==pass3 || 
  inputPass==pass4 || inputPass==pass5 || inputPass==pass6 || 
  inputPass==pass7 || inputPass==pass8){
    return 1;
  }
  else{
    return 0;
  }
}



void setup() {
  pinMode(doorBtn, INPUT);
  pinMode(pir, INPUT);
  pinMode(lockUnlockButton, INPUT);

  pinMode(doorLamp, OUTPUT);
  pinMode(emargencyLamp, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(lcdPower, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);

  servo.attach(lock);
  servo.write(90);
  lockState=1;
  digitalWrite(ledGreen, HIGH);

  lcd.begin(20, 4);
  BT.begin(9600);
  Serial.begin(9600);
  
}

void loop() {

  //Autolock
  if(lockState==0){
    delay(5000);
    doorState = digitalRead(doorBtn);
    if(doorState==1){
      servo.write(90);
      lockState=1;
      digitalWrite(ledGreen, HIGH);
      digitalWrite(ledRed, LOW);
    }
    else{
      beep(3);
    }
  }
  digitalWrite(emargencyLamp, LOW);

  //Bluetooth unlock
  if(BT.available()){
      while(BT.available()){
      delay(10);
      char c = BT.read();
      inputPass+=c;
    }
    
    passMatch=passCheck();
    if(passMatch==1){
      servo.write(0);
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledRed, HIGH);
      lcd.clear();
      lcd.setCursor(4,1);
      lcd.print("Door Unlocked.");
      lockState=0;
      passLen = 0;
      inputPass="";
      errorCount=0;
      digitalWrite(emargencyLamp, HIGH);
      /*indoorLight = analogRead(12);
      if(indoorLight<20){
        digitalWrite(emargencyLamp, HIGH);
      }*/
      delay(5000);
    }
    else{
      inputPass="";
    }
  }
  
  //Button Lock unlock
  buttonPressed = digitalRead(lockUnlockButton);
  if(buttonPressed==1){
    if(lockState==0){
      doorState = digitalRead(doorBtn);
      if(doorState==1){
        servo.write(90);
        lockState=1;
        temp=0;
        digitalWrite(ledGreen, HIGH);
        digitalWrite(ledRed, LOW);
        delay(5000);
      }
      else{
        beep(3);
      }
    }
    else{
      servo.write(0);
      lockState=0;
      digitalWrite(ledGreen, LOW);
      digitalWrite(ledRed, HIGH);
      delay(5000);
    }
  }

  //actOnMotion(passLen);
  isMotion = digitalRead(pir);
  if(isMotion==1){
    digitalWrite(lcdPower, HIGH);
    outdoorLight = analogRead(15);
    if(outdoorLight<30){
      digitalWrite(doorLamp, HIGH);
    }
    if(passLen==0){
      lcdControl();
    }
    //passControl();
    
  }
  else{
    lcd.clear();
    digitalWrite(lcdPower, LOW);
    digitalWrite(doorLamp, LOW);
    return;
  }

  //Password lock unlock
  keyPressed = keypad.getKey();
  if(keyPressed){
    if(keyPressed=='D'){
      beep(1);
    }
    else if(keyPressed=='*'){
      if(passLen>0){
        passMatch = passCheck();
        if(passMatch==1){
          servo.write(0);
          digitalWrite(ledGreen, LOW);
          digitalWrite(ledRed, HIGH);
          lcd.clear();
          lcd.setCursor(4,1);
          lcd.print("Door Unlocked.");
          lockState=0;
          passLen = 0;
          inputPass="";
          errorCount=0;
          digitalWrite(emargencyLamp, HIGH);
          /*indoorLight = analogRead(12);
          if(indoorLight<20){
            digitalWrite(emargencyLamp, HIGH);
          }*/
          delay(5000);
        }
        else{
          lcd.clear();
          lcd.setCursor(1,1);
          lcd.print("Wrong Password!");
          passLen=0;
          inputPass="";
          errorCount++;
          if(errorCount>2){
            beep(2);
            errorCount=0;
          }
          delay(2000);
        }
      }
    }
    else if(keyPressed=='#'){
      inputPass="";
      passLen=0;
      lcd.clear();
      lcd.setCursor(6,1);
    }
    else{
      if(passLen==0){
        lcd.clear();
        lcd.setCursor(6,1);
        lcd.print("*");
        inputPass+=keyPressed;
        passLen++;
      }
      else{
        if(passLen<7){
          lcd.setCursor(6+passLen, 1);
          lcd.print("*");
          inputPass+=keyPressed;
          passLen++;
        }
      }
    }
  }

  //Act on motion 2
  isMotion = digitalRead(pir);
  if(isMotion==1){
    digitalWrite(lcdPower, HIGH);
    outdoorLight = analogRead(15);
    if(outdoorLight<15){
      digitalWrite(doorLamp, HIGH);
    }
    if(passLen==0){
      lcdControl();
    }
    //passControl();
    
  }
  else{
    lcd.clear();
    digitalWrite(lcdPower, LOW);
    digitalWrite(doorLamp, LOW);
    return;
  }

    
}
