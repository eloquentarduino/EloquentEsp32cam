/**
 * Anomaly detection
 * Detect when the frame changes by a reasonable amount
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/anomaly/detection.h>

using eloq::camera;
using eloq::anomaly::detection;

/**
 *
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___ANOMALY DETECTION___");

    // camera settings
    // replace with your own model!
    camera.pinout.xiao();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // configure anomaly detection
    detection.skip(4);
    // the higher the stride, the faster the detection
    // the higher the stride, the less the granularity
    detection.stride(1);
    // the higher the threshold, the less the sensitivity
    // (at pixel level)
    detection.threshold(5);
    // the higher the detectionRatio, the less the sensitivity
    // (at image level, from 0 to 1)
    detection.detectionRatio(0.5);
    // the higher the referenceRatio, the more the reference image can change over time
    // (at image level, from 0 to 1)
    detection.referenceRatio(0.2);
    // optionally, you can enable rate limiting (aka debounce)
    // anomaly won't trigger more often than the specified frequency
    //detection.rate.atMostOnceEvery(5).seconds();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Awaiting anomaly...");
}

/**
 *
 */
void loop() {

    // Don't run more often than the time for an anomaly to come into view as the reference image can 'drift' away from 'normal'
    delay(1000);
    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }
    float anomalyProbability;
    
    // run anomaly detection
        Serial.println(detection.exception.toString());
        return;
    }

    // on anomaly, perform action
    if (detection.triggered()) {
        Serial.print("Anomaly detected: "); Serial.println(detection.movingRatio);
    }

