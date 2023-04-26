/**
 * Example 4: Mjpeg Stream
 * This sketch shows how to access the live camera feed
 * from a browser using HTTP as an mjpeg Stream
 */

// Replace with your WiFi credentials
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

#include "esp32cam.h"
#include "esp32cam/http/MjpegStream.h"


/**
 *
 */
void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    camera.aithinker();
    camera.highQuality();
    camera.qvga();

    while (!camera.begin())
        Serial.println(camera.getErrorMessage());

    // Initialize mjpeg server
    // If something goes wrong, print the error message
    while (!mjpegStream.begin())
        Serial.println(mjpegStream.getErrorMessage());

    // Display the IP address of the camera
    // html preview of the mjpeg stream is available at http://<ip>/
    // mjpeg stream is available at http://<ip>/mjpeg
    // jpeg still picture is available at http://<ip>/jpeg
    Serial.println(mjpegStream.getWelcomeMessage());
}


/**
 *
 */
void loop() {
    mjpegStream.handle();
}
