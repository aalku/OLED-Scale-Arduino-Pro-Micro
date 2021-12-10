#include <HX711.h>
#include <LowPower.h>
#include <OakOLED.h>
#include <Adafruit_GFX.h>

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
OakOLED oled;

#define DOUT  8
#define CLK  9

int powerPin = 5;
int buttonPin = 2;

volatile bool wantZero = true;

volatile int debug = 0;

HX711 scale(DOUT, CLK);

float calibration_factor = -2480; //-7050 worked for my 440lb max scale setup

long t0;
long t0a;

float p = -12345.6;

const long powerOffMillis = 32 * 1000; // Max 32 seconds

void powerOffBefore() {
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.setTextSize(1);
  oled.println("Shutting down...");
  oled.display();
  digitalWrite(powerPin, LOW);
  pinMode(powerPin, INPUT);
  pinMode(CLK, INPUT); // Prevent scale cosumes energy when powered off
}

void powerOffAfter() {
  pinMode(CLK, OUTPUT); // Restore scale pin config
  debug = 1;
  if (false) { // FIXME
    // LED is ON from interrupt handler
    // but it has to blink a bit if code is resuming from here
    for (int i = 0; i < 5; i++) {
      blinkOffOn(); 
    }
  }
  setup2();
}

void measure() {
  float m = scale.get_units(5);

  char s = m > 0.0 ? ' ' : '-';
  int a = abs((int)m);
  int b = abs(int(m*10))%10;

  oled.clearDisplay();
  oled.setCursor(0,0);

  char displayString[10] = "";
  
  oled.setTextSize(2);
  oled.println("uScale");
  oled.println("");
  
  oled.setTextSize(2); sprintf(displayString, "%c", s); oled.print(displayString);
  oled.setTextSize(3); sprintf(displayString, "%4d", a); oled.print(displayString);
  oled.setCursor(oled.getCursorX(), oled.getCursorY()+4);
  oled.setTextSize(2); sprintf(displayString, ".%dg", b); oled.print(displayString);

  if (abs(p - m) > 2) { // 2g change resets t0
    t0 = millis();
  }

  int x = int(OLED_WIDTH * (millis()-t0)/(float)powerOffMillis);
  if (x >= 0 && x <= OLED_WIDTH) {
    oled.drawFastHLine(x/2, OLED_HEIGHT-1, OLED_WIDTH-x, WHITE);
  }
  oled.display();

  p = m;
}

void setupOled() {
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  delay(50);
  Wire.begin();
  delay(50);
  debug=5;
  oled.begin();
  //oled.begin(SH1106_SWITCHCAPVCC, 0x3C, false);
  oled.clearDisplay();
  oled.setTextSize(3);
  oled.setTextColor(WHITE);
  oled.setCursor(0,0);
  oled.display();
}

void setupPower() {
  pinMode(powerPin, OUTPUT);
  digitalWrite(powerPin, HIGH);
}

void powerOff() {
  powerOffBefore();
  delay(100);
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF); 
  powerOffAfter();
}

void setupScale() {
  scale.set_scale();
  scale.read_average(); //Get a baseline reading
  scale.set_scale(calibration_factor); //Adjust to this calibration factor
}

void zero() {
  oled.clearDisplay();
  oled.setCursor(0,0);
  oled.setTextSize(2);
  oled.println("uScale");
  oled.println("");
  oled.setTextSize(3);
  oled.println("Zero!");
  oled.display();
  while (digitalRead(buttonPin)==LOW) {
    if ((millis() - t0a) / 1000 > 60) { // 1 minute break
      break;
    } else {
      delay(10);
    }
  }
  delay(200);
  scale.tare(5);  //Reset the scale to 0
  oled.clearDisplay();
  oled.display();
  digitalWrite(LED_BUILTIN, LOW);
}

// ============================================================================

void setup() {
  // Wire.setClock(3400000);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  
  pinMode(buttonPin, INPUT_PULLUP);
  setupInterrupt();
  setup2();
}

void blinkOffOn() {
  delay(30);
  digitalWrite(LED_BUILTIN, LOW);  
  delay(30);
  digitalWrite(LED_BUILTIN, HIGH);  
}

void blinkOnOff() {
  delay(30);
  digitalWrite(LED_BUILTIN, HIGH);  
  delay(30);
  digitalWrite(LED_BUILTIN, LOW);  
}

void setup2() {
  debug = 2;
  t0 = millis(); // Boot time, always 0
  t0a = t0;
  blinkOffOn();
  debug = 3;
  setupPower();
  blinkOffOn();
  debug = 4;
  setupOled();
  debug = 7;
  blinkOffOn();
  debug = 8;
  setupScale();
  blinkOffOn();
  debug = 9;
  wantZero = true;
}

void setupInterrupt() {
  attachInterrupt(digitalPinToInterrupt(buttonPin), signalWantZero, CHANGE);
}

void signalWantZero() {
  wantZero = true;
  if (false) {
    // Show debug data value
    digitalWrite(LED_BUILTIN, LOW);
    delayMicroseconds(30000);
    for (int i = 1; i <= debug; i++) {
      digitalWrite(LED_BUILTIN, HIGH);
      delayMicroseconds(10000);
      digitalWrite(LED_BUILTIN, LOW);
      delayMicroseconds(5000);
    }
    delayMicroseconds(30000);
  }
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  blinkOnOff();
  debug = 10;
  if (wantZero) {
    zero();
    wantZero = false;
    t0 = millis();
  }
  if ((millis() - t0) > powerOffMillis) { // x seconds power off if no change
    powerOff();
  } else if ((millis() - t0a) / 1000 > 300) { // 5 minutes power off always
    powerOff();
  } else {
    measure();
  }
}
