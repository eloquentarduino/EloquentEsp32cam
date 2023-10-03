/**
 * Example XXX: Convert JPEG frame to RGB565
 * This sketch shows the basic code you need to take a picture
 * from your Esp32 camera and convert to RGB565.
 *
 * RGB565 is required e.g. for FOMO or image processing.
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
    Serial.println("___CONVERT TO RGB565___");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
    camera.brownout.disable();
    camera.xclk.slow();
    camera.resolution.vga();
    camera.quality.high();

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

    if (!camera.rgb565.convert().isOk()) {
      Serial.println(camera.rgb565.exception.toString());
      return;
    }

    for (int i = 0; i < camera.rgb565.size; i++) {
        const uint16_t pixel_565 = camera.rgb565[i];
        Serial.print(pixel_565);
        Serial.print(',');
    }

    Serial.println();

    // you can convert to grayscale if needed 
    // (e.g. for FOMO)
    camera.rgb565.toGrayscale();

    for (int i = 0; i < camera.rgb565.size; i++) {
        const uint8_t pixel_gray = camera.rgb565[i];
        Serial.print(pixel_gray);
        Serial.print(',');
    }

    Serial.println();
    delay(1000);
}