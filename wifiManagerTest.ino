#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

WiFiClient client;
const size_t capacity = JSON_OBJECT_SIZE(3);


void setup() {
    // put your setup code here, to run once:
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
}

void loop() {

  DynamicJsonBuffer jsonBuffer(capacity);
  JsonObject& obj = jsonBuffer.createObject();
  obj["value"] = 42;
  obj["lat"] = 48.748010;
  obj["lon"] = 2.293491;
  delay(2000);
    // put your main code here, to run repeatedly:
  if (!client.connect("agrasp.wifizone.org", 80)) {
    return;
  }
  String JSBuffer;
  obj.printTo(JSBuffer);
  client.println(JSBuffer);
  unsigned long time = millis();
  while(!client.available() && (millis() - time) < 1000){
  }
  while(client.available()) {
    String line = client.readStringUntil('\r');
    Serial.println(line);
  }
//  client.print("yes");
  client.stop();
}
