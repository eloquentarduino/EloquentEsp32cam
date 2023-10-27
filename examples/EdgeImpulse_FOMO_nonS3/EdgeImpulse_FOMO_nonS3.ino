/**
 * Run Edge Impulse FOMO model.
 * It works on both S3 and non-S3 boards.
 * 
 * The difference from the S3-only version
 * is that this only runs at 96x96 frames,
 * while S3-only version runs on VGA or higher
 * resolution.
 * 
 * The S3 version can be found in my
 * "Mastering the ESP32(S3) Camera" book
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#include <your-fomo-model.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>

using namespace eloq;


/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("__EDGE IMPULSE FOMO (NON-S3)__");

    // camera settings
    // replace with your own model!
    camera.pinout.aithinker();
    camera.brownout.disable();
    // non-S3 FOMO only works on 96x96 RGB565 images
    camera.resolution.yolo();
    camera.pixformat.rgb565();


    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Put object in front of camera");
}


void loop() {
    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // run FOMO
    if (!fomo.detectObjects(camera.frame).isOk()) {
      Serial.println(fomo.exception.toString());
      return;
    }

    // if no object is detected, return
    if (!fomo.foundAnyObject())
      return;

    // how many objects were found?
    Serial.printf(
      "Found %d object(s) in %dms\n", 
      fomo.count(),
      fomo.benchmark.millis()
    );

    // if you expect to find a single object, use fomo.first
    Serial.printf(
      "Found %s at (x, y) = (%d, %d) (size %d x %d)\n",
      fomo.first.label,
      fomo.first.x,
      fomo.first.y,
      fomo.first.width,
      fomo.first.height
    );

    // if you expect to find many objects, use fomo.forEach
    fomo.forEach([](int i, bbox_t bbox) {
      Serial.printf(
        "#%d) Found %s at (x, y) = (%d, %d) (size %d x %d)\n",
        i,
        bbox.label,
        bbox.x,
        bbox.y,
        bbox.width,
        bbox.height
      );
    });
}