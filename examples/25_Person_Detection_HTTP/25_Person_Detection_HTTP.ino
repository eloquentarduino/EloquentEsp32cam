// 25_Person_Detection_HTTP.ino
#define MAX_RESOLUTION_1024x768

/**
 * Run a development HTTP server to see live
 * the person detection results.
 * You need at least 8 MB of DRAM
 */

#include "esp32cam.h"
#include "esp32cam/http/PersonDetectorHTTP.h"

using namespace Eloquent::Esp32cam;


Cam cam;
Applications::PersonDetector person(cam);
Http::PersonDetectorHTTP http(person);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.grayscale();
    cam.xga();
    cam.highQuality();
    person.minConfidence(190);

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!person.begin()) {
        Serial.print("Detector init error: ");
        Serial.println(person.getErrorMessage());
    }

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