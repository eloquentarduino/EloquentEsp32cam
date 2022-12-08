//
// Created by Simone on 07/09/22.
//

#ifndef ELOQUENTESP32CAM_SETMODELPINS_H
#define ELOQUENTESP32CAM_SETMODELPINS_H

namespace Eloquent {
    namespace Esp32cam {
        /**
         * Configure pins based on camera model
         */
        class SetModelPins {
        public:

            /**
             *
             */
            void aithinker() {
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
                _pins.sscb_sda = 26;
                _pins.sscb_scl = 27;
                _pins.pwdn = 32;
                _pins.reset = -1;
                _pins.flashlight = 4;
            }

            /**
             *
             */
            void m5() {
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
                _pins.sscb_sda = 25;
                _pins.sscb_scl = 23;
                _pins.pwdn = -1;
                _pins.reset = 15;
                _pins.flashlight = 16;
            }

            /**
             *
             */
            void m5wide() {
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
                _pins.sscb_sda = 22;
                _pins.sscb_scl = 23;
                _pins.pwdn = -1;
                _pins.reset = 15;
                _pins.flashlight = 14;
            }

            /**
             *
             */
            void eye() {
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
                _pins.sscb_sda = 18;
                _pins.sscb_scl = 23;
                _pins.pwdn = -1;
                _pins.reset = -1;
                _pins.flashlight = 22;
            }

            /**
             *
             */
            void wrover() {
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
                _pins.sscb_sda = 26;
                _pins.sscb_scl = 27;
                _pins.pwdn = -1;
                _pins.reset = -1;
                _pins.flashlight = -1;
            }

            /**
             *
             */
            void ttgoLCD() {
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
                _pins.sscb_sda = 18;
                _pins.sscb_scl = 23;
                _pins.pwdn = -1;
                _pins.reset = -1;
                _pins.flashlight = -1;
            }

            /**
             *
             */
            void simcam() {
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
                _pins.sscb_sda = 4;
                _pins.sscb_scl = 5;
                _pins.pwdn = -1;
                _pins.reset = 18;
                _pins.flashlight = -1;
            }

        protected:
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
                int8_t sscb_sda;
                int8_t sscb_scl;
                int8_t pwdn;
                int8_t reset;
                int8_t flashlight;
            } _pins;


            /**
             *
             * @param config
             */
            void setPins(camera_config_t *config) {
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
                config->pin_sscb_sda = _pins.sscb_sda;
                config->pin_sscb_scl = _pins.sscb_scl;
                config->pin_pwdn = _pins.pwdn;
                config->pin_reset = _pins.reset;

                if (_pins.flashlight >= 0)
                    pinMode(_pins.flashlight, OUTPUT);
            }

            /**
             * Test if model has been chosed
             *
             * @return
             */
            bool hasPinsSet() {
                return _pins.d0 != _pins.d1;
            }
        };
    }
}

#endif //ELOQUENTESP32CAM_SETMODELPINS_H
