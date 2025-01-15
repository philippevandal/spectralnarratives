#include <AccelStepper.h>
#include <WiFi.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <WiFiUdp.h>

// WiFi credentials
const char* ssid = "spectral";
const char* password = "narratives";

IPAddress staticIP(10, 0, 1, 12); // ESP32 static IP
IPAddress gateway(10, 0, 1, 1);    // IP Address of your network gateway (router)
IPAddress subnet(255, 255, 255, 0);   // Subnet mask

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

#define IN1_PIN2 14
#define IN2_PIN2 27
#define IN3_PIN2 26
#define IN4_PIN2 25
#define LED_PIN2 33

#define STEPS_PER_REV 4096
#define LEDC_CHANNEL_UV 0
#define LEDC_CHANNEL_W 1
#define LEDC_TIMER_13_BIT 10
#define LEDC_BASE_FREQ 30000

const int MAX_ANGLE = 45;

AccelStepper stepperUV(AccelStepper::FULL4WIRE, IN1_PIN1, IN3_PIN1, IN2_PIN1, IN4_PIN1);
AccelStepper stepperW(AccelStepper::FULL4WIRE, IN1_PIN2, IN3_PIN2, IN2_PIN2, IN4_PIN2);

// Function to handle incoming OSC messages for stepper motors and LEDs
void ledUV (OSCMessage &msg) {
  int brightness = msg.getFloat(0) * 1024;
  ledcWrite(LEDC_CHANNEL_UV, brightness);
  Serial.print("UV: ");
  Serial.println(brightness);
}

void ledW (OSCMessage &msg) {
  int brightness = msg.getFloat(0) * 1024;
  ledcWrite(LEDC_CHANNEL_W, brightness);
  Serial.print("W: ");
  Serial.println(brightness);
}

void stepper_UV (OSCMessage &msg) {
  int angle = (msg.getFloat(0) * 2 * MAX_ANGLE) - MAX_ANGLE;
  int steps = map(angle, -MAX_ANGLE, MAX_ANGLE, -STEPS_PER_REV / 12, STEPS_PER_REV / 12);
  Serial.print("stepperUV: ");
  Serial.println(angle);
  stepperUV.moveTo(steps);
  while (stepperUV.distanceToGo() != 0) {
    stepperUV.run();
  }
  // stepperUV.run();
}

void stepper_W (OSCMessage &msg) {
  int angle = (msg.getFloat(0) * 2 * MAX_ANGLE) - MAX_ANGLE;
  int steps = map(angle, -MAX_ANGLE, MAX_ANGLE, -STEPS_PER_REV / 12, STEPS_PER_REV / 12);
  Serial.print("stepperW: ");
  Serial.println(angle);
  stepperW.moveTo(steps);
  while (stepperW.distanceToGo() != 0) {
  stepperW.run();
  }
}

  // stepperUV.moveTo(stepsUV);
  // stepperW.moveTo(stepsW);
  // while (stepperUV.distanceToGo() != 0 || stepperW.distanceToGo() != 0) {
  //   stepperUV.run();
  //   stepperW.run();
  // }

  // stepperUV.moveTo(0);
  // stepperW.moveTo(0);
  // while (stepperUV.distanceToGo() != 0 || stepperW.distanceToGo() != 0) {
  //   stepperUV.run();
  //   stepperW.run();
  // }
  // stepperUV.run();
  // stepperW.run();
// }

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

  // Motor setup
  stepperUV.setMaxSpeed(1000.0);
  stepperUV.setAcceleration(500.0);
  stepperW.setMaxSpeed(1000.0);
  stepperW.setAcceleration(500.0);

  // LED setup
  ledcSetup(LEDC_CHANNEL_UV, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_W, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcAttachPin(LED_PIN1, LEDC_CHANNEL_UV);
  ledcAttachPin(LED_PIN2, LEDC_CHANNEL_W);
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
  // OSC receive
  WiFiCheck();
  OSCBundle msgIN;
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    while (packetSize--) {
      msgIN.fill(Udp.read());
    }
    if (!msgIN.hasError()) {
      msgIN.dispatch("/stepperUV", stepper_UV);
      msgIN.dispatch("/stepperW", stepper_W);
      msgIN.dispatch("/ledUV", ledUV);
      msgIN.dispatch("/ledW", ledW);
      msgIN.empty();
    }
  }
 
}
