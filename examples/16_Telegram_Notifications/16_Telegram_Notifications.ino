/**
 * Example 16: Telegram notifications
 * This sketch shows how to send text and images to
 * a Telegram bot.
 *
 * Open the Serial Monitor and enter 'capture' (without quotes)
 * to capture a new image and send to Telegram.
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */

// if you define WIFI_SSID and WIFI_PASS before importing the library
// the camera will automatically connect
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

// if you define TELEGRAM_TOKEN and CHAT_ID before importing the library
// the telegram bot will be automatically created
#define TELEGRAM_TOKEN "1111111111:xxxxxxxxxxxxxxxxxxxxxxx-yyyyyyyyyyy"
#define TELEGRAM_CHAT_ID "1234567890"

#include "esp32camera.h"
#include "esp32camera/services/TelegramChatBot.h"


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    camera.model.aithinker();
    camera.resolution.vga();
    camera.quality.high();

    // Init camera
    while (!camera.begin())
        Serial.println(camera.getErrorMessage());

    // Init bot
    while (!telegramChat.begin())
        Serial.println(telegramChat.getErrorMessage());

    Serial.println("Camera OK");
    Serial.println("Telegram OK");
    Serial.println("Enter 'capture' to capture a new picture");
}


void loop() {
    // await for "capture" from the Serial Monitor
    if (!Serial.available())
        return;

    if (Serial.readStringUntil('\n') != "capture") {
        Serial.println("I understand only 'capture'");
        return;
    }

    // capture picture
    if (!camera.capture()) {
        Serial.println(camera.getErrorMessage());
        return;
    }

    // send via Telegram
    Serial.println("Capture OK. Sending to Telegram... ");
    Serial.println(telegramChat.sendText("Motion detected") ? "Text OK" : "Text ERROR");
    Serial.println(telegramChat.sendPicture() ? "Picture OK" : "Picture ERROR");
}