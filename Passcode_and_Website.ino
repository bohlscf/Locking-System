//http://arduino.esp8266.com/stable/package_esp8266com_index.jsoc
// Load Wi-Fi library
//use board: Generic ESP 8266
//#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid     = "Missing_home";
const char* password = "lalala777";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output5State = "off";
String output4State = "off";  

// Assign output variables to GPIO pins
const int output5 = 5;
const int output4 = 4;
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void setup() {
  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output5, OUTPUT);
  pinMode(output4, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output5, LOW);
  digitalWrite(output4, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  Serial.begin(9600);
  pinMode(green, OUTPUT);
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  myservo.attach(servoPin);
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /5/on") >= 0) {
              Serial.println("GPIO 5 on");
              output5State = "on";
              digitalWrite(output5, HIGH);
            } else if (header.indexOf("GET /5/off") >= 0) {
              Serial.println("GPIO 5 off");
              output5State = "off";
              digitalWrite(output5, LOW);
            } else if (header.indexOf("GET /4/on") >= 0) {
              Serial.println("GPIO 4 on");
              output4State = "on";
              digitalWrite(output4, HIGH);
            } else if (header.indexOf("GET /4/off") >= 0) {
              Serial.println("GPIO 4 off");
              output4State = "off";
              digitalWrite(output4, LOW);
            }
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #77878A;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>ESP8266 Web Server</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 5  
            client.println("<p>GPIO 5 - State " + output5State + "</p>");
            // If the output5State is off, it displays the ON button       
            if (output5State=="off") {
              client.println("<p><a href=\"/5/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/5/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            // Display current state, and ON/OFF buttons for GPIO 4  
            client.println("<p>GPIO 4 - State " + output4State + "</p>");
            // If the output4State is off, it displays the ON button       
            if (output4State=="off") {
              client.println("<p><a href=\"/4/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/4/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
  //---------------------------------------------------------------------------------------------------------------------------------------
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
          randNumber = random(9999);
          Serial.print("Number to verify reserved locker: ");
          Serial.println(randNumber);
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
}*/
