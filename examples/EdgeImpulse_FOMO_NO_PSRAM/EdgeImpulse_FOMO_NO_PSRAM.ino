/**
 * Run Edge Impulse FOMO model.
 * It works on both PSRAM and non-PSRAM boards.
 * 
 * The difference from the PSRAM version
 * is that this sketch only runs on 96x96 frames,
 * while PSRAM version runs on higher resolutions too.
 * 
 * The PSRAM version can be found in my
 * "ESP32S3 Camera Mastery" course
 * at https://dub.sh/ufsDj93
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#include <your-fomo-project_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>

using eloq::camera;
using eloq::ei::fomo;


/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("__EDGE IMPULSE FOMO (NO-PSRAM)__");

    // camera settings
    // replace with your own model!
    camera.pinout.aithinker();
    camera.brownout.disable();
    // NON-PSRAM FOMO only works on 96x96 (yolo) RGB565 images
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
    if (!fomo.run().isOk()) {
      Serial.println(fomo.exception.toString());
      return;
    }

    // how many objects were found?
    Serial.printf(
      "Found %d object(s) in %dms\n", 
      fomo.count(),
      fomo.benchmark.millis()
    );

    // if no object is detected, return
    if (!fomo.foundAnyObject())
      return;

    // if you expect to find a single object, use fomo.first
    Serial.printf(
      "Found %s at (x = %d, y = %d) (size %d x %d). "
      "Proba is %.2f\n",
      fomo.first.label,
      fomo.first.x,
      fomo.first.y,
      fomo.first.width,
      fomo.first.height,
      fomo.first.proba
    );

    // if you expect to find many objects, use fomo.forEach
    if (fomo.count() > 1) {
      fomo.forEach([](int i, bbox_t bbox) {
        Serial.printf(
          "#%d) Found %s at (x = %d, y = %d) (size %d x %d). "
          "Proba is %.2f\n",
          i + 1,
          bbox.label,
          bbox.x,
          bbox.y,
          bbox.width,
          bbox.height,
          bbox.proba
        );
      });
    }
}