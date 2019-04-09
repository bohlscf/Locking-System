#include <Keypad.h>

int green = 2;
int yellow = 3;
int red = 13;

char password [4]= {'1', '2', '3', '4'}; //initializing password
char keypadPW[4];   //password applied to keypad
int position = 0;   //keypad position
int lockerOpen = 1; //1 when the locker is open, 0 when the locker is locked
int reserved = 0;   //1 when the locker is reserved, 0 when the locker is free to be reserved

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
int start = 0;

//------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
}

//-----------------------------------------------------------------------------------------------------

void loop() {
  char customKey = myKeypad.getKey(); //defining the key that was pressed

  if(start == 0){
    digitalWrite(green, HIGH);
    start++;
    Serial.println("If the locker is reserved, type in the number you received"); //this might be good to put in an if statement for when the locker is reserved
    Serial.println("Here are your options:");
    Serial.println("1. press '*' to reset your guess");
    Serial.println("2. press '#' to set new password (locker must be unlocked)");
    Serial.println("3. press 'A' to lock the locker");
    Serial.println("4. press 'B' to reseve the locker");
    Serial.println("5. press any numbers to type in passcode"); 
    Serial.println("");
  }
  
  if(customKey) {
   
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

    if(customKey == '#' && lockerOpen == 0){
        position--; //set equal to zero if you want to reset the password with this
        i--;    //set equal to zero if you want to reset the password with this
        Serial.println("Unlock first to reset password");
    }
      
    //Set the password
    if(customKey == '#' && lockerOpen == 1){  //add in: || lockerOpen == 2
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
        //Serial.println("test" + password[0]);// + password[0] + "a" +password[1] +"a"+password[2]+"a"+password[3]);
    } 
    
    if(customKey == 'A'){
        lockerOpen = 0;
        Serial.println("Locker is locked");
        position--;
        i--;
    }

    if(customKey == 'B'){
        position--;
        i--;
        if(reserved == 0){ 
          reserved = 1;
          Serial.println("Locker is now reserved");
          digitalWrite(yellow, HIGH);
        }
        else{
          Serial.println("Locker is already reserved or in use");
        }
        
    }    
  }

  //check the password
  if(position == 4) {
    
    //password is correct
    if ((strncmp(password, keypadPW, 4) == 0)) {
      lockerOpen = 1;
      Serial.println("Locker is open");
      position = 0;
      i = 0;
        //add motor in here
    }
    
    //incorrect password
    else {
      lockerOpen = 0;
      Serial.println("Locker is locked");
      position = 0;
      i = 0;
      //delay(1000);
    }
    
  }

  if(lockerOpen == 1){
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
  }
  
  else{
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
  }
}
//void myMethod(int i, int position, char customKey, int lockerOpen){



