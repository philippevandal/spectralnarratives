// Define PWM properties
const int pwmFreq = 5000;     // PWM frequency in Hz
const int pwmResolution = 8;  // PWM resolution in bits (0-255)

// GPIO pin definitions
const int brightLED = 32;     // GPIO23 for brightLED 23
const int forwardPump = 13;   // GPIO13 for forwardPump
const int backwardPump = 27;  // GPIO27 for backwardPump
const int CWstirrer = 26;     // GPIO26 for clockwise stirrer
const int CCWstirrer = 25;    // GPIO25 for counter-clockwise stirrer
const int UVLeds = 23;        // GPIO33 for UV LEDs
const int WhiteLeds = 33;     // GPIO32 for White LEDs

// Assign unique PWM channels to each pin
const int channelBrightLED = 0;
const int channelForwardPump = 1;
const int channelBackwardPump = 2;
const int channelCWStirrer = 3;
const int channelCCWStirrer = 4;
const int channelUVLeds = 5;
const int channelWhiteLeds = 6;

void setup() {
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

  // Initialize Serial for debugging
  Serial.begin(115200);
}

void loop() {
  // Set PWM values for each component
  int pwmValueBrightLED = 200;     // Adjust PWM value between 0 and 255 for brightLED
  int pwmValueForwardPump = 255;   // Adjust PWM value for forwardPump
  int pwmValueBackwardPump = 0;    // Adjust PWM value for backwardPump (off)
  int pwmValueCWStirrer = 150;     // Adjust PWM value for CW stirrer
  int pwmValueCCWStirrer = 0;      // Adjust PWM value for CCW stirrer (off)
  // int pwmValueUVLeds = 255;        // Maximum brightness for UV LEDs
  // int pwmValueWhiteLeds = 255;     // Medium brightness for White LEDs

  // Write the PWM values to the corresponding channels
  ledcWrite(channelBrightLED, pwmValueBrightLED);
  ledcWrite(channelForwardPump, pwmValueForwardPump);
  ledcWrite(channelBackwardPump, pwmValueBackwardPump);
  ledcWrite(channelCWStirrer, pwmValueCWStirrer);
  ledcWrite(channelCCWStirrer, pwmValueCCWStirrer);
  ledcWrite(channelUVLeds, 255);
  ledcWrite(channelWhiteLeds, 0);

  // Print PWM values for debugging
  // Serial.print("brightLED PWM: "); Serial.println(pwmValueBrightLED);
  // Serial.print("forwardPump PWM: "); Serial.println(pwmValueForwardPump);
  // Serial.print("backwardPump PWM: "); Serial.println(pwmValueBackwardPump);
  // Serial.print("CWStirrer PWM: "); Serial.println(pwmValueCWStirrer);
  // Serial.print("CCWStirrer PWM: "); Serial.println(pwmValueCCWStirrer);
  // Serial.print("UVLeds PWM: "); Serial.println(pwmValueUVLeds);
  // Serial.print("WhiteLeds PWM: "); Serial.println(pwmValueWhiteLeds);

  // Delay before updating the PWM values again (adjust as necessary)
  delay(250);
  ledcWrite(channelUVLeds, 0);
  ledcWrite(channelWhiteLeds, 100);
  delay(250);
}
