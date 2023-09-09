/**
 * Capture frames from camera and save them to SPIFFS filesystem.
 * The filesystem has an automatic naming scheme, so you won't
 * overwrite existing images even after reboot.
 * 
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */
#include "esp32camera.h"
#include "esp32camera/extra/esp32/fs/SpiffsFilesystem.h"

// all library global objects are scoped under 
// the `e` namespace
using namespace e;


void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___SAVE IMAGE TO SPIFFS___");

    // camera settings
    camera.brownout.disable();
    camera.xclk.fast();
    camera.pinout.m5wide();
    camera.resolution.vga();
    camera.quality.high();

    // Init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // init SPIFFS
    while (!spiffsFS.begin().isOk())
        Serial.println(spiffsFS.exception.toString());

    Serial.println("Camera OK");
    Serial.println("SpiffsFS OK");
}


void loop() {
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    if (!camera.saveTo(spiffsFS).isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    delay(3000);
}