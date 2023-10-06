/**
 * Send text and photo to Telegram
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */

// if you define WIFI_SSID and WIFI_PASS before importing the library, 
// you can call connect() instead of connect(ssid, pass) on the wifi object
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

// replace with your bot token and chat id
#define TELEGRAM_TOKEN "1234567890:AABBCCDDEEFFGGHHIILLMMN-NOOPPQQRRSS"
#define TELEGRAM_CHAT "0123456789"

#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/extra/esp32/telegram.h>

using namespace e;


/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___TELEGRAM___");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // connect to WiFi
    while (!wifi.connect().isOk())
      Serial.println(wifi.exception.toString());

    // connect to Telegram API
    while (!telegram.begin().isOk())
      Serial.println(telegram.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Telegram OK");
    Serial.println("Enter 'photo' (without quotes) to send a photo");
    Serial.println("or any other text to send a text message");
}


void loop() {
    // await for command
    if (!Serial.available())
      return;

    String command = Serial.readStringUntil('\n');

    if (command == "photo")
      sendPhoto();
    else
      sendText(command);

  Serial.println("Enter 'photo' (without quotes) to send a photo");
  Serial.println("or any other text to send a text message");
}


/**
 * 
 */
void sendPhoto() {
  Serial.println("Sending photo...");
      
  if (!camera.capture().isOk()) {
    Serial.println(camera.exception.toString());
    return;
  }
  
  if (telegram.to(TELEGRAM_CHAT).send(camera.frame).isOk())
    Serial.println("Photo sent to Telegram");
  else
    Serial.println(telegram.exception.toString());
}


/**
 * 
 */
void sendText(String text) {
  Serial.println("Sending text...");
      
  if (telegram.to(TELEGRAM_CHAT).send(text).isOk())
    Serial.println("Message sent to Telegram");
  else
    Serial.println(telegram.exception.toString());
}
