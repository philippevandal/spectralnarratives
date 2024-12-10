#include <WiFi.h>
#include <OSCMessage.h>

// Define WiFi credentials
const char* ssid = "Belleville";
const char* password = "ETpourquoipas545!";

// Define OSC properties
const int localPort = 8000;   // Port for OSC communication

// Define PWM properties
const int pwmFreq = 5000;     // PWM frequency in Hz
const int pwmResolution = 8;  // PWM resolution in bits (0-255)

// GPIO pin definitions
const int brightLED = 23;     // GPIO23 for brightLED
const int forwardPump = 13;   // GPIO13 for forwardPump
const int backwardPump = 27;  // GPIO27 for backwardPump
const int CWstirrer = 26;     // GPIO26 for clockwise stirrer
const int CCWstirrer = 25;    // GPIO25 for counter-clockwise stirrer
const int UVLeds = 33;        // GPIO33 for UV LEDs
const int WhiteLeds = 32;     // GPIO32 for White LEDs

// Assign unique PWM channels to each pin
const int channelBrightLED = 0;
const int channelForwardPump = 1;
const int channelBackwardPump = 2;
const int channelCWStirrer = 3;
const int channelCCWStirrer = 4;
const int channelUVLeds = 5;
const int channelWhiteLeds = 6;

// Function to check and maintain WiFi connection
void WiFiCheck() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Attempting to reconnect...");
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.print(".");
    }
    Serial.println();
    Serial.println("Reconnected to WiFi.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  }
}

void setup() {
  // Initialize Serial for debugging
  Serial.begin(115200);

  // Start WiFi connection
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  
  // Wait until connected to WiFi
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure PWM channels and attach them to GPIO pins
  ledcSetup(channelBrightLED, pwmFreq, pwmResolution);
  ledcAttachPin(brightLED, channelBrightLED);

  ledcSetup(channelForwardPump, pwmFreq, pwmResolution);
  ledcAttachPin(forwardPump, channelForwardPump);

  ledcSetup(channelBackwardPump, pwmFreq, pwmResolution);
  ledcAttachPin(backwardPump, channelBackwardPump);

  ledcSetup(channelCWStirrer, pwmFreq, pwmResolution);
  ledcAttachPin(CWstirrer, channelCWStirrer);

  ledcSetup(channelCCWStirrer, pwmFreq, pwmResolution);
  ledcAttachPin(CCWstirrer, channelCCWStirrer);

  ledcSetup(channelUVLeds, pwmFreq, pwmResolution);
  ledcAttachPin(UVLeds, channelUVLeds);

  ledcSetup(channelWhiteLeds, pwmFreq, pwmResolution);
  ledcAttachPin(WhiteLeds, channelWhiteLeds);
}

void loop() {
  // Check WiFi connection
  WiFiCheck();

  // Create an OSCMessage object
  OSCMessage msg;

  // Check for incoming OSC messages
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    // Read the incoming packet into the message
    int n = Udp.read(msg.buffer(), 255);
    msg.fill(n);

    // Process the OSC message
    if (!msg.empty()) {
      // Check the OSC address
      if (msg.match("/brightLED")) {
        float value = msg.getFloat(0); // Get the first argument
        int pwmValue = map(value, 0, 1, 0, 255);
        ledcWrite(channelBrightLED, pwmValue);
        Serial.print("OSC brightLED PWM: "); Serial.println(pwmValue);
      }
      else if (msg.match("/forwardPump")) {
        float value = msg.getFloat(0);
        int pwmValue = map(value, 0, 1, 0, 255);
        ledcWrite(channelForwardPump, pwmValue);
        Serial.print("OSC forwardPump PWM: "); Serial.println(pwmValue);
      }
      else if (msg.match("/backwardPump")) {
        float value = msg.getFloat(0);
        int pwmValue = map(value, 0, 1, 0, 255);
        ledcWrite(channelBackwardPump, pwmValue);
        Serial.print("OSC backwardPump PWM: "); Serial.println(pwmValue);
      }
      else if (msg.match("/CWstirrer")) {
        float value = msg.getFloat(0);
        int pwmValue = map(value, 0, 1, 0, 255);
        ledcWrite(channelCWStirrer, pwmValue);
        Serial.print("OSC CWStirrer PWM: "); Serial.println(pwmValue);
      }
      else if (msg.match("/CCWstirrer")) {
        float value = msg.getFloat(0);
        int pwmValue = map(value, 0, 1, 0, 255);
        ledcWrite(channelCCWStirrer, pwmValue);
        Serial.print("OSC CCWStirrer PWM: "); Serial.println(pwmValue);
      }
      else if (msg.match("/UVLeds")) {
        float value = msg.getFloat(0);
        int pwmValue = map(value, 0, 1, 0, 255);
        ledcWrite(channelUVLeds, pwmValue);
        Serial.print("OSC UVLeds PWM: "); Serial.println(pwmValue);
      }
      else if (msg.match("/WhiteLeds")) {
        float value = msg.getFloat(0);
        int pwmValue = map(value, 0, 1, 0, 255);
        ledcWrite(channelWhiteLeds, pwmValue);
        Serial.print("OSC WhiteLeds PWM: "); Serial.println(pwmValue);
      }
    }
  }

  delay(10);  // Small delay to prevent spamming loop
}
