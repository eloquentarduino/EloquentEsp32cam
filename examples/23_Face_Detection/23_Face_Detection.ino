// 23_Face_Detection.ino

#include "esp32cam.h"
#include "esp32cam/apps/FaceDetector.h"

using namespace Eloquent::Esp32cam;


Cam cam;
Applications::FaceDetector faces(cam);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.highQuality();
    cam.highestSaturation();
    cam.qcif();

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
        Serial.print("Detected ");
        Serial.print(faces.numFaces());
        Serial.println(" faces");
        Serial.print("Face detection took ");
        Serial.print(faces.getExecutionTimeInMillis());
        Serial.println("ms");

        // print center coordinates of each face
        faces.forEach([](int i, Applications::FaceBBox bbox) {
            Serial.print("Face center at <x, y> = <");
            Serial.print(bbox.cx);
            Serial.print(", ");
            Serial.print(bbox.cy);
            Serial.println(">");

            // you have access to the following attributes
            //  bbox.x (alias for .left)
            //  bbox.y (alias for .top)
            //  bbox.left
            //  bbox.right
            //  bbox.top
            //  bbox.bottom
            //  bbox.width
            //  bbox.height
            //  bbox.cx
            //  bbox.cy
        });

        // you can also access a specific face by index
        if (faces.select(0)) {
            //Serial.print("Face 0 center at <x, y> = <");
            //Serial.print(faces.bbox.cx);
            //Serial.print(", ");
            //Serial.print(faces.bbox.cy);
            //Serial.println(">");
        }
    }
}
