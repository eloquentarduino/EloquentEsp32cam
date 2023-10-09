/**
 * Motion detection
 * Detect when the frame changes by a reasonable amount
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/motion/detection.h>

using namespace e;


/**
 *
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___MOTION DETECTION___");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // configure motion detection
    // the higher the stride, the faster the detection
    // the higher the stride, the lesser granularity
    motion::detection.stride(4);
    // the higher the threshold, the lesser sensitivity
    // (at pixel level)
    motion::detection.threshold(5);
    // the higher the threshold, the lesser sensitivity
    // (at image level)
    motion::detection.ratio(0.2);
    // optionally, you can enable rate limiting (aka debounce)
    // motion won't trigger more often than the specified frequency
    motion::detection.rate.atMostOnceEvery(5).seconds();

    // Initialize the camera
    // If something goes wrong, print the error message
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera init OK");
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

    // convert JPEG to RGB565
    if (!camera.rgb565.convert().isOk()) {
        Serial.println(camera.rgb565.exception.toString());
        return;
    }

    // run motion detection
    if (!motion::detection.update(camera.rgb565).isOk()) {
        Serial.println(motion::detection.exception.toString());
        return;
    }

    // on motion, perform action
    if (motion::detection.triggered())
        Serial.println("Motion detected");
}