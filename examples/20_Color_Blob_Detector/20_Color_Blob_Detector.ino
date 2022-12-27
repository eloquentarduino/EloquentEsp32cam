// 20_Color_Blob_Detector.ino

/**
 * Detect blobs of given color
 */
#define MAX_RESOLUTION_VGA
#define MAX_RECURSION_DEPTH 13

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/apps/ColorBlobDetector.h"

using namespace Eloquent::Esp32cam;

Cam cam;
JpegDecoder decoder;
Applications::ColorBlobDetector detector(8*16, 7*16, 4*16); // yellow-ish


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.highQuality();
    cam.vga();
    cam.highestSaturation();
    cam.disableAutomaticWhiteBalance();
    cam.disableAutomaticExposureControl();
    cam.disableGainControl();

    /**
     * Set detector tollerance
     * The higher, the more shade of colors it will pick
     */
    detector.tollerate(16);
    /**
     * Skip blob localization (slow) if not enough
     * pixels match color
     */
    detector.setMinArea(30 * 30);

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());
}


void loop() {
    if (!cam.capture()) {
        Serial.println(cam.getErrorMessage());
        return;
    }

    if (!decoder.decode(cam)) {
        Serial.println(decoder.getErrorMessage());
        return;
    }

    /**
     * Detect blob in frame
     */
    if (detector.detect(decoder)) {
        Serial.print("Blob detected from top-left ");
        Serial.print(detector.blob.top);
        Serial.print(", ");
        Serial.print(detector.blob.left);
        Serial.print(" to bottom-right ");
        Serial.print(detector.blob.bottom);
        Serial.print(", ");
        Serial.println(detector.blob.right);
        Serial.print("Blob detection run in ");
        Serial.print(detector.getExecutionTimeInMillis());
        Serial.println("ms");
    }
    else {
        Serial.println(detector.getErrorMessage());
    }

    // while debugging, these may turn out useful
    Serial.print(detector.maskCount);
    Serial.println(" pixels match target color");
    Serial.println(detector.toString());
    detector.printTo(Serial);


}