#include <Keypad.h>
#include <Servo.h>
int pressurePin = A0;
int LEDpin1 = 2;
int force1;
int pos=0;
Servo myservo;

int green = 2;
int yellow = 3;
int servoPin = 12;
int red = 13;

char password [4]= {'1', '2', '3', '4'}; //initializing password
char reservedPassword [4] = {'5', '6', '7', '8'}; //this will be generated somewhere else
char keypadPW[4];   //password applied to keypad 
int position = 0;   //keypad position
int lockerOpen = 0; //1 when the locker is open, 0 when the locker is locked
int reserved = 0;   //0 when the locker is free to be reserved, 1 when the locker is reserved and needs verified, 2 when locker is reserved and verified

const byte ROWS = 4; //rows and columns for keypad
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {11, 10, 9, 8}; //{A0, A1, A2, A3};

byte colPins[COLS] = {7, 6, 5, 4};

Keypad myKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);
int i = 0;
int start1 = 0; //used to set up when device turns on
int start2 = 0; //used to set up when locker is first reserved

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  myservo.attach(servoPin);
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void loop() {
  char customKey = myKeypad.getKey(); //defining the key that was pressed
  
  if(start1 == 0){
    //start1Method();
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
    Serial.println("Press 'B' (0) to reseve the locker");
    openLocker();
    start1++;
  }
  if(start2 == 0 && reserved == 2){
     //start2Method();
     Serial.println("Here are your options:");
     Serial.println("1. press '*' to reset your guess");
     Serial.println("2. press '#' to set new password (locker must be unlocked)");
     Serial.println("3. press 'A' (0) to lock the locker");
     Serial.println("4. press any numbers to type in passcode"); 
     Serial.println("");
     start2++;
  }
    
  if(customKey) {
    if(reserved == 2){
          keypadPW[i] = customKey; //inputting the key pressed into an array
          Serial.println(customKey);
          i++; //incrementing array
          position++; //move to next password input
      
          //Reset the guess
          if(customKey == '*'){
              digitalWrite(red, HIGH);
              digitalWrite(green, LOW);
              position = 0;
              i = 0;
              Serial.println("guess is reset");
          }
      
          else if(customKey == '#' && lockerOpen == 0){
              position--; //set equal to zero if you want to reset the password with this
              i--;    //set equal to zero if you want to reset the password with this
              Serial.println("Unlock first to reset password");
          }
            
          //Set the password
          else if(customKey == '#' && lockerOpen == 1){
              position = 0;
              i = 0;
              Serial.println("type in new password");
        
              int j = 0;
              while(j<4){
                char customKey = myKeypad.getKey();
                if(customKey) {
                  password [j] = customKey; //inputting the key pressed into an array
                  Serial.println(customKey);
                  j++;
                  if(customKey == '#' || customKey == '*' || customKey == 'A' || customKey == 'B'){
                    Serial.println("This character can not be in password. Continue password.");
                    j--;
                  }
                }
                delay(100);
              }
              Serial.println("password set");
          } 
          
          else if(customKey == '0'){  //   should be A
              lockerOpen = 0;
              Serial.println("Locker is locked");
              position--;
              i--;
              closeLocker();
          }

          else if(lockerOpen == 1){
              i--;
              position--;
              Serial.println("The locker is already open");
          }
    } 

    else if(reserved == 0 && customKey == '0'){// should be B
          position = 0;
          i = 0;
          reserved = 1;
          Serial.println("Locker is now reserved");
          digitalWrite(yellow, HIGH);
          Serial.println("Please type in the number you received to claim your locker");
    }
  }
    while(reserved == 1){
          char myKey = myKeypad.getKey();
          if(myKey){
            keypadPW[i] = myKey; 
            Serial.println(myKey);
            i++; 
            position++; 
          }
          if(position == 4) {
             
            if ((strncmp(reservedPassword, keypadPW, 4) == 0)) {
              reserved = 2;
              lockerOpen = 1;
              Serial.println("Locker is now able to be used");
              position = 0;
              i = 0;
            }          
             
            else {
              Serial.println("Incorrect passcode. Locker still needs to be varified");
              position = 0;
              lockerOpen = 0;
              i = 0;
            }
          }
  }

  //check the password
  if(position == 4) {
      if ((strncmp(password, keypadPW, 4) == 0)) {
          Serial.println("Locker is open");
          position = 0;
          i = 0;
          openLocker();
          lockerOpen = 1;  
      }
      else {
          Serial.println("Locker is locked");
          position = 0;
          i = 0;
          closeLocker();
          lockerOpen = 0;
      }
  }  
  
  if(lockerOpen == 1){
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
      openLocker();
  }
  
  else{
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
  }
}
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void myMethod(int i, int position, char customKey, int lockerOpen){

void closeLocker(){
    pos = 170;
    myservo.write(pos);
}

void openLocker(){
  pos = 40;
  myservo.write(pos);
}
/*
void start2Method(){
        Serial.println("Here are your options:");
        Serial.println("1. press '*' to reset your guess");
        Serial.println("2. press '#' to set new password (locker must be unlocked)");
        Serial.println("3. press 'A' to lock the locker");
        Serial.println("4. press any numbers to type in passcode"); 
        Serial.println("");
        return;
}
*/


