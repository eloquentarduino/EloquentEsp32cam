// 29_Cloud_Upload_Motion.ino

/**
 * When the camera detects motion, upload the picture
 * to the online dashboard at https://esp32camstorage.eloquentarduino.com
 *
 * This is the combination of 10_Image_Wise_Motion_Detection + 28_Cloud_Upload
 */
#define MAX_RESOLUTION_VGA

// Replace with your WiFi credentials
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

// replace with the device token from https://esp32camstorage.eloquentarduino.com
#define DEVICE_TOKEN "abcDefghi0LMNO1qR"

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/motion/Detector.h"
#include "esp32cam/motion/SimpleChange.h"


Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::JpegDecoder decoder;
Eloquent::Esp32cam::Motion::SimpleChange algorithm;
Eloquent::Esp32cam::Motion::Detector detector(algorithm);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.highQuality();
    cam.vga();
    cam.highestSaturation();
    /**
     * For motion detection to perform well, I suggest
     * you disable automatic controls, otherwise
     * the camera sensor will artificially alter the
     * pixels and those will be mis-labelled as foreground
     */
    cam.disableAutomaticWhiteBalance();
    cam.disableAutomaticExposureControl();
    cam.disableGainControl();

    // see 10_Image_Wise_Motion_Detection.ino for details
    detector.trainFor(30);
    detector.retrainAfter(33ULL * 600);
    detector.triggerAbove(0.2);
    detector.denoise();
    detector.debounceSeconds(10);
    algorithm.differBy(20);
    algorithm.smooth(0.9);
    algorithm.onlyUpdateBackground();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    // Connect to WiFi
    // If something goes wrong, print the error message
    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());
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
     * Test if motion was detected
     */
    if (detector.triggered()) {
        Serial.println("Motion");
        Serial.print("Uploading image...");
        cam.uploadToCloudStorage(DEVICE_TOKEN);
        Serial.println(cam.isOk() ? "OK. Go check your dashboard to see the image" : cam.getErrorMessage());
    }
}