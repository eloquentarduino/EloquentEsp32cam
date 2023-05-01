/**
 * Example 8: Browse images over HTTP
 * This sketch shows how to access the pictures stored
 * on the SD/SPIFFS from the browser.
 * This example uses SPIFFS filesystem, see
 * 5_Save_To_SD_MMC to use the SD card.
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

#include <SPIFFS.h>
#include "esp32camera.h"
#include "esp32camera/http/ImageBrowser.h"


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    camera.model.aithinker();
    camera.quality.high();
    camera.resolution.qvga();

    // Init camera
    // If something goes wrong, print the error message
    while (!camera.begin())
        Serial.println(camera.getErrorMessage());

    // Init storage
    // If something goes wrong, print the error message
    while (!camera.storage.spiffs())
        Serial.println(camera.storage.getErrorMessage());

    // Init image browser server
    // If something goes wrong, print the error message
    while (!imageBrowser.begin())
        Serial.println(imageBrowser.getErrorMessage());

    Serial.println("Camera OK");
    Serial.println("ImageBrowser OK");
    Serial.println(imageBrowser.getWelcomeMessage());
}


void loop() {
    // serve requests
    imageBrowser.handle();
}