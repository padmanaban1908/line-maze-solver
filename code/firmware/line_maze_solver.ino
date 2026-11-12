/**
 * Line Maze Solver Robot Firmware
 * Algorithms: PID Line Following + Full LSRB (Left-Straight-Right-Back) State Machine
 * Features:
 *   - Autonomous exploration (Run 1) with real-time dead-end cancellation
 *   - High-speed playback of optimized path (Run 2)
 *   - Inch-forward junction verification
 * Target Boards: Arduino Uno/Nano, ESP32, STM32
 */

#include <Arduino.h>

// ==========================================
// 1. PIN DEFINITIONS & HARDWARE SETUP
// ==========================================

// Motor Driver Pins (TB6612FNG or L298N)
const int PIN_MOTOR_L_PWM = 5;
const int PIN_MOTOR_L_IN1 = 7;
const int PIN_MOTOR_L_IN2 = 8;

const int PIN_MOTOR_R_PWM = 6;
const int PIN_MOTOR_R_IN1 = 9;
const int PIN_MOTOR_R_IN2 = 10;

// Sensor Array Pins (5 IR sensors: L2, L1, C, R1, R2)
const int SENSOR_PINS[5] = {A0, A1, A2, A3, A4};
const int NUM_SENSORS = 5;
const int THRESHOLD = 500; // Reflectance threshold: > 500 = Black Line

// Status & Mode Button
const int PIN_BUTTON = 2;  // Pushbutton to toggle Explore & Speed Run
const int PIN_LED = 13;

// ==========================================
// 2. PID & SPEED CONSTANTS
// ==========================================
float Kp = 18.0;
float Ki = 0.0;
float Kd = 10.0;

int baseSpeed = 160;
int maxSpeed = 230;
int turnSpeed = 150;
int inchSpeed = 120;
int inchTimeMs = 120; // Time in ms to move axle over junction center

float lastError = 0;
float integral = 0;

// ==========================================
// 3. MAZE RECORDING & SOLVING STATE
// ==========================================
#define MAX_PATH_LENGTH 200
char path[MAX_PATH_LENGTH];
int pathLength = 0;
int currentStep = 0;

enum RobotMode {
    MODE_EXPLORE = 0,    // Run 1: Left-hand exploration & path mapping
    MODE_SPEED_RUN = 1   // Run 2: Re-run optimized shortest path
};

RobotMode currentMode = MODE_EXPLORE;
bool mazeSolved = false;

// ==========================================
// 4. MOTOR CONTROL FUNCTIONS
// ==========================================

void setMotorSpeeds(int leftSpeed, int rightSpeed) {
    leftSpeed = constrain(leftSpeed, -255, 255);
    rightSpeed = constrain(rightSpeed, -255, 255);

    // Left Motor
    if (leftSpeed >= 0) {
        digitalWrite(PIN_MOTOR_L_IN1, HIGH);
        digitalWrite(PIN_MOTOR_L_IN2, LOW);
        analogWrite(PIN_MOTOR_L_PWM, leftSpeed);
    } else {
        digitalWrite(PIN_MOTOR_L_IN1, LOW);
        digitalWrite(PIN_MOTOR_L_IN2, HIGH);
        analogWrite(PIN_MOTOR_L_PWM, -leftSpeed);
    }

    // Right Motor
    if (rightSpeed >= 0) {
        digitalWrite(PIN_MOTOR_R_IN1, HIGH);
        digitalWrite(PIN_MOTOR_R_IN2, LOW);
        analogWrite(PIN_MOTOR_R_PWM, rightSpeed);
    } else {
        digitalWrite(PIN_MOTOR_R_IN1, LOW);
        digitalWrite(PIN_MOTOR_R_IN2, HIGH);
        analogWrite(PIN_MOTOR_R_PWM, -rightSpeed);
    }
}

void stopMotors() {
    setMotorSpeeds(0, 0);
}

void inchForward(int durationMs) {
    setMotorSpeeds(inchSpeed, inchSpeed);
    delay(durationMs);
}

void turnLeft() {
    setMotorSpeeds(-turnSpeed, turnSpeed);
    delay(150); // Clear current line
    while (analogRead(SENSOR_PINS[2]) < THRESHOLD) {
        setMotorSpeeds(-turnSpeed, turnSpeed);
    }
    stopMotors();
}

void turnRight() {
    setMotorSpeeds(turnSpeed, -turnSpeed);
    delay(150);
    while (analogRead(SENSOR_PINS[2]) < THRESHOLD) {
        setMotorSpeeds(turnSpeed, -turnSpeed);
    }
    stopMotors();
}

void turnAround() {
    setMotorSpeeds(-turnSpeed, turnSpeed);
    delay(300); // Rotate 180 degrees
    while (analogRead(SENSOR_PINS[2]) < THRESHOLD) {
        setMotorSpeeds(-turnSpeed, turnSpeed);
    }
    stopMotors();
}

// ==========================================
// 5. SENSOR READINGS & PID
// ==========================================

void readSensorsRaw(int *s) {
    for (int i = 0; i < NUM_SENSORS; i++) {
        s[i] = (analogRead(SENSOR_PINS[i]) > THRESHOLD) ? 1 : 0;
    }
}

int calculatePositionError(const int *s) {
    int activeCount = 0;
    int weightedSum = 0;
    for (int i = 0; i < NUM_SENSORS; i++) {
        if (s[i] == 1) {
            activeCount++;
            weightedSum += (i - 2) * 1000; // -2000, -1000, 0, 1000, 2000
        }
    }
    if (activeCount == 0) return 9999; // Dead end or no line
    return weightedSum / activeCount;
}

void followLinePID() {
    int s[5];
    readSensorsRaw(s);
    int error = calculatePositionError(s);

    if (error == 9999) return;

    integral += error;
    float derivative = error - lastError;
    lastError = error;

    float correction = (Kp * error) + (Ki * integral) + (Kd * derivative);

    int leftMotor = baseSpeed + (int)correction;
    int rightMotor = baseSpeed - (int)correction;

    setMotorSpeeds(leftMotor, rightMotor);
}

// ==========================================
// 6. LSRB PATH SIMPLIFICATION ENGINE
// ==========================================

void simplifyPath() {
    // Only triggers when the second-to-last action was a dead-end 'B'
    if (pathLength < 3 || path[pathLength - 2] != 'B') {
        return;
    }

    int totalAngle = 0;
    for (int i = 1; i <= 3; i++) {
        switch (path[pathLength - i]) {
            case 'R': totalAngle += 90; break;
            case 'L': totalAngle += 270; break;
            case 'B': totalAngle += 180; break;
            case 'S': totalAngle += 0; break;
        }
    }

    totalAngle = totalAngle % 360;

    switch (totalAngle) {
        case 0:   path[pathLength - 3] = 'S'; break;
        case 90:  path[pathLength - 3] = 'R'; break;
        case 180: path[pathLength - 3] = 'B'; break;
        case 270: path[pathLength - 3] = 'L'; break;
    }

    pathLength -= 2;
    path[pathLength] = '\0';

    Serial.print("⚡ Simplified Path: ");
    Serial.println(path);
}

void recordAction(char action) {
    if (pathLength < MAX_PATH_LENGTH - 1) {
        path[pathLength++] = action;
        path[pathLength] = '\0';
        Serial.print("Recorded: ");
        Serial.print(action);
        Serial.print(" | Path: ");
        Serial.println(path);
        simplifyPath();
    }
}

// ==========================================
// 7. JUNCTION HANDLING & EXECUTION
// ==========================================

void handleJunction() {
    int s[5];
    readSensorsRaw(s);

    bool leftBranch = (s[0] == 1);
    bool rightBranch = (s[4] == 1);
    bool centerLine = (s[2] == 1);

    // Dead End: All sensors reading white
    if (!s[0] && !s[1] && !s[2] && !s[3] && !s[4]) {
        if (currentMode == MODE_EXPLORE) {
            turnAround();
            recordAction('B');
        }
        return;
    }

    // Check for junction (Left, Right, T, or Cross)
    if (leftBranch || rightBranch) {
        // Inch forward to position wheel axle over junction center
        inchForward(inchTimeMs);

        // Re-sample sensors to check if a straight line continues ahead
        readSensorsRaw(s);
        bool straightAvailable = (s[1] == 1 || s[2] == 1 || s[3] == 1);

        // End of Maze Check: Large solid black square / finish marker
        if (s[0] && s[1] && s[2] && s[3] && s[4]) {
            stopMotors();
            mazeSolved = true;
            digitalWrite(PIN_LED, HIGH);
            Serial.println("🏁 FINISH LINE REACHED! Maze Solved.");
            Serial.print("Final Optimal Path: ");
            Serial.println(path);
            while (true) {
                // Wait for button press to restart or run Run 2
                if (digitalRead(PIN_BUTTON) == LOW) {
                    delay(300);
                    currentMode = MODE_SPEED_RUN;
                    currentStep = 0;
                    mazeSolved = false;
                    break;
                }
            }
            return;
        }

        // ==============================
        // MODE 1: EXPLORATION (LSRB)
        // ==============================
        if (currentMode == MODE_EXPLORE) {
            if (leftBranch) {
                turnLeft();
                recordAction('L');
            } else if (straightAvailable) {
                // Continue straight
                recordAction('S');
            } else if (rightBranch) {
                turnRight();
                recordAction('R');
            }
        }
        // ==============================
        // MODE 2: SPEED RUN (OPTIMIZED)
        // ==============================
        else if (currentMode == MODE_SPEED_RUN) {
            if (currentStep < pathLength) {
                char action = path[currentStep++];
                Serial.print("Speed Run Junction #");
                Serial.print(currentStep);
                Serial.print(": Executing ");
                Serial.println(action);

                switch (action) {
                    case 'L': turnLeft(); break;
                    case 'R': turnRight(); break;
                    case 'S': /* continue straight */ break;
                }
            }
        }
    }
}

// ==========================================
// 8. SETUP & MAIN LOOP
// ==========================================

void setup() {
    Serial.begin(115200);

    pinMode(PIN_MOTOR_L_PWM, OUTPUT);
    pinMode(PIN_MOTOR_L_IN1, OUTPUT);
    pinMode(PIN_MOTOR_L_IN2, OUTPUT);

    pinMode(PIN_MOTOR_R_PWM, OUTPUT);
    pinMode(PIN_MOTOR_R_IN1, OUTPUT);
    pinMode(PIN_MOTOR_R_IN2, OUTPUT);

    pinMode(PIN_BUTTON, INPUT_PULLUP);
    pinMode(PIN_LED, OUTPUT);

    Serial.println("🤖 Line Maze Solver Robot Ready.");
    Serial.println("Mode: EXPLORATION (LSRB Rule Active)");
}

void loop() {
    if (mazeSolved) return;

    // Line Following with PID
    followLinePID();

    // Check for Junctions & Dead Ends
    handleJunction();
}
