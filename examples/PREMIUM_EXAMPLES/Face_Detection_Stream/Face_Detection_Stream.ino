/**
 * Display face detection results in a web browser.
 * ONLY WORKS ON ESP32S3
 * 
 * BE SURE TO SET "TOOLS > PARTITION SCHEME = HUGE APP"
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */

// if you define WIFI_SSID and WIFI_PASS before importing the library, 
// you can call connect() instead of connect(ssid, pass)
//
// If you set HOSTNAME and your router supports mDNS, you can access
// the camera at http://{HOSTNAME}.local

#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"
#define HOSTNAME "esp32cam"

#include "eloquent_esp32cam.h"
#include "eloquent_esp32cam/viz/face_stream.h"

using eloq::camera;
using eloq::wifi;
using eloq::face::detection;
using eloq::viz::faceStream;

/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("__FACE DETECTION STREAM___");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
    camera.brownout.disable();
    camera.resolution.face();
    camera.quality.high();

    // you can customize the face detector
    // see the Face_Detection.ino example
    // for a detailed explanation
    detection.accurate();
    detection.confidence(0.5);

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // connect to WiFi
    while (!wifi.connect().isOk())
      Serial.println(wifi.exception.toString());

    // init face detection http server
    while (!faceStream.begin().isOk())
        Serial.println(faceStream.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Face Detection Stream OK");
    Serial.println(faceStream.address());
    Serial.println("Put your face in front of the camera");
}

/**
 * 
 */
void loop() {
    // server runs in a separate thread, no need to do anything here
}