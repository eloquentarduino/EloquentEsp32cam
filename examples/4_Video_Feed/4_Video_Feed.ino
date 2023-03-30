/**
 * Example 4: Video Feed over HTTP
 * This sketch shows how to access the live camera feed
 * from a browser using HTTP
 */
#include "esp32cam.h"
#include "esp32cam/http/LiveFeed.h"

// Replace with your WiFi credentials
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"


Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::Http::LiveFeed feed(cam);


/**
 *
 */
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

    // Connect to WiFi
    // If something goes wrong, print the error message
    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());

    //Initialize live feed http server
    // If something goes wrong, print the error message
    while (!feed.begin())
        Serial.println(feed.getErrorMessage());

    // make the camera accessible at http://esp32cam.local
    if (!cam.mDNS("esp32cam"))
        Serial.println("Cannot create alias, use the IP address");

    // display the IP address of the camera
    Serial.println(feed.getWelcomeMessage());
}


/**
 *
 */
void loop() {
    // do nothing, web server handles the requests
}
