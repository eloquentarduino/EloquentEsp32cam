/**
 * Stream Edge Impulse's FOMO results to the browser.
 * It only works on ESP32S3 boards with OPI PSRAM.
 * 
 * This sketch is taken from my eBook
 * "Mastering the ESP32(S3) Camera"
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASS"
#define HOSTNAME "esp32cam"

#include <your-fomo-project_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/viz/ei/fomo_stream.h>

using eloq::camera;
using eloq::wifi;
using eloq::viz::ei::fomoStream;


/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("__EDGE IMPULSE FOMO STREAM__");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
    camera.brownout.disable();
    // start with a low resolution
    camera.resolution.qvga();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // connect to WiFi
    while (!wifi.connect().isOk())
      Serial.println(wifi.exception.toString());

    // init FOMO stream http server
    while (!fomoStream.begin().isOk())
        Serial.println(fomoStream.exception.toString());

    Serial.println("Camera OK");
    Serial.println("FOMO Stream Server OK");
    Serial.println(fomoStream.address());
    Serial.println("Put object in front of camera");
}

/**
 * 
 */
void loop() {
    // HTTP server runs in a task, no need to do anything here
}