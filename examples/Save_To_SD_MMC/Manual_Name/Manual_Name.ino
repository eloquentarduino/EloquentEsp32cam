/**
 * SDMMC file storage: manual filename
 * 
 * This sketch shows how to save a picture on the SD Card
 * filesystem by specifying the filename manually
 *
 * Open the Serial Monitor and enter 'capture' (without quotes)
 * to capture a new image and save it to SD
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */

#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/extra/esp32/fs/sdmmc.h>

using namespace eloq;


void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___SAVE PIC TO SD CARD___");

    // camera settings
    // replace with your own model!
    camera.pinout.freenove_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    // you can configure each pin of SDMMC (if needed)
    // (delete these lines if you are not sure)
    sdmmc.pinout.clk(39);
    sdmmc.pinout.cmd(38);
    sdmmc.pinout.d0(40); 
    // or shorter
    sdmmc.pinout.freenove_s3();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // init SD
    while (!sdmmc.begin().isOk())
        Serial.println(sdmmc.exception.toString());

    Serial.println("Camera OK");
    Serial.println("SD card OK");
    Serial.println("Enter the filename where to save the picture");
}


void loop() {
    // await for filename from the Serial Monitor
    if (!Serial.available())
        return;

    String filename = Serial.readStringUntil('\n');
    filename.trim();

    if (!filename.endsWith(".jpg") && !filename.endsWith(".jpeg"))
        filename = filename + ".jpg";

    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // save to SD
    if (sdmmc.save(camera.frame).to(filename).isOk()) {
      Serial.print("File written to ");
      Serial.println(sdmmc.session.lastFilename);
    }
    else Serial.println(sdmmc.session.exception.toString());

    // you can also save under a nested folder
    if (sdmmc.save(camera.frame).inside("myfolder").to(filename).isOk()) {
      Serial.print("File written to ");
      Serial.println(sdmmc.session.lastFilename);
    }
    else Serial.println(sdmmc.session.exception.toString());

    // restart the loop
    Serial.println("Enter another filename");
}