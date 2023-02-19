// 19_Live_YCbCr.ino

/**
 * Create an HTTP web server to debug Y, Cb, Cr components
 * of image in real-time
 */

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/http/YCbCrHTTP.h"

using namespace Eloquent::Esp32cam;

Cam cam;
JpegDecoder decoder;
Http::YCbCrHTTP feed(cam, decoder);


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

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!cam.connect("SSID", "PASSWORD"))
        Serial.println(cam.getErrorMessage());

    while (!feed.begin())
        Serial.println(feed.getErrorMessage());

    if (!cam.viewAt("esp32cam"))
        Serial.println("Cannot configure mDNS");

    Serial.println(feed.getWelcomeMessage());
}


void loop() {
    feed.handle();
}