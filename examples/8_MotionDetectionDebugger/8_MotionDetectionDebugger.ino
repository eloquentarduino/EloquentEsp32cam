#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"
#define HOSTNAME "esp32cam"

#include "esp32camera.h"
#include "esp32camera/components/ColorJpeg.h"
#include "esp32camera/motion/MotionDebugger.h"


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    camera.model.aithinker();
    camera.resolution.vga();
    camera.quality.high();

    camera.sensor.disableAllAutomaticControls();

    motionDetection.shouldBlurBeforeDetection(false);
    motionDetection.shouldSmoothDetectionNoise(true);
    motionDetection.shouldOnlyUpdateBackground(true);
    motionDetection.setHistoryWeight(0.9);
    motionDetection.setPixelDeltaThreshold(20);
    motionDetection.setCountThreshold(0.15);

    if (!camera.begin()) {
        Serial.println(camera.getErrorMessage());
        ESP.restart();
    }

    if (!motionDebugger.begin()) {
        Serial.println(motionDebugger.getErrorMessage());
        ESP.restart();
    }

    Serial.println("Camera OK");
    Serial.println("MotionDebugger OK");
    Serial.println(motionDebugger.getWelcomeMessage());
}


void loop() {
    // 
}