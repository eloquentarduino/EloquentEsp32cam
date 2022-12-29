#define MAX_RESOLUTION_VGA

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"


Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::JpegDecoder decoder;


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.highQuality();
    cam.vga();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());
}


void loop() {
    if (!cam.capture()) {
        Serial.println(cam.getErrorMessage());
        return;
    }

    if (!decoder.decode(cam)) {
        Serial.println(decoder.getErrorMessage());
        return;
    }

    // we can access the grayscale image at decoder.luma
    // you have access to
    //   - luma.pixels for the uint8_t pixel values
    //   - luma.length for the size of the luma.pixels array
    Serial.print("Luma length: ");
    Serial.println(decoder.luma.length);
    Serial.print("Decoding time: ");
    Serial.print(decoder.getExecutionTimeInMillis());
    Serial.println("ms");
    delay(1000);
}