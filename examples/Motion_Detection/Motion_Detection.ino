/**
 * Motion detection
 * Detect when the frame changes by a reasonable amount
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/motion/detection.h>

using eloq::camera;
using eloq::motion::detection;


/**
 *
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___MOTION DETECTION___");

    // camera settings
    // replace with your own model!
    camera.pinout.freenove_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // configure motion detection
    // the higher the stride, the faster the detection
    // the higher the stride, the lesser granularity
    detection.stride(1);
    // the higher the threshold, the lesser sensitivity
    // (at pixel level)
    detection.threshold(5);
    // the higher the threshold, the lesser sensitivity
    // (at image level, from 0 to 1)
    detection.ratio(0.2);
    // optionally, you can enable rate limiting (aka debounce)
    // motion won't trigger more often than the specified frequency
    detection.rate.atMostOnceEvery(5).seconds();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Awaiting for motion...");
}

/**
 *
 */
void loop() {
    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // run motion detection
    if (!detection.run().isOk()) {
        Serial.println(detection.exception.toString());
        return;
    }

    // on motion, perform action
    if (detection.triggered())
        Serial.println("Motion detected!");
}