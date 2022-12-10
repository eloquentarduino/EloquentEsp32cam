// 18_Line_Crossing_Counter_Feed.ino

#define MAX_RESOLUTION_VGA

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/motion/Detector.h"
#include "esp32cam/motion/SimpleChange.h"
#include "esp32cam/apps/LineCrossingCounter.h"
#include "esp32cam/http/LineCrossingCounterFeed.h"


#define WIFI_SSID "Abc"
#define WIFI_PASS "12345678"

using namespace Eloquent::Esp32cam;

Cam cam;
JpegDecoder decoder;
Motion::SimpleChange algorithm;
Motion::Detector detector(algorithm);
Applications::LineCrossingCounter counter(detector);
Http::LineCrossingCounterFeed feed(cam, decoder, detector, counter);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    // see 3_Get_Your_First_Picture for more details
    cam.aithinker();
    cam.highQuality();
    cam.vga();
    cam.disableAutomaticWhiteBalance();
    cam.disableAutomaticExposureControl();
    cam.disableGainControl();

    // see 10_Image_Wise_Motion_Detection for details
    detector.trainFor(30);
    detector.retrainAfter(33ULL * 600);
    detector.triggerAbove(0.2);
    detector.denoise();

    algorithm.differBy(20);
    algorithm.smooth(0.9);
    algorithm.onlyUpdateBackground();

    // see 17_Line_Crossing_Counter for details
    counter.lineAt(0.5);
    counter.above(0);
    counter.below(200);
    counter.lag(3);
    counter.wideness(4);
    counter.debounceSeconds(3);

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());

    while (!feed.begin())
        Serial.println(feed.getErrorMessage());

    cam.viewAt("esp32cam");
    Serial.println(feed.getWelcomeMessage());
}


void loop() {
    feed.handle();
}