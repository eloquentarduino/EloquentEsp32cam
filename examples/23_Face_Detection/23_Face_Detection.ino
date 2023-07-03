// 23_Face_Detection.ino

#include "esp32camera.h"
#include "esp32camera/tinyml/FaceDetection.h"



void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    camera.model.aithinker();
    camera.resolution.qcif();
    camera.quality.high();

    while (!camera.begin())
        Serial.println(camera.getErrorMessage());

    while (!faceDetection.begin())
        Serial.println(faceDetection.getErrorMessage());

    Serial.println("Camera OK");
    Serial.println("FaceDetection OK");
    Serial.println("Point the camera at your face...");
    delay(2000);
}


void loop() {
    if (!camera.capture()) {
        Serial.println(camera.getErrorMessage());
        return;
    }

    if (faceDetection.detect()) {
        Serial.printf(
            "Detected %d faces in %d ms\n",
            faceDetection.numFaces(),
            faceDetection.getExecutionTimeInMillis()
        );

        // print center coordinates of each face
        faces.forEach([](int i, Eloquent::Esp32cam::TinyML::FaceBBox bbox) {
            Serial.printf(
                "Face center at <x, y> = <%d, %d>\n",
                bbox.cx,
                bbox.cy
            );

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
        if (faceDetection.select(0)) {
            Serial.printf(
                "Face #0 center at <x, y> = <%d, %d>\n",
                faceDetection.bbox.cx,
                faceDetection.bbox.cy
            );
        }
    }
}
