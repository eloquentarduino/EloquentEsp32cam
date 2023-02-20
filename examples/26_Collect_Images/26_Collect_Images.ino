// 26_Collect_Images.ino
#define MAX_RESOLUTION_XGA 1

/**
 * Run a development HTTP server to capture images
 * for TinyML tasks
 */

#include "esp32cam.h"
#include "esp32cam/http/FomoImageCollectionServer.h"

using namespace Eloquent::Esp32cam;


Cam cam;
Http::FOMO::CollectImagesServer http(cam);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    /**
     * Replace with your camera model.
     * Available: aithinker, m5, m5wide, wrover, eye, ttgoLCD
     */
    cam.aithinker();
    cam.highQuality();
    cam.highestSaturation();
    cam.xga();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    // replace with your SSID and PASSWORD
    while (!cam.connect("SSID", "PASSWORD"))
        Serial.println(cam.getErrorMessage());

    while (!http.begin())
        Serial.println(http.getErrorMessage());

    Serial.println(http.getWelcomeMessage());
    cam.mDNS("esp32cam");
}


void loop() {
    http.handle();
}