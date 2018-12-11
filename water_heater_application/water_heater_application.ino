/*thermometers sketch
@Author: Aditya Tyagi, TEAM GRASP*/
#include <WiFi.h>

//constants
//defining pin A0 as analog voltage input pin
int temp_sensor = A0;
//defining pin D0 as output pin for water heater relay 
int relay = D0;
//tempterature setpoint to be sourced from server
//default setpoint temperature to 48 degrees celsius (warm shower water temperature)
float set_temperature = 48;

//I assume NC (normally closed) connection on relay. 

void setup() {
  Serial.begin(9600);
  pinMode(temp_sensor, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //@LAM: Establish connection to MQTT broker
  //@LAM: subscribe to relevant MQTT topic

  }


//reads temperature_sensor and returns value
float sense_temp() {
  float input_voltage; 
  float read_temperature;
  
  
  //read ADC value
  input_voltage = analogRead(temp_sensor);
  
  
  //rescale to 0-1 range by dividing by 1024 (10-bit ADC)
  input_voltage = input_voltage/1024;
  read_temperature = input_voltage * 96.1538;

  return read_temperature;
}


void loop() {
 //@LAM: receive current temperature setpoint value from MQTT
 
 
  //read current temperature
  float curr_temp = sense_temp();
  
  //to prevent hysteresis, define an interval of 1 degree for setpoint temperature
  if (curr_temp <= set_temperature - 0.5) {
    Serial.print("The current temp. is: ");
    Serial.println(curr_temp);
    Serial.print("The setpoint temp is ");
    Serial.println(set_temperature);
    Serial.println("-----> The heater is on!");
    Serial.println();
    Serial.println();
    //turn water heater on
    digitalWrite(relay, HIGH);
    //turn LED on to indicate that the heater is on
    digitalWrite(LED_BUILTIN, HIGH); 
    }
  else if (curr_temp >= set_temperature + 0.5) {
    Serial.print("The current temp. is: ");
    Serial.println(curr_temp);
    Serial.print("The setpoint temp is ");
    Serial.println(set_temperature);
    Serial.println("------> The heater is off!");
    Serial.println();
    Serial.println();
    //turn the water heater off
    digitalWrite(relay, LOW);
    //turn LED off to indicate that the heater is off
    digitalWrite(LED_BUILTIN, LOW);
    } 

 //@LAM: publish data to MQTT broker on relevant topic

  delay(400);  
  }
