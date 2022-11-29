// 16_Face_Detect.ino

#include "esp32cam.h"
#include "esp32cam/FaceDetection.h"


Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::FaceDetection faces(cam);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.m5wide();
    cam.highQuality();
    cam.qqvga();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!faces.begin())
        Serial.println(faces.getErrorMessage());

    Serial.println("Initialized");
    delay(2000);
}


void loop() {
    if (!cam.capture()) {
        Serial.println(cam.getErrorMessage());
        return;
    }

    if (faces.detect()) {
        Serial.print("Face detected in ");
        Serial.print(faces.getExecutionTimeInMillis());
        Serial.println("ms");
    }
}
