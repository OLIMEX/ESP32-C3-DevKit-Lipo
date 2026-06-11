/*
  Rui Santos & Sara Santos - Random Nerd Tutorials
  Complete project details at https://RandomNerdTutorials.com/esp32-door-status-telegram/
  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

Olimex ESP32-C3-DevKit-Lipo https://www.olimex.com/Products/IoT/ESP32-C3/ESP32-C3-DevKit-Lipo/open-source-hardware
  push button - GPIO9	or Door magnetic switch https://www.olimex.com/Products/Components/Switches/MAG-SW/
  LED status  - GPIO8
  PIR sensor  - GPIO0	https://www.olimex.com/Products/Breadboarding/Sensors/PIR-SB312/

  The project is described here: https://olimex.wordpress.com/2026/06/11/home-automation-with-phone-notifications-for-under-e5-yes-its-possible/
  
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Set GPIOs for LED, reedswitch and PIR sensor
const int reedSwitch = 9;
const int led = 8; //optional
const int pirSensor = 0;

// Detects whenever the door changed state
bool changeState = false;

// Detects whenever the PIR sensor triggered
bool pirTriggered = false;

// Holds reedswitch state (1=opened, 0=close)
bool state;
String doorState;

// Auxiliary variables (it will only detect changes that are 1500 milliseconds apart)
unsigned long previousMillis = 0;
const long interval = 1500;

// PIR debounce: ignore re-triggers within 5000 ms
unsigned long pirPreviousMillis = 0;
const long pirInterval = 5000;

const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "XXXXXXXXXX"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Runs whenever the reedswitch changes state
ICACHE_RAM_ATTR void changeDoorStatus() {
  Serial.println("Door State changed");
  changeState = true;
}

// Runs whenever the PIR sensor detects motion (output goes HIGH)
ICACHE_RAM_ATTR void motionDetected() {
  pirTriggered = true;
}

void setup() {
  // Serial port for debugging purposes
  Serial.begin(115200);  

  // Read the current door state
  pinMode(reedSwitch, INPUT_PULLUP);
  state = digitalRead(reedSwitch);

  // Set LED state to match door state
  pinMode(led, OUTPUT);
  digitalWrite(led, !state);
  
  // Set the reedswitch pin as interrupt, assign interrupt function and set CHANGE mode
  attachInterrupt(digitalPinToInterrupt(reedSwitch), changeDoorStatus, CHANGE);

  // Set PIR sensor pin as input and attach interrupt on RISING edge (output goes HIGH)
  pinMode(pirSensor, INPUT);
  attachInterrupt(digitalPinToInterrupt(pirSensor), motionDetected, RISING);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");  

  bot.sendMessage(CHAT_ID, "ESP32-C3 Bot started up", "");
}

void loop() {
  if (changeState){
    unsigned long currentMillis = millis();
    if(currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
      // If a state has occurred, invert the current door state   
        state = !state;
        if(state) {
          doorState = "closed";
        }
        else{
          doorState = "open";
        }
        digitalWrite(led, !state);
        changeState = false;
        Serial.println(state);
        Serial.println(doorState);
        
        //Send notification
        bot.sendMessage(CHAT_ID, "The door is " + doorState, "");
    }
  }

  if (pirTriggered) {
    unsigned long currentMillis = millis();
    if (currentMillis - pirPreviousMillis >= pirInterval) {
      pirPreviousMillis = currentMillis;
      pirTriggered = false;
      Serial.println("Motion detected");
      bot.sendMessage(CHAT_ID, "Motion detected", "");
    } else {
      pirTriggered = false;
    }
  }
}
