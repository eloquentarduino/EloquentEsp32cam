/**
 * Send image from camera to Telegram
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
// WiFi credentials
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

// replace with your bot token and chat id
#define TELEGRAM_TOKEN "1234567890:AABBCCDDEEFFGGHHIILLMMN-NOOPPQQRRSS"
#define TELEGRAM_CHAT "0123456789"

#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/extra/esp32/telegram.h>

using eloq::camera;
using eloq::wifi;
using eloq::telegram;


/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___TELEGRAM IMAGE___");

    // camera settings
    // replace with your own model!
    camera.pinout.aithinker();
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
    Serial.println("Enter 'image' (without quotes) to send image to Telegram chat");
}


void loop() {
    // await for command
    if (!Serial.available())
      return;

    if (Serial.readStringUntil('\n') != "image") {
        Serial.println("I only understand 'image' (without quotes)");
        return;
    }

    Serial.println("Sending photo...");
      
    // capture new frame
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }
    
    // send
    if (telegram.to(TELEGRAM_CHAT).send(camera.frame).isOk())
        Serial.println("Photo sent to Telegram");
    else
        Serial.println(telegram.exception.toString());

    Serial.println("Enter 'image' (without quotes) to send image to Telegram chat");
}