#include <FastLED.h>
#include <Wire.h>
#include <Adafruit_VEML7700.h>

#define LED_PIN 18
#define NUMPIXELS 60
#define TRIG_PIN 26
#define ECHO_PIN 27
#define TOUCH_PIN 4

CRGB leds[NUMPIXELS];
Adafruit_VEML7700 veml;
bool ledState = false;  // To track the LED on/off state

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(TOUCH_PIN, INPUT); // Initialize the touch pin as an input
  FastLED.addLeds<WS2811, LED_PIN, BRG>(leds, NUMPIXELS);
  FastLED.setBrightness(255); // Set initial brightness

  if (!veml.begin()) {
    Serial.println("Failed to initialize VEML7700 sensor!");
    while (1);
  }
}

void loop() {
  float lux = veml.readLux();

  // Check for touch input
  if (digitalRead(TOUCH_PIN) == HIGH) {
    delay(50); // Debounce delay
    if (digitalRead(TOUCH_PIN) == HIGH) { // Check again to confirm the touch
      ledState = !ledState; // Toggle the state of the LEDs
      Serial.println(ledState ? "LEDs turned on" : "LEDs turned off");
      while (digitalRead(TOUCH_PIN) == HIGH); // Wait for the touch to be released
    }
  }

  // Measure the distance from the ultrasonic sensor
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * 0.034 / 2;

  // Print sensor outputs to the Serial Monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Lux: ");
  Serial.println(lux);

  if (ledState && distance < 10) {
    CRGB color = (lux > 20) ? CRGB::Blue : CRGB::Red;
    int brightness = calculateBrightness(lux);
    setLights(brightness, color);
    Serial.println("Object detected, lights on!");
  } else if (ledState) {
    FastLED.clear();
    FastLED.show();
    Serial.println("No object detected, lights remain off.");
  }
  delay(100); // Adjust for faster sensor updates
}

int calculateBrightness(float lux) {
  // Simple logic to calculate brightness based on ambient light
  if (lux < 200.0) {
    return 255; // Maximum brightness
  } else if (lux < 400.0) {
    return 128; // Medium brightness
  } else {
    return 25;  // Low brightness
  }
}

void setLights(int brightness, CRGB color) {
  FastLED.setBrightness(brightness);
  fill_solid(leds, NUMPIXELS, color);
  FastLED.show();
}
