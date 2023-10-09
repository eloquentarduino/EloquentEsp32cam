/**
 * Face detection
 * ONLY WORKS ON ESP32S3 camera
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/face/detection.h>

using namespace e;


void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___FACE DETECTION___");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
    camera.brownout.disable();
    // face detection only works at 240x240
    camera.resolution.face();
    camera.quality.high();

    // you can choose fast detection (50ms)
    face::detection.fast();
    // or accurate detection (80ms)
    face::detection.accurate();

    // Initialize the camera
    // If something goes wrong, print the error message
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera init OK");
    Serial.println("Awaiting for face...");

}


void loop() {
    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // run detection
    if (!face::detection.detect().isOk()) {
        Serial.println(face::detection.exception.toString());
        return;
    }

    // if face is found, perform action
    if (face::detection.found()) {
        Serial.printf("Face detected in %dms!\n", face::detection.benchmark.millis());

        // you can access the first detected face with
        int x = face::detection.first.x;
        int y = face::detection.first.y;
        
        Serial.printf(" > face #1 located at (x, y) = (%d, %d)\n", x, y);

        // you can print the coordinates of each face
        face::detection.forEach([](int i, face_t face) {
            Serial.printf(" > face #%d located at (x, y) = (%d, %d)\n", i + 1, face.x, face.y);

            // if you enabled accurate detection, you have access to the keypoints
            if (face.hasKeypoints()) {
                auto le = face.leftEye;
                auto re = face.rightEye;
                auto nose = face.nose;
                auto lm = face.leftMouth;
                auto rm = face.rightMouth;

                Serial.printf("   > left eye at %d, %d\n", le.x, le.y);
                Serial.printf("   > right eye at %d, %d\n", re.x, re.y);
                Serial.printf("   > nose at %d, %d\n", nose.x, nose.y);
                Serial.printf("   > left mouth at %d, %d\n", lm.x, lm.y);
                Serial.printf("   > right mouth at %d, %d\n", rm.x, rm.y);
            }
        });
    }
}