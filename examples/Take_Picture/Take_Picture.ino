/**
 * Example 3: Get your First picture
 * This sketch shows the basic code you need to take a picture
 * from your Esp32 camera.
 *
 * Open the Serial Monitor and enter 'capture' (without quotes)
 * to capture a new image
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
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
    // supported models:
    // - aithinker
    // - m5
    // - m5_wide
    // - m5_timer
    // - eye
    // - wrover
    // - wroom_s3
    // - freenove_camera_s3
    // - xiao_s3
    // - ttgo_lcd
    // - simcam
    camera.pinout.aithinker();
    camera.brownout.disable();
    // supported resolutions
    // - yolo (96x96)
    // - qqvga
    // - qcif
    // - face (240x240)
    // - qvga
    // - cif
    // - hvga
    // - vga
    // - svga
    // - xga
    // - hd
    // - sxga
    // - uxga
    // - fhd
    // - qxga
    // ...
    camera.resolution.vga();
    // supported qualities:
    // - low
    // - high
    // - best
    camera.quality.high();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
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

    delay(1000);
}