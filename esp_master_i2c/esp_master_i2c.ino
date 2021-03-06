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
const size_t capacity = JSON_OBJECT_SIZE(4) + 2*JSON_ARRAY_SIZE(2) + 4*JSON_OBJECT_SIZE(2);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
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
}

void setup() {
    Serial.begin(115200);

    //WiFiManager
    //Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;
    //reset saved settings
//    wifiManager.resetSettings();
    
    //set custom ip for portal
    //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //here  "AutoConnectAP"
    //and goes into a blocking loop awaiting configuration
    wifiManager.autoConnect("AutoConnectAP");
    //or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
    pinMode(16, OUTPUT);
    pinMode(15, OUTPUT);
    pinMode(12, OUTPUT);
    pinMode(13, OUTPUT);
    pinMode(14, OUTPUT);
    pubsubclient.setServer("agrasp.wifizone.org", 80);
    pubsubclient.connect("testing");
    pubsubclient.setCallback(callback);
    pubsubclient.subscribe("testing"); 
    Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
    pinMode(A0, INPUT);
}

void reconnect() {
  // Loop until we're reconnected
  while (!pubsubclient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (pubsubclient.connect("testing")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      pubsubclient.publish("test", "hello world");
      pubsubclient.subscribe("testing");
      // ... and resubscribe
    } else {
      Serial.print("failed, rc=");
      Serial.print(pubsubclient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
// Wire.beginTransmission(8); /* begin with device address 8 */
// Wire.write("Hello Arduino");  /* sends hello string */
// Wire.endTransmission();    /* stop transmitting */
 long num;
 int soil1;
 int soil2;
 float temp_voltage = analogRead(A0);
 Serial.println(temp_voltage);
 Wire.requestFrom(8, 4); /* request & read data of size 13 from slave */
 while(Wire.available()){
//    char c = Wire.read();
    num = Wire.read() << 0;
    num |= Wire.read() << 8;
    num |= Wire.read() << 16;
    num |= Wire.read() << 24;
 }
 Serial.print(num);
 Serial.println();
delay(10);
 Wire.requestFrom(9, 4); /* request & read data of size 13 from slave */
 while(Wire.available()){
    soil1 = Wire.read() << 0;
    soil1 |= Wire.read() << 8;    
    soil2 = Wire.read() << 0;
    soil2 |= Wire.read() << 8;
 }
 Serial.println(soil1);
 Serial.print(soil2);
 Serial.println();
   if (!pubsubclient.connected()) {
    reconnect();
  }
  float temperature = ((temp_voltage * (3300.0/1024)) - 500) / 10;
  StaticJsonBuffer<capacity> jsonBuffer;
  JsonObject& obj = jsonBuffer.createObject();
  obj["device_id"] = "9e1fc51deb46bff3";
  obj["device_type"] = "plant";
  JsonArray& data = jsonBuffer.createArray();
  JsonObject& data_pt_cap = jsonBuffer.createObject();
  data_pt_cap["d_label"] = "cap";
  data_pt_cap["d_value"] = 1.44 / (180000*3.0*num);
  JsonObject& data_pt_temp = jsonBuffer.createObject();
  data_pt_temp["d_label"] = "temp";
  data_pt_temp["d_value"] = temperature;
  JsonArray& setpoints = jsonBuffer.createArray();
  JsonObject& set_pt_led = jsonBuffer.createObject();
  set_pt_led["d_label"] = "led";
  set_pt_led["d_value"] = 1 ^ digitalRead(16);
  JsonObject& set_pt_light = jsonBuffer.createObject();
  set_pt_light["d_label"] = "light";
  set_pt_light["d_value"] = digitalRead(15);
  data.add(data_pt_cap);
  data.add(data_pt_temp);
  setpoints.add(set_pt_led);
  setpoints.add(set_pt_light);
  obj["data"] = data;
  obj["setpoint"] = setpoints;
  char JSBuffer[256];
  obj.printTo(JSBuffer);
  Serial.println(JSBuffer);
  
  pubsubclient.publish("test", JSBuffer);
  pubsubclient.loop();
 delay(4000);
}


//void loop() {

////  while(client.available()) {
////    String line = client.readStringUntil('\r');
////    Serial.println(line);
////  }
////  client.close();
//}
