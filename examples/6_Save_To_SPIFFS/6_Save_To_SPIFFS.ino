/**
 * Example 6: Store pictures to SPIFFS
 * This sketch shows how to save a picture on the SPIFFS
 * filesystem using automatic incrementing file naming.
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

#include <SPIFFS.h>
#include "esp32cam.h"


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    camera.model.autodetect();
    camera.resolution.vga();
    camera.quality.high();

    // if connected to the internet, try to get time from NTP
    // (these are the defaults, so you can remove them)
    camera.ntp.gmt(0);
    camera.ntp.daylight(false);
    camera.ntp.server("pool.ntp.org");

    // init camera
    while (!camera.begin()) {
        Serial.println(camera.getErrorMessage());
        delay(1000);
    }

    // init storage
    while (!camera.storage.spiffs())
        Serial.println(camera.getErrorMessage());

    Serial.println("Init OK. Enter 'capture' to capture a new picture");
}


void loop() {
    // await for "capture" from the Serial Monitor
    if (!Serial.available())
        return;

    if (Serial.readStringUntil('\n') != "capture") {
        Serial.println("Only 'capture'");
        return;
    }

    // capture picture
    if (!camera.capture()) {
        Serial.println(camera.getErrorMessage());
        return;
    }

    // save to disk
    Serial.println("Capture ok, saving...");
    camera.storage.save();
}