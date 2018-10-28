#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266HTTPClient.h>
#include <vector>
#include <set>

const char* apSsid     = "ap-ssid";
const char* apPassword = "ap-password";
const char* clientSsid     = "CalVisitor";
const char* clientPassword = "";

HTTPClient http;
WiFiClient client;

WiFiEventHandler probeRequestPrintHandler;

String macToString(const unsigned char* mac) {
  char buf[20];
  snprintf(buf, sizeof(buf), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}
std::set<String> mySet;
std::vector<WiFiEventSoftAPModeProbeRequestReceived> myList;

void onProbeRequestPrint(const WiFiEventSoftAPModeProbeRequestReceived& evt) {
//  myList.push_back(evt);
  mySet.insert(macToString(evt.mac));
}

void setup() {
//  Serial.begin(115200);
//  Serial.println("Hello!");

  // Don't save WiFi configuration in flash - optional
  WiFi.persistent(false);

  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSsid, apPassword);
  WiFi.setAutoReconnect(true);
  WiFi.begin(clientSsid, clientPassword);
  while (WiFi.status() != WL_CONNECTED) {
//    Serial.print(".");
    delay(100);
  }
//  Serial.println("");
  probeRequestPrintHandler = WiFi.onSoftAPModeProbeRequestReceived(&onProbeRequestPrint);
}

void loop() {
  delay(10000);
  String json = "mac:";
//  for(WiFiEventSoftAPModeProbeRequestReceived w : myList){
//    JsonObject& probe = probes.createNestedObject();
//    probe["address"] = macToString(w.mac);
//    probe["rssi"] = w.rssi;
//  }
  for (String s : mySet) {
    json = json + " " + s;
  }

  myList.clear();
  mySet.clear();
//  Serial.println("json:" + json);
  if (!client.connect("agrasp.wifizone.org", 80)) {
    return;
  }  
  client.print(json);
  client.stop();

}
