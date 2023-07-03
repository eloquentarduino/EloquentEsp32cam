// 24_Face_Detection_HTTP.ino

/**
 * Run a development HTTP server to see live
 * the face detection results
 */

#include "esp32cam.h"
#include "esp32cam/http/FaceDetectorHTTP.h"

using namespace Eloquent::Esp32cam;


Cam cam;
Applications::FaceDetector faces(cam);
Http::FaceDetectorHTTP http(faces);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.bestQuality();
    cam.highestSaturation();
    cam.qvga();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!faces.begin())
        Serial.println(faces.getErrorMessage());

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