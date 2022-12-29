#define MAX_RESOLUTION_VGA

#include "esp32cam.h"

Eloquent::Esp32cam::Cam cam;


/**
 *
 */
void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    /**
     * Configure camera model
     * Allowed values are:
     *  - aithinker()
     *  - m5()
     *  - m5wide()
     *  - eye()
     *  - wrover()
     */
    cam.aithinker();

    /**
     * Configure camera resolution
     * Allowed values are:
     * - _96x96()
     * - qqvga()
     * - qcif()
     * - hqvga()
     * - _240x240()
     * - qvga()
     * - cif()
     * - hvga()
     * - vga()
     * - svga()
     * - xga()
     * - hd()
     * - sxga()
     * - uxga()
     */
    cam.vga();

    /**
     * Configure JPEG quality
     * Allowed values are:
     *  - lowQuality()
     *  - highQuality()
     *  - bestQuality()
     *  - setQuality(quality), ranging from 10 (best) to 64 (lowest)
     *
     *  Default is highQuality
     */
    cam.highQuality();

    /**
     * Initialize the camera
     * If something goes wrong, print the error message
     */
    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    Serial.println("Camera OK");
}

/**
 *
 */
void loop() {
    /**
     * Try to capture a frame
     * If something goes wrong, print the error message
     */
    if (!cam.capture()) {
        Serial.println(cam.getErrorMessage());
        return;
    }

    Serial.print("Jpeg size: ");
    Serial.println(cam.getFileSize());

    /**
     * Do whatever you want with the captured frame.
     * See next examples.
     */
}