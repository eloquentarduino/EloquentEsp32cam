#include <FS.h>
#include <SPIFFS.h>
#include "esp32cam.h"
#include "esp32cam/NtpClient.h"

#define WIFI_SSID "Your SSID"
#define WIFI_PASS "Your password"


Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::NtpClient ntp;


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    cam.aithinker();
    cam.highQuality();
    cam.vga();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    /**
     * Initialize the filesystem
     * If something goes wrong, print an error message
     */
    while (!SPIFFS.begin(true))
        Serial.println("Cannot init SD Card");

    /**
     * Connect to WiFi
     * If something goes wrong, print the error message
     */
    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());

    /**
     * Configure NTP server
     * gmt offset is in hours
     */
    ntp.gmt(1);
    ntp.daylight();
    ntp.server("pool.ntp.org");

    while (!ntp.begin())
        Serial.println("Cannot get NTP time");

    Serial.println("Enter 'capture' in the Serial Monitor");
}


void loop() {
    if (!Serial.available())
        return;

    if (Serial.readStringUntil('\n') != "capture")
        return;

    if (!cam.capture()) {
        Serial.println(cam.getErrorMessage());
        return;
    }

    String filename = ntp.getFilename();

    if (cam.saveTo(SPIFFS, filename)) {
        Serial.println(filename + " saved to disk");
    }
    else {
        Serial.print("Cannot save ");
        Serial.print(filename);
        Serial.print(": ");
        Serial.println(cam.getErrorMessage());
    }
}