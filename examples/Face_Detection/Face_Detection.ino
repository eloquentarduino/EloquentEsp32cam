/**
 * Face detection
 * ONLY WORKS ON ESP32S3
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/face/detection.h>

using eloq::camera;
using eloq::face_t;
using eloq::face::detection;


/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___FACE DETECTION___");

    // camera settings
    // !!!!REPLACE WITH YOUR OWN MODEL!!!!
    camera.pinout.freenove_s3(); // e.g. xiao(), lilygo_tcamera_s3(), ...
    camera.brownout.disable();
    // face detection only works at 240x240
    camera.resolution.face();
    camera.quality.high();

    // you can choose fast detection (50ms)
    detection.fast();
    // or accurate detection (80ms)
    detection.accurate();

    // you can set a custom confidence score
    // to consider a face valid
    // (in range 0 - 1, default is 0.5)
    detection.confidence(0.7);

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Awaiting for face...");
}

/**
 * 
 */
void loop() {
    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // run detection
    if (!detection.run().isOk()) {
        Serial.println(detection.exception.toString());
        return;
    }

    // if face is not found, abort
    if (detection.notFound())
      return;
      
    Serial.printf(
        "Face(s) detected in %dms!\n", 
        detection.benchmark.millis()
    );

    // you can access the first detected face
    // at detection.first
    Serial.printf(
      " > face #1 located at (%d, %d)\n"
      "   proba is %.2f\n",
      detection.first.x,
      detection.first.y,
      detection.first.score
    );

    // if you expect multiple faces, you use forEach
    if (detection.count() > 1) {
      detection.forEach([](int i, face_t face) {
          Serial.printf(
              " > face #%d located at (%d, %d)\n", 
              i + 1, 
              face.x, 
              face.y
          );
  
          // if you enabled accurate detection
          // you have access to the keypoints
          if (face.hasKeypoints()) {
              Serial.printf(
                "   > left  eye   at (%d, %d)\n"
                "   > right eye   at (%d, %d)\n"
                "   > nose        at (%d, %d)\n"
                "   > left mouth  at (%d, %d)\n"
                "   > right mouth at (%d, %d)\n",
                face.leftEye.x,
                face.leftEye.y,
                face.rightEye.x,
                face.rightEye.y,
                face.nose.x,
                face.nose.y,
                face.leftMouth.x,
                face.leftMouth.y,
                face.rightMouth.x,
                face.rightMouth.y
              );
          }
          
      });
    }
}