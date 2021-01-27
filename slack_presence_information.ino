/**************************************************************************
 Code for a Slack Presense Information Updater
 By Nilton Freitas <mail@niltonfreitas.com>
 Based on Becky Stern Instructable "Slack Status Updater ESP8266"

 ======
 
 This code is intended to use with the following hardware: 
 NodeMCU ESP12-e
 128x64 pixel oled display
 Rotary encoder with push button

 ======

 You will need to rename the file Credentials.h.sample to Credentials.h 
 and add your WiFi details and a Slack bearer token, see library readme for more details:
 https://github.com/witnessmenow/arduino-slack-api
 
 ======
 
 BSD license, check license.txt for more information
 **************************************************************************/


#include <Button2.h>;
#include <ESPRotary.h>;
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoSlack.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Wire.h>
#include "Credentials.h"
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SLACK_ENABLE_DEBUG

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/////////////////////////////////////////////////////////////////

//WiFi and Slack API Client
WiFiClientSecure client;
ArduinoSlack slack(client, SLACK_ACCESS_TOKEN);
unsigned long delayBetweenRequests = 15000; // Time between requests
unsigned long requestDueTime;               // time when request due
volatile int position;


/////////////////////////////////////////////////////////////////
//ESPRotary settings
#define ROTARY_PIN1  D6
#define ROTARY_PIN2 D7
#define BUTTON_PIN  D4

#define CLICKS_PER_STEP   16   // this number depends on your rotary encoder 
#define MIN_POS   0
#define MAX_POS   6

ESPRotary r = ESPRotary(ROTARY_PIN1, ROTARY_PIN2, CLICKS_PER_STEP, MIN_POS, MAX_POS);
Button2 b = Button2(BUTTON_PIN);

/////////////////////////////////////////////////////////////////
//Status Array
const char * const status[7][2] = {
  {"Available", ":zap:"},
  {"Focus", ":crystal_ball:"},
  {"Coffee", ":coffee:"},
  {"Lunch", ":bento:"},
  {"Meeting", ":spiral_calendar_pad:"},
  {"Offline", ":radio_button:" },
  {"Day off", ":beach_with_umbrella:"}
};
/////////////////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);

  delay(50);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000);

  // Clear the buffer
  display.clearDisplay();

  r.setChangedHandler(rotate);
  r.setLeftRotationHandler(showDirection);
  r.setRightRotationHandler(showDirection);

  b.setTapHandler(click);
  b.setLongClickHandler(resetPosition);

  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(SSID);
  WiFi.begin(SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  client.setFingerprint(SLACK_FINGERPRINT);
}

void displayProfile(SlackProfile profile)
{
  if (!profile.error)
  {
    Serial.println("--------- Profile ---------");


    Serial.print("Display Name: ");
    Serial.println(profile.displayName);

    Serial.print("Status Text: ");
    Serial.println(profile.statusText);

    Serial.print("Status Emoji: ");
    Serial.println(profile.statusEmoji);

    Serial.print("Status Expiration: ");
    Serial.println(profile.statusExpiration);

    Serial.println("------------------------");
  } else {
    Serial.println("error getting profile");
  }
}

void loop() {
  r.loop();
  b.loop();
}

void showNewStatus(int statusId) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15, 26);
  display.println(status[statusId][0]);
  display.display();
}

/////////////////////////////////////////////////////////////////

// on change
void rotate(ESPRotary& r) {
  position = r.getPosition();
  showNewStatus(position);
  Serial.println(position);
}

// on left or right rotation
void showDirection(ESPRotary& r) {
  Serial.println(r.directionToString(r.getDirection()));
}

// single click
void click(Button2& btn) {
  Serial.println("Click!");
  SlackProfile profile;
  profile = slack.setCustomStatus(status[position][0], status[position][1]);
  displayProfile(profile);
}

// long click
void resetPosition(Button2& btn) {
  r.resetPosition();
  Serial.println("Reset!");
}

/////////////////////////////////////////////////////////////////
