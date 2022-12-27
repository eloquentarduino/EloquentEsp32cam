// 21_Color_Blob_Detection_HTTP.ino

/**
 * Run an HTTP server to debug the ColorBlobDetector
 */
#define MAX_RESOLUTION_VGA
#define MAX_RECURSION_DEPTH 13

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/apps/ColorBlobDetector.h"
#include "esp32cam/http/ColorBlobDetectorHTTP.h"

using namespace Eloquent::Esp32cam;

Cam cam;
JpegDecoder decoder;
Applications::ColorBlobDetector detector(0, 0, 0); // will configure from the URL
Http::ColorBlobDetectorHTTP http(cam, decoder, detector);



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

    while (!cam.connect("SSID", "PASSWORD"))
        Serial.println(cam.getErrorMessage());

    while (!http.begin())
        Serial.println(http.getErrorMessage());

    cam.mDNS("esp32cam");
    Serial.println(http.getWelcomeMessage());
}


void loop() {
    http.handle();
}