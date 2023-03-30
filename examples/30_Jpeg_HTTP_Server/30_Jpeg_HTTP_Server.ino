#include "esp32cam.h"
#include "esp32cam/http/api/ApiServer.h"
#include "esp32cam/http/api/GetJpeg.h"

// Replace with your WiFi credentials
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"


Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::Http::Api::ApiServer server;
Eloquent::Esp32cam::Http::Api::GetJpeg getJpeg;


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.m5wide();
    cam.vga();
    cam.highQuality();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());

    server.add(getJpeg);
    server.begin();

    cam.mDNS("esp32cam");
    Serial.println("Camera OK");
    Serial.println(server.getWelcomeMessage());
}


void loop() {
    server.handle();
}