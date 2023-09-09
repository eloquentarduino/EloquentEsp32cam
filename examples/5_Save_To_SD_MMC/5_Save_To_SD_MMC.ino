/**
 * Example 5: Store pictures to SD Card
 * This sketch shows how to save a picture on the SD Card
 * filesystem using automatic incrementing file naming.
 *
 * Open the Serial Monitor and enter 'capture' (without quotes)
 * to capture a new image and save it to disk
 *
 * BE SURE TO SET "TOOLS > CORE DEBUG LEVEL = DEBUG"
 * to turn on debug messages
 */

// if you define WIFI_SSID and WIFI_PASS before importing the library
// the camera will automatically connect and synchronize the NTP server
// for filename timestamping.
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

#include "esp32camera.h"
#include "esp32camera/extra/esp32/wifi/sta.h"
#include "esp32camera/extra/esp32/fs/sdmmc.h"

using namespace e;


void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___SAVE PIC TO SD CARD___");

    // camera settings
    // replace with your own model!
    camera.pinout.wroom_s3();
    camera.brownout.disable();
    camera.xclk.slow();
    camera.resolution.vga();
    camera.quality.high();
    camera.rateLimit.atMost(33).fps();

    // sdmmc.h includes NTP object, too for filename timestamping
    // if connected to the internet, try to get time from NTP
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
    
    ntp.isntDaylight();
    ntp.isDaylight();
    ntp.server("pool.ntp.org");

    /**
     * Initialize the camera
     * If something goes wrong, print the error message
     */
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    /**
     * Initialize SD card
     * If something goes wrong, print the error message.
     */
    // you can configure each pin (if needed)
    sdmmc.pinout.clk(39);
    sdmmc.pinout.cmd(38);
    sdmmc.pinout.d0(40); 
    // or leverage the list of builtin modules
    sdmmc.pinout.freenove_camera_s3();
    
    while (!sdmmc.begin().isOk())
        Serial.println(sdmmc.exception.toString());

    /**
     * Connect to WiFi to sync NTP
     * If something goes wrong, print the error message
     */
    while (!wifiSta.connect().isOk())
      Serial.println(wifiSta.exception.toString());

    /**
     * Configure NTP service
     * If something goes wrong, print the error message
     */
    while (!ntp.begin().isOk())
      Serial.println(ntp.exception.toString());

    while (true) {
      sdmmc.saveCurrentFrame();
      delay(4000);
    }

    Serial.println("Camera OK");
    Serial.println("SD card OK");
    Serial.println("Enter 'capture' to capture a new picture");
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

    // save to disk
    Serial.println("Capture ok, saving...");

    /**
     * If you don't supply a name, an incremental (sorted) filename
     * (that persists across reboots) will be used.
     * If NTP is available, the current timestamp will 
     * also be appended.
     */
    if (!sdmmc.saveCurrentFrame().isOk()) {
        Serial.println(sdmmc.exception.toString());
        return;
    }

    // or you can supply your own filename
    if (!sdmmc.saveCurrentFrameAs("/picture.jpg").isOk()) {
        Serial.println(sdmmc.exception.toString());
        return;
    }

    Serial.println("Enter 'capture' to capture a new picture");
}