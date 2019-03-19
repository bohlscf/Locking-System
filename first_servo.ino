#include <Servo.h>

Servo myservo; //make servo name (uses arduino servo library)
int pos;

void setup() {

  Serial.begin(9600);
  
  myservo.attach(9); 
  //declare which pin is the control wire to the servo
}

void loop() {
  //first, swivel between middle position, fully CCW, fully CW using Pulse width modulation
  //for (int j=0; j<=3; j++){
  if(pos > 60){
    myservo.write(40);
    pos = 40;
    Serial.println("open lock");
    delay(3000);
}
  else{
    myservo.write(170);
    pos = 170;
    Serial.println("close lock");
    delay(3000);
  }

//Get feedback from servo and print the current position
int x = myservo.read();
Serial.println("angle: ");
Serial.println(x);
delay(1000);

} 
