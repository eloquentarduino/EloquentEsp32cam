/**
 * Take a photo and upload to the online dashboard at
 * https://esp32camstorage.eloquentarduino.com
 *
 * To take the photo, send 'upload' over the Serial Monitor
 */
#include "esp32cam.h"

// Replace with your WiFi credentials
#define WIFI_SSID "SSID"
#define WIFI_PASS "PASSWORD"

// replace with the device token from https://esp32camstorage.eloquentarduino.com
#define DEVICE_TOKEN "abcDefghi0LMNO1qR"


Eloquent::Esp32cam::Cam cam;


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    // on the Free plan, VGA + best quality works fine (640x480)
    // on the PRO plan, you can go up to xga (1024x768)
    cam.aithinker();
    cam.bestQuality();
    cam.vga();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    // Connect to WiFi
    // If something goes wrong, print the error message
    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());

    Serial.println("Setup is OK. Send 'upload' to take a picture and upload to your dashboard");
}


void loop() {
    // await for Serial command
    if (!Serial.available())
        return;

    if (Serial.readStringUntil('\n') != "upload")
        return;

    Serial.println("Capturing image...");
    cam.capture();

    Serial.print("Uploading image...");
    cam.uploadToCloudStorage(DEVICE_TOKEN);

    Serial.println(cam.isOk() ? "OK. Go check your dashboard to see the image" : cam.getErrorMessage());
}