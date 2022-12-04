// 16_Line_Crossing_Counter.ino

#define MAX_RESOLUTION_VGA

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/motion/Detector.h"
#include "esp32cam/motion/SimpleChange.h"
#include "esp32cam/apps/LineCrossingCounter.h"


Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::JpegDecoder decoder;
Eloquent::Esp32cam::Motion::SimpleChange algorithm;
Eloquent::Esp32cam::Motion::Detector detector(algorithm);
Eloquent::Esp32cam::Applications::LineCrossingCounter counter(detector);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    cam.aithinker();
    cam.highQuality();
    cam.vga();
    cam.disableAutomaticWhiteBalance();
    cam.disableAutomaticExposureControl();
    cam.disableGainControl();

    // see 10_Image_Wise_Motion_Detection for details
    detector.trainFor(30);
    detector.retrainAfter(33ULL * 600);
    detector.triggerAbove(0.2);
    detector.denoise();

    algorithm.differBy(20);
    algorithm.smooth(0.9);
    algorithm.onlyUpdateBackground();

    /**
     * At which x-coordinate is the vertical line to cross?
     */
    counter.lineAt(640 / 2);
    /**
     * Limit analysis to region above given coordinate
     * 0 is bottom of the image
     */
    counter.above(0);
    /**
    * Limit analysis to region below given coordinate
    * 0 is bottom of the image
    */
    counter.below(200);
    /**
     * Increase this value to accomodate slower transitions
     */
    counter.lag(3);
    /**
     * Decrease this number to catch smaller objects
     */
    counter.sparsity(4);
    /**
     * After an event, don't trigger for the
     * next given seconds
     */
    counter.debounceSeconds(3);


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

    /**
     * Update counter algorithm
     */
    if (!counter.update()) {
        Serial.println(counter.getErrorMessage());
        return;
    }

    if (counter.crossedFromLeftToRight()) {
        Serial.println("Crossing from left to right.");
        Serial.print(counter.getLeftToRightCount());
        Serial.println(" objects so far in this direction.");
    }
    else if (counter.crossedFromRightToLeft()) {
        Serial.println("Crossing from right to left.");
        Serial.print(counter.getRightToLeftCount());
        Serial.println(" objects so far in this direction.");
    }
    else {
        // when debugging, it may be useful to turn these on
        //Serial.println(counter.debug());
        //detector.triggered();
        //Serial.println(detector.getTriggerStatus());
    }
}