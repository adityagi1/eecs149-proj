#include <FreqCount.h>
#include <Wire.h>

volatile int soil1;
volatile int soil2;

void setup() {
 Wire.begin(9);                /* join i2c bus with address 8 */
 Wire.onReceive(receiveEvent); /* register receive event */
 Wire.onRequest(requestEvent); /* register request event */
 Serial.begin(9600);    
 pinMode(A2, INPUT);
 pinMode(A1, INPUT);      
}

void loop() {
  delay(500);
  noInterrupts();
  soil1 = analogRead(A1);
  delay(10);
  soil2 = analogRead(A2);  
  interrupts();
}

// function that executes whenever data is received from master
void receiveEvent(int howMany) {
 while (0 < Wire.available()) {
    char c = Wire.read();      /* receive byte as a character */
    Serial.print(c);         
  }
 Serial.println();           
}

// function that executes whenever data is requested from master
void requestEvent() {
  int s1 = soil1;
  int s2 = soil2;
  Wire.write(s1 & 0xFF);
  Wire.write((s1 >> 8) & 0xFF);
  Wire.write(s2 & 0xFF);
  Wire.write((s2 >> 8) & 0xFF);
}
