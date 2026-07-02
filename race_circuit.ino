// Motor pin definitions
#define LEFT_MOTOR_FORWARD  3
#define LEFT_MOTOR_BACKWARD 5
#define RIGHT_MOTOR_FORWARD 6
#define RIGHT_MOTOR_BACKWARD 9

// IR sensor pin definitions
#define LEFT_SENSOR 2
#define CENTER_SENSOR 4
#define RIGHT_SENSOR 7

// ==========================================
// --- TUNING PARAMETERS (ADJUST THESE) ---
// ==========================================
#define BASE_SPEED 150       // Normal driving speed (0-255)
#define MAX_SPEED 255        // Absolute maximum speed limit
#define CORNER_SPEED 120     // Speed used while spinning in 90-degree turns
#define ALIGN_DELAY 100      // Milliseconds to inch forward before a 90-degree turn
#define BRAKE_DROP 50        // Speed to subtract when dynamically braking for curves

float Kp = 45.0; // Proportional: Reacts to being off-center
float Kd = 25.0; // Derivative: Resists wobbling

int lastError = 0;

void setup() {
  pinMode(LEFT_MOTOR_FORWARD, OUTPUT);
  pinMode(LEFT_MOTOR_BACKWARD, OUTPUT);
  pinMode(RIGHT_MOTOR_FORWARD, OUTPUT);
  pinMode(RIGHT_MOTOR_BACKWARD, OUTPUT);

  pinMode(LEFT_SENSOR, INPUT);
  pinMode(CENTER_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
}

void loop() {
  int L = digitalRead(LEFT_SENSOR);
  int C = digitalRead(CENTER_SENSOR);
  int R = digitalRead(RIGHT_SENSOR);

  // ==========================================
  // 1. INTERSECTION & 90-DEGREE OVERRIDES
  // ==========================================
  
  // Cross Intersection: All sensors see line. Go straight.
  if (L == 1 && C == 1 && R == 1) {
    setMotors(BASE_SPEED, BASE_SPEED);
    return; // Skip PD math for this loop iteration
  }
  
  // 90-Degree Left Turn Candidate
  else if (L == 1 && C == 1 && R == 0) {
    setMotors(0, 0); // Stop briefly to debounce
    delay(15);
    if (digitalRead(LEFT_SENSOR) == 1 && digitalRead(CENTER_SENSOR) == 1) {
      pivotLeft90();
    }
    return;
  }
  
  // 90-Degree Right Turn Candidate
  else if (L == 0 && C == 1 && R == 1) {
    setMotors(0, 0); // Stop briefly to debounce
    delay(15);
    if (digitalRead(RIGHT_SENSOR) == 1 && digitalRead(CENTER_SENSOR) == 1) {
      pivotRight90();
    }
    return;
  }

  // ==========================================
  // 2. PD ERROR MAPPING & MEMORY
  // ==========================================
  int error = 0;

  if      (L == 0 && C == 1 && R == 0) error = 0;   // Perfectly centered
  else if (L == 0 && C == 0 && R == 1) error = 2;   // Drifting left (Right sees line)
  else if (L == 0 && C == 1 && R == 1) error = 1;   // Slightly left
  else if (L == 1 && C == 0 && R == 0) error = -2;  // Drifting right (Left sees line)
  else if (L == 1 && C == 1 && R == 0) error = -1;  // Slightly right
  else if (L == 0 && C == 0 && R == 0) {
    // Line Lost (Memory feature)
    if (lastError > 0) error = 3;
    else if (lastError < 0) error = -3;
    else error = 0;
  }

  // ==========================================
  // 3. PD CALCULATION & DYNAMIC BRAKING
  // ==========================================
  int P = error;
  int D = error - lastError;
  int correction = (Kp * P) + (Kd * D);
  
  lastError = error; // Save for next loop

  // Dynamic Braking: Slow down if we are correcting a heavy drift
  int currentBaseSpeed = BASE_SPEED;
  if (abs(error) > 1) {
    currentBaseSpeed = BASE_SPEED - BRAKE_DROP; 
  }

  // Apply correction
  int leftMotorSpeed = currentBaseSpeed + correction;
  int rightMotorSpeed = currentBaseSpeed - correction;

  leftMotorSpeed = constrain(leftMotorSpeed, -MAX_SPEED, MAX_SPEED);
  rightMotorSpeed = constrain(rightMotorSpeed, -MAX_SPEED, MAX_SPEED);

  setMotors(leftMotorSpeed, rightMotorSpeed);
}

// ==========================================
// --- MOTOR & MOVEMENT FUNCTIONS ---
// ==========================================

void setMotors(int leftSpeed, int rightSpeed) {
  // Left Motor
  if (leftSpeed >= 0) {
    analogWrite(LEFT_MOTOR_FORWARD, leftSpeed);
    analogWrite(LEFT_MOTOR_BACKWARD, 0);
  } else {
    analogWrite(LEFT_MOTOR_FORWARD, 0);
    analogWrite(LEFT_MOTOR_BACKWARD, -leftSpeed); // Active braking
  }

  // Right Motor
  if (rightSpeed >= 0) {
    analogWrite(RIGHT_MOTOR_FORWARD, rightSpeed);
    analogWrite(RIGHT_MOTOR_BACKWARD, 0);
  } else {
    analogWrite(RIGHT_MOTOR_FORWARD, 0);
    analogWrite(RIGHT_MOTOR_BACKWARD, -rightSpeed); // Active braking
  }
}

void pivotLeft90() {
  // 1. Inch forward to align wheels over the corner
  setMotors(BASE_SPEED, BASE_SPEED);
  delay(ALIGN_DELAY); 
  
  // 2. Hard left tank turn
  setMotors(-CORNER_SPEED, CORNER_SPEED);
  
  // 3. Spin until center sensor clears the old line
  while(digitalRead(CENTER_SENSOR) == 1) {}
  
  // 4. Spin until center sensor finds the new line
  while(digitalRead(CENTER_SENSOR) == 0) {}
  
  // 5. Kill momentum before resuming normal driving
  setMotors(0, 0);
  delay(50);
}

void pivotRight90() {
  // 1. Inch forward to align wheels over the corner
  setMotors(BASE_SPEED, BASE_SPEED);
  delay(ALIGN_DELAY); 
  
  // 2. Hard right tank turn
  setMotors(CORNER_SPEED, -CORNER_SPEED);
  
  // 3. Spin until center sensor clears the old line
  while(digitalRead(CENTER_SENSOR) == 1) {}
  
  // 4. Spin until center sensor finds the new line
  while(digitalRead(CENTER_SENSOR) == 0) {}
  
  // 5. Kill momentum before resuming normal driving
  setMotors(0, 0);
  delay(50);
}