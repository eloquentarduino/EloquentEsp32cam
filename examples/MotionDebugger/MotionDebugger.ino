#define WIFI_SSID "Novanetworks_Spagro2017"
#define WIFI_PASS "AGRIMAG01"
#define HOSTNAME "esp32cam"

#include "esp32camera.h"
#include "esp32camera/components/ColorJpeg.h"
#include "esp32camera/motion/MotionDebugger.h"


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    camera.model.m5wide();
    camera.resolution.vga();
    camera.quality.high();

    motionDetection.shouldBlurBeforeDetection(false);
    motionDetection.shouldSmoothDetectionNoise(false);
    motionDetection.shouldOnlyUpdateBackground(true);

    // Init camera
    while (!camera.begin())
        Serial.println(camera.getErrorMessage());

    // init mjper http server
    while (!motionDebugger.begin())
        Serial.println(motionDebugger.getErrorMessage());

    Serial.println("Camera OK");
    Serial.println("MotionDebugger OK");
    Serial.println(motionDebugger.getWelcomeMessage());
}


void loop() {
    // 
}