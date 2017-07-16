// WIFI Client Lib
#include <WiFiClientSecure.h>
// MQTT Lib
#include <PubSubClient.h>
// OLED Display
#include <SSD1306.h>
// include IR Lib
#include <esp32_rmt.h>


// Include Images
#include "images.h"
//#include "bat.h"

//-------- Customise these values -----------
const char* ssid = "ubserssid";
const char* password = "passw0rd";
const char* mqttServer = "172.16.1.10";
const int   mqttPort = 8883;
const char* mqttUser = "iot";
const char* mqttPassword = "passw0rd";
byte mac[6];
boolean bRFlag, bLFlag;
boolean bRTrig, bLTrig;

// Initialize the OLED display using Wire library
SSD1306  display(0x3c, 5, 4);
// Init WLAN
WiFiClientSecure wifiClientSecure;
// Init MQTT
PubSubClient client(mqttServer, mqttPort, wifiClientSecure);
// Init Encoder
byte interruptPin1 = 25;
byte interruptPin2 = 26;
byte interruptPin3 = 36;
volatile byte state = LOW;

// Init IR
ESP32_RMT rem1, rem2;
uint8_t cnt;
uint16_t cmd, addr;

void setup() {
  // put your setup code here, to run once:
  // Init Serial
  Serial.begin(115200);
  delay(100);
  Serial.println("Serial init...");
  // Init Display
  Serial.println("Display init...");
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Hallo Jan");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 16, "es klappt");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 32, "Juhuuuu");
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 48, "Zeile 4");

  display.display();
  // Init WIFI
  Serial.println("Wifi init...");
  initWiFi();
  // Init MQTT
  Serial.println("MQTT init...");
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  Serial.print("Connecting...");
  char data[13];
  sprintf(data, "Panel-%.2x%.2x%.2x", mac[3], mac[4], mac[5]);
  while (!client.connected()) {
    if (client.connect(data, mqttUser, mqttPassword )) {
    } else {
      Serial.print(".");
      delay(500);
    }
  }
  Serial.println("");
  Serial.println("Subscribing...");
  client.subscribe("test");
  client.publish("esp32", data);
  // Init Rotary
  Serial.println("Encoder init...");
  pinMode(interruptPin1, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin1), int_25, CHANGE);
  pinMode(interruptPin2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), int_26, CHANGE);
  pinMode(interruptPin3, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin3), int_2, CHANGE);
  // Init IR
  Serial.println("IR init...");
  rem1.begin(17, 1);
  rem2.begin(16, 1);
  Serial.println("Loop init...");
}

void loop() {
  // put your main code here, to run repeatedly:
  // MQTT Loop
  client.loop();

  // ir transmit loop
  if (Serial.available() > 0)
  {
    switch (Serial.read())
    {
      case '0': Serial.println("power"); rem2.necSend(0x3000, 0xfd02); break;
      case '1': Serial.println("volume up"); rem2.necSend(0x3000, 0x7d82); break;
      case '2': Serial.println("volume down"); rem2.necSend(0x3000, 0x7c83); break;
      default: break;
    }

  }

  // Encoder loop
  if (bRTrig == true)
  {
    Serial.println("Int Rechts: ");
    bRFlag = false;
    bLFlag = false;
    bRTrig = false;
  }
  if (bLTrig == true)
  {
    Serial.println("Int Links: ");
    bRFlag = false;
    bLFlag = false;
    bLTrig = false;
  }
  // End Main Loop
}

void initWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  if (strcmp (WiFi.SSID().c_str(), ssid) != 0) {
    Serial.println("Connecting...");
    WiFi.begin(ssid, password);
    WiFi.macAddress(mac);
  }
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 0.5 second for re-trying
    delay(500);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);
}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_10);
  display.drawString(0, 0, String((char *)payload));
  display.display();
  Serial.println();
  Serial.println("-----------------------");

}

void int_25() {
  bRFlag = true;
  if (bLFlag == true)
  {
    bRTrig = true;
  }
}

void int_26() {
  state = !state;
  Serial.println("pressed!");
}
void int_2() {
  bLFlag = true;
  if (bRFlag == true)
  {
    bLTrig = true;
  }
}