#include "esp32cam.h"
#include "esp32cam/http/LiveStyleTransfer.h"

// Replace with your WiFi credentials
#define WIFI_SSID "Your SSID"
#define WIFI_PASS "Your password"

// 80 is the port to listen to
// You can change it to whatever you want, 80 is the default for HTTP
Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::Http::LiveStyleTransfer styleTransfer(cam, 80);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.highQuality();
    cam.qvga();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());

    //Initialize live style transfer http server
    // If something goes wrong, print the error message
    while (!styleTransfer.begin())
        Serial.println(styleTransfer.getErrorMessage());

    cam.viewAt("esp32cam");

    // display the IP address of the camera
    Serial.println(styleTransfer.getWelcomeMessage());
}


void loop() {
}
