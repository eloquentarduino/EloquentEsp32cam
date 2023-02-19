// 26_Collect_Images.ino
#define MAX_RESOLUTION_XGA 1

/**
 * Run a development HTTP server to capture images
 * for TinyML tasks
 */

#include "esp32cam.h"
#include "esp32cam/http/api/ApiServer.h"
#include "esp32cam/http/api/GetJpeg.h"
#include "esp32cam/http/api/GetGrayscale.h"

using namespace Eloquent::Esp32cam;


Cam cam;
Http::Api::ApiServer api;
Http::Api::GetJpeg getJpeg(cam);
Http::Api::GetGrayscale getGrayscale(cam);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.m5wide();
    cam.highQuality();
    cam.highestSaturation();
    cam.xga();

    api.add(getJpeg);
    api.add(getGrayscale);

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!cam.connect("SSID", "PASSWORD"))
        Serial.println(cam.getErrorMessage());

    while (!api.begin())
        Serial.println(api.getErrorMessage());

    Serial.print("Camera IP address: ");
    Serial.println(cam.getAddress());
    Serial.println(api.getWelcomeMessage());
    cam.mDNS("esp32cam");
}


void loop() {
    api.handle();
}