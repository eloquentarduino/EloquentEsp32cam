# Unleash the full power of you ESP32 camera

This Arduino library kickstarts your ESP32 camera projects
by providing you a set of tools to easily interact your the camera.

## Take a picture

Forget complex configurations and verbose code.

```cpp
#include <eloquent_esp32cam.h>

using eloq::camera;

void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___GET YOUR FIRST PICTURE___");
    
    camera.pinout.aithinker();
    camera.brownout.disable();
    camera.resolution.vga();
    
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());
}

void loop() {
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }
    
    Serial.printf(
        "JPEG size in bytes: %d. Width: %dpx. Height: %dpx.\n",
        camera.getSizeInBytes(),
        camera.resolution.getWidth(),
        camera.resolution.getHeight()
    );
}
```

## Save picture to SD card

Timestamped file storage on your SD card that you can finally understand!

```cpp
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/extra/esp32/ntp.h>
#include <eloquent_esp32cam/extra/esp32/fs/sdmmc.h>

void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("___SAVE PIC TO SD CARD___");

    camera.pinout.freenove_s3();
    camera.brownout.disable();
    camera.resolution.vga();
    camera.quality.high();

    ntp.offsetFromGreenwhich(0);
    ntp.isDaylight();
    ntp.server("pool.ntp.org");

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
}

void loop() {
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // save under nested directory
    String date = ntp.date();
    String datetime = ntp.datetime();

    if (sdmmc.save(camera.frame).inside(date).to(datetime, "jpg").isOk()) {
      Serial.print("File written to ");
      Serial.println(sdmmc.session.lastFilename);
    }
    else Serial.println(sdmmc.session.exception.toString());
}
```

## Object detection

Running Edge Impulse FOMO object detection is a piece of cake.

```cpp
#include <your-fomo-project_inferencing.h>
#include <eloquent_esp32cam.h>
#include <eloquent_esp32cam/edgeimpulse/fomo.h>

using eloq::camera;
using eloq::ei::fomo;

void setup() {
    delay(3000);
    Serial.begin(115200);
    Serial.println("__EDGE IMPULSE FOMO (NO-PSRAM)__");

    // camera settings
    camera.pinout.freenove_s3();
    camera.brownout.disable();
    camera.resolution.yolo();
    camera.pixformat.rgb565();

    // init camera
    while (!camera.begin().isOk())
        Serial.println(camera.exception.toString());

    Serial.println("Camera OK");
    Serial.println("Put object in front of camera");
}


void loop() {
    if (!camera.capture().isOk()) {
        Serial.println(camera.exception.toString());
        return;
    }

    // run FOMO
    if (!fomo.run().isOk()) {
      Serial.println(fomo.exception.toString());
      return;
    }

    // how many objects were found?
    Serial.printf(
      "Found %d object(s) in %dms\n", 
      fomo.count(),
      fomo.benchmark.millis()
    );

    // if no object is detected, return
    if (!fomo.foundAnyObject())
      return;

    // if you expect to find a single object, use fomo.first
    Serial.printf(
      "Found %s at (x = %d, y = %d) (size %d x %d). "
      "Proba is %.2f\n",
      fomo.first.label,
      fomo.first.x,
      fomo.first.y,
      fomo.first.width,
      fomo.first.height,
      fomo.first.proba
    );

    // if you expect to find many objects, use fomo.forEach
    if (fomo.count() > 1) {
      fomo.forEach([](int i, bbox_t bbox) {
        Serial.printf(
          "#%d) Found %s at (x = %d, y = %d) (size %d x %d). "
          "Proba is %.2f\n",
          i + 1,
          bbox.label,
          bbox.x,
          bbox.y,
          bbox.width,
          bbox.height,
          bbox.proba
        );
      });
    }
}
```