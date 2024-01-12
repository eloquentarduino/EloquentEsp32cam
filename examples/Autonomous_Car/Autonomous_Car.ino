/**
 * Autonomous car driven by Edge Impulse FOMO
 * From: https://eloquentarduino.com/esp32-cam-autonomous-car
 *
 * Tested on ESP32S3 camera
 */
#include <your_edge_impulse_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/car.h>

using eloq::camera;
using eloq::car::Motor;
using eloq::car::Car;

/**
 * Replace with your motor pins
 */
Motor left(39, 40);
Motor right(42, 41);
Car fomoCar(left, right);



/**
 *
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___AUTONOMOUS CAR___");

    // replace with your board
    camera.pinout.freenove_s3();
    camera.brownout.disable();
    camera.resolution.yolo();
    camera.pixformat.rgb565();

    // how many millis motors will run
    // to follow given object
    fomoCar.defaultDuration(100);
    fomoCar.stop();

    // if you mounted the camera "backward"
    // (see video), you have to reverse the motors
    // left.reverse();
    // right.reverse();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Put object in front of camera");
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

    // run FOMO
    if (!fomo.run().isOk()) {
        Serial.println(fomo.exception.toString());
        return;
    }

    // let the car follow the object
    fomoCar.follow(fomo);
}