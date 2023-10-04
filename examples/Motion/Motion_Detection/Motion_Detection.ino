/**
 * Example: Motion detection
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
    motion_detection.stride(4);
    // the higher the threshold, the lesser sensitivity
    // (at pixel level)
    motion_detection.threshold(5);
    // the higher the threshold, the lesser sensitivity
    // (at image level)
    motion_detection.ratio(0.2);
    // optionally, you can enable rate limiting (aka debounce)
    // motion won't trigger more often than the specified frequency
    motion_detection.rate_limiter.atMostOnceEvery(5).seconds();

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

    // detect motion
    if (!motion_detection.update(camera.rgb565).isOk()) {
      Serial.println(motion_detection.exception.toString());
      return;
    }

    // on motion, perform action
    if (motion_detection.triggered())
      Serial.println("Motion detected");
}