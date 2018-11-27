#include <Wire.h>
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <PubSubClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

WiFiClient client;
PubSubClient pubsubclient(client);
const size_t capacity = JSON_OBJECT_SIZE(4) + 2*JSON_ARRAY_SIZE(1) + 2*JSON_OBJECT_SIZE(2);

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
    pubsubclient.setServer("agrasp.wifizone.org", 80);
    pubsubclient.connect("testing");
    pubsubclient.setCallback(callback);
    pubsubclient.subscribe("testing"); 
    Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
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
   if (!pubsubclient.connected()) {
    reconnect();
  }
  StaticJsonBuffer<capacity> jsonBuffer;
  JsonObject& obj = jsonBuffer.createObject();
  obj["device_id"] = "9e1fc51deb46bff3";
  obj["device_type"] = "smart_garden";
  JsonArray& data = jsonBuffer.createArray();
  JsonObject& data_pt = jsonBuffer.createObject();
  data_pt["d_label"] = "capacitance";
  data_pt["d_value"] = 1.44 / (180000*3.0*num);
  JsonArray& setpoints = jsonBuffer.createArray();
  JsonObject& set_pt = jsonBuffer.createObject();
  set_pt["d_label"] = "led";
  set_pt["d_value"] = 1 ^ digitalRead(16);
  data.add(data_pt);
  setpoints.add(set_pt);
  obj["data"] = data;
  obj["setpoint"] = setpoints;
  char JSBuffer[256];
  obj.printTo(JSBuffer);
  Serial.println(JSBuffer);
  
  pubsubclient.publish("test", JSBuffer);
  pubsubclient.loop();
 delay(10000);
}


//void loop() {

////  while(client.available()) {
////    String line = client.readStringUntil('\r');
////    Serial.println(line);
////  }
////  client.close();
//}
