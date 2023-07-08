/**
 * Include the EloquentEsp32Cam library
 */
#include "esp32cam.h"


/**
 * Instantiate the camera
 */
Eloquent::Esp32cam::Cam cam;


void setup() {
    /**
     * Most of the settings you found on the CameraWebServer page
     * are available for modification
     */

    cam.vflip();
    cam.hmirror();

    cam.lowestBrightness(); // set brightness = -2
    cam.lowBrightness(); // set brightness = -1
    cam.highBrightness(); // set brightness = +1
    cam.highestBrightness(); // set brightness = +2
    cam.setBrightness(0);

    cam.lowestSaturation(); // set saturation = -2
    cam.lowSaturation(); // set saturation = -1
    cam.highSaturation(); // set saturation = +1
    cam.highestSaturation(); // set saturation = +2
    cam.setSaturation(0);

    cam.negative();
    cam.grayscale();
    cam.redTint();
    cam.greenTint();
    cam.blueTint();
    cam.sepia();

    cam.enableAutomaticWhiteBalance();
    cam.disableAutomaticWhiteBalance();

    cam.enableGainControl();
    cam.disableGainControl();

    cam.enableExposureControl();
    cam.disableExposureControl();

    cam.enableAutomaticWhiteBalanceGain();
    cam.disableAutomaticWhiteBalanceGain();

    cam.enableAutomaticGainControl();
    cam.disableAutomaticGainControl();

    cam.enableAutomaticExposureControl();
    cam.disableAutomaticExposureControl();

    cam.enableDCW();
    cam.disableDCW();

    cam.enableBPC();
    cam.disableBPC();

    cam.enableWPC();
    cam.disableWPC();

    cam.enableLensCorrection();
    cam.disableLensCorrection();

    cam.enableRawGamma();
    cam.disableRawGamma();

    // custom configuration
    cam.configureSensor([](sensor_t *sensor) {
        // do whatever you want with the sensor
    });
}


void loop() {

}