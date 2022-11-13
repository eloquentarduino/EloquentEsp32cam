#include <FS.h>
#include <SD_MMC.h>
#include <SPI.h>
#include "esp32cam.h"


Eloquent::Esp32cam::Cam cam;
uint32_t counter = 1;


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

    // Initialize the filesystem
    // If something goes wrong, print an error message
    while (!SD_MMC.begin() || SD_MMC.cardType() == CARD_NONE)
        Serial.println("Cannot init SD Card");

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

    String filename = String("/picture_") + counter + ".jpg";

    if (cam.saveTo(SD_MMC, filename)) {
        Serial.println(filename + " saved to disk");
        counter += 1;
    }
    else {
        Serial.println(cam.getErrorMessage());
    }
}
