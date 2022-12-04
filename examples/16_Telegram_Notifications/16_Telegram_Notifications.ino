// 10_Image_Wise_Motion_Detection.ino


#define MAX_RESOLUTION_VGA

#define WIFI_SSID "Abc"
#define WIFI_PASS "12345678"

#define TELEGRAM_TOKEN "bot token"
#define CHAT_ID 01234567

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/motion/Detector.h"
#include "esp32cam/motion/SimpleChange.h"
#include "esp32cam/http/TelegramChat.h"


Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::JpegDecoder decoder;
Eloquent::Esp32cam::Motion::SimpleChange algorithm;
Eloquent::Esp32cam::Motion::Detector detector(algorithm);
Eloquent::Esp32cam::Http::TelegramChat telegram(TELEGRAM_TOKEN, CHAT_ID);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.m5wide();
    cam.highQuality();
    cam.vga();
    cam.highestSaturation();
    cam.disableAutomaticWhiteBalance();
    cam.disableAutomaticExposureControl();
    cam.disableGainControl();

    detector.trainFor(30);
    detector.retrainAfter(33ULL * 600);
    detector.triggerAbove(0.2);
    detector.denoise();
    detector.debounceSeconds(5);

    algorithm.differBy(20);
    algorithm.smooth(0.9);
    algorithm.onlyUpdateBackground();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());

    /**
     * Initialize bot
     */
    while (!telegram.begin())
        Serial.println(telegram.getErrorMessage());
}


void loop() {
    if (!cam.capture()) {
        Serial.println(cam.getErrorMessage());
        return;
    }

    if (!decoder.decode(cam)) {
        Serial.println(decoder.getErrorMessage());
        return;
    }

    if (!detector.update(decoder.luma)) {
        Serial.println(detector.getErrorMessage());
        return;
    }

    /**
     * When motion is detected...
     */
    if (detector.triggered()) {
        Serial.print("Motion detected. Sending to Telegram... ");

        /**
         * ...send message and picture to Telegram chat
         */
        telegram.sendMessage("Motion detected");
        telegram.sendPhoto(cam);
        Serial.println("done");
    }
}