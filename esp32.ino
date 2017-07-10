// WIFI Client Lib
#include <ssl_client.h>
#include <WiFiClientSecure.h>
// MQTT Lib
#include <PubSubClient.h>
// OLED Display
#include <OLEDDisplay.h>
#include <OLEDDisplayFonts.h>
#include <OLEDDisplayUi.h>
#include <SSD1306.h>
#include <SSD1306Wire.h>
// Rotary Encoder Lib
#include <ky-040.h>

//-------- Customise these values -----------
const char* ssid = "uberssid";
const char* password = "securewlanpass";
const char* mqttServer = "mqtt.server.local";
const int   mqttPort = 8883;
const char* mqttUser = "mqttuserName";
const char* mqttPassword = "mqttpassw0rd";
const char* mqttDeviceName = "DeviceName";

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5, 4);
// Init WLAN
WiFiClientSecure wifiClientSecure;
// Init MQTT
PubSubClient client(mqttServer, mqttPort, wifiClientSecure);
// Init Encoder
#define ENCODER_CLK1         2      // This pin must have a minimum 0.47 uF capacitor
                                    // The current library only allows pins 2 or 3
#define ENCODER_DT1         12      // data pin
#define ENCODER_SW1         11      // switch pin (active LOW)
#define MAX_ROTARIES1        1      // this example define two rotaries for this encoder
#define ROTARY_ID1           0      // Ids can range from 0 to 254, 255 is reserved
ky040 encoder1(ENCODER_CLK1, ENCODER_DT1, ENCODER_SW1, MAX_ROTARIES1 );


void setup() {
  // put your setup code here, to run once:
  // Init Display
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  // Init WIFI
  initWiFi();
  // Init MQTT
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
     if (client.connect(mqttDeviceName, mqttUser, mqttPassword )) {
     } else {
       delay(1000);
     }
  }
  client.subscribe("test");
  // Init Rotary
  encoder1.AddRotaryCounter(ROTARY_ID1, 10, -100, 100, 1, true );
  encoder1.SetRotary(ROTARY_ID1);
  encoder1.SetChanged(ROTARY_ID1); // This way we can force an update the first time through
}

void loop() {
  // put your main code here, to run repeatedly:
  // MQTT Loop
  client.loop();
}

void initWiFi() {
  if (strcmp (WiFi.SSID().c_str(), ssid) != 0) {
   WiFi.begin(ssid, password);
 }
 while (WiFi.status() != WL_CONNECTED) {
   delay(500);
 } 
}

void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}
