/**
 * SPIFFS file storage: NTP filename
 * 
 * This sketch shows how to save a picture on SPIFFS
 * filesystem by generating a filename using current time
 *
 * Open the Serial Monitor and enter 'capture' (without quotes)
 * to capture a new image and save it
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
#include <eloquent_esp32cam/extra/esp32/fs/spiffs.h>

using namespace eloq;


void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___SAVE PIC TO SPIFFS___");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
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

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    // init SPIFFS
    while (!spiffs.begin().isOk())
        Serial.println(spiffs.exception.toString());

    // connect to WiFi to sync NTP
    while (!wifi.connect().isOk())
      Serial.println(wifi.exception.toString());

    // get NTP time
    while (!ntp.begin().isOk())
      Serial.println(ntp.exception.toString());

    Serial.println("Camera OK");
    Serial.println("SPIFFS OK");
    Serial.println("NTP OK");
    Serial.println("Enter 'capture' to capture a new picture and save to SPIFFS");
}


void loop() {
    // await for "capture" from the Serial Monitor
    if (!Serial.available())
        return;

    if (Serial.readStringUntil('\n') != "capture") {
        Serial.println("I only understand 'capture' (without quotes)");
        return;
    }

    // capture picture
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // save under root directory
    if (spiffs.save(camera.frame).to(ntp.datetime(), "jpg").isOk()) {
      Serial.print("File written to ");
      Serial.println(spiffs.session.lastFilename);
    }
    else Serial.println(spiffs.session.exception.toString());

    // save under nested directory
    if (spiffs.save(camera.frame).inside(ntp.date()).to(ntp.datetime(), "jpg").isOk()) {
      Serial.print("File written to ");
      Serial.println(spiffs.session.lastFilename);
    }
    else Serial.println(spiffs.session.exception.toString());
    
    // restart the loop
    Serial.println("Enter another filename");
}