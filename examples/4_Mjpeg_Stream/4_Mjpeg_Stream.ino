/**
 * Example 4: View camera MJPEG stream
 * Start an HTTP server to access the live video feed
 * of the camera from the browser.
 *
 * Endpoints are:
 *  - / -> displays a webpage with the stream
 *  - /mjpeg -> displays the raw MJPEG stream
 *  - /jpeg -> captures a still image
 *
 *  The /mjpeg stream is handy if you want to open the
 *  stream from VLC or Python
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */

// if you define WIFI_SSID and WIFI_PASS before importing the library
// the camera will automatically connect
//
// If you set HOSTNAME and your router supports mDNS, you can access
// the camera at http://{HOSTNAME}.local
// esp32cam is the default
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"
#define HOSTNAME "esp32cam"


#include "esp32camera.h"
#include "esp32camera/http/MjpegStream.h"


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    camera.model.aithinker();
    camera.resolution.vga();
    camera.quality.high();

    // Init camera
    while (!camera.begin())
        Serial.println(camera.getErrorMessage());

    // init mjper http server
    while (!mjpegStream.begin())
        Serial.println(mjpegStream.getErrorMessage());

    Serial.println("Camera OK");
    Serial.println("MjpegStream OK");
    Serial.println(mjpegStream.getWelcomeMessage());
}


void loop() {
    // serve requests
    mjpegStream.handle();
}