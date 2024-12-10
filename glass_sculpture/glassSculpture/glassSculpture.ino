#include <WiFi.h>

const char* ssid = "Belleville";
const char* password = "ETpourquoipas545!";

int pumpWhitePin = 32;
int pumpUVPin = 25;
int whiteLEDPin = 26;
int UVLEDPin = 27;

int pumpWhite = 0;
int pumpUV = 1;
int whiteLED = 2;
int UVLED = 3;

const int freq = 30000;
const int resolution = 8;


void setup() {

  ledcSetup(pumpWhite, freq, resolution);
  ledcSetup(pumpUV, freq, resolution);
  ledcSetup(whiteLED, freq, resolution);
  ledcSetup(UVLED, freq, resolution);

  ledcAttachPin(pumpWhitePin, pumpWhite);
  ledcAttachPin(pumpUVPin, pumpUV);
  ledcAttachPin(whiteLEDPin, whiteLED);
  ledcAttachPin(UVLEDPin, UVLED);

  ledcWrite(pumpWhite, 0);
  ledcWrite(pumpUV, 0);
  ledcWrite(whiteLED, 0);
  ledcWrite(UVLED, 0);

  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

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

  ledcWrite(pumpWhite, 255);
  ledcWrite(pumpUV, 50);
  ledcWrite(whiteLED, 100);
  ledcWrite(UVLED, 255);

}