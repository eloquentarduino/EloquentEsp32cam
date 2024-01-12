/**
 * Send text to Telegram
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

using eloq::wifi;
using eloq::telegram;


/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___TELEGRAM TEXT MESSAGE___");

    // connect to WiFi
    while (!wifi.connect().isOk())
      Serial.println(wifi.exception.toString());

    // connect to Telegram API
    while (!telegram.begin().isOk())
      Serial.println(telegram.exception.toString());

    Serial.println("Telegram OK");
    Serial.println("Enter the text you want to send to Telegram chat");
}


void loop() {
    // await for text
    if (!Serial.available())
      return;

    String text = Serial.readStringUntil('\n');

    Serial.print("Sending text: ");
    Serial.println(text);
      
    // send
    if (telegram.to(TELEGRAM_CHAT).send(text).isOk())
        Serial.println("Message sent to Telegram!");
    else
        Serial.println(telegram.exception.toString());

  Serial.println("Enter the text you want to send to Telegram chat");
}