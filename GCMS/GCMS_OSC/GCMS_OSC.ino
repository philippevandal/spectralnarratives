#include <WiFi.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "esp_task_wdt.h"

// Define WiFi credentials
const char* ssid = "spectral";
const char* password = "narratives";

IPAddress staticIP(10, 0, 1, 14); // ESP32 static IP
IPAddress gateway(10, 0, 1, 1);    // IP Address of your network gateway (router)
IPAddress subnet(255, 255, 255, 0);   // Subnet mask

// Define OSC properties
WiFiUDP Udp;  // UDP instance
const IPAddress remoteIP(192, 168, 1, 100);  // Replace with the IP of your OSC client
const unsigned int remotePort = 8000;  // OSC send port
const unsigned int localPort = 9000;   // OSC receive port

// Define PWM properties
const int pwmFreq = 30000;     // PWM frequency in Hz
const int pwmResolution = 10;  // PWM resolution in bits (0-1024)

// GPIO pin definitions
const int brightLED = 32;     // GPIO23 for brightLED
const int forwardPump = 13;   // GPIO13 for forwardPump
const int backwardPump = 27;  // GPIO27 for backwardPump
const int CWstirrer = 26;     // GPIO26 for clockwise stirrer
const int CCWstirrer = 25;    // GPIO25 for counter-clockwise stirrer
const int UVLeds = 33;        // GPIO33 for UV LEDs
const int WhiteLeds = 23;     // GPIO32 for White LEDs

// Assign unique PWM channels to each pin
const int channelBrightLED = 0;
const int channelForwardPump = 1;
const int channelBackwardPump = 2;
const int channelCWStirrer = 3;
const int channelCCWStirrer = 4;
const int channelUVLeds = 5;
const int channelWhiteLeds = 6;

#define WDT_TIMEOUT 20

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

  if(!WiFi.config(staticIP, gateway, subnet)) {
    Serial.println("Failed to configure Static IP");
  } else {
    Serial.println("Static IP configured!");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Udp.begin(localPort);

  ArduinoOTA.setHostname("module_3");
  ArduinoOTA.begin();
  Serial.println("OTA ready");

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

  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
}

void bright_LED (OSCMessage &msg) {
  float value = msg.getFloat(0); // Get the first argument
  int pwmValue =value * 1024;
  ledcWrite(channelBrightLED, pwmValue);
  Serial.print("OSC brightLED: "); Serial.println(pwmValue);
}

void pump (OSCMessage &msg) {
  float value = msg.getFloat(0); // Get the first argument
  int pwmValue = (value * (1024-(-1024)))-1024;
  if (pwmValue > 0) {
    ledcWrite(channelForwardPump, abs(pwmValue));
    ledcWrite(channelBackwardPump, 0);
  } else if (pwmValue < 0) {
    ledcWrite(channelForwardPump, 0);
    ledcWrite(channelBackwardPump, abs(pwmValue));
  } else if (pwmValue == 0) {
     ledcWrite(channelForwardPump, 0);
    ledcWrite(channelBackwardPump, 0);   
  }
  Serial.print("OSC pump PWM: "); Serial.println(value);
}

void stirrers (OSCMessage &msg) {
  float value = msg.getFloat(0); // Get the first argument
  int pwmValue = (value * (650-(-650)))-650;
  if (pwmValue > 0) {
    ledcWrite(channelCWStirrer, 0);
    ledcWrite(channelCCWStirrer, abs(pwmValue));
  } else if (pwmValue < 0) {
    ledcWrite(channelCWStirrer, abs(pwmValue));
    ledcWrite(channelCCWStirrer, 0);
  } else if (pwmValue == 0) {
    ledcWrite(channelCWStirrer, 0);
    ledcWrite(channelCCWStirrer, 0);
  }
  Serial.print("OSC stirrers PWM: "); Serial.println(value);
}

void blackLED (OSCMessage &msg) {
  float value = msg.getFloat(0);
  int pwmValue = value * 1024;
  ledcWrite(channelUVLeds, pwmValue);
  Serial.print("OSC UVLeds PWM: "); Serial.println(pwmValue);
}
void whiteLED (OSCMessage &msg) {
  float value = msg.getFloat(0);
  int pwmValue = value * 1024;
  ledcWrite(channelWhiteLeds, pwmValue);
  Serial.print("OSC WhiteLeds PWM: "); Serial.println(pwmValue);
}

void loop() {
  // Check WiFi connection
  WiFiCheck();
  esp_task_wdt_reset();
  ArduinoOTA.handle();

  // Create an OSCMessage object
  OSCBundle msgIN;

  // Check for incoming OSC messages
  int packetSize = Udp.parsePacket();
  if (packetSize > 0) {
    // Read the incoming packet into the message
    while (packetSize--) {
      msgIN.fill(Udp.read());
    }
    // Process the OSC message
    if (!msgIN.hasError()) {
      // Check the OSC address
      msgIN.dispatch("/brightLED", bright_LED);
      msgIN.dispatch("/pump", pump);
      msgIN.dispatch("/stirrers", stirrers);
      msgIN.dispatch("/blackLED", blackLED);
      msgIN.dispatch("/whiteLED", whiteLED);
      msgIN.empty();
    }
  }

  delay(10);  // Small delay to prevent spamming loop
}
