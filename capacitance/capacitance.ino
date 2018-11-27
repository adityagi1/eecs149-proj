#include <FreqCount.h>
#include <Wire.h>

unsigned volatile long count;

void setup() {
 Wire.begin(8);                /* join i2c bus with address 8 */
 Wire.onReceive(receiveEvent); /* register receive event */
 Wire.onRequest(requestEvent); /* register request event */
 Serial.begin(9600);          
 FreqCount.begin(1000);
}

void loop() {
  if (FreqCount.available()) {
    count = FreqCount.read();
    Serial.println(count);
  }
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
    Wire.write(count & 0xFF);
    Wire.write((count >> 8) & 0xFF);
    Wire.write((count >> 16) & 0xFF);
    Wire.write((count >> 24) & 0xFF);  
}
