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
Ticker tick_temp;
Ticker tick_soil_moisture_1;
Ticker tick_soil_moisture_2;
Ticker tick_water;
Ticker tick_light;
Ticker tick_heap;
const size_t capacity = 2*JSON_OBJECT_SIZE(4) + 2*JSON_OBJECT_SIZE(2) + 2*JSON_ARRAY_SIZE(1);

void callback(char* topic, byte* payload, unsigned int length) {
  // Switch on the LED if an 1 was received as first character
  noInterrupts();
  char results[10];
  if ((char)payload[0] == '1') {
    digitalWrite(16, LOW);  
  } else {
    digitalWrite(16, HIGH);  // Turn the LED off by making the voltage HIGH
  }
  if ((char)payload[1] == '1') {
    digitalWrite(13, HIGH);  
  } else {
    digitalWrite(13, LOW);  // Turn the LIGhT off by making the voltage LOW
  }
  if ((char)payload[2] == '1') {
    digitalWrite(14, HIGH);  
  } else {
    digitalWrite(14, LOW);  // Turn the PUMP1 off by making the voltage LOW
  }
  if ((char)payload[3] == '1') {
    digitalWrite(12, HIGH);  
  } else {
    digitalWrite(12, LOW);  // Turn the PUMP2ing off by making the voltage LOW
  }
  strncpy(results, (char*)payload, 8);
  results[8] = '\0'; 
  publish_data("", "setpoints", "", results);
  interrupts();
}

void setup() {
    pinMode(16, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(14, OUTPUT);
    digitalWrite(12, LOW);
    digitalWrite(13, LOW);
    digitalWrite(14, LOW);
    digitalWrite(15, LOW);
    digitalWrite(16, HIGH);
    Serial.begin(115200);
    //WiFiManager
    WiFiManager wifiManager;
    //reset saved settings
//    wifiManager.resetSettings();
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
    wifiManager.autoConnect("AutoConnectAP");
    //if you get here you have connected to the WiFi
    pubsubclient.setServer("agrasp.wifizone.org", 80);
    pubsubclient.connect("testing");
    pubsubclient.setCallback(callback);
    pubsubclient.subscribe("testing"); 
    Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
    pinMode(A0, INPUT);
    start_tickers();
}

void reconnect() {
  // Loop until we're reconnected
  stop_tickers();
  ESP.wdtDisable();
  while (!pubsubclient.connected()) {
    Serial.println("not connected");
    // Attempt to connect
    if (pubsubclient.connect("testing")) {
      // Once connected, publish an announcement...
      publish_data("wifi", "", "reconnected", "");
      pubsubclient.subscribe("testing");
      // ... and resubscribe
    } else {
      delay(5000);
    }
  }
  start_tickers();
  ESP.wdtEnable(1000);
}

void publish_data(char* data_name, char* setpoint_name, char* datapt, char* setpt) {
  ESP.wdtDisable();
  StaticJsonBuffer<capacity> jsonBuffer;
  JsonObject& obj = jsonBuffer.createObject();
  obj["device_id"] = "9e1fc51deb46bff3";
  obj["device_type"] = "plant";
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
  pubsubclient.publish("test", JSBuffer);
  pubsubclient.loop();
  ESP.wdtEnable(1000);
  ESP.wdtFeed();
}

void soil_moisture_1() {
  int soil1;
  int soil2;
  Wire.requestFrom(9, 4); /* request & read data of size 13 from slave */
  while(Wire.available()){
    soil1 = Wire.read() << 0;
    soil1 |= Wire.read() << 8;    
    soil2 = Wire.read() << 0;
    soil2 |= Wire.read() << 8;
  }
  char soil[10];
//  itoa(soil1, soil, 10);
  float soil_float = soil1 * 1 / 10.24;
  dtostrf(soil_float, 7, 4, soil);
  char pump[5];
  itoa(1 & digitalRead(D6), pump, 10);
  publish_data("soil_1", "pump_1", soil, pump);
}

void soil_moisture_2() {
  int soil1;
  int soil2;
  Wire.requestFrom(9, 4); /* request & read data of size 13 from slave */
  while(Wire.available()){
    soil1 = Wire.read() << 0;
    soil1 |= Wire.read() << 8;    
    soil2 = Wire.read() << 0;
    soil2 |= Wire.read() << 8;
  }
  char soil[10];
//  itoa(soil2, soil, 10);
  float soil_float = soil2 * 1 / 10.24;
  dtostrf(soil_float, 7, 4, soil);
  char pump[5];
  itoa(1 & digitalRead(D7), pump, 10);
  publish_data("soil_2", "pump_2", soil, pump);
}

void light() {
  char light[5];
  itoa(1 & digitalRead(D5), light, 10);
  publish_data("", "light", "", light); 
}

void stop_tickers() {
  tick_temp.detach();
  tick_water.detach();
  tick_soil_moisture_1.detach();
  tick_soil_moisture_2.detach();
  tick_light.detach();
  tick_heap.detach();
}

void start_tickers() {
  tick_temp.attach(5, temp);
  tick_water.attach(5, water_level);
  tick_soil_moisture_1.attach(5, soil_moisture_1);
  tick_soil_moisture_2.attach(5, soil_moisture_2);
  tick_light.attach(10, light);
  tick_heap.attach(10, heap_space);
}


void water_level() {
  long num;
  Wire.requestFrom(8, 4); /* request & read data of size 13 from slave */
  while(Wire.available()){
    num = Wire.read() << 0;
    num |= Wire.read() << 8;
    num |= Wire.read() << 16;
    num |= Wire.read() << 24;
  }
  float capacitance = 1.44 * 1000000000 / (180000*3.0*num);
  float y = (capacitance - 0.0797) * (100.0) / (0.1125 - 0.0797);
  char cap[10 + 2];
  dtostrf(y, 7, 3, cap);
  publish_data("water", "", cap, "");
}

void temp() {
  int temp_val = analogRead(A0);
  float temperature = ((temp_val * (3300.0/1024)) - 500) / 10;
  char temp[10 + 2];
  dtostrf(temperature * 0.6, 9, 5, temp);
  publish_data("temp", "", temp, "");
}

void heap_space() {
  int heap_space = ESP.getFreeHeap();
  char heap_char[14];
  itoa(heap_space, heap_char, 10);
  publish_data("heap", "", heap_char, "");
}

void loop() {
  ESP.wdtFeed();
  if (!pubsubclient.connected()) {
    reconnect();
  }
  pubsubclient.loop();
}
