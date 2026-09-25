#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <U8g2lib.h>   // Library Manager: "U8g2" by olikraus

// ------------- Bangla bitmaps for the OLED (XBM, 1 = lit pixel) -------------
// Pre-rendered from real Bangla text so conjuncts (যুক্তাক্ষর) are shaped correctly.
#define BN_STATUS_H 24
#define BN_ROW_H 22
#define BN_CLEAR_W 91
const uint8_t BN_CLEAR[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0E,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x08,0x00,0x40,0x03,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x02,0x18,0x00,0x40,0x02,0x00,0x00,0x00,0x20,0x00,0x00,0x3C,0xEE,0x79,0x80,0xC7,0xFF,0xFF,0xFF,0xFF,0xEF,0xFF,0x07,0x66,0x32,0x0B,0xC0,0x4C,0x06,0x30,0x26,0x20,0x30,0x80,0x01,0xC3,0x3A,0x0A,0x60,0x58,0x06,0x30,0x2F,0x20,0x30,0x80,0x01,0xE7,0x3B,0x0B,0xE0,0x7C,0x06,0x38,0x3B,0xF8,0x20,0xC0,0x01,0x3D,0x83,0x0B,0xA0,0x67,0x06,0x36,0x3B,0x2F,0x23,0xB0,0x01,0x18,0xF3,0x09,0x00,0x63,0x86,0x31,0x9E,0x23,0x22,0x8C,0x01,0x0C,0xF2,0x08,0x80,0x41,0xC6,0x31,0x80,0xA3,0x23,0x8E,0x01,0x00,0xE2,0x09,0x00,0x40,0xC6,0x33,0x00,0xA7,0x23,0x9E,0x01,0x00,0x02,0x0B,0x00,0x40,0x86,0x37,0x00,0xAC,0x23,0xBC,0x01,0x00,0x02,0x0C,0x00,0x40,0x06,0x3C,0x00,0x30,0x20,0xE0,0x01,0x00,0x02,0x08,0x00,0x40,0x06,0x3B,0x00,0x30,0x20,0xD8,0x01,0x00,0x02,0x08,0x00,0x40,0x06,0x33,0x00,0x20,0x20,0x98,0x01,0x00,0x02,0x00,0x00,0x40,0x04,0x33,0x00,0x00,0x20,0x98,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#define BN_OBSTACLE_W 39
const uint8_t BN_OBSTACLE[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x10,0x20,0x00,0x20,0x00,0x10,0x60,0xFF,0xEF,0x9C,0x77,0x60,0x00,0x33,0x86,0x19,0x60,0x00,0x33,0xE6,0x19,0x60,0x80,0x23,0xFC,0x11,0x60,0x60,0x23,0xB8,0x11,0x60,0x18,0x23,0x9C,0x11,0x60,0x1C,0x23,0x8E,0x11,0x60,0x3C,0x23,0x9C,0x11,0x20,0x78,0x23,0xB0,0x11,0x00,0xC0,0x23,0xC0,0x11,0x00,0x80,0x23,0x80,0x11,0x00,0x00,0x23,0x80,0x11,0x00,0x00,0x23,0x00,0x11,0x60,0x00,0x02,0x00,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#define BN_DIST_W 35
const uint8_t BN_DIST[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0x7F,0x00,0x02,0x00,0x02,0x00,0x00,0x42,0x00,0x02,0x00,0x00,0xF2,0x81,0x13,0x0F,0x06,0xFA,0xE0,0x93,0x17,0x06,0xDE,0x38,0x92,0x27,0x00,0xCE,0x38,0x22,0x27,0x00,0xC6,0x78,0x62,0x20,0x00,0xC0,0xE0,0x42,0x30,0x00,0xC0,0xE0,0x83,0x19,0x00,0x80,0x70,0x03,0x0F,0x00,0x80,0x60,0x02,0x0F,0x06,0x00,0x00,0x82,0x0D,0x06,0xE0,0x00,0x80,0x0F,0x00,0x20,0x00,0x00,0x0C,0x00,0xE0,0x03,0x00,0x08,0x00,0x00,0x04,0x00,0x00,0x00,0x00,0x08,0x00,0x00,0x00};
#define BN_CM_W 32
const uint8_t BN_CM[] PROGMEM = {0x00,0x00,0xFC,0x03,0x00,0x00,0x0C,0x04,0x00,0x00,0x06,0x00,0x00,0x00,0x04,0x00,0xF0,0xFF,0xFF,0xFF,0x18,0x42,0xCC,0x31,0x0C,0x44,0x8C,0x33,0x06,0x44,0x0C,0x32,0x02,0x7C,0x0C,0x36,0x03,0x6C,0x8C,0x37,0x83,0x4E,0x8C,0x37,0x83,0x46,0x8C,0x3F,0x02,0x47,0x8C,0x3B,0x32,0x40,0x0C,0x30,0x3E,0x40,0x0C,0x20,0x3C,0x40,0x00,0x20,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#define BN_PLUS_W 11
const uint8_t BN_PLUS[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x60,0x00,0x60,0x00,0x60,0x00,0x60,0x00,0xFF,0x07,0x60,0x00,0x60,0x00,0x60,0x00,0x60,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
#define BN_DIGIT_W 10
const uint8_t BN_D0[] PROGMEM = {0x00,0x00,0x00,0x00,0x00,0x00,0x78,0x00,0x9C,0x00,0x06,0x01,0x06,0x01,0x02,0x01,0x82,0x01,0xC4,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BN_D1[] PROGMEM = {0x0C,0x00,0x0C,0x00,0x1C,0x00,0x38,0x00,0x70,0x00,0x60,0x00,0xC0,0x00,0xC0,0x00,0xFC,0x00,0x7C,0x00,0x7E,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BN_D2[] PROGMEM = {0x04,0x00,0x0E,0x00,0x1E,0x00,0x38,0x00,0x60,0x00,0x40,0x00,0x41,0x00,0x7F,0x00,0x3F,0x00,0x1E,0x00,0x30,0x00,0xC0,0x00,0x80,0x01,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BN_D3[] PROGMEM = {0x00,0x00,0xF0,0x00,0xF8,0x01,0x79,0x01,0x79,0x03,0x31,0x02,0x02,0x02,0x02,0x02,0x04,0x03,0x04,0x03,0x98,0x01,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BN_D4[] PROGMEM = {0x78,0x00,0xCC,0x00,0x86,0x00,0x86,0x00,0xCE,0x00,0x7C,0x00,0xFC,0x00,0xCE,0x00,0x86,0x01,0x86,0x00,0xC6,0x00,0x7C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BN_D5[] PROGMEM = {0x00,0x00,0x38,0x00,0x3C,0x00,0xE6,0x00,0xC3,0x01,0x63,0x00,0x31,0x00,0x11,0x00,0x19,0x00,0x13,0x00,0x3E,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BN_D6[] PROGMEM = {0x00,0x00,0x00,0x00,0x1C,0x00,0x19,0x00,0x19,0x00,0x99,0x01,0xDA,0x01,0xF2,0x02,0x06,0x02,0x04,0x03,0x98,0x01,0xF0,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BN_D7[] PROGMEM = {0x00,0x00,0x38,0x00,0x6C,0x00,0xC6,0x00,0xC6,0x00,0xC6,0x00,0xFC,0x00,0xC0,0x00,0xC0,0x00,0xC0,0x00,0xC0,0x00,0x80,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BN_D8[] PROGMEM = {0x00,0x00,0x03,0x00,0x07,0x00,0x06,0x00,0x06,0x02,0xBE,0x03,0x66,0x00,0x66,0x00,0x66,0x00,0x66,0x00,0x36,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t BN_D9[] PROGMEM = {0x02,0x00,0x07,0x00,0x06,0x00,0x3E,0x00,0xF8,0x00,0xE0,0x01,0x9C,0x01,0x36,0x01,0x62,0x01,0xE2,0x01,0xEE,0x00,0x6C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
const uint8_t* const BN_DIGITS[10] = {BN_D0,BN_D1,BN_D2,BN_D3,BN_D4,BN_D5,BN_D6,BN_D7,BN_D8,BN_D9};


// ------------------ Wi-Fi Credentials (AP Mode) ------------------
const char* ssid = "ESP32-Robot";
const char* password = "12345678";

// --------------------- Motor Driver Pins -------------------------
#define ENA 25
#define ENB 13
#define IN1 27
#define IN2 26
#define IN3 14
#define IN4 12

// ----------------------- IR Sensor Pins --------------------------
#define LEFT_SENSOR 34
#define CENTER_SENSOR 32
#define RIGHT_SENSOR 35

// ------------------- Ultrasonic Sensor Pins ----------------------
// Adjust TRIG_PIN / ECHO_PIN to match your wiring
#define TRIG_PIN 5
#define ECHO_PIN 4

// ------------------- Buzzer + LED (alert) -------------------------
#define BUZZER_PIN 18   // active buzzer (+) -> GPIO18, (-) -> GND
#define LED_PIN 19      // LED anode -> 220 ohm -> GPIO19, cathode -> GND

// ------------------- OLED (I2C, SSD1306 128x64) -------------------
// SDA = GPIO21, SCL = GPIO22, VCC = 3.3V, GND = GND
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// ------------------- Web Server Initialization -------------------
AsyncWebServer server(80);

// ------------------------ Global Variables ------------------------
int speedValue = 180;           // default motor speed
bool lineFollowerMode = false;  // line follower on/off

// Operating Mode
// 0 = IR Only
// 1 = IR + Ultrasonic
// 2 = Remote + Ultrasonic
// 3 = Remote Only
int operationMode = 0;

// Follow line settings
bool followBlack = true;  // true = follow black line, false = follow white line
bool autoTrackDetection = false;

// PID-related variables
float Kp = 0.5;
float Kd = 0.15;
float Ki = 0.001;
int lastError = 0;
float integral = 0;
unsigned long lastTime = 0;

// Smoothing parameters for IR
const int numSamples = 10;
int leftSamples[numSamples] = {0};
int centerSamples[numSamples] = {0};
int rightSamples[numSamples] = {0};
int sampleIndex = 0;

// Threshold for detecting line loss
const int lineThreshold = 15;
bool lineLost = false;
int recoveryDirection = 1;
unsigned long lineSearchStartTime = 0;
const unsigned long maxSearchTime = 2000;

// Deadband for PID error
const int errorDeadband = 10;

// Curve detection
const int curveErrorThreshold = 45;
const unsigned long curveDetectionTime = 500;
unsigned long curveTurnStartTime = 0;
bool inCurveTurn = false;

// Square turn detection
const int squareTurnErrorThreshold = 60;
const unsigned long squareTurnDetectionTime = 300;
unsigned long squareTurnStartTime = 0;
bool inSquareTurn = false;

// Debug print
unsigned long lastSensorPrint = 0;

// Motor smoothing
int prevLeftSpeed = 0;
int prevRightSpeed = 0;
const float speedSmoothingFactor = 0.4;

// Manual control tracking
bool manualControlActive = false;
unsigned long lastManualCommandTime = 0;
const unsigned long manualTimeoutMs = 1000;

// ------------------- Ultrasonic Parameters -------------------
const int obstacleThreshold = 30;    // cm; closer than this => obstacle (project spec: 15 cm)
const int obstacleHysteresis = 5;    // obstacle clears only when distance >= threshold + 5
unsigned long ultrasonicCheckTimer = 0;
const unsigned long ultrasonicCheckInterval = 100;  // measure every 100 ms

// Shared sensor state (used by motors, buzzer/LED, OLED and the web dashboard)
long currentDistance = 999;   // last ultrasonic reading in cm (999 = no echo)
bool obstacleNear = false;
int obstacleHitCount = 0;              // consecutive close readings (debounce)
const int obstacleDebounceCount = 3;   // need this many in a row to trigger
int irRawL = 0, irRawC = 0, irRawR = 0;

// OLED refresh bookkeeping
unsigned long lastDisplayUpdate = 0;
long shownDistance = -2;
int shownState = -1;

// ------------------ Function Prototypes ----------------------
long getDistance();
void moveMotors(int targetLeftSpeed, int targetRightSpeed);
void stopMotors();
void lineFollower();
int normalizeReading(int reading);
int getSmoothedReading(int rawReading, int *samples);
int calculateWeightedPosition(int left, int center, int right);
void updateSensors();
void updateAlert();
void obstacleGuard();
void updateDisplay(bool force);

// ----------------------------------------------------------------
// Read ultrasonic distance in centimeters.
// If no obstacle is found in range, returns 999.
// ----------------------------------------------------------------
long getDistance() {
  // Send a 10us pulse to trigger
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo time (12 ms timeout ~ 200 cm max, keeps the loop fast)
  long duration = pulseIn(ECHO_PIN, HIGH, 12000UL);

  // Convert to centimeters
  long distance = duration * 0.0343 / 2;
  if (distance == 0) distance = 999;  // no echo => treat as far away
  return distance;
}

// ----------------------------------------------------------------
// Measure distance + read IR sensors every 100 ms and update obstacleNear
// (with hysteresis so the alert does not flicker at the threshold).
// ----------------------------------------------------------------
void updateSensors() {
  if (millis() - ultrasonicCheckTimer < ultrasonicCheckInterval) return;
  ultrasonicCheckTimer = millis();

  currentDistance = getDistance();

  // Debounce: require several consecutive close readings before triggering,
  // so a single noisy/echo spike from far away can't cause a false stop.
  if (currentDistance < obstacleThreshold) {
    if (obstacleHitCount < obstacleDebounceCount) obstacleHitCount++;
  } else {
    obstacleHitCount = 0;
  }

  if (!obstacleNear && obstacleHitCount >= obstacleDebounceCount) {
    obstacleNear = true;
  } else if (obstacleNear && currentDistance >= obstacleThreshold + obstacleHysteresis) {
    obstacleNear = false;
    obstacleHitCount = 0;
  }

  irRawL = analogRead(LEFT_SENSOR);
  irRawC = analogRead(CENTER_SENSOR);
  irRawR = analogRead(RIGHT_SENSOR);
}

// ----------------------------------------------------------------
// Local alert: LED solid + buzzer beeping (non-blocking) while an obstacle is near.
// ----------------------------------------------------------------
void updateAlert() {
  digitalWrite(LED_PIN, obstacleNear ? HIGH : LOW);
  bool beep = obstacleNear && ((millis() / 150) % 2 == 0);
  digitalWrite(BUZZER_PIN, beep ? HIGH : LOW);
}

// ----------------------------------------------------------------
// Remote + Ultrasonic mode (2): block FORWARD motion while an obstacle is near.
// Backward / turning is still allowed so the driver can get away.
// (Mode 1 handles its stop inside lineFollower().)
// ----------------------------------------------------------------
void obstacleGuard() {
  if (operationMode == 2 && obstacleNear && prevLeftSpeed > 0 && prevRightSpeed > 0) {
    Serial.println("Obstacle in remote+US mode! Easing to a stop.");
    moveMotors(0, 0);  // goes through the normal speed-smoothing, so it eases down
  }
}

// ----------------------------------------------------------------
// Bangla OLED screen. Redraws only when the status or distance changes.
//   top row    : status  ("পথ পরিষ্কার" / inverted "বাধা!")
//   bottom row : "দূরত্ব: <digits> সেমি"
// ----------------------------------------------------------------
void updateDisplay(bool force) {
  if (!force && millis() - lastDisplayUpdate < 250) return;
  lastDisplayUpdate = millis();

  int state = obstacleNear ? 1 : 0;
  long d = currentDistance;
  if (d > 200) d = 201;  // 201 => show "২০০+"
  if (!force && state == shownState && d == shownDistance) return;
  shownState = state;
  shownDistance = d;

  u8g2.clearBuffer();

  // Status row
  const uint8_t* img = state ? BN_OBSTACLE : BN_CLEAR;
  int w = state ? BN_OBSTACLE_W : BN_CLEAR_W;
  if (state) {                       // inverted banner for obstacle
    u8g2.setDrawColor(1);
    u8g2.drawBox(0, 0, 128, BN_STATUS_H + 4);
    u8g2.setDrawColor(0);
  }
  u8g2.drawXBM((128 - w) / 2, 2, w, BN_STATUS_H, img);
  u8g2.setDrawColor(1);

  // Distance row: word + digits (+ "+") + unit, centred
  int value = (d > 200) ? 200 : (int)d;
  char buf[5];
  snprintf(buf, sizeof(buf), "%d", value);
  int nd = strlen(buf);
  const int gap = 3;
  int total = BN_DIST_W + gap + nd * BN_DIGIT_W + (d > 200 ? BN_PLUS_W : 0) + gap + BN_CM_W;
  int x = (128 - total) / 2;
  if (x < 0) x = 0;
  const int y = 38;
  u8g2.drawXBM(x, y, BN_DIST_W, BN_ROW_H, BN_DIST);
  x += BN_DIST_W + gap;
  for (int i = 0; i < nd; i++) {
    u8g2.drawXBM(x, y, BN_DIGIT_W, BN_ROW_H, BN_DIGITS[buf[i] - '0']);
    x += BN_DIGIT_W;
  }
  if (d > 200) {
    u8g2.drawXBM(x, y, BN_PLUS_W, BN_ROW_H, BN_PLUS);
    x += BN_PLUS_W;
  }
  x += gap;
  u8g2.drawXBM(x, y, BN_CM_W, BN_ROW_H, BN_CM);

  u8g2.sendBuffer();
}

// ----------------------------------------------------------------
// Move motors with smoothing, plus deadzone compensation.
// Also sets manualControlActive unless we're in line follower logic.
// ----------------------------------------------------------------
void moveMotors(int targetLeftSpeed, int targetRightSpeed) {
  // If we are not in line follower OR we were already triggered by a manual command
  if (!lineFollowerMode || manualControlActive) {
    manualControlActive = true;
    lastManualCommandTime = millis();
  }

  // Minimum torque boost
  const int startingBoost = 50;
  if (targetLeftSpeed > 0 && targetLeftSpeed < startingBoost) targetLeftSpeed = startingBoost;
  if (targetLeftSpeed < 0 && targetLeftSpeed > -startingBoost) targetLeftSpeed = -startingBoost;
  if (targetRightSpeed > 0 && targetRightSpeed < startingBoost) targetRightSpeed = startingBoost;
  if (targetRightSpeed < 0 && targetRightSpeed > -startingBoost) targetRightSpeed = -startingBoost;

  // Deadzone
  const int deadzone = 40;
  if (abs(targetLeftSpeed) < deadzone && targetLeftSpeed != 0)
    targetLeftSpeed = (targetLeftSpeed > 0) ? deadzone : -deadzone;
  if (abs(targetRightSpeed) < deadzone && targetRightSpeed != 0)
    targetRightSpeed = (targetRightSpeed > 0) ? deadzone : -deadzone;

  // Smoothing factor
  float smoothFactor = manualControlActive ? 0.6 : speedSmoothingFactor;
  int leftSpeed = prevLeftSpeed + (targetLeftSpeed - prevLeftSpeed) * smoothFactor;
  int rightSpeed = prevRightSpeed + (targetRightSpeed - prevRightSpeed) * smoothFactor;

  // Store speeds
  prevLeftSpeed = leftSpeed;
  prevRightSpeed = rightSpeed;

  // Output pins
  digitalWrite(IN1, leftSpeed > 0);
  digitalWrite(IN2, leftSpeed < 0);
  digitalWrite(IN3, rightSpeed > 0);
  digitalWrite(IN4, rightSpeed < 0);
  analogWrite(ENA, abs(leftSpeed));
  analogWrite(ENB, abs(rightSpeed));

  // Debug
  if (manualControlActive) {
    Serial.print("Manual Control: L=");
    Serial.print(leftSpeed);
    Serial.print(", R=");
    Serial.println(rightSpeed);
  }
}

// ----------------------------------------------------------------
void stopMotors() {
  manualControlActive = false;
  prevLeftSpeed = 0;
  prevRightSpeed = 0;
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}

// ----------------------------------------------------------------
// Normalizes raw reading to 0..100
// ----------------------------------------------------------------
int normalizeReading(int reading) {
  const int sensorMin = 150;
  const int sensorMax = 4095;
  reading = constrain(reading, sensorMin, sensorMax);
  return map(reading, sensorMin, sensorMax, 0, 100);
}

// ----------------------------------------------------------------
int getSmoothedReading(int rawReading, int *samples) {
  samples[sampleIndex] = rawReading;
  long sum = 0;
  for (int i = 0; i < numSamples; i++) {
    sum += samples[i];
  }
  return sum / numSamples;
}

// ----------------------------------------------------------------
// Weighted average for position calculation
// ----------------------------------------------------------------
int calculateWeightedPosition(int left, int center, int right) {
  const int centerWeight = 3;

  if (followBlack) {
    int weightedSum = left + (center * centerWeight) + right;
    if (weightedSum == 0) return 50;  // fallback to center
    int position = ((left * 0) + (center * centerWeight * 50) + (right * 100)) / weightedSum;
    return position;
  } else {
    // White line -> invert readings
    left = 100 - left;
    center = 100 - center;
    right = 100 - right;
    int weightedSum = left + (center * centerWeight) + right;
    if (weightedSum == 0) return 50;
    int position = ((left * 0) + (center * centerWeight * 50) + (right * 100)) / weightedSum;
    return position;
  }
}

// ----------------------------------------------------------------
// Line follower logic with PID, plus optional ultrasonic check.
// Only runs if operationMode == 0 (IR only) or == 1 (IR + Ultra).
// If operationMode == 1, we check for obstacles and avoid if needed.
// ----------------------------------------------------------------
void lineFollower() {
  if (!lineFollowerMode) return;  // not in line follow mode at all

  if (manualControlActive && (millis() - lastManualCommandTime < manualTimeoutMs)) {
    return;  // user recently pressed a manual command
  } else if (manualControlActive) {
    manualControlActive = false;  // reset if timed out
  }

  // Only IR-only or IR+ultrasonic modes
  if (operationMode != 0 && operationMode != 1) return;

  // --- If IR+Ultrasonic: stay stopped while an obstacle is near, resume when clear ---
  if (operationMode == 1 && obstacleNear) {
    moveMotors(0, 0);     // eases down instead of an instant stop
    lastTime = millis();  // avoid a huge PID time-step after the pause
    integral = 0;
    return;
  }

  unsigned long currentTime = millis();
  float deltaTime = (currentTime - lastTime) / 1000.0;
  if (deltaTime <= 0) deltaTime = 0.001;
  lastTime = currentTime;

  // Read raw sensors
  int leftRaw = analogRead(LEFT_SENSOR);
  int centerRaw = analogRead(CENTER_SENSOR);
  int rightRaw = analogRead(RIGHT_SENSOR);

  // Smoothing
  int smoothLeft = getSmoothedReading(leftRaw, leftSamples);
  int smoothCenter = getSmoothedReading(centerRaw, centerSamples);
  int smoothRight = getSmoothedReading(rightRaw, rightSamples);
  sampleIndex = (sampleIndex + 1) % numSamples;

  // Normalize
  int normLeft = normalizeReading(smoothLeft);
  int normCenter = normalizeReading(smoothCenter);
  int normRight = normalizeReading(smoothRight);

  // If autoTrackDetection is on, decide black/white
  if (autoTrackDetection) {
    followBlack = (normCenter >= 50);
  }

  // Check if sensors are off the line
  bool leftOff = followBlack ? (normLeft < lineThreshold) : (normLeft > (100 - lineThreshold));
  bool centerOff = followBlack ? (normCenter < lineThreshold) : (normCenter > (100 - lineThreshold));
  bool rightOff = followBlack ? (normRight < lineThreshold) : (normRight > (100 - lineThreshold));

  // ---------- Handle line lost logic (all off) ----------
  if (leftOff && centerOff && rightOff) {
    // Square turn detection first
    if (abs(lastError) > squareTurnErrorThreshold) {
      if (squareTurnStartTime == 0) {
        squareTurnStartTime = currentTime;
      }
      if (currentTime - squareTurnStartTime > squareTurnDetectionTime) {
        inSquareTurn = true;
      }
    } else {
      squareTurnStartTime = 0;
      inSquareTurn = false;
    }

    if (inSquareTurn) {
      if (lastError < 0) {
        moveMotors(-speedValue, speedValue);  // turn left
      } else {
        moveMotors(speedValue, -speedValue);  // turn right
      }
      return;  // the flags are reset in the "else" branch below once the line is found again
    }

    // Normal line-lost recovery
    if (!lineLost) {
      lineLost = true;
      lineSearchStartTime = currentTime;
      recoveryDirection = (lastError > 0) ? 1 : -1;
    }
    if (currentTime - lineSearchStartTime > maxSearchTime) {
      recoveryDirection = -recoveryDirection;
      lineSearchStartTime = currentTime;
    }
    int recoverySpeed = speedValue * 0.6;
    moveMotors(recoveryDirection * recoverySpeed, -recoveryDirection * recoverySpeed);
    squareTurnStartTime = 0;
    inSquareTurn = false;
    return;
  } else {
    lineLost = false;
    squareTurnStartTime = 0;
    inSquareTurn = false;
  }

  // ---------- Normal line follow with PID ----------
  int position = calculateWeightedPosition(normLeft, normCenter, normRight);
  int error = position - 50;

  // Deadband
  if (abs(error) < errorDeadband) {
    error = 0;
    integral = 0;
  }

  // Curve detection
  if (abs(error) > curveErrorThreshold) {
    if (curveTurnStartTime == 0) {
      curveTurnStartTime = currentTime;
    }
    if (currentTime - curveTurnStartTime > curveDetectionTime) {
      inCurveTurn = true;
    }
  } else {
    curveTurnStartTime = 0;
    inCurveTurn = false;
  }

  if (inCurveTurn) {
    if (error < 0) moveMotors(-speedValue, speedValue);
    else moveMotors(speedValue, -speedValue);
    return;
  }

  // PID
  float P = Kp * error;
  integral += error * deltaTime;
  integral = constrain(integral, -30, 30);
  float I = Ki * integral;

  static float filteredDerivative = 0;
  float derivative = (error - lastError) / deltaTime;
  filteredDerivative = filteredDerivative * 0.8 + derivative * 0.2;
  float D = Kd * filteredDerivative;

  int correction = (int)(P + I + D);
  lastError = error;

  // Adjust base speed
  int baseSpeed = speedValue;
  if (abs(error) > 30) {
    baseSpeed = speedValue * 0.85;
  } else if (abs(error) < 10) {
    baseSpeed = speedValue * 0.95;
  }

  // Differential drive
  int leftMotorSpeed, rightMotorSpeed;
  int adjustedCorrection = correction;
  if (abs(correction) > 50) {
    adjustedCorrection = correction * 1.2;
  }

  if (adjustedCorrection > 0) {
    leftMotorSpeed = baseSpeed;
    rightMotorSpeed = baseSpeed - adjustedCorrection;
  } else {
    leftMotorSpeed = baseSpeed + adjustedCorrection;
    rightMotorSpeed = baseSpeed;
  }

  // Slight reverse for very sharp turns
  if (abs(error) > 45) {
    if (adjustedCorrection > 0) rightMotorSpeed = -baseSpeed * 0.25;
    else leftMotorSpeed = -baseSpeed * 0.25;
  }

  moveMotors(constrain(leftMotorSpeed, -255, 255),
             constrain(rightMotorSpeed, -255, 255));
}

// ----------------------------------------------------------------
// The HTML page, with a Mode Toggle to cycle among 4 modes
// ----------------------------------------------------------------
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8"/>
  <meta name="viewport" content="width=device-width, initial-scale=1"/>
  <title>ESP32 Robot Control</title>
  <style>
    body { background: #f7f7f7; font-family: Arial, sans-serif; text-align: center; color: #333; }
    header { background: #007ACC; padding: 20px; color: #fff; }
    .container { padding: 20px; max-width: 500px; margin: auto; }
    .btn { background: #007ACC; color: #fff; border: none; border-radius: 5px; width: 120px; height: 50px; font-size: 16px; margin: 10px; cursor: pointer; touch-action: none; user-select: none; -webkit-user-select: none; }
    .btn:hover { background: #005a99; }
    .btn:active { background: #004080; }
    .slider { width: 80%; margin: 20px 0; }
    .control-grid { display: grid; grid-template-columns: 1fr 1fr 1fr; gap: 5px; max-width: 300px; margin: auto; }
    .control-grid .btn { width: 100%; margin: 5px; }
    .spacer { visibility: hidden; }
    .value-display { font-weight: bold; margin-left: 10px; }
    .card { background: #fff; border-radius: 8px; padding: 14px; margin: 0 0 20px; box-shadow: 0 1px 4px rgba(0,0,0,.2); }
    .big { font-size: 30px; font-weight: bold; margin-bottom: 10px; }
    .badge { color: #fff; border-radius: 6px; padding: 10px; font-size: 24px; font-weight: bold; }
    .ok { background: #2e7d32; }
    .warn { background: #c62828; }
    .ir { display: flex; align-items: center; gap: 8px; margin: 8px 0; }
    .ir .lbl { width: 42px; text-align: left; }
    .ir .val { width: 36px; text-align: right; }
    .bar { flex: 1; height: 14px; background: #ddd; border-radius: 7px; overflow: hidden; }
    .bar div { height: 100%; width: 0%; background: #007ACC; }
    .small { font-size: 13px; color: #666; margin: 8px 0 0; }
  </style>
</head>
<body>
  <header>
    <h1>ESP32 Robot Control</h1>
  </header>
  <div class="container">
    <div class="card">
      <div class="big" id="dist">দূরত্ব: -- সেমি</div>
      <div id="badge" class="badge ok">পথ পরিষ্কার</div>
      <div class="ir"><span class="lbl">বাম</span><div class="bar"><div id="barL"></div></div><span class="val" id="valL">০</span></div>
      <div class="ir"><span class="lbl">মাঝ</span><div class="bar"><div id="barC"></div></div><span class="val" id="valC">০</span></div>
      <div class="ir"><span class="lbl">ডান</span><div class="bar"><div id="barR"></div></div><span class="val" id="valR">০</span></div>
      <p class="small" id="stateLine">connecting...</p>
    </div>
    <label>Speed:</label>
    <input type="range" min="0" max="255" value="180" class="slider" id="speedSlider" onchange="setSpeed(this.value)">
    <span id="speedValue" class="value-display">180</span>
    <br>
    <label>Kp:</label>
    <input type="range" min="0" max="1000" value="50" class="slider" id="kpSlider" onchange="setKp(this.value)">
    <span id="kpValue" class="value-display">0.50</span>
    <br>
    <label>Kd:</label>
    <input type="range" min="0" max="500" value="15" class="slider" id="kdSlider" onchange="setKd(this.value)">
    <span id="kdValue" class="value-display">0.15</span>
    <br>
    <label>Ki:</label>
    <input type="range" min="0" max="100" value="1" class="slider" id="kiSlider" onchange="setKi(this.value)">
    <span id="kiValue" class="value-display">0.001</span>
    <br>
    <div class="control-grid" oncontextmenu="return false;">
      <div class="spacer"></div>
      <button class="btn"
        onpointerdown="startHold('forward')" onpointerup="endHold()"
        onpointerleave="endHold()" onpointercancel="endHold()">
        Forward
      </button>
      <div class="spacer"></div>

      <button class="btn"
        onpointerdown="startHold('left')" onpointerup="endHold()"
        onpointerleave="endHold()" onpointercancel="endHold()">
        Left
      </button>
      <button class="btn" onclick="sendCommand('stop')">Stop</button>
      <button class="btn"
        onpointerdown="startHold('right')" onpointerup="endHold()"
        onpointerleave="endHold()" onpointercancel="endHold()">
        Right
      </button>

      <div class="spacer"></div>
      <button class="btn"
        onpointerdown="startHold('backward')" onpointerup="endHold()"
        onpointerleave="endHold()" onpointercancel="endHold()">
        Backward
      </button>
      <div class="spacer"></div>
    </div>
    <br>
    <button class="btn" id="modeBtn" onclick="toggleMode()">Enable Line Follower</button>
    <br>
    <button class="btn" onclick="toggleTrackMode()">Track Mode</button>
    <button class="btn" onclick="toggleLineType()">Line Color</button>
    <br><br>
    <p id="trackModeDisplay">Track Mode: Manual (Fixed Black)</p>
    <br><hr>
    <button class="btn" onclick="toggleOperationMode()" id="operationModeBtn">Cycle Robot Mode</button>
    <p id="operationModeLabel">Current Mode: IR Only</p>
  </div>

  <script>
    let holdTimer = null;

    function sendCommand(cmd) {
      fetch("/" + cmd);
      console.log("Command: " + cmd);
    }
    // While a direction button is held, keep re-sending the command so the
    // robot's manual-control timeout (1 s) does not stop it.
    function startHold(cmd) {
      endHold();
      sendCommand(cmd);
      holdTimer = setInterval(function () { fetch("/" + cmd); }, 100);
    }
    function endHold() {
      if (holdTimer !== null) {
        clearInterval(holdTimer);
        holdTimer = null;
        sendCommand('stop');
      }
    }
    function setSpeed(val) {
      fetch("/speed?value=" + val);
      document.getElementById("speedValue").innerText = val;
    }
    function setKp(val) {
      fetch("/setKp?value=" + val);
      document.getElementById("kpValue").innerText = (val/100).toFixed(2);
    }
    function setKd(val) {
      fetch("/setKd?value=" + val);
      document.getElementById("kdValue").innerText = (val/100).toFixed(2);
    }
    function setKi(val) {
      fetch("/setKi?value=" + val);
      document.getElementById("kiValue").innerText = (val/1000).toFixed(3);
    }
    function toggleMode() {
      fetch("/toggleMode")
        .then(r => r.text())
        .then(mode => {
          document.getElementById("modeBtn").innerText =
            (mode === "1") ? "Disable Line Follower" : "Enable Line Follower";
        });
    }
    function toggleLineType() {
      fetch("/toggleLineType")
        .then(r => r.text())
        .then(val => {
          alert("Follow " + (val === "1" ? "Black" : "White") + " line now.");
        });
    }
    function toggleTrackMode() {
      fetch("/toggleTrackMode")
        .then(r => r.text())
        .then(mode => {
          let msg = (mode === "1") ? "Automatic (Detect Black/White)" : "Manual (Fixed Black)";
          document.getElementById("trackModeDisplay").innerText = "Track Mode: " + msg;
        });
    }
    function toggleOperationMode() {
      fetch("/toggleOperationMode")
        .then(r => r.text())
        .then(val => {
          let modeNumber = parseInt(val);
          let label = "";
          switch(modeNumber) {
            case 0: label = "IR Only"; break;
            case 1: label = "IR + Ultrasonic"; break;
            case 2: label = "Remote + Ultrasonic"; break;
            case 3: label = "Remote Only"; break;
          }
          document.getElementById("operationModeLabel").innerText = "Current Mode: " + label;
        });
    }

    // ---------- Live dashboard: poll /data every 300 ms ----------
    const BN = '০১২৩৪৫৬৭৮৯';
    const MODES = ['IR Only', 'IR + Ultrasonic', 'Remote + Ultrasonic', 'Remote Only'];
    function bn(v) { return String(v).replace(/[0-9]/g, function (d) { return BN[d]; }); }
    let polling = false;
    function poll() {
      if (polling) return;
      polling = true;
      fetch("/data", { cache: "no-store" })
        .then(r => r.json())
        .then(j => {
          document.getElementById("dist").innerText = "দূরত্ব: " + (j.d > 200 ? "২০০+" : bn(j.d)) + " সেমি";
          const b = document.getElementById("badge");
          b.innerText = j.o ? "বাধা!" : "পথ পরিষ্কার";
          b.className = "badge " + (j.o ? "warn" : "ok");
          ["L", "C", "R"].forEach(function (k, i) {
            const v = [j.l, j.c, j.r][i];
            document.getElementById("bar" + k).style.width = v + "%";
            document.getElementById("val" + k).innerText = bn(v);
          });
          document.getElementById("stateLine").innerText =
            "Mode: " + MODES[j.m] + " | Line follower: " + (j.lf ? "ON" : "OFF") +
            " | Line: " + (j.fb ? "Black" : "White") + " | Raw L/C/R: " + j.rl + "/" + j.rc + "/" + j.rr;
          document.getElementById("modeBtn").innerText = j.lf ? "Disable Line Follower" : "Enable Line Follower";
          document.getElementById("operationModeLabel").innerText = "Current Mode: " + MODES[j.m];
          document.getElementById("trackModeDisplay").innerText =
            "Track Mode: " + (j.at ? "Automatic (Detect Black/White)" : "Manual (Fixed Black)");
        })
        .catch(() => { document.getElementById("dist").innerText = "সংযোগ নেই"; })
        .finally(() => { polling = false; });
    }
    setInterval(poll, 300);
    poll();
  </script>
</body>
</html>
)rawliteral";

// ----------------------------------------------------------------
// Setup server & endpoints
// ----------------------------------------------------------------
void setupServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", index_html);
  });

  server.on("/forward", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Remote + Ultrasonic mode: do not drive forward into an obstacle
    if (operationMode == 2 && obstacleNear) {
      stopMotors();
      request->send(200, "text/plain", "Blocked: obstacle ahead");
      return;
    }
    manualControlActive = true;
    lastManualCommandTime = millis();
    moveMotors(speedValue, speedValue);
    request->send(200, "text/plain", "Moving Forward");
  });

  server.on("/backward", HTTP_GET, [](AsyncWebServerRequest *request) {
    manualControlActive = true;
    lastManualCommandTime = millis();
    moveMotors(-speedValue, -speedValue);
    request->send(200, "text/plain", "Moving Backward");
  });

  server.on("/left", HTTP_GET, [](AsyncWebServerRequest *request) {
    manualControlActive = true;
    lastManualCommandTime = millis();
    moveMotors(-speedValue, speedValue);
    request->send(200, "text/plain", "Turning Left");
  });

  server.on("/right", HTTP_GET, [](AsyncWebServerRequest *request) {
    manualControlActive = true;
    lastManualCommandTime = millis();
    moveMotors(speedValue, -speedValue);
    request->send(200, "text/plain", "Turning Right");
  });

  server.on("/stop", HTTP_GET, [](AsyncWebServerRequest *request) {
    stopMotors();
    request->send(200, "text/plain", "Stopped");
  });

  server.on("/speed", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      speedValue = constrain(request->getParam("value")->value().toInt(), 0, 255);
      Serial.print("Speed set to: ");
      Serial.println(speedValue);
    }
    request->send(200, "text/plain", "Speed Set");
  });

  server.on("/setKp", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      int kpVal = request->getParam("value")->value().toInt();
      Kp = kpVal / 100.0;
      Serial.print("Kp set to: ");
      Serial.println(Kp);
    }
    request->send(200, "text/plain", "Kp Set");
  });

  server.on("/setKd", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      int kdVal = request->getParam("value")->value().toInt();
      Kd = kdVal / 100.0;
      Serial.print("Kd set to: ");
      Serial.println(Kd);
    }
    request->send(200, "text/plain", "Kd Set");
  });

  server.on("/setKi", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("value")) {
      int kiVal = request->getParam("value")->value().toInt();
      Ki = kiVal / 1000.0;
      Serial.print("Ki set to: ");
      Serial.println(Ki);
    }
    request->send(200, "text/plain", "Ki Set");
  });

  server.on("/toggleMode", HTTP_GET, [](AsyncWebServerRequest *request) {
    lineFollowerMode = !lineFollowerMode;
    if (!lineFollowerMode) {
      stopMotors();
    }
    Serial.print("Line Follower Mode: ");
    Serial.println(lineFollowerMode ? "Enabled" : "Disabled");
    request->send(200, "text/plain", lineFollowerMode ? "1" : "0");
  });

  server.on("/toggleLineType", HTTP_GET, [](AsyncWebServerRequest *request) {
    followBlack = !followBlack;
    Serial.print("Line Type: ");
    Serial.println(followBlack ? "Black" : "White");
    request->send(200, "text/plain", followBlack ? "1" : "0");
  });

  server.on("/toggleTrackMode", HTTP_GET, [](AsyncWebServerRequest *request) {
    autoTrackDetection = !autoTrackDetection;
    Serial.print("Track Mode: ");
    Serial.println(autoTrackDetection ? "Automatic" : "Manual");
    request->send(200, "text/plain", autoTrackDetection ? "1" : "0");
  });

  // Cycle among the 4 operation modes
  server.on("/toggleOperationMode", HTTP_GET, [](AsyncWebServerRequest *request) {
    operationMode = (operationMode + 1) % 4;
    Serial.print("Operation Mode changed to: ");
    switch (operationMode) {
      case 0: Serial.println("IR Only"); break;
      case 1: Serial.println("IR + Ultrasonic"); break;
      case 2: Serial.println("Remote + Ultrasonic"); break;
      case 3: Serial.println("Remote Only"); break;
    }
    request->send(200, "text/plain", String(operationMode));
  });

  // Live data for the dashboard on the web page
  server.on("/data", HTTP_GET, [](AsyncWebServerRequest *request) {
    String j = "{";
    j += "\"d\":" + String(currentDistance);
    j += ",\"o\":" + String(obstacleNear ? 1 : 0);
    j += ",\"m\":" + String(operationMode);
    j += ",\"lf\":" + String(lineFollowerMode ? 1 : 0);
    j += ",\"at\":" + String(autoTrackDetection ? 1 : 0);
    j += ",\"fb\":" + String(followBlack ? 1 : 0);
    j += ",\"l\":" + String(normalizeReading(irRawL));
    j += ",\"c\":" + String(normalizeReading(irRawC));
    j += ",\"r\":" + String(normalizeReading(irRawR));
    j += ",\"rl\":" + String(irRawL);
    j += ",\"rc\":" + String(irRawC);
    j += ",\"rr\":" + String(irRawR);
    j += "}";
    request->send(200, "application/json", j);
  });

  server.begin();
  Serial.println("Web server started");
}

// ----------------------------------------------------------------
// Setup
// ----------------------------------------------------------------
void setup() {
  Serial.begin(115200);

  // Motor pins
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // IR sensor pins
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(CENTER_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  // Ultrasonic pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Buzzer + LED
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);

  // OLED (I2C: SDA = 21, SCL = 22)
  Wire.begin(21, 22);
  u8g2.begin();
  u8g2.setBusClock(400000);

  // Stop motors initially
  stopMotors();

  // Start AP mode
  WiFi.softAP(ssid, password);
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  setupServer();

  lastTime = millis();

  // Initialize smoothing arrays
  for (int i = 0; i < numSamples; i++) {
    leftSamples[i] = analogRead(LEFT_SENSOR);
    centerSamples[i] = analogRead(CENTER_SENSOR);
    rightSamples[i] = analogRead(RIGHT_SENSOR);
  }

  // Power-on self test: short beep + LED blink, then show the Bangla screen
  digitalWrite(BUZZER_PIN, HIGH);
  digitalWrite(LED_PIN, HIGH);
  delay(150);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  updateDisplay(true);

  Serial.println("Robot initialized and ready!");
}

// ----------------------------------------------------------------
// Loop
// ----------------------------------------------------------------
void loop() {
  // Sensors, alert and display run in every mode
  updateSensors();  // ultrasonic + IR (every 100 ms)
  updateAlert();    // buzzer + LED while an obstacle is near
  obstacleGuard();  // mode 2: stop forward motion when blocked

  // If the user manually pressed a command, ensure we stop after inactivity
  if (manualControlActive && (millis() - lastManualCommandTime > manualTimeoutMs)) {
    stopMotors();
    manualControlActive = false;
  }

  // Modes 0 (IR only) and 1 (IR + Ultrasonic) => line follower.
  // Mode 1 stops inside lineFollower() while obstacleNear is true.
  // Modes 2 and 3 are manual driving (mode 2 has the forward-block guard above).
  if (operationMode == 0 || operationMode == 1) {
    if (lineFollowerMode && !manualControlActive) {
      lineFollower();
    }
  }

  updateDisplay(false);  // Bangla OLED (redraws only on change)

  // Debug printing every second (uses the stored readings)
  if (millis() - lastSensorPrint > 1000) {
    Serial.print("[DEBUG] IR raw -> L:"); Serial.print(irRawL);
    Serial.print(" C:"); Serial.print(irRawC);
    Serial.print(" R:"); Serial.print(irRawR);
    Serial.print(" | norm L:"); Serial.print(normalizeReading(irRawL));
    Serial.print(" C:"); Serial.print(normalizeReading(irRawC));
    Serial.print(" R:"); Serial.print(normalizeReading(irRawR));
    Serial.print(" | Ultrasonic: "); Serial.print(currentDistance);
    Serial.println("cm");
    lastSensorPrint = millis();
  }
}
