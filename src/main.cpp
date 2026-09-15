#include <Arduino.h>
#include <Servo.h>

// ---- Configuration ----
const uint8_t SERVO_PIN = 9;     // Connected via jumper: "Pul" (Động cơ Servo) -> "PB1-9"
const int MIN_ANGLE = 0;
const int MAX_ANGLE = 180;

Servo myServo;

String inputBuffer = "";
bool commandReady = false;

// ---- Function prototypes ----
void readSerialCommand();
void processCommand(String cmd);
bool isValidNumber(const String &s);

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  myServo.write(90);           // start at a safe neutral position
  Serial.println(F("READY - Send angle (0-180) followed by Enter"));
}

void loop() {
  readSerialCommand();

  if (commandReady) {
    processCommand(inputBuffer);
    inputBuffer = "";
    commandReady = false;
  }
}

// Reads characters until newline, builds up inputBuffer
void readSerialCommand() {
  while (Serial.available() > 0 && !commandReady) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        commandReady = true;
      }
    } else {
      inputBuffer += c;
    }
  }
}

void processCommand(String cmd) {
  cmd.trim();

  // Validate: must be numeric (allow optional leading '-')
  if (!isValidNumber(cmd)) {
    Serial.print(F("ERROR: '"));
    Serial.print(cmd);
    Serial.println(F("' is not a valid number"));
    Serial.println(F("STATUS: Command rejected"));
    return;
  }

  int angle = cmd.toInt();

  if (angle < MIN_ANGLE || angle > MAX_ANGLE) {
    Serial.print(F("ERROR: Angle "));
    Serial.print(angle);
    Serial.print(F(" out of range ["));
    Serial.print(MIN_ANGLE);
    Serial.print(F(","));
    Serial.print(MAX_ANGLE);
    Serial.println(F("]"));
    Serial.println(F("STATUS: Command rejected"));
    return;
  }

  myServo.write(angle);
  delay(15); // allow servo time to move (safe minimum)

  Serial.print(F("OK: Servo moved to "));
  Serial.print(angle);
  Serial.println(F(" degrees"));
  Serial.println(F("STATUS: Command accepted"));
}

bool isValidNumber(const String &s) {
  if (s.length() == 0) return false;

  int start = 0;
  if (s.charAt(0) == '-') {
    start = 1;
    if (s.length() == 1) return false; // just "-"
  }

  for (unsigned int i = start; i < s.length(); i++) {
    if (!isDigit(s.charAt(i))) return false;
  }
  return true;
}