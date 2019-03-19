#include <Keypad.h>

int green = 2;
int red = 13;

char password [4]= {'1', '2', '3', '4'}; //initializing password
char keypadPW[4]; //password applied to keypad
int position = 0; //keypad position
int locked = true;

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
int f = 5000;
//------------------------------------------------------------------------------------------------------
void setup() {
  Serial.begin(9600);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
}
//-----------------------------------------------------------------------------------------------------
void loop() {

  char customKey = myKeypad.getKey(); //defining the key that was pressed
 
  if(customKey) {
    keypadPW[i] = customKey; //inputting the key pressed into an array
    Serial.println(customKey);
    i++; //incrementing array
   position++; //move to next password input
  }
  
  if(position == 4) {
    Serial.println("end of password");
    
    //the statements below are called when the user input matches the password (motor can be used here)
    if ((strncmp(password, keypadPW, 4) == 0)) {
      i = 0;
      digitalWrite(green, HIGH);
      digitalWrite(red, LOW);
      position = 0;
    }
    
    else {
      i = 0;
      digitalWrite(green, LOW);
      digitalWrite(red, HIGH);
      position = 0;
      delay(1000);
    }
    
  }
  
}
