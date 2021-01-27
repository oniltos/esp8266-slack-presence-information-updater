# ESP8266 Slack Presence Information Updater
Based on [Slack Status Updater With ESP8266](https://www.instructables.com/Slack-Status-Updater-With-ESP8266/) by Becky Stern

## Hardware 
- NodeMCU ESP12-e
- 128x64 pixel oled display
- Rotary encoder with push button

## Instructions for coding
 You will need to rename the file Credentials.h.sample to Credentials.h 
 and add your WiFi details and a Slack bearer token, see library readme for more details:
 https://github.com/witnessmenow/arduino-slack-api

## Instructions for use
By turning the rotary encoder the available status will be shown on the oled screen. 
By clicking the rotary encoder button, the chosen status will be set via Slack API.

## License
BSD license, check license.txt for more information
