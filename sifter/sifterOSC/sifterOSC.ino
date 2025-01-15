#include <AccelStepper.h>
#include <WiFi.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "spectral"; //ACAB
const char* password = "narratives"; //vivavivaPal3stinA!

IPAddress staticIP(10, 0, 1, 11); // ESP32 static IP
IPAddress gateway(10, 0, 1, 1);    // IP Address of your network gateway (router)
IPAddress subnet(255, 255, 255, 0);   // Subnet mask

// OSC settings
WiFiUDP Udp;  // UDP instance
const IPAddress remoteIP(192, 168, 1, 100);  // Replace with the IP of your OSC client
const unsigned int remotePort = 8000;  // OSC send port
const unsigned int localPort = 9000;   // OSC receive port

const int dirPin0 = 32;
const int stepPin0 = 33;

int motorsPins[] = {26, 25};
const int freq = 30000;
const int resolution = 10;
int dutyCycle = 500;
int step = 10;
const int ledPin = 13;
const int ledSOILPin = 27; //TBD
const int channelLed = 10;
const int channelLedSOIL = 3; 

#define motorInterfaceType 1
AccelStepper stepper1(motorInterfaceType, stepPin0, dirPin0);

unsigned long previousMillis = 0;
const long interval = 5000;

void stirrer(OSCMessage &msg) {
  if (msg.isFloat(0)) {
    int speed = msg.getFloat(0) * 1024.0;
    // Serial.print("stirrer:");
    // Serial.print(speed);
    // Serial.println();
    ledcWrite(0, speed);
    ledcWrite(1, 0); 
  }
}

void led(OSCMessage &msg) {
  if (msg.isFloat(0)) {
    int brightness = msg.getFloat(0) * 1024.0;
    ledcWrite(channelLed, brightness);
    // Serial.print("led:");
    // Serial.print(brightness);
    // Serial.println();
  }
}

void ledSOIL(OSCMessage &msg) {
  if (msg.isFloat(0)) {
    int brightness = msg.getFloat(0) * 1024.0;
    ledcWrite(channelLedSOIL, brightness);
    // Serial.print("led:");
    // Serial.print(brightness);
    // Serial.println();
  }
}

void stepper(OSCMessage &msg) {
  int steps;
  if (msg.isFloat(0)) {
    steps = (msg.getFloat(0) * 10.0) - 5.0;
    // Serial.print("steps:");
    // Serial.print(steps);
    // Serial.println();
  }
  stepper1.moveTo(steps);
  while (stepper1.distanceToGo() != 0) {
    stepper1.run();
  }
}

void setup() {
  // WiFi Setup
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if(!WiFi.config(staticIP, gateway, subnet)) {
    Serial.println("Failed to configure Static IP");
  } else {
    Serial.println("Static IP configured!");
  }
  Serial.println("\nWiFi connected");
  Serial.println(WiFi.localIP());
  Udp.begin(localPort);

  ledcSetup(channelLed, freq, resolution);
  ledcAttachPin(ledPin, channelLed);
  ledcSetup(channelLedSOIL, freq, resolution);
  ledcAttachPin(ledSOILPin, channelLedSOIL);
  // ledcWrite(channelLed, 0);
  delay(500);

  for (int i = 0; i < 2; i++) {
    ledcSetup(i, freq, resolution);
    ledcAttachPin(motorsPins[i], i);
    delay(500);
    // ledcWrite(i, 0);
  }

  stepper1.setMaxSpeed(200.0);
  stepper1.setAcceleration(100.0);
}

void WiFiCheck(){
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(ssid, password);
    Serial.print("\nConnecting to ");
    Serial.print(ssid);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("Connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    delay(100);
  }  
}

void loop() {
  WiFiCheck();
  
  OSCBundle msgIN;
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    while (packetSize--) {
      msgIN.fill(Udp.read());
    }
    if (!msgIN.hasError()) {
      msgIN.dispatch("/stirrer", stirrer);
      msgIN.dispatch("/led", led);
      msgIN.dispatch("/stepper", stepper);
      msgIN.dispatch("/ledSOIL", ledSOIL);
      msgIN.empty();
    }
  }
}
