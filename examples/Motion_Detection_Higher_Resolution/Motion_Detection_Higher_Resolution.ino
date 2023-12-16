/**
 * Run motion detection at low resolution.
 * On motion, capture frame at higher resolution
 * for SD storage.
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
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
    Serial.println("___MOTION DETECTION + SWITCH RESOLUTION___");

    // camera settings
    // replace with your own model!
    camera.pinout.freenove_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // see example of motion detection for config values
    detection.skip(5);
    detection.stride(1);
    detection.threshold(5);
    detection.ratio(0.2);
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
    if (detection.triggered()) {
        Serial.printf(
          "Motion detected on frame of size %dx%d (%d bytes)\n",
          camera.resolution.getWidth(),
          camera.resolution.getHeight(),
          camera.getSizeInBytes()
        );

        Serial.println("Taking photo of motion at higher resolution");

        camera.resolution.at(FRAMESIZE_UXGA, []() {
          Serial.printf(
            "Switched to higher resolution: %dx%d. It took %d ms to switch\n",
            camera.resolution.getWidth(),
            camera.resolution.getHeight(),
            camera.resolution.benchmark.millis()
          );

          camera.capture();
          
          Serial.printf(
            "Frame size is now %d bytes\n", 
            camera.getSizeInBytes()
          );

          // save to SD...
        });

        Serial.println("Resolution switched back to VGA");
    }
}