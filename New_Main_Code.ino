#include <Keypad.h> //arduino library from Mark Stanley and Alexander Brevig
#include <Servo.h>
#include <LiquidCrystal.h>

//pins being used:
int pos=0;
Servo myservo;

int green = 2;
int yellow = 3;
int servoPin = 12;
int red = 13;
int buzzerPin = 23;
int pressurePin = A15;
int reservePin = A3;

char password [4]= {'1', '2', '3', '4'}; //initializing password
char reservedPassword [4] = {'5', '6', '7', '8'}; //this will be generated somewhere else
char keypadPW[4];   //password applied to keypad 
int position = 0;   //keypad position
int lockerOpen = 0; //1 when the locker is open, 0 when the locker is locked
int reserved = 0;   //0 when the locker is free to be reserved, 1 when the locker is reserved and needs verified, 2 when locker is reserved and verified
long randNumber;

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
int reserveSignal = 0;
int force;

const int rs = 22, en = 24, d4 = 26, d5 = 28, d6 = 30, d7 = 32;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
String opt1;


//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  Serial.begin(9600);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  myservo.attach(servoPin);
  randomSeed(analogRead(0));

  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.clear();
  delay(1000);
}

//-------------------------------------     MAIN LOOP    ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void loop() {
  char customKey = myKeypad.getKey(); //defining the key that was pressed
  
  if(start1 == 0){
    openLocker();
    start1Method();    
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

     lcd.clear();
     lcd.print("* reset guess");
     lcd.setCursor(0,1);
     lcd.print("# new password");
     delay(2000);
     lcd.clear();
     lcd.setCursor(0,0);
     lcd.print("0 to lock");
  }

  reserveSignal = analogRead(reservePin);
  //Serial.println(reserveSignal);
  force = analogRead(pressurePin);
  if(reserved == 0 && force > 100){
    tone(buzzerPin, 440);
  }
  else{
    noTone(buzzerPin);
  }

  if(reserved == 0 && reserveSignal > 600){
      position = 0;
      i = 0;
      reserved = 1;
      Serial.println("Locker is now reserved");
      Serial.print("Number to verify reserved locker: ");
      randNumber = random(9999);
      Serial.println(randNumber);
      Serial.println("Please type in the number you received to claim your locker");
      
      digitalWrite(yellow, HIGH);
      delay(10);
      
      lcd.clear();
      lcd.print("Locker is now");
      lcd.setCursor(0,1);
      lcd.print("reserved");
      delay(1500);

      lcd.clear();
      lcd.print("Your number is");
      lcd.setCursor(0,1);
      lcd.print(randNumber);

      for(int j = 3; j >= 0; j--){
        reservedPassword[j] = char(48 + randNumber % 10);
        password[j] = reservedPassword[j];
        position = 0;
        //Serial.println(reservedPassword[j]);
        //Serial.println(randNumber % 10);
        randNumber = randNumber / 10;
      }
          
      //String opt4= String(reservedPassword);
      //Serial.println(opt4);
      
      //lcddisp(opt4);delay(1000);
      //lcddisp("Please type in the number you received to claim your locker");
  }
  else if (reserved == 2 && reserveSignal < 100){
    i = 0;
    position = 0;
    reserved = 0;
    lockerOpen = 0;
    openLocker();
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
    digitalWrite(yellow, LOW);
    Serial.println("Locker is no longer reserved");
    lcd.clear();
    lcd.print("Locker is no");
    lcd.setCursor(0,1);
    lcd.print("longer reserved");
    delay(1000);
    start1 = 0;
  }
  
//-----------------------------    KEYPAD INPUTS    -----------------------------------------------------------------------------------------    
  if(customKey) {
    Serial.println(customKey);
    if(reserved == 2){
          keypadPW[i] = customKey;//inputting the key pressed into an array
          
          //Serial.println(customKey);
          
          String opt2= String(customKey);
          lcd.clear();
          lcd.print(opt2);
          
          i++; //incrementing array
          position++; //move to next password input
      
          //Reset the guess
          if(customKey == '*'){
              digitalWrite(red, HIGH);
              digitalWrite(green, LOW);
              position = 0;
              i = 0;
              Serial.println("guess is reset");
              lcd.clear();
              lcd.print("guess is reset");
          }
      
          else if(customKey == '#' && lockerOpen == 0){
              position--; //set equal to zero if you want to reset the password with this
              i--;    //set equal to zero if you want to reset the password with this
              Serial.println("Unlock first to reset password");
              lcd.clear();
              lcd.print("Unlock to reset");
              lcd.setCursor(0,1);
              lcd.print("password");
          }
            
          //Set the password
          else if(customKey == '#' && lockerOpen == 1){
              position = 0;
              i = 0;
              Serial.println("type in new password");
              lcd.clear();
              lcd.print("type in new");
              lcd.setCursor(0,1);
              lcd.print("password");
        
              int j = 0;
              while(j<4){
                char customKey = myKeypad.getKey();
                if(customKey) {
                  password [j] = customKey; //inputting the key pressed into an array
                  Serial.println(customKey);
                  String opt3= String(customKey);
                  lcd.clear();
                  lcd.print(opt3);
                  j++;
                  if(customKey == '#' || customKey == '*' || customKey == 'A' || customKey == 'B'){
                    Serial.println("This character can not be in password. Continue password.");
                    lcd.clear();
                    lcd.print("Character cannot");
                    lcd.setCursor(0,1);
                    lcd.print("be in password");
                    j--;
                  }
                }
                delay(100);
              }
              Serial.println("password set");
              lcd.clear();
              lcd.print("password set");
          } 
          
          else if(customKey == '0' || customKey == 'A'){  //   should be A
              lockerOpen = 0;
              Serial.println("Locker is locked");
              lcd.clear();
              lcd.print("Locker is locked");
              position = 0;
              i = 0;
              closeLocker();
          }
//This will be replaced by logging out on the app
          else if(customKey == 'C' && lockerOpen == 1){
            i = 0;
            position = 0;
            reserved = 0;
            lockerOpen = 0;
            closeLocker();
            digitalWrite(yellow, LOW);
            Serial.println("Locker is no longer reserved");
            lcd.clear();
            lcd.print("Locker is no");
            lcd.setCursor(0,1);
            lcd.print("longer reserved");
          }

          else if(lockerOpen == 1){
              i--;
              position--;
              Serial.println("The locker is already open");
              lcd.clear();
              lcd.print("Locker already");
              lcd.setCursor(0,1);
              lcd.print("open");
          }
    } 

//----------------------------------   LOCKER UNUSABLE   ---------------------------------------------------------
//method when using keypad, not needed when using website/app
    else if(reserved == 0){
      if( customKey == '0' || customKey == 'B' ){
          position = 0;
          i = 0;
          reserved = 1;
          Serial.println("Locker is now reserved");
          digitalWrite(yellow, HIGH);
          randNumber = random(9999);
          Serial.print("Number to verify reserved locker: ");
          Serial.println(randNumber);
          Serial.println("Please type in the number you received to claim your locker");
          
          lcd.clear();
          lcd.print("Locker is");
          lcd.setCursor(0,1);
          lcd.print("reserved");
      }
    }
  }
  
    while(reserved == 1){
          char myKey = myKeypad.getKey();
          reserveSignal = analogRead(reservePin);

          
          if(myKey){
            keypadPW[i] = myKey; 
            Serial.println(myKey);
            lcd.clear();
            lcd.print(myKey);
            i++; 
            position++; 
          }
          if(position == 4) {
             
            if ((strncmp(reservedPassword, keypadPW, 4) == 0)) {
              reserved = 2;
              lockerOpen = 1;
              openLocker();
              Serial.println("Locker is now able to be used");
              position = 0;
              i = 0;
            }          
             
            else {
              Serial.println("Incorrect passcode. Locker still needs to be varified");
              lcd.clear();
              lcd.print("Wrong passcode");
              position = 0;
              lockerOpen = 0;
              i = 0;
            }
          }
          if(reserveSignal < 100){
              i = 0;
              position = 0;
              reserved = 0;
              lockerOpen = 0;
              openLocker();
              digitalWrite(green, LOW);
              digitalWrite(red, HIGH);
              digitalWrite(yellow, LOW);
              Serial.println("Locker is no longer reserved");
              lcd.clear();
              lcd.print("Locker is no");
              lcd.setCursor(0,1);
              lcd.print("longer reserved");
          }
    }
//-------------------------------------    CHECK PASSWORD   -----------------------------------------------------------------
  //check the password
  if(position == 4) {
      if ((strncmp(password, keypadPW, 4) == 0)) {
          position = 0;
          i = 0;
          openLocker();
          lockerOpen = 1;
      }
      else {
          position = 0;
          i = 0;
          closeLocker();
          lockerOpen = 0;
      }
  } 
}
//---------------------------       OTHER METHODS         ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//void myMethod(int i, int position, char customKey, int lockerOpen){

void closeLocker(){
    pos = 170;
    myservo.write(pos);
    digitalWrite(green, LOW);
    digitalWrite(red, HIGH);
    Serial.println("Locker is locked");
    lcd.clear();
    lcd.print("Locker is locked");
}

void openLocker(){
  pos = 40;
  myservo.write(pos);
  digitalWrite(green, HIGH);
  digitalWrite(red, LOW);
  Serial.println("Locker is open");
  lcd.clear();
  lcd.print("Locker is open");
          
}

void start1Method(){
  digitalWrite(green, LOW);
  digitalWrite(red, HIGH);
  Serial.println();
  Serial.println("Reserve the locker from website");
  Serial.println("Press 'B' (0) to reseve the locker");

  lcd.clear();
  lcd.print("Reserve locker");
  lcd.setCursor(0,1);
  lcd.print("from website"); 
}

void testMethod(){
  
}


void lcddisp(String opt){
  int size1= opt1.length()-15;
  lcd.print(opt);

  for (int positionCounter = 0; positionCounter < size1; positionCounter++) {
    // scroll one position left:
    lcd.scrollDisplayLeft();
    // wait a bit:
    delay(300);
  }
  delay(1000);
  lcd.clear();
}
