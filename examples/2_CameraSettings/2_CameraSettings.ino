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