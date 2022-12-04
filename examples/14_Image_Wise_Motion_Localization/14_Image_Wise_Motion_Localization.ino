// 14_Image_Wise_Motion_Localization.ino


#define MAX_RESOLUTION_VGA

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/motion/Detector.h"
#include "esp32cam/motion/SimpleChange.h"
#include "esp32cam/motion/localization/SlidingWindow.h"

using namespace Eloquent::Esp32cam::Motion;

Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::JpegDecoder decoder;
SimpleChange algorithm;
Detector detector(algorithm);
Localization::SlidingWindow localizer(detector);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.highQuality();
    cam.vga();
    cam.highestSaturation();

    // see 10_Image_Wise_Motion_Detection for details
    cam.disableAutomaticWhiteBalance();
    cam.disableAutomaticExposureControl();
    cam.disableGainControl();

    detector.trainFor(30);
    detector.retrainAfter(33ULL * 600);
    detector.triggerAbove(0.2);
    detector.denoise();

    algorithm.differBy(20);
    algorithm.smooth(0.9);
    algorithm.onlyUpdateBackground();


    /**
     * How many pixels of each sliding window should be foreground
     * to trigger the localization? (in percent, from 0 to 1)
     */
    localizer.setMinCoverage(0.7);
    /**
     * When we found a region that has the minimum converage,
     * shall we continue searching for a larger one or shall
     * we stop right away?
     */
    localizer.earlyStop();
    localizer.dontEarlyStop();


    while (!cam.begin())
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

    if (detector.triggered()) {
        Serial.println("Motion detected");

        /**
         * If motion was detected, try to localize where it happened.
         * This may fail if the motion is spread across the whole image
         * and not localized in a specific region
         */
        if (localizer.localize()) {
            Serial.print(" > localized at coordinates (top-left corner) ");
            Serial.print(localizer.x);
            Serial.print(", ");
            Serial.print(localizer.y);
            Serial.print(" in a rect of size ");
            Serial.print(localizer.width);
            Serial.print(" x ");
            Serial.print(localizer.height);
            Serial.print(" (coverage ");
            Serial.print(100 * localizer.coverage);
            Serial.print("%, processing time ");
            Serial.print(localizer.getExecutionTimeInMicros());
            Serial.println("us)");
        }
    }

    /**
     * After the call to `triggered()`, you can debug the internal
     * status of the detector if you want to find out why it triggered or not
     */
    Serial.println(detector.getTriggerStatus());
}