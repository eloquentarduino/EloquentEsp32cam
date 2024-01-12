/**
 * SDMMC file storage: NTP filename
 * 
 * This sketch shows how to save a picture on the SD Card
 * filesystem by generating a filename using current time
 *
 * Open the Serial Monitor and enter 'capture' (without quotes)
 * to capture a new image and save it to SD
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = INFO"
 * to turn on debug messages
 */

// if you define WIFI_SSID and WIFI_PASS before importing the library
// you can call wifi.connect() instead of wifi.connect(ssid, password)
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/extra/esp32/ntp.h>
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

    // if connected to the internet, try to get time from NTP
    // you can set your timezone offset from Greenwich
    ntp.offsetFromGreenwhich(0);
    // or any of
    ntp.cst();
    ntp.ist();
    ntp.eest();
    ntp.cest();
    ntp.bst();
    ntp.west();
    ntp.cet();
    ntp.gmt();
    ntp.edt();
    ntp.pdt();

    // enable/disable daylight saving
    ntp.isntDaylight();
    ntp.isDaylight();
    
    ntp.server("pool.ntp.org");

    // you can configure each pin of SDMMC (if needed)
    // (delete these lines if not sure)
    sdmmc.pinout.clk(39);
    sdmmc.pinout.cmd(38);
    sdmmc.pinout.d0(40);

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // init SD
    while (!sdmmc.begin().isOk())
        Serial.println(sdmmc.exception.toString());

    // connect to WiFi to sync NTP
    while (!wifi.connect().isOk())
      Serial.println(wifi.exception.toString());

    // get NTP time
    while (!ntp.begin().isOk())
      Serial.println(ntp.exception.toString());

    Serial.println("Camera OK");
    Serial.println("SD card OK");
    Serial.println("NTP OK");
    Serial.print("Current time is ");
    Serial.println(ntp.datetime());
    Serial.println("Enter 'capture' to capture a new picture");
}


void loop() {
    // await for "capture" from the Serial Monitor
    if (!Serial.available())
        return;

    if (Serial.readStringUntil('\n') != "capture") {
        Serial.println("I only understand 'capture'");
        return;
    }

    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // save under root directory
    if (sdmmc.save(camera.frame).to(ntp.datetime(), "jpg").isOk()) {
      Serial.print("File written to ");
      Serial.println(sdmmc.session.lastFilename);
    }
    else Serial.println(sdmmc.session.exception.toString());

    // save under nested directory
    String date = ntp.date();
    String datetime = ntp.datetime();

    if (sdmmc.save(camera.frame).inside(date).to(datetime, "jpg").isOk()) {
      Serial.print("File written to ");
      Serial.println(sdmmc.session.lastFilename);
    }
    else Serial.println(sdmmc.session.exception.toString());
    
    // restart the loop
    Serial.println("Enter 'capture' to capture a new picture");
}