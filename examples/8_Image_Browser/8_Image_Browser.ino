#include <FS.h>
#include <SD_MMC.h>
#include <SPI.h>
#include "esp32cam.h"
#include "esp32cam/http/ImageBrowser.h"

// Replace with your WiFi credentials
#define WIFI_SSID "Your SSID"
#define WIFI_PASS "Your password"

// 80 is the port to listen to
// You can change it to whatever you want, 80 is the default for HTTP
Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::Http::ImageBrowser browser(SD_MMC, 80);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    cam.aithinker();
    cam.highQuality();
    cam.qvga();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!SD_MMC.begin() || SD_MMC.cardType() == CARD_NONE)
        Serial.println("Cannot init SD Card");

    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());

    //Initialize image browser web server
    // If something goes wrong, print the error message
    while (!browser.begin())
        Serial.println(browser.getErrorMessage());

    // set max number of files displayed on a single page
    browser.setMaxNumFilesPerPage(30);

    cam.viewAt("esp32cam");

    // display the IP address of the camera
    Serial.println(browser.getWelcomeMessage());
}


void loop() {
    browser.handle();
}
