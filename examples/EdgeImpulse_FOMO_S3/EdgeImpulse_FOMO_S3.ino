/**
 * Edge Impulse's FOMO object detection on ESP32S3
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#include <your-ei-fomo-library.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo_s3.h>

// all esp32camera objects (e.g. `camera`) 
// are scoped under the `e` namespace
using namespace e;


/**
 *
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___EDGE IMPULSE FOMO___");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // initialize camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera init OK");
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

    // convert jpeg to raw pixels
    if (!camera.rgb565.convert().isOk()) {
        Serial.println(camera.rgb565.exception.toString());
        return;
    }

    // run FOMO
    if (!fomo.detectObjects(camera.rgb565).isOk()) {
      Serial.println(fomo.exception.toString());
      return;
    }

    // no object detected, go to next loop
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
      "Object of class %d detected at (x, y) = (%d, %d) with size %d x %d\n",
      fomo.first.value,
      fomo.first.x,
      fomo.first.y,
      fomo.first.width,
      fomo.first.height
    );

    // if you expect to find many objects, use fomo.forEach
    fomo.forEach([](int i, bbox_t bbox) {
      Serial.printf(
        "#%d Object of class %d detected at (x, y) = (%d, %d) with size %d x %d\n",
        i,
        bbox.value,
        bbox.x,
        bbox.y,
        bbox.width,
        bbox.height
      );
    });
}