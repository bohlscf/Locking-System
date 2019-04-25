
#include <Servo.h>
int pressurePin = A0;
int LEDpin1 = 2;
int force1;
int pos=0;
Servo myservo;
void setup() {
Serial.begin(9600);
pinMode(LEDpin1,OUTPUT);

}
void loop() {
 force1 = analogRead(pressurePin);
 Serial.print("force = ");
 Serial.println(force1);
 delay(10);
 if (force1>300)
 digitalWrite(LEDpin1,HIGH),
 pos=180; pos<=90; pos-=1;
 myservo.write(pos);
 delay(10);
 if (force1<300)
 digitalWrite(LEDpin1,LOW),
 delay(10);
 
}

