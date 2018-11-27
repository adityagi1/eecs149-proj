/*thermometers sketch
@Author: Aditya Tyagi, TEAM GRASP*/

//constants
//defining pin A0 as analog voltage input pin
int analog_inp_pin = A0;


void setup() {
  Serial.begin(9600);
  pinMode(analog_inp_pin, INPUT);
  
  }

void loop() {
  float input_voltage; 
  float temperature;
  
  //read ADC value
  input_voltage = analogRead(analog_inp_pin);
  
  
  //rescale to 0-1 range by dividing by 1024 (10-bit ADC)
  input_voltage = input_voltage/1024;
  temperature = input_voltage * 125;

  //print on serial
  Serial.print("Current Voltage was: ");
  Serial.print(input_voltage);
  Serial.print("  Current Temperature is ");
  Serial.print(temperature);
  Serial.println("C");
  delay(400);  
  }
