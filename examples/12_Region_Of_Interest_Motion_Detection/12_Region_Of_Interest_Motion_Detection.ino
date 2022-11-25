// 12_Region_Of_Interest_Motion_Detection.ino

#define MAX_RESOLUTION_VGA

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/motion/Detector.h"
#include "esp32cam/motion/SimpleChange.h"
// include rect and circle Region of Interest
#include "esp32cam/motion/roi/Rect.h"
#include "esp32cam/motion/roi/Circle.h"


Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::JpegDecoder decoder;
Eloquent::Esp32cam::Motion::SimpleChange algorithm;
Eloquent::Esp32cam::Motion::Detector detector(algorithm);

/**
 * Create a rect region
 *
 * Parameters are x, y, width, height, changes treashold
 * Since this is a specific region, the threshold can be
 * pretty high (0.5 == 50% of area)
 */
Eloquent::Esp32cam::Motion::RoI::Rect rect1(0, 0, 100, 100, 0.5);

/**
 * Alternatively, you can create a named struct for clarity
 */
struct {
    int x = 0;
    int y = 0;
    int width = 100;
    int height = 100;
    float threshold = 0.5;
} r;
Eloquent::Esp32cam::Motion::RoI::Rect rect2(r);

/**
 * Create a circle region
 *
 * Parameters are x, y, radius, changes threshold
 * When using float values, they're interpreted as percent
 * values relative to the width and height of the frame.
 * In the following line, the circle center matches the
 * frame center.
 */
Eloquent::Esp32cam::Motion::RoI::Circle circle1(0.5, 0.5, 50, 0.5);

/**
 * Alternatively, you can create a named struct for clarity
 */
struct {
    float x = 0.5;
    float y = 0.5;
    int radius = 50;
    float threshold = 0.5;
} c;
Eloquent::Esp32cam::Motion::RoI::Circle circle2(c);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.highQuality();
    cam.vga();
    cam.highestSaturation();
    cam.disableAutomaticWhiteBalance();
    cam.disableAutomaticExposureControl();
    cam.disableGainControl();

    // See 10_Image_Wise_Motion_Detection for details
    detector.trainFor(30);
    detector.retrainAfter(33ULL * 600);
    detector.triggerAbove(0.1);
    detector.denoise();

    algorithm.differBy(10);
    algorithm.smooth(0.9);
    algorithm.onlyUpdateBackground();


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
     * Test if motion was detected in rect
     */
    if (detector.triggeredIn(rect1)) {
        Serial.println("Motion detected in rect region");
    }

    /**
     * Test if motion was detected in circle
     */
    if (detector.triggeredIn(circle1)) {
        Serial.println("Motion detected in circle region");
    }

    /**
     * After the call to `triggeredIn()`, you can debug the internal
     * status of the region of interest if you want to find out why it triggered or not
     */
    Serial.print("rect status: ");
    Serial.println(rect1.getTriggerStatus());
    Serial.print("circle status: ");
    Serial.println(circle1.getTriggerStatus());
}