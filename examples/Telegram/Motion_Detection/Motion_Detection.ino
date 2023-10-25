/**
 * Send image to Telegram when motion is detected
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
#include <eloquent_esp32cam/motion/detection.h>
#include <eloquent_esp32cam/extra/esp32/telegram.h>

using namespace eloq;


/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___TELEGRAM MOTION DETECTION__");

    // camera settings
    // replace with your own model!
    camera.pinout.aithinker();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // motion detection settings
    motion::detection.stride(1);
    motion::detection.threshold(5);
    motion::detection.ratio(0.2);
    motion::detection.rate.atMostOnceEvery(20).seconds();

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
    Serial.println("Move your camera to trigger motion");
}


void loop() {
    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // convert JPEG to RGB565
    if (!camera.rgb565.convert().isOk()) {
        Serial.println(camera.rgb565.exception.toString());
        return;
    }

    // run motion detection
    if (!motion::detection.detect(camera.rgb565).isOk()) {
        Serial.println(motion::detection.exception.toString());
        return;
    }

    // on motion, send Telegram notification
    if (motion::detection.triggered()) {
        Serial.println("Motion detected. Sending picture to Telegram...");

        if (!telegram.to(TELEGRAM_CHAT).send(camera.frame).isOk()) {
            Serial.println(telegram.exception.toString());
            return;
        }
        
        Serial.println("Picture sent to Telegram!");
    }
}