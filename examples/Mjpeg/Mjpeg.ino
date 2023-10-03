/**
 * View camera MJPEG stream
 * 
 * Start an HTTP server to access the live video feed
 * of the camera from the browser.
 *
 * Endpoints are:
 *  - / -> displays the raw MJPEG stream
 *  - /jpeg -> captures a still image
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */

// if you define WIFI_SSID and WIFI_PASS before importing the library, 
// you can call connect() instead of connect(ssid, pass) on the wifi object
//
// If you set HOSTNAME and your router supports mDNS, you can access
// the camera at http://{HOSTNAME}.local

#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"
#define HOSTNAME "esp32cam"

#include "esp32camera.h"
#include "esp32camera/mjpeg.h"


// this will import all global variables under the e(loquent) namespace
using namespace e;


/**
 * 
 */
void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___MJPEG STREAM SERVER___");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // connect to WiFi
    while (!wifi.connect().isOk())
      Serial.println(wifi.exception.toString());

    // start mjpeg http server
    while (!mjpeg.begin().isOk())
        Serial.println(mjpeg.exception.toString());

    Serial.println("Camera OK");
    Serial.println("MjpegStream OK");
    Serial.println(mjpeg.address());
}


void loop() {
    // HTTP server runs in a task, no need to do anything here
}