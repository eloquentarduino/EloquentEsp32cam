// 22_Person_Detection.ino

#include "esp32cam.h"
#include "esp32cam/apps/PersonDetector.h"

using namespace Eloquent::Esp32cam;

Cam cam;
Applications::PersonDetector detector(cam);

/**
 * Detect people in frame.
 * Requires EloquentTinyML library version 2.4.4
 */
void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    // person detection requires grayscale image
    cam.grayscale();
    // person detection requires a 96x96 image
    // since the JPEG decoder downscales the imabe by 8
    // we need XGA resolution (1024x768)
    cam.xga();
    cam.highQuality();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    // configure a threshold for "robust" person detection
    // if no threshold is set, "person" would be detected everytime
    // person_score > not_person_score, even if just by 1
    // by trial and error, considering that scores range from 0 to 255,
    // a threshold of 190-200 dramatically reduces the number of false positives
    detector.minConfidence(190);

    // abort if an error occurred on the detector
    while (!detector.begin()) {
        Serial.print("Detector init error: ");
        Serial.println(detector.getErrorMessage());
    }
}

void loop() {
    if (!cam.capture()) {
        Serial.println(cam.getErrorMessage());
        return;
    }

    // perform detection
    // it takes ~5 seconds per frame
    if (!detector.detect()) {
        Serial.println(detector.getErrorMessage());
        return;
    }

    Serial.println(detector.isPerson ? "Person detected" : "No person detected");
    Serial.print("\t > It took ");
    Serial.print(detector.getExecutionTimeInMillis());
    Serial.println("ms to detect");
    Serial.print("\t > Person score: ");
    Serial.println(detector.person.getPersonScore());
    Serial.print("\t > Not person score: ");
    Serial.println(detector.person.getNotPersonScore());
}