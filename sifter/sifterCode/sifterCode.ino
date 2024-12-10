#include <AccelStepper.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>

const char* ssid = "BIOLAB";
const char* password = "filtration2022";

const int localPort = 8888;
WiFiUDP Udp;
OSCMessage msg;
char packetBuffer[255];

const int dirPin0 = 32;
const int stepPin0 = 33;

int motorsPins[] = {26, 25};
int previousDir = 1;
int previousSpeed = 550;
const int freq = 30000;
const int resolution = 10;
int dutyCycle = 500;
int step = 10;
const int ledPin = 27;

#define motorInterfaceType 1
AccelStepper stepper1(motorInterfaceType, stepPin0, dirPin0);

unsigned long previousMillis = 0;
const long interval = 5000;

void setup() {
  Serial.begin(115200);
  // WiFi.begin(ssid, password);
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.println("Connecting to WiFi..");
  // }
  // Udp.begin(localPort);
  for (int i = 0; i < 2; i++) {
    ledcSetup(i, freq, resolution);
    ledcAttachPin(motorsPins[i], i);
    delay(500);
    ledcWrite(i, 0);
  }
  ledcSetup(3, freq, resolution);
  ledcAttachPin(ledPin, 3);
  // pinMode(ledPin, OUTPUT);
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

void rampUp(int dir, int speed){
  for (int i = previousSpeed; i < speed; i += step){
    if (dir == 1){
      ledcWrite(0, i);
      ledcWrite(1, 0);      
    } else {
      ledcWrite(0, 0);
      ledcWrite(1, i);      
    }
    delay(300);
  }
}

void rampDown(int dir, int speed){
  for (int i = previousSpeed; i > speed; i -= step){
    if (dir == 1){
      ledcWrite(0, i);
      ledcWrite(1, 0);     
    } else {
      ledcWrite(0, 0);
      ledcWrite(1, i);       
    }
    delay(300);
  }
}

void stirrer(int dir, int speed) {
  if (dir == previousDir){
    if (previousSpeed < speed){
      rampUp(dir, speed);
    } else {
      rampDown( dir, speed);
    }
  } else {
    rampDown(previousDir, 550);
    previousSpeed = 550;
    rampUp(dir, speed);
  }
  previousSpeed = speed;
  previousDir = dir;
}

void sifter() {
  ledcWrite(3, 4096);
  // digitalWrite(ledPin, HIGH);
  for (int i = 0; i < 10; i++) {
    stepper1.moveTo(5);
    while (stepper1.distanceToGo() != 0) {
      stepper1.run();
    }
    stepper1.moveTo(-5);
    while (stepper1.distanceToGo() != 0) {
      stepper1.run();
    }
  }
  ledcWrite(3, 0);
  // digitalWrite(ledPin, LOW);
}

void fadeLed(int brightness) {
  // for (int brightness = 0; brightness <= 255; brightness++) {
  ledcWrite(3, brightness);
    // delay(10);
  // }
}

void dispatchOscMessage(OSCMessage &msg) {
  if (msg.fullMatch("/0/sifter")) {
    // int value;
    // msg.getFloat(0, value);
    sifter();
  } else if (msg.fullMatch("/0/led")) {
    int value;
    value = msg.getInt(0);
    fadeLed(value);
  } else if (msg.fullMatch("/0/stirrer")) {
    int direction;
    int speed;
    direction = msg.getInt(0);
    speed = msg.getInt(1);
    stirrer(direction, speed);
  }
}

void loop() {
  // WiFiCheck();
  // int packetSize = Udp.parsePacket();
  // if (packetSize) {
  //   while (packetSize--) {
  //     if (Udp.read(packetBuffer, 255) > 0) {
  //       OSCMessage oscMessage(packetBuffer);
  //       dispatchOscMessage(oscMessage);
  //       oscMessage.empty(); 
  //     }
  //   }
  // }
  sifter();
  delay(1000);
  fadeLed(4096);
  delay(6000);
  fadeLed(0);
  stirrer(1, 900);
  delay(20000);
  stirrer(1, 0);
  delay(3000);
}
