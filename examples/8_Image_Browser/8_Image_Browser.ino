/**
 * Example 8: Browse images over HTTP
 * This sketch shows how to access the pictures stored
 * on the SD/SPIFFS from the browser
 */

// Replace with your WiFi credentials
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

#include "esp32cam.h"
#include "esp32cam/http/ImageBrowser.h"


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

    // Initialize image browser server
    // If something goes wrong, print the error message
    while (!imageBrowser.begin())
        Serial.println(imageBrowser.getErrorMessage());

    // display the IP address of the camera
    Serial.println(imageBrowser.getWelcomeMessage());
}


/**
 *
 */
void loop() {
    imageBrowser.handle();
}