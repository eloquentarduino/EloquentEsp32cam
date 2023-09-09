/**
 * Example 3: Get your First picture
 * This sketch shows the basic code you need to take a picture
 * from your Esp32 camera.
 *
 * Open the Serial Monitor and enter 'capture' (without quotes)
 * to capture a new image
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */
#include "esp32camera.h"

// all esp32camera objects (e.g. `camera`) 
// are scoped under the `e` namespace
using namespace e;


/**
 *
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___GET YOUR FIRST PICTURE___");

    // camera settings
    // replace with your own model!
    camera.pinout.xiao_s3();
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

    // print image info
    Serial.printf(
        "JPEG size in bytes: %d. Width: %dpx. Height: %dpx.\n",
        camera.getSizeInBytes(),
        camera.resolution.getWidth(),
        camera.resolution.getHeight()
    );
}