#include <WiFi.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <WiFiUdp.h>

const char* ssid = "spectral";
const char* password = "narratives";

IPAddress staticIP(10, 0, 1, 13); // ESP32 static IP
IPAddress gateway(10, 0, 1, 1);    // IP Address of your network gateway (router)
IPAddress subnet(255, 255, 255, 0);   // Subnet mask

// OSC settings
WiFiUDP Udp;  // UDP instance
const IPAddress remoteIP(192, 168, 1, 100);  // Replace with the IP of your OSC client
const unsigned int remotePort = 8000;  // OSC send port
const unsigned int localPort = 9000;   // OSC receive port

int pumpWhitePin = 25;
int pumpUVPin = 32;
int whiteLEDPin = 26;
int UVLEDPin = 27;

int pumpWhite = 0;
int pumpUV = 1;
int ledW = 2;
int ledUV = 3;

const int freq = 30000;
const int resolution = 10;

void setup() {

  ledcSetup(pumpWhite, freq, resolution);
  ledcSetup(pumpUV, freq, resolution);
  ledcSetup(ledW, freq, resolution);
  ledcSetup(ledUV, freq, resolution);

  ledcAttachPin(pumpWhitePin, pumpWhite);
  ledcAttachPin(pumpUVPin, pumpUV);
  ledcAttachPin(whiteLEDPin, ledW);
  ledcAttachPin(UVLEDPin, ledUV);

  ledcWrite(pumpWhite, 0);
  ledcWrite(pumpUV, 0);
  ledcWrite(ledW, 0);
  ledcWrite(ledUV, 0);

  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  if(!WiFi.config(staticIP, gateway, subnet)) {
    Serial.println("Failed to configure Static IP");
  } else {
    Serial.println("Static IP configured!");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Udp.begin(localPort);

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

void pump_White (OSCMessage &msg) {
  int value = msg.getFloat(0) * 1024.0;
  ledcWrite(pumpWhite, value);
  Serial.print("pump W: ");
  Serial.println(value);
}

void pump_UV (OSCMessage &msg) {
  int value = msg.getFloat(0) * 1024.0;
  ledcWrite(pumpUV, value);
  Serial.print("pump UV: ");
  Serial.println(value);
}

void led_UV (OSCMessage &msg) {
  int value = msg.getFloat(0) * 1024.0;
  ledcWrite(ledUV, value);
  Serial.print("UV: ");
  Serial.println(value);
}

void led_W (OSCMessage &msg) {
  int value = msg.getFloat(0) * 1024.0;
  ledcWrite(ledW, value);
  Serial.print("W: ");
  Serial.println(value);
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
      msgIN.dispatch("/pumpUV", pump_UV);
      msgIN.dispatch("/pumpWhite", pump_White);
      msgIN.dispatch("/ledUV", led_UV);
      msgIN.dispatch("/ledWhite", led_W);
      msgIN.empty();
    }
  }
}