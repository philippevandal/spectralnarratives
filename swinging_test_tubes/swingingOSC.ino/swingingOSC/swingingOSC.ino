#include <AccelStepper.h>
#include <WiFi.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// OSC settings
WiFiUDP Udp;  // UDP instance
const IPAddress remoteIP(192, 168, 1, 100);  // Replace with the IP of your OSC client
const unsigned int remotePort = 8000;  // OSC send port
const unsigned int localPort = 9000;   // OSC receive port

#define IN1_PIN1 23
#define IN2_PIN1 22
#define IN3_PIN1 21
#define IN4_PIN1 19
#define LED_PIN1 13

#define IN1_PIN3 14
#define IN2_PIN3 27
#define IN3_PIN3 26
#define IN4_PIN3 25
#define LED_PIN3 32

#define STEPS_PER_REV 2048
#define LEDC_CHANNEL_0 0
#define LEDC_CHANNEL_2 2
#define LEDC_TIMER_13_BIT 13
#define LEDC_BASE_FREQ 5000

const int MAX_ANGLE = 80;
const long MEDIAN_POSITION_DELAY = 5000;

AccelStepper stepper1(AccelStepper::FULL4WIRE, IN1_PIN1, IN3_PIN1, IN2_PIN1, IN4_PIN1);
AccelStepper stepper3(AccelStepper::FULL4WIRE, IN1_PIN3, IN3_PIN3, IN2_PIN3, IN4_PIN3);

unsigned long previousMillis = 0;
const long interval = 2000;

int randomCount = 0;

// Function to handle incoming OSC messages for stepper motors and LEDs
void handleStepControl(OSCMessage &msg) {
  if (msg.isInt(0)) {
    int step1 = msg.getInt(0);  // Read first integer argument for stepper1
    stepper1.moveTo(step1);
    Serial.print("Received Stepper 1 Position: ");
    Serial.println(step1);
  }
  if (msg.isInt(1)) {
    int step3 = msg.getInt(1);  // Read second integer argument for stepper3
    stepper3.moveTo(step3);
    Serial.print("Received Stepper 3 Position: ");
    Serial.println(step3);
  }
  
  // Handle LED brightness from OSC
  if (msg.isInt(2)) {
    int brightness1 = msg.getInt(2);  // Read third integer argument for LED 1 brightness
    ledcWrite(LEDC_CHANNEL_0, brightness1);
    Serial.print("Received LED 1 Brightness: ");
    Serial.println(brightness1);
  }
  if (msg.isInt(3)) {
    int brightness3 = msg.getInt(3);  // Read fourth integer argument for LED 3 brightness
    ledcWrite(LEDC_CHANNEL_2, brightness3);
    Serial.print("Received LED 3 Brightness: ");
    Serial.println(brightness3);
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
  Serial.println("\nWiFi connected");
  Udp.begin(localPort);

  // Motor setup
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(500.0);
  stepper3.setMaxSpeed(1000.0);
  stepper3.setAcceleration(500.0);

  // LED setup
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN1, LEDC_CHANNEL_0);
  ledcAttachPin(LED_PIN3, LEDC_CHANNEL_2);
}

void loop() {
  // OSC receive
  OSCMessage msgIN;
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    while (packetSize--) {
      msgIN.fill(Udp.read());
    }
    if (!msgIN.hasError()) {
      msgIN.dispatch("/control", handleStepControl);
    }
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    randomCount++;

    if (randomCount >= 10) {
      stepper1.moveTo(0);
      stepper3.moveTo(0);
      while (stepper1.distanceToGo() != 0 || stepper3.distanceToGo() != 0) {
        stepper1.run();
        stepper3.run();
      }
      delay(MEDIAN_POSITION_DELAY);
      randomCount = 0;
    } else {
      int angle1 = random(-MAX_ANGLE, MAX_ANGLE + 1);
      int angle3 = random(-MAX_ANGLE, MAX_ANGLE + 1);
      int steps1 = (long(angle1) * STEPS_PER_REV) / 360;
      int steps3 = map(angle3, -MAX_ANGLE, MAX_ANGLE, -STEPS_PER_REV / 8, STEPS_PER_REV / 8);
      
      stepper1.moveTo(steps1);
      stepper3.moveTo(steps3);
      
      // Send OSC message with stepper position and brightness values
      OSCMessage msgOUT("/status");
      msgOUT.add(angle1);
      msgOUT.add(angle3);
      Udp.beginPacket(remoteIP, remotePort);
      msgOUT.send(Udp);
      Udp.endPacket();
      msgOUT.empty();
    }
  }
  
  stepper1.run();
  stepper3.run();
}
