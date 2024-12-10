#include <AccelStepper.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>

// Define the pins for the first motor
#define IN1_PIN1 14
#define IN2_PIN1 27
#define IN3_PIN1 26
#define IN4_PIN1 25
#define LED_PIN1 33 // Define the pin for the first LED

// Define the pins for the second motor
#define IN1_PIN2 32
#define IN2_PIN2 35
#define IN3_PIN2 34
#define IN4_PIN2 16
#define LED_PIN2 17 // Define the pin for the second LED

// Define the pins for the third motor
#define IN1_PIN3 13
#define IN2_PIN3 12
#define IN3_PIN3 4
#define IN4_PIN3 2
#define LED_PIN3 15 // Define the pin for the third LED

// WiFi credentials
const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

// OSC
WiFiUDP Udp;
const IPAddress localIp(192, 168, 1, 100); // Change to your ESP32's local IP
const unsigned int localPort = 8000;      // Local port to listen for OSC messages

// Steps per revolution (adjust if necessary)
#define STEPS_PER_REV 2048

// LED PWM properties
#define LEDC_CHANNEL_0 0
#define LEDC_CHANNEL_1 1
#define LEDC_CHANNEL_2 2
#define LEDC_TIMER_13_BIT 13
#define LEDC_BASE_FREQ 5000

// Create three instances of AccelStepper
AccelStepper stepper1(AccelStepper::FULL4WIRE, IN1_PIN1, IN3_PIN1, IN2_PIN1, IN4_PIN1);
AccelStepper stepper2(AccelStepper::FULL4WIRE, IN1_PIN2, IN3_PIN2, IN2_PIN2, IN4_PIN2);
AccelStepper stepper3(AccelStepper::FULL4WIRE, IN1_PIN3, IN3_PIN3, IN2_PIN3, IN4_PIN3);

unsigned long lastWiFiCheck = 0;
const unsigned long wifiCheckInterval = 10000; // Check WiFi every 10 seconds

void setup() {
  // Set the maximum speed and acceleration for each motor
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(500.0);
  stepper2.setMaxSpeed(1000.0);
  stepper2.setAcceleration(500.0);
  stepper3.setMaxSpeed(1000.0);
  stepper3.setAcceleration(500.0);

  // Initialize serial communication for debugging purposes
  Serial.begin(115200);

  // Initialize WiFi
  connectToWiFi();

  // Initialize UDP for OSC
  Udp.begin(localPort);

  // Configure LED PWM functionalities
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);

  // Attach the LED pins to PWM channels
  ledcAttachPin(LED_PIN1, LEDC_CHANNEL_0);
  ledcAttachPin(LED_PIN2, LEDC_CHANNEL_1);
  ledcAttachPin(LED_PIN3, LEDC_CHANNEL_2);
}

void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to WiFi. IP address: ");
  Serial.println(WiFi.localIP());
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi();
  }
}

void controlMotor1(OSCMessage &msg) {
  int angle1 = msg.getInt(0);
  int steps1 = map(angle1, -45, 45, -STEPS_PER_REV / 8, STEPS_PER_REV / 8);
  stepper1.moveTo(steps1);
}

void controlMotor2(OSCMessage &msg) {
  int angle2 = msg.getInt(0);
  int steps2 = map(angle2, -45, 45, -STEPS_PER_REV / 8, STEPS_PER_REV / 8);
  stepper2.moveTo(steps2);
}

void controlMotor3(OSCMessage &msg) {
  int angle3 = msg.getInt(0);
  int steps3 = map(angle3, -45, 45, -STEPS_PER_REV / 8, STEPS_PER_REV / 8);
  stepper3.moveTo(steps3);
}

void controlLED1(OSCMessage &msg) {
  int brightness1 = msg.getInt(0);
  ledcWrite(LEDC_CHANNEL_0, brightness1);
}

void controlLED2(OSCMessage &msg) {
  int brightness2 = msg.getInt(0);
  ledcWrite(LEDC_CHANNEL_1, brightness2);
}

void controlLED3(OSCMessage &msg) {
  int brightness3 = msg.getInt(0);
  ledcWrite(LEDC_CHANNEL_2, brightness3);
}

void loop() {
  OSCMessage msg;
  int size = Udp.parsePacket();
  if (size > 0) {
    while (size--) {
      msg.fill(Udp.read());
    }
    if (!msg.hasError()) {
      msg.dispatch("/motor1", controlMotor1);
      msg.dispatch("/motor2", controlMotor2);
      msg.dispatch("/motor3", controlMotor3);
      msg.dispatch("/led1", controlLED1);
      msg.dispatch("/led2", controlLED2);
      msg.dispatch("/led3", controlLED3);
    }
  }

  // Run all motors to ensure simultaneous movement
  stepper1.run();
  stepper2.run();
  stepper3.run();

  // Check WiFi connection periodically
  unsigned long currentMillis = millis();
  if (currentMillis - lastWiFiCheck >= wifiCheckInterval) {
    lastWiFiCheck = currentMillis;
    checkWiFiConnection();
  }
}
