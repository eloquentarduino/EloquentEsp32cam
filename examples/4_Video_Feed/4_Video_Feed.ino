#include "esp32cam.h"
#include "esp32cam/http/LiveFeed.h"

// Replace with your WiFi credentials
#define WIFI_SSID "Your SSID"
#define WIFI_PASS "Your password"

// 80 is the port to listen to
// You can change it to whatever you want, 80 is the default for HTTP
Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::Http::LiveFeed feed(cam, 80);


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
    if (!cam.viewAt("esp32cam"))
        Serial.println("Cannot create alias, use the IP address");
    else
        Serial.println("Live Feed available at http://esp32cam.local");

    // display the IP address of the camera
    Serial.println(feed.getWelcomeMessage());
}


void loop() {
}
