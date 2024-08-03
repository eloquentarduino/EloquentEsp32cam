#ifndef ELOQUENT_ESP32CAM_CAMERA_PINOUT
#define ELOQUENT_ESP32CAM_CAMERA_PINOUT

#include <Preferences.h>
#include <esp_camera.h>

namespace Eloquent {
    namespace Esp32cam {
        namespace Camera {
            /**
             * Configure pins based on camera model
             */
            class Pinout {
            public:
                struct {
                    int8_t d0 = 0;
                    int8_t d1 = 0;
                    int8_t d2;
                    int8_t d3;
                    int8_t d4;
                    int8_t d5;
                    int8_t d6;
                    int8_t d7;
                    int8_t xclk;
                    int8_t pclk;
                    int8_t vsync;
                    int8_t href;
                    int8_t sccb_sda;
                    int8_t sccb_scl;
                    int8_t pwdn;
                    int8_t reset;
                    int8_t flashlight;
                } pins;

                /**
                 * Test if pinout is set
                 */
                operator bool() const {
                    return pins.d0 != 0 || pins.d1 != 0;
                }

                /**
                 *
                 */
                bool aithinker() {
                    pins.d0 = 5;
                    pins.d1 = 18;
                    pins.d2 = 19;
                    pins.d3 = 21;
                    pins.d4 = 36;
                    pins.d5 = 39;
                    pins.d6 = 34;
                    pins.d7 = 35;
                    pins.xclk = 0;
                    pins.pclk = 22;
                    pins.vsync = 25;
                    pins.href = 23;
                    pins.sccb_sda = 26;
                    pins.sccb_scl = 27;
                    pins.pwdn = 32;
                    pins.reset = -1;
                    pins.flashlight = 4;
                    return true;
                }

                /**
                 *
                 */
                bool m5() {
                    pins.d0 = 32;
                    pins.d1 = 35;
                    pins.d2 = 34;
                    pins.d3 = 5;
                    pins.d4 = 39;
                    pins.d5 = 18;
                    pins.d6 = 36;
                    pins.d7 = 19;
                    pins.xclk = 27;
                    pins.pclk = 21;
                    pins.vsync = 22;
                    pins.href = 26;
                    pins.sccb_sda = 25;
                    pins.sccb_scl = 23;
                    pins.pwdn = -1;
                    pins.reset = 15;
                    pins.flashlight = 16;
                    return true;
                }

                /**
                 *
                 */
                bool m5_wide() {
                    pins.d0 = 32;
                    pins.d1 = 35;
                    pins.d2 = 34;
                    pins.d3 = 5;
                    pins.d4 = 39;
                    pins.d5 = 18;
                    pins.d6 = 36;
                    pins.d7 = 19;
                    pins.xclk = 27;
                    pins.pclk = 21;
                    pins.vsync = 25;
                    pins.href = 26;
                    pins.sccb_sda = 22;
                    pins.sccb_scl = 23;
                    pins.pwdn = -1;
                    pins.reset = 15;
                    pins.flashlight = 14;
                    return true;
                }

                /**
                 *
                 */
                bool eye() {
                    pins.d0 = 34;
                    pins.d1 = 13;
                    pins.d2 = 14;
                    pins.d3 = 35;
                    pins.d4 = 39;
                    pins.d5 = 38;
                    pins.d6 = 37;
                    pins.d7 = 36;
                    pins.xclk = 4;
                    pins.pclk = 25;
                    pins.vsync = 5;
                    pins.href = 27;
                    pins.sccb_sda = 18;
                    pins.sccb_scl = 23;
                    pins.pwdn = -1;
                    pins.reset = -1;
                    pins.flashlight = 22;
                    return true;
                }

                /**
                 *
                 */
                bool wrover() {
                    pins.d0 = 4;
                    pins.d1 = 5;
                    pins.d2 = 18;
                    pins.d3 = 19;
                    pins.d4 = 36;
                    pins.d5 = 39;
                    pins.d6 = 34;
                    pins.d7 = 35;
                    pins.xclk = 21;
                    pins.pclk = 22;
                    pins.vsync = 25;
                    pins.href = 23;
                    pins.sccb_sda = 26;
                    pins.sccb_scl = 27;
                    pins.pwdn = -1;
                    pins.reset = -1;
                    pins.flashlight = -1;
                    return true;
                }

                /**
                 *
                 */
                bool wroom_s3() {
                    pins.d0 = 11;
                    pins.d1 = 9;
                    pins.d2 = 8;
                    pins.d3 = 10;
                    pins.d4 = 12;
                    pins.d5 = 18;
                    pins.d6 = 17;
                    pins.d7 = 16;
                    pins.xclk = 15;
                    pins.pclk = 13;
                    pins.vsync = 6;
                    pins.href = 7;
                    pins.sccb_sda = 4;
                    pins.sccb_scl = 5;
                    pins.pwdn = -1;
                    pins.reset = -1;
                    pins.flashlight = -1;
                    return true;
                }

                /**
                 * 
                 */
                bool freenove_s3() {
                    return wroom_s3();
                }

                /**
                 *
                 */
                bool ttgo_lcd() {
                    pins.d0 = 34;
                    pins.d1 = 13;
                    pins.d2 = 26;
                    pins.d3 = 35;
                    pins.d4 = 39;
                    pins.d5 = 38;
                    pins.d6 = 37;
                    pins.d7 = 36;
                    pins.xclk = 4;
                    pins.pclk = 25;
                    pins.vsync = 5;
                    pins.href = 27;
                    pins.sccb_sda = 18;
                    pins.sccb_scl = 23;
                    pins.pwdn = -1;
                    pins.reset = -1;
                    pins.flashlight = -1;
                    return true;
                }

                /**
                 *
                 */
                bool ttgo_pir() {
                    pins.d0 = 5;
                    pins.d1 = 14;
                    pins.d2 = 4;
                    pins.d3 = 15;
                    pins.d4 = 18;
                    pins.d5 = 23;
                    pins.d6 = 36;
                    pins.d7 = 39;
                    pins.xclk = 32;
                    pins.pclk = 19;
                    pins.vsync = 27;
                    pins.href = 25;
                    pins.sccb_sda = 13;
                    pins.sccb_scl = 12;
                    pins.pwdn = 26;
                    pins.reset = -1;
                    pins.flashlight = -1;
                    return true;
                }

                /**
                 *
                 */
                bool simcam() {
                    pins.d0 = 11;
                    pins.d1 = 9;
                    pins.d2 = 8;
                    pins.d3 = 10;
                    pins.d4 = 12;
                    pins.d5 = 17;
                    pins.d6 = 16;
                    pins.d7 = 15;
                    pins.xclk = 14;
                    pins.pclk = 13;
                    pins.vsync = 6;
                    pins.href = 7;
                    pins.sccb_sda = 4;
                    pins.sccb_scl = 5;
                    pins.pwdn = -1;
                    pins.reset = 18;
                    pins.flashlight = -1;
                    return true;
                }

                /**
                 * 
                 */
                bool xiao() {
                    pins.d0 = 15;
                    pins.d1 = 17;
                    pins.d2 = 18;
                    pins.d3 = 16;
                    pins.d4 = 14;
                    pins.d5 = 12;
                    pins.d6 = 11;
                    pins.d7 = 48;
                    pins.xclk = 10;
                    pins.pclk = 13;
                    pins.vsync = 38;
                    pins.href = 47;
                    pins.sccb_sda = 40;
                    pins.sccb_scl = 39;
                    pins.pwdn = -1;
                    pins.reset = -1;
                    pins.flashlight = -1;
                    return true;
                }

                /**
                 * 
                 */
                bool m5_timer() {
                    pins.d0 = 32;
                    pins.d1 = 35;
                    pins.d2 = 34;
                    pins.d3 = 5;
                    pins.d4 = 39;
                    pins.d5 = 18;
                    pins.d6 = 36;
                    pins.d7 = 19;
                    pins.xclk = 27;
                    pins.pclk = 21;
                    pins.vsync = 22;
                    pins.href = 26;
                    pins.sccb_sda = 25;
                    pins.sccb_scl = 23;
                    pins.pwdn = -1;
                    pins.reset = 15;
                    pins.flashlight = 2;
                    return true;
                }

                /**
                 *
                 */
                bool lilygo_tcamera_s3() {
                    pins.d0 = 14;
                    pins.d1 = 47;
                    pins.d2 = 48;
                    pins.d3 = 21;
                    pins.d4 = 13;
                    pins.d5 = 11;
                    pins.d6 = 10;
                    pins.d7 = 9;
                    pins.xclk = 38;
                    pins.pclk = 12;
                    pins.vsync = 8;
                    pins.href = 18;
                    pins.sccb_sda = 5;
                    pins.sccb_scl = 4;
                    pins.pwdn = -1;
                    pins.reset = 39;
                    pins.flashlight = -1;
                    return true;
                }

                bool eye_s3() {
                    return wroom_s3();
                }

                bool kaluga() {
                    pins.d0 = 36;
                    pins.d1 = 37;
                    pins.d2 = 41;
                    pins.d3 = 42;
                    pins.d4 = 39;
                    pins.d5 = 40;
                    pins.d6 = 21;
                    pins.d7 = 38;
                    pins.xclk = 1;
                    pins.pclk = 33;
                    pins.vsync = 2;
                    pins.href = 3;
                    pins.sccb_sda = 8;
                    pins.sccb_scl = 7;
                    pins.pwdn = -1;
                    pins.reset = -1;
                    pins.flashlight = -1;
                    return true;
                }

                /**
                 * Automatically detect model
                 */
                bool autodetect() {
                    preferences.begin("pinout:1", false);

                    bool detected = false
                        || aithinker() && test("aithinker")
                        || wroom_s3() && test("wroom_s3")
                        || xiao() && test("xiao")
                        || m5() && test("M5")
                        || m5_wide() && test("M5 wide")
                        || m5_timer() && test("M5 timer")
                        || eye() && test("ESP-EYE")
                        || eye_s3() && test("ESP-EYE S3")
                        || wrover() && test("Wrover")
                        || ttgo_lcd() && test("TTGO LCD")
                        || ttgo_pir() && test("TTGO PIR")
                        || lilygo_tcamera_s3() && test("T-CAMERA S3")
                        || simcam() && test("SIMCAM")
                        ;

                    preferences.end();

                    return detected;
                }

            protected:
                Preferences preferences;

                /**
                 * Test current configuration
                 * @return
                 */
                bool test(const char *model) {
                    ESP_LOGI("Pinout", "Testing pinout for model %s", model);
                    int8_t existing = preferences.getChar(model, 0);

                    // since a wrong config may cause a restart,
                    // mark a failure by default
                    preferences.putChar(model, -1);

                    // 1 is for correct config
                    if (existing == 1)
                        return true;

                    // -1 is for wrong config
                    if (existing == -1)
                        return false;

                    // new config, try it
                    camera_config_t config;

                    config.pin_d0 = pins.d0;
                    config.pin_d1 = pins.d1;
                    config.pin_d2 = pins.d2;
                    config.pin_d3 = pins.d3;
                    config.pin_d4 = pins.d4;
                    config.pin_d5 = pins.d5;
                    config.pin_d6 = pins.d6;
                    config.pin_d7 = pins.d7;
                    config.pin_xclk = pins.xclk;
                    config.pin_pclk = pins.pclk;
                    config.pin_vsync = pins.vsync;
                    config.pin_href = pins.href;
                    config.pin_sccb_sda = pins.sccb_sda;
                    config.pin_sccb_scl = pins.sccb_scl;
                    config.pin_pwdn = -1;
                    config.pin_reset = -1;

                    config.ledc_channel = LEDC_CHANNEL_0;
                    config.ledc_timer = LEDC_TIMER_0;
                    config.fb_count = 1;
                    config.frame_size = FRAMESIZE_QQVGA;
                    config.pixel_format = PIXFORMAT_JPEG;
                    config.jpeg_quality = 10;
                    config.xclk_freq_hz = 10000000ULL;
                    config.grab_mode = CAMERA_GRAB_LATEST;

                    camera_fb_t *frame;

                    if (esp_camera_init(&config) == ESP_OK) {
                        ESP_LOGI("Pinout", "Init ok! Trying to capture frame...");
                        frame = esp_camera_fb_get();

                        if (frame != NULL && frame->len > 0) {
                            ESP_LOGI("Pinout", "Match found!");
                            preferences.putChar(model, 1);
                            esp_camera_fb_return(frame);
                            esp_camera_deinit();
                            return true;
                        }
                        else
                            ESP_LOGI("Pinout", "Cannot capture frame, moving to next model...");
                    }

                    if (frame != NULL)
                        esp_camera_fb_return(frame);

                    esp_camera_deinit();
                    return false;
                }
            };
        }
    }
}

#endif
