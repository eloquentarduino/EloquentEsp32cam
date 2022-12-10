// 17_Line_Crossing_Counter.ino

#define MAX_RESOLUTION_VGA

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/motion/Detector.h"
#include "esp32cam/motion/SimpleChange.h"
#include "esp32cam/apps/LineCrossingCounter.h"

using namespace Eloquent::Esp32cam;

Cam cam;
JpegDecoder decoder;
Motion::SimpleChange algorithm;
Motion::Detector detector(algorithm);
Applications::LineCrossingCounter counter(detector);

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
     * You can use absolute numbers to give the exact coordinate
     * or relative numbers (between 0 and 1) to given percent
     * values relative to the width.
     *
     * Assuming a VGA frame (640 x 480), the following two lines
     * do the same thing
     */
    counter.lineAt(640 / 2);
    counter.lineAt(0.5);
    /**
     * Limit analysis to region above given coordinate
     * 0 is bottom of the image
     */
    counter.above(0);
    /**
    * Limit analysis to region below given coordinate
    * 0 is bottom of the image.
    *
    * You can use absolute numbers to give the exact coordinate
    * or relative numbers (between 0 and 1) to given percent
    * values relative to the width.
    *
    * Assuming a VGA frame (640 x 480), the following two lines
    * do the same thing
    */
    counter.below(240);
    counter.below(0.5);
    /**
     * To detect crossing, the software defines "transition zones"
     * immediately before and after the line you set (3 before, 3 after).
     * You can specify how wide these zones should be.
     *
     * Decrease this number to catch smaller objects
     */
    counter.wideness(4);
    /**
     * The greater this value, the more time it is allowed
     * for an object to move across different "trnasition zones".
     *
     * Increase this value to catch slower transitions.
     */
    counter.lag(3);
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