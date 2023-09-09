/**
 * Example 4: Browse images
 * Start an HTTP server to access the stored images from the browser.
 *
 * Endpoints are:
 *  - / -> displays a webpage with the browser files
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */

// if you define WIFI_SSID and WIFI_PASS before importing the library
// you can call connect() instead of connect(ssid, pass)
//
// If you set HOSTNAME and your router supports mDNS, you can access
// the camera at http://{HOSTNAME}.local
// esp32cam is the default
#define WIFI_SSID "Novanetworks_Spagro2017"
#define WIFI_PASS "AGRIMAG01"
#define HOSTNAME "esp32cam"


#include "esp32camera.h"
#include "esp32camera/extra/esp32/fs/SpiffsFilesystem.h"
#include "esp32camera/browser/ImageBrowserServer.h"

using namespace e;


void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___IMAGE BROWSER SERVER___");

    // camera settings
    camera.brownout.disable();
    camera.xclk.fast();
    camera.pinout.m5wide();
    camera.resolution.vga();
    camera.quality.high();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // init filesystem
    while (!spiffsFS.begin().isOk())
        Serial.println(spiffsFS.exception.toString());

    // connect to WiFi
    while (!wifiSta.connect().isOk())
        Serial.println(wifiSta.exception.toString());

    // init image browser http server
    while (!imageBrowserServer.begin().isOk())
        Serial.println(imageBrowserServer.exception.toString());

    Serial.println("Camera OK");
    Serial.println("ImageBrowser OK");
}


void loop() {
    // server runs in a task, no need to do anything here
}