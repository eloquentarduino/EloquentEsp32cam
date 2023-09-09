//
// Created by Simone on 07/09/22.
//

#ifndef ELOQUENTESP32CAM_SETMODELPINS_H
#define ELOQUENTESP32CAM_SETMODELPINS_H


#include <esp_camera.h>
#include "../../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Features {
            /**
             * Configure pins based on camera model
             */
             template<class Camera>
            class ConfiguresPins : public HasErrorMessage {
            public:

                /**
                 *
                 * @param camera
                 */
                ConfiguresPins(Camera *camera) :
                    cam(camera) {

                }

                /**
                 *
                 */
                bool aithinker() {
                    _pins.d0 = 5;
                    _pins.d1 = 18;
                    _pins.d2 = 19;
                    _pins.d3 = 21;
                    _pins.d4 = 36;
                    _pins.d5 = 39;
                    _pins.d6 = 34;
                    _pins.d7 = 35;
                    _pins.xclk = 0;
                    _pins.pclk = 22;
                    _pins.vsync = 25;
                    _pins.href = 23;
                    _pins.sccb_sda = 26;
                    _pins.sccb_scl = 27;
                    _pins.pwdn = 32;
                    _pins.reset = -1;
                    _pins.flashlight = 4;

                    return true;
                }

                /**
                 *
                 */
                bool m5() {
                    _pins.d0 = 32;
                    _pins.d1 = 35;
                    _pins.d2 = 34;
                    _pins.d3 = 5;
                    _pins.d4 = 39;
                    _pins.d5 = 18;
                    _pins.d6 = 36;
                    _pins.d7 = 19;
                    _pins.xclk = 27;
                    _pins.pclk = 21;
                    _pins.vsync = 22;
                    _pins.href = 26;
                    _pins.sccb_sda = 25;
                    _pins.sccb_scl = 23;
                    _pins.pwdn = -1;
                    _pins.reset = 15;
                    _pins.flashlight = 16;

                    return true;
                }

                /**
                 *
                 */
                bool m5wide() {
                    _pins.d0 = 32;
                    _pins.d1 = 35;
                    _pins.d2 = 34;
                    _pins.d3 = 5;
                    _pins.d4 = 39;
                    _pins.d5 = 18;
                    _pins.d6 = 36;
                    _pins.d7 = 19;
                    _pins.xclk = 27;
                    _pins.pclk = 21;
                    _pins.vsync = 25;
                    _pins.href = 26;
                    _pins.sccb_sda = 22;
                    _pins.sccb_scl = 23;
                    _pins.pwdn = -1;
                    _pins.reset = 15;
                    _pins.flashlight = 14;

                    return true;
                }

                /**
                 *
                 */
                bool eye() {
                    _pins.d0 = 34;
                    _pins.d1 = 13;
                    _pins.d2 = 14;
                    _pins.d3 = 35;
                    _pins.d4 = 39;
                    _pins.d5 = 38;
                    _pins.d6 = 37;
                    _pins.d7 = 36;
                    _pins.xclk = 4;
                    _pins.pclk = 25;
                    _pins.vsync = 5;
                    _pins.href = 27;
                    _pins.sccb_sda = 18;
                    _pins.sccb_scl = 23;
                    _pins.pwdn = -1;
                    _pins.reset = -1;
                    _pins.flashlight = 22;

                    return true;
                }

                /**
                 *
                 */
                bool wrover() {
                    _pins.d0 = 4;
                    _pins.d1 = 5;
                    _pins.d2 = 18;
                    _pins.d3 = 19;
                    _pins.d4 = 36;
                    _pins.d5 = 39;
                    _pins.d6 = 34;
                    _pins.d7 = 35;
                    _pins.xclk = 21;
                    _pins.pclk = 22;
                    _pins.vsync = 25;
                    _pins.href = 23;
                    _pins.sccb_sda = 26;
                    _pins.sccb_scl = 27;
                    _pins.pwdn = -1;
                    _pins.reset = -1;
                    _pins.flashlight = -1;

                    return true;
                }

                /**
                 *
                 */
                bool ttgoLCD() {
                    _pins.d0 = 34;
                    _pins.d1 = 13;
                    _pins.d2 = 26;
                    _pins.d3 = 35;
                    _pins.d4 = 39;
                    _pins.d5 = 38;
                    _pins.d6 = 37;
                    _pins.d7 = 36;
                    _pins.xclk = 4;
                    _pins.pclk = 25;
                    _pins.vsync = 5;
                    _pins.href = 27;
                    _pins.sccb_sda = 18;
                    _pins.sccb_scl = 23;
                    _pins.pwdn = -1;
                    _pins.reset = -1;
                    _pins.flashlight = -1;

                    return true;
                }

                /**
                 *
                 */
                bool simcam() {
                    _pins.d0 = 11;
                    _pins.d1 = 9;
                    _pins.d2 = 8;
                    _pins.d3 = 10;
                    _pins.d4 = 12;
                    _pins.d5 = 17;
                    _pins.d6 = 16;
                    _pins.d7 = 15;
                    _pins.xclk = 14;
                    _pins.pclk = 13;
                    _pins.vsync = 6;
                    _pins.href = 7;
                    _pins.sccb_sda = 4;
                    _pins.sccb_scl = 5;
                    _pins.pwdn = -1;
                    _pins.reset = 18;
                    _pins.flashlight = -1;

                    return true;
                }

                /**
                 * 
                 * @return
                 */
                bool xiao() {
                    _pins.d0 = 15;
                    _pins.d1 = 17;
                    _pins.d2 = 18;
                    _pins.d3 = 16;
                    _pins.d4 = 14;
                    _pins.d5 = 12;
                    _pins.d6 = 11;
                    _pins.d7 = 48;
                    _pins.xclk = 10;
                    _pins.pclk = 13;
                    _pins.vsync = 38;
                    _pins.href = 47;
                    _pins.sccb_sda = 40;
                    _pins.sccb_scl = 39;
                    _pins.pwdn = -1;
                    _pins.reset = -1;
                    _pins.flashlight = -1;

                    return true;
                }

                /**
                 *
                 */
                void autodetect() {
                    _pins.d0 = _pins.d1 = 0;
                }

                /**
                 * Set model pins
                 *
                 * @return
                 */
                bool begin() {
                    if (_pins.d0 == _pins.d1) {
                        ESP_LOGW("Model not set, trying to auto-detect", "ConfiguresPins");

                        bool foundModel =
                                probe("AiThinker", [this]() { aithinker(); }) ||
                                probe("Esp-Eye", [this]() { eye(); }) ||
                                probe("M5", [this]() { m5(); }) ||
                                probe("M5Wide", [this]() { m5wide(); }) ||
                                probe("Wrover", [this]() { wrover(); }) ||
                                probe("TTGO LCD", [this]() { ttgoLCD(); }) ||
                                probe("SimCam", [this]() { simcam(); });

                        return foundModel ? setErrorMessage("") : setErrorMessage("Cannot detect model", "ConfiguresPins");
                    }

                    camera_config_t *config = cam->getConfig();
                    assign(config);

                    return esp_camera_init(config) == ESP_OK ? setErrorMessage("") : setErrorMessage("Cannot init camera");
                }

            protected:
                Camera *cam;
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
                } _pins;


                /**
                 *
                 * @tparam PinAssignment
                 * @param model
                 * @param pinAssignment
                 * @return
                 */
                template<typename PinAssignment>
                bool probe(const char *model, PinAssignment pinAssignment) {
                    ESP_LOGD("ConfiguresPins", "Probing camera model %s", model);
                    pinAssignment();

                    camera_config_t *config = cam->getConfig();

                    assign(config);

                    if (esp_camera_init(config) != ESP_OK)
                        return setErrorMessage("Probe failed", "ConfiguresPins");

                    ESP_LOGI("ConfiguresPins", "Probe succeded");

                    return setErrorMessage("");
                }

                /**
                 *
                 * @param config
                 */
                void assign(camera_config_t *config) {
                    config->pin_d0 = _pins.d0;
                    config->pin_d1 = _pins.d1;
                    config->pin_d2 = _pins.d2;
                    config->pin_d3 = _pins.d3;
                    config->pin_d4 = _pins.d4;
                    config->pin_d5 = _pins.d5;
                    config->pin_d6 = _pins.d6;
                    config->pin_d7 = _pins.d7;
                    config->pin_xclk = _pins.xclk;
                    config->pin_pclk = _pins.pclk;
                    config->pin_vsync = _pins.vsync;
                    config->pin_href = _pins.href;
                    config->pin_sccb_sda = _pins.sccb_sda;
                    config->pin_sccb_scl = _pins.sccb_scl;
                    config->pin_pwdn = _pins.pwdn;
                    config->pin_reset = _pins.reset;
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_SETMODELPINS_H
