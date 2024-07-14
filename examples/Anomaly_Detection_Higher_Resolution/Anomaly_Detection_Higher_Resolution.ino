/**
 * Run anomaly detection at low resolution.
 * On anomaly, capture frame at higher resolution
 * for SD storage.
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
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
    Serial.println("___ANOMALY DETECTION + SWITCH RESOLUTION___");

    // camera settings
    // replace with your own model!
    camera.pinout.freenove_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // see example of anomaly detection for config values
    detection.skip(5);
    detection.stride(1);
    detection.threshold(5);
    // the higher the detectionRatio, the less the sensitivity
    // (at image level, from 0 to 1)
    detection.detectionRatio(0.5);
    // the higher the referenceRatio, the more the reference image can change over time
    // (at image level, from 0 to 1)
    detection.referenceRatio(0.2);

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Awaiting for anomaly...");
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

    // run anomaly detection
    if (!detection.run().isOk()) {
        Serial.println(detection.exception.toString());
        return;
    }

    // on anomaly, perform action
    if (detection.triggered()) {
        Serial.printf(
          "Anomaly of %.2f detected on frame of size %dx%d (%d bytes)\n",
		  detection.movingRatio,
          camera.resolution.getWidth(),
          camera.resolution.getHeight(),
          camera.getSizeInBytes()
        );

        Serial.println("Taking photo of anomaly at higher resolution");

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