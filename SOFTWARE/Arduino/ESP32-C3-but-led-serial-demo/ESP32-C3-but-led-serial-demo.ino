//Simple demo for LED, button, and serial output
//Remember to open the serial monitor and press the button
//to change the state of the LED

#define BUTTON_PIN 9
#define LED_PIN    8

bool ledState = false;

// debounce
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

int lastButtonReading = HIGH;
int buttonState = HIGH;

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("ESP32-C3 Button -> LED Toggle Demo");

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  digitalWrite(LED_PIN, HIGH); // OFF (active LOW LED)
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        ledState = !ledState;

        // inverted logic here
        digitalWrite(LED_PIN, ledState ? LOW : HIGH);

        Serial.print("Button pressed -> LED is now: ");
        Serial.println(ledState ? "ON" : "OFF");
      }
    }
  }

  lastButtonReading = reading;
}
