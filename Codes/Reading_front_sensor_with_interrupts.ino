#include <Wire.h>
#include <Adafruit_VL53L0X.h>
//#include "StepperControl.h"   // <-- your header file for stepper motors

#define NUM_SENSORS 6

// XSHUT pins for sensors
int xshutPins[NUM_SENSORS] = {33, 34, 35, 36, 37, 38};

// Sensor objects
Adafruit_VL53L0X sensors[NUM_SENSORS];

// Unique I2C addresses
uint8_t sensorAddresses[NUM_SENSORS] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35};

// Interrupt pin for 4th sensor (index 3, XSHUT=36)
const int TOF_INT_PIN = 19;
volatile bool tofDataReady = false;

void tofISR() {
  tofDataReady = true;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Hold all sensors in reset
  for (int i = 0; i < NUM_SENSORS; i++) {
    pinMode(xshutPins[i], OUTPUT);
    digitalWrite(xshutPins[i], LOW);
  }
  delay(10);

  // Bring up sensors one by one and assign unique addresses
  for (int i = 0; i < NUM_SENSORS; i++) {
    digitalWrite(xshutPins[i], HIGH);
    delay(10);

    if (!sensors[i].begin(0x29, &Wire)) {
      Serial.print("Failed to init sensor ");
      Serial.println(i);
      while (1);
    }

    sensors[i].setAddress(sensorAddresses[i]);
    Serial.print("Sensor ");
    Serial.print(i);
    Serial.print(" at 0x");
    Serial.println(sensorAddresses[i], HEX);
  }

  // Configure interrupt for 4th sensor (index 3)
  pinMode(TOF_INT_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(TOF_INT_PIN), tofISR, CHANGE);

  // --- Hardware filtering: assert GPIO when distance < 200 mm ---
  // Use THRESHOLD_CROSSED_LOW with low=200 mm, ignore <50 mm in software
  FixPoint1616_t lowThresh  = (200 * 65536.0);   // 200 mm
  FixPoint1616_t highThresh = (0   * 65536.0);   // not used

  sensors[3].setGpioConfig(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING,
                           VL53L0X_GPIOFUNCTIONALITY_THRESHOLD_CROSSED_LOW,
                           VL53L0X_INTERRUPTPOLARITY_LOW);

  sensors[3].setInterruptThresholds(lowThresh, highThresh, true);
  sensors[3].setDeviceMode(VL53L0X_DEVICEMODE_CONTINUOUS_RANGING, false);
  sensors[3].startMeasurement();

  // Initialize your stepper motors if needed
  // StepperControl::initMotors();

  Serial.println("Setup complete.");
}

void loop() {
  VL53L0X_RangingMeasurementData_t measure;

  // Only handle interrupt-driven sensor (index 3)
  if (tofDataReady) {
    tofDataReady = false;

    sensors[3].getRangingMeasurement(&measure, false);

    if (measure.RangeStatus != 4) {
      int distance = measure.RangeMilliMeter;

      // Valid window: 50 mm <= distance < 200 mm
      if (distance >= 70 && distance < 200) {
        Serial.print("Forward Sensor (index 3): ");
        Serial.print(distance);
        Serial.println(" mm");

        // Example: run your stepper motors when object detected in range
        // StepperControl::runMotorsForward();
      }
    } else {
      Serial.println("Forward sensor out of range");
    }

    // Clear interrupt so sensor can trigger again
    sensors[3].clearInterruptMask(false);
  }

  // Other sensors can be polled here if needed
  // ...
}