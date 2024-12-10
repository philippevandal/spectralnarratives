#include <AccelStepper.h>

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

// Steps per revolution (adjust if necessary)
#define STEPS_PER_REV 2048

// LED PWM properties
#define LEDC_CHANNEL_0 0
// #define LEDC_CHANNEL_1 1
#define LEDC_CHANNEL_2 2
#define LEDC_TIMER_13_BIT 13
#define LEDC_BASE_FREQ 5000

// Maximum angle
const int MAX_ANGLE = 80;

// Delay at the median position (in milliseconds)
const long MEDIAN_POSITION_DELAY = 5000;

// Create three instances of AccelStepper
AccelStepper stepper1(AccelStepper::FULL4WIRE, IN1_PIN1, IN3_PIN1, IN2_PIN1, IN4_PIN1);
// AccelStepper stepper2(AccelStepper::FULL4WIRE, IN1_PIN2, IN3_PIN2, IN2_PIN2, IN4_PIN2);
AccelStepper stepper3(AccelStepper::FULL4WIRE, IN1_PIN3, IN3_PIN3, IN2_PIN3, IN4_PIN3);

unsigned long previousMillis = 0;
const long interval = 2000;  // Interval at which to generate random numbers (2 seconds)

int randomCount = 0;

void setup() {
  // Set the maximum speed and acceleration for each motor
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(500.0);
  // stepper2.setMaxSpeed(1000.0);
  // stepper2.setAcceleration(500.0);
  stepper3.setMaxSpeed(1000.0);
  stepper3.setAcceleration(500.0);

  // Initialize serial communication for debugging purposes
  Serial.begin(115200);

  // Configure LED PWM functionalities
  ledcSetup(LEDC_CHANNEL_0, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  // ledcSetup(LEDC_CHANNEL_1, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);
  ledcSetup(LEDC_CHANNEL_2, LEDC_BASE_FREQ, LEDC_TIMER_13_BIT);

  // Attach the LED pins to PWM channels
  ledcAttachPin(LED_PIN1, LEDC_CHANNEL_0);
  // ledcAttachPin(LED_PIN2, LEDC_CHANNEL_1);
  ledcAttachPin(LED_PIN3, LEDC_CHANNEL_2);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    randomCount++;

    // Check if it's time to return the motors to the median position
    if (randomCount >= 10) {
      // Move the motors to the median position (0 degrees)
      stepper1.moveTo(0);
      // stepper2.moveTo(0);
      stepper3.moveTo(0);

      // Run all motors to ensure they reach the median position
      // while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0 || stepper3.distanceToGo() != 0) {
      while (stepper1.distanceToGo() != 0 || stepper3.distanceToGo() != 0) {

        stepper1.run();
        // stepper2.run();
        stepper3.run();
      }

      // Keep the motors at the median position for the specified delay duration
      delay(MEDIAN_POSITION_DELAY);

      randomCount = 0; // Reset the counter
    } else {
      // Generate random angles between -MAX_ANGLE and MAX_ANGLE degrees
      int angle1 = random(-MAX_ANGLE, MAX_ANGLE + 1);
      // int angle2 = random(-MAX_ANGLE, MAX_ANGLE + 1);
      int angle3 = random(-MAX_ANGLE, MAX_ANGLE + 1);

      // Print the angles for debugging
      Serial.print("Angle 1: ");
      Serial.print(angle1);
      // Serial.print(" degrees, Angle 2: ");
      // Serial.print(angle2);
      Serial.print(" degrees, Angle 3: ");
      Serial.print(angle3);
      Serial.println(" degrees");

      // Convert angles to steps
      // int steps1 = map(angle1, -MAX_ANGLE, MAX_ANGLE, -STEPS_PER_REV / 8, STEPS_PER_REV / 8);
      int steps1 = (long(angle1) * STEPS_PER_REV) / 360;
      Serial.println(steps1);
      // int steps2 = map(angle2, -MAX_ANGLE, MAX_ANGLE, -STEPS_PER_REV / 8, STEPS_PER_REV / 8);
      int steps3 = map(angle3, -MAX_ANGLE, MAX_ANGLE, -STEPS_PER_REV / 8, STEPS_PER_REV / 8);

      // Move the motors to the new positions
      stepper1.moveTo(steps1);
      // stepper2.moveTo(steps2);
      stepper3.moveTo(steps3);

      // Generate random brightness values between 0 and 8191 (for 13-bit resolution)
      int brightness1 = random(0, 8192);
      // int brightness2 = random(0, 8192);
      int brightness3 = random(0, 8192);
      // int brightness3 = 8192;

      // Set the LED brightness
      ledcWrite(LEDC_CHANNEL_0, brightness1);
      // ledcWrite(LEDC_CHANNEL_1, brightness2);
      ledcWrite(LEDC_CHANNEL_2, brightness3);
    }
  }

  // Run all motors to ensure simultaneous movement
  stepper1.run();
  // stepper2.run();
  stepper3.run();
}
