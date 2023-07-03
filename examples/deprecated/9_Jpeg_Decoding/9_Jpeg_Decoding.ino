/**
 * Example 9: Decode JPEG image into YCbCr
 *
 * This sketch shows how to extract Y (luma), Cb (blue component)
 * and Cr (red component) from a JPEG picture.
 *
 * IMPORTANT NOTE: for memory reasons, the JPEG decoder returns
 * a 1/8th version of the image!
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */
#include "esp32camera.h"
#include "esp32camera/components/ColorJpeg.h"


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    camera.model.aithinker();
    camera.quality.high();
    camera.resolution.qvga();

    // Init camera
    while (!camera.begin())
        Serial.println(camera.getErrorMessage());

    Serial.println("Camera OK");
}


void loop() {
    // capture frame
    if (!camera.capture()) {
        Serial.println(camera.getErrorMessage());
        return;
    }

    // decode jpeg
    if (!jpeg.decode()) {
        Serial.println(jpeg.getErrorMessage());
        return;
    }

    // we can access the YCbCr channels at
    //
    // - gray (luma)
    // - blue
    // - red
    //
    // For each of them, you have access to
    //   - pixels for the uint8_t pixel values
    //   - length for the size of the array
    //   - width
    //   - height
    //
    // Remeber that the JPEG decoder extracts a 1/8th
    // version of the original image!
    Serial.printf("Gray (luma) length: %lu\n", jpeg.gray.length);
    Serial.printf("Decoding time: %lums\n", jpeg.benchmark.inMillis());
    delay(1000);
}