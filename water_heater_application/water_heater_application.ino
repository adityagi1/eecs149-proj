/*thermometers sketch
@Author: Aditya Tyagi, TEAM GRASP*/
#include <WiFi.h>

//constants
//defining pin A0 as analog voltage input pin
int temp_sensor = A0;
//defining pin D0 as output pin for water heater relay 
int relay = D0;
//tempterature setpoint to be sourced from server
//default setpoint temperature to 25 degrees celsius (room temperature)
int set_temperature = 35;

//I assume NO (normally open) connection on relay. 

void setup() {
  Serial.begin(9600);
  pinMode(temp_sensor, INPUT);
  pinMode(relay, OUTPUT);
  //@LAM: Establish connection to MQTT broker
  //@LAM: subscribe to relevant MQTT topic

  }


//reads temperature_sensor and returns value
float read_temperature() {
  float input_voltage; 
  float read_temperature;
  
  
  //read ADC value
  input_voltage = analogRead(temp_sensor);
  
  
  //rescale to 0-1 range by dividing by 1024 (10-bit ADC)
  input_voltage = input_voltage/1024;
  read_temperature = input_voltage * 125;

  return read_temperature;
}


void loop() {
 //@LAM: receive current temperature setpoint value from MQTT
 
  
  //read current temperature
  float curr_temp = read_temperature();
  
  //to prevent hysteresis, define an interval of 1 degree for setpoint temperature
  if (read_temperature <= set_temperature - 0.5) {
    //turn water heater on
    digitalWrite(relay, HIGH);
    }
  else if (read_temperature >= set_temperature + 0.5) {
    //turn the water heater off
    digitalWrite(relay, LOW);
    } 

 //@LAM: publish data to MQTT broker on relevant topic
   
  delay(400);  
  }
