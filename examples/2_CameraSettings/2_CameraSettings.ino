/**
 * Example 2: Camera Configuration
 * This sketch shows the available sensor configurations for the
 * Esp32 camera.sensor.
 * It does nothing: it's only a reference of the available methods
 */

#include "esp32camera.h"


void setup() {
    /**
     * Most of the settings you found on the CameraWebServer page
     * are available for modification
     */

    // set model
    camera.pinout.aithinker();
    camera.pinout.m5();
    camera.pinout.m5_wide();
    camera.pinout.eye();
    camera.pinout.ttgo_lcd();
    camera.pinout.simcam();
    camera.pinout.xiao_s3();
    camera.pinout.wroom_s3();

    // you can disable the brownout detector, if you need
    camera.brownout.disable();

    // XCLK speed is 10MHz
    camera.xclk.slow();
    // XCLK speed is 20MHz
    camera.xclk.fast();

    // set resolution
    camera.resolution.yolo(); // 96x96
    camera.resolution.qqvga();
    camera.resolution.qcif();
    camera.resolution.hqvga();
    camera.resolution.face(); // 240x240
    camera.resolution.qvga();
    camera.resolution.cif();
    camera.resolution.hvga();
    camera.resolution.vga();
    camera.resolution.svga();
    camera.resolution.xga();
    camera.resolution.hd();
    camera.resolution.sxga();
    camera.resolution.uxga();
    camera.resolution.fhd();
    camera.resolution.p_hd();
    camera.resolution.p_3mp();
    camera.resolution.qxga();
    camera.resolution.wqxga();
    camera.resolution.p_fhd();
    camera.resolution.qsxga();

    // set jpeg quality
    camera.quality.worst(); // 64
    camera.quality.low();   // 30
    camera.quality.high();  // 20
    camera.quality.best();  // 10

    // configure image sensor
    camera.sensor.vflip();
    camera.sensor.hmirror();

    camera.sensor.lowestBrightness(); // set brightness = -2
    camera.sensor.lowBrightness(); // set brightness = -1
    camera.sensor.highBrightness(); // set brightness = +1
    camera.sensor.highestBrightness(); // set brightness = +2
    camera.sensor.setBrightness(0);

    camera.sensor.lowestSaturation(); // set saturation = -2
    camera.sensor.lowSaturation(); // set saturation = -1
    camera.sensor.highSaturation(); // set saturation = +1
    camera.sensor.highestSaturation(); // set saturation = +2
    camera.sensor.setSaturation(0);

    camera.sensor.negative();
    camera.sensor.grayscale();
    camera.sensor.redTint();
    camera.sensor.greenTint();
    camera.sensor.blueTint();
    camera.sensor.sepia();

    camera.sensor.enableAutomaticWhiteBalance();
    camera.sensor.disableAutomaticWhiteBalance();

    camera.sensor.enableGainControl();
    camera.sensor.disableGainControl();

    camera.sensor.enableExposureControl();
    camera.sensor.disableExposureControl();

    camera.sensor.enableAutomaticWhiteBalanceGain();
    camera.sensor.disableAutomaticWhiteBalanceGain();

    camera.sensor.enableAutomaticGainControl();
    camera.sensor.disableAutomaticGainControl();

    camera.sensor.enableAutomaticExposureControl();
    camera.sensor.disableAutomaticExposureControl();

    camera.sensor.enableDCW();
    camera.sensor.disableDCW();

    camera.sensor.enableBPC();
    camera.sensor.disableBPC();

    camera.sensor.enableWPC();
    camera.sensor.disableWPC();

    camera.sensor.enableLensCorrection();
    camera.sensor.disableLensCorrection();

    camera.sensor.enableRawGamma();
    camera.sensor.disableRawGamma();

    // custom configuration
    camera.sensor.configure([](sensor_t *sensor) {
        // do whatever you want with the sensor
    });
}


void loop() {

}