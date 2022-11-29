// Appendix1_Flashlight_Toggle.ino

#include "esp32cam.h"

Eloquent::Esp32cam::Cam cam;


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());
}


void loop() {
    Serial.println("ON");
    cam.flashlightOn();
    delay(1000);
    Serial.println("OFF");
    cam.flashlightOff();
    delay(1000);
}
