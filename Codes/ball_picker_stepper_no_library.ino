// ULN2003 + 28BYJ-48 Stepper Motor Test (No library)

int IN1 = 39;
int IN2 = 40;
int IN3 = 41;
int IN4 = 42;

int stepDelay = 3;  // delay between steps (ms) -> lower = faster

// 8-step half-stepping sequence
int stepSequence[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
}

void loop() {
  // Rotate one full revolution clockwise
  rotateFull(1);   // 1 = clockwise

  delay(1000);

  // Rotate one full revolution counter-clockwise
  // rotateFull(-1);  // -1 = anticlockwise

  // delay(1000);
}

// Rotate 360 degrees (full turn)
void rotateFull(int direction) {
  int stepsPerRevolution = 4096;   // 360°

  for (int i = 0; i < stepsPerRevolution; i++) {
    stepMotor(direction);
  }
}

// Step one position
void stepMotor(int direction) {
  static int stepIndex = 0;

  for (int pin = 0; pin < 4; pin++) {
    digitalWrite(IN1 + pin, stepSequence[stepIndex][pin]);
  }

  stepIndex += direction;

  if (stepIndex > 7) stepIndex = 0;
  if (stepIndex < 0) stepIndex = 7;

  delay(stepDelay/2);
}
