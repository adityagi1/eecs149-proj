/*thermometers sketch
@Author: Aditya Tyagi, TEAM GRASP*/
#include <Wire.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <Ticker.h>

WiFiClient client;
PubSubClient pubsubclient(client);
Ticker tick;
const size_t capacity = 2*JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(2) + 2*JSON_ARRAY_SIZE(1);

//constants
//defining pin A0 as analog voltage input pin
int temp_sensor = A0;
//defining pin D0 as output pin for water heater relay 
int relay = D0;
//tempterature setpoint to be sourced from server
//default setpoint temperature to 48 degrees celsius (warm shower water temperature)
volatile int set_temperature = 48;

//I assume NC (normally closed) connection on relay. 

void callback(char* topic, byte* payload, unsigned int length) {
  // Switch on the LED if an 1 was received as first character
  noInterrupts();
  char results[10];
  strncpy(results, "000000000", 10);
  int null_spot = min(8, int(length));
  for (int i = 0; i < null_spot + 1; i++) {
    results[i] = (char)payload[i];
  }
  results[null_spot + 1] = '\0'; 
  set_temperature = atoi(results);
  publish_data("", "setpoint", "", results);
  interrupts();
}

void setup() {
    WiFiManager wifiManager;
    wifiManager.autoConnect("AutoConnectAP");
    pubsubclient.setServer("agrasp.wifizone.org", 80);
    pubsubclient.connect("water_heater");
    pubsubclient.setCallback(callback);
    pubsubclient.subscribe("testing2"); 
    start_tickers();
    
  Serial.begin(115200);
  pinMode(temp_sensor, INPUT);
  pinMode(relay, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  //@LAM: Establish connection to MQTT broker
  //@LAM: subscribe to relevant MQTT topic

}


void publish_data(char* data_name, char* setpoint_name, char* datapt, char* setpt) {
  ESP.wdtDisable();
  StaticJsonBuffer<capacity> jsonBuffer;
  JsonObject& obj = jsonBuffer.createObject();
  obj["device_id"] = "ae72f969d94e10b3";
  obj["device_type"] = "water_heater";
  JsonArray& data = jsonBuffer.createArray();
  JsonArray& setpoints = jsonBuffer.createArray();
  
  JsonObject& data_pt = jsonBuffer.createObject();
  data_pt.set("d_label", data_name);
  data_pt.set("d_value", datapt);
  data.add(data_pt);

  JsonObject& set_pt = jsonBuffer.createObject();
  set_pt.set("d_label", setpoint_name);
  set_pt.set("d_value", setpt);
  setpoints.add(set_pt);

  obj.set("data", data);
  obj.set("setpoint", setpoints);
  
  char JSBuffer[256];
  obj.printTo(JSBuffer);
  pubsubclient.publish("test2", JSBuffer);
  pubsubclient.loop();
  ESP.wdtEnable(1000);
  ESP.wdtFeed();
}


void stop_tickers() {
  tick.detach();
}

void start_tickers() {
  tick.attach(0.4, update_heater);
}

void reconnect() {
  // Loop until we're reconnected
  stop_tickers();
  ESP.wdtDisable();
  while (!pubsubclient.connected()) {
    Serial.println("not connected");
    // Attempt to connect
    if (pubsubclient.connect("water_heater")) {
      // Once connected, publish an announcement...
      publish_data("wifi", "", "reconnected", "");
      pubsubclient.subscribe("testing2");
      // ... and resubscribe
    } else {
      delay(5000);
    }
  }
  start_tickers();
  ESP.wdtEnable(1000);
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
  ESP.wdtFeed();
  if (!pubsubclient.connected()) {
    reconnect();
  }
  pubsubclient.loop();
}

void update_heater() {
  float curr_temp = sense_temp();
  char temp_buf[10 + 2];
  dtostrf(curr_temp, 5, 2, temp_buf);
  char set_buf[10];
  itoa(set_temperature, set_buf, 10);
  publish_data("temp", "setpoint", temp_buf, set_buf);
  if (curr_temp <= set_temperature - 0.5) {
    digitalWrite(relay, LOW);
    digitalWrite(LED_BUILTIN, HIGH); 
  }
  else if (curr_temp >= set_temperature + 0.5) {
    digitalWrite(relay, HIGH);
    digitalWrite(LED_BUILTIN, LOW);
  } 
}
