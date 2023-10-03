/**
 * Example: Edge Impulse's FOMO object detection
 * This sketch shows how to run a FOMO model on the ESP32
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */
#define EI_CLASSIFIER_TFLITE_ENABLE_ESP_NN 1
#include <your-ei-fomo-model-here.h>
#include <esp32camera.h>
#include <esp32camera/edgeimpulse/fomo.h>

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
    camera.xclk.slow();
    camera.resolution.vga();
    camera.quality.high();
    camera.rateLimit.atMost(33).fps();

    /**
     * Initialize the camera
     * If something goes wrong, print the error message
     */
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

    // run FOMO model
    if (!fomo.detectObjects().isOk()) {
        Serial.println(fomo.exception.toString());
        return;
    }

    ESP_LOGI("App", "FOMO inference took %dms", fomo.benchmark.millis());

    // check if any object was detected
    if (!fomo.hasObjects())
        return;

    // print each object's bbox to serial
    // (replace with your own application logic!)
    fomo.forEach([](size_t ix, ei_impulse_result_bounding_box_t bbox) {
        Serial.printf(
            " > BBox of label %s at (x, y) = (%d, %d)\n", 
            bbox.label, 
            bbox.x,
            bbox.y
        );

        // you also have access to bbox.width and bbox.height
    });
  
    Serial.println();
    delay(1000);
}