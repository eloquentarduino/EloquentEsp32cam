// 13_Motion_Live_Feed_With_RoIs.ino

#define MAX_RESOLUTION_VGA

#include "esp32cam.h"
#include "esp32cam/JpegDecoder.h"
#include "esp32cam/motion/Detector.h"
#include "esp32cam/motion/roi/Rect.h"
#include "esp32cam/motion/roi/Circle.h"
#include "esp32cam/motion/SimpleChange.h"
#include "esp32cam/http/MotionLiveFeed.h"

// Replace with your WiFi credentials
#define WIFI_SSID "Your SSID"
#define WIFI_PASS "Your password"


// See 12_Region_Of_Interest_Motion_Detection for details
struct {
    int x = 0;
    int y = 0;
    int width = 200;
    int height = 480;
    float threshold = 0.5;
} r;

struct {
    int x = 100;
    int y = 200;
    int radius = 100;
    float threshold = 0.5;
} c;

Eloquent::Esp32cam::Cam cam;
Eloquent::Esp32cam::JpegDecoder decoder;
Eloquent::Esp32cam::Motion::SimpleChange algorithm;
Eloquent::Esp32cam::Motion::Detector detector(algorithm);
Eloquent::Esp32cam::Motion::RoI::Rect rect(r);
Eloquent::Esp32cam::Motion::RoI::Circle circle(c);
Eloquent::Esp32cam::Http::MotionLiveFeed feed(cam, detector, 80);


void setup() {
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");

    cam.aithinker();
    cam.highQuality();
    cam.vga();
    cam.highestSaturation();
    cam.disableAutomaticWhiteBalance();
    cam.disableAutomaticExposureControl();
    cam.disableGainControl();

    // See 10_Image_Wise_Motion_Detection for details
    detector.trainFor(30);
    detector.triggerAbove(0.1);
    detector.retrainAfter(33ULL * 600);
    detector.denoise();

    algorithm.differBy(20);
    algorithm.smooth(0.9);
    algorithm.onlyUpdateBackground();

    /**
     * Display RoIs over image feed
     */
    feed.addRoI(rect);
    feed.addRoI(circle);

    while (!cam.begin())
        Serial.println(cam.getErrorMessage());

    while (!cam.connect(WIFI_SSID, WIFI_PASS))
        Serial.println(cam.getErrorMessage());

    /**
     * Initialize Motion Live Feed HTTP server
     */
    while (!feed.begin())
        Serial.println(feed.getErrorMessage());

    cam.viewAt("esp32cam");

    /**
     * Display the IP address of the camera
     */
    Serial.println(feed.getWelcomeMessage());
}


void loop() {
    feed.handle();
}