/**
 * Get your first picture with ESP32
 *
 * Open the Serial Monitor and enter 'capture' (without quotes)
 * to capture a new image
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#include <eloquent_esp32cam.h>

// all global objects (e.g. `camera`) 
// are scoped under the `eloq` namespace
using eloq::camera;


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
    // - freenove_s3
    // - xiao
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
    Serial.println("Enter 'capture' (without quotes) to shot");
}

/**
 *
 */
void loop() {
    // await for Serial command
    if (!Serial.available())
        return;

    if (Serial.readStringUntil('\n') != "capture") {
        Serial.println("I only understand 'capture'");
        return;
    }

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

    Serial.println("Enter 'capture' (without quotes) to shot");
}