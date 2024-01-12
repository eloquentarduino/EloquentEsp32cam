#ifndef ELOQUENT_EXTRA_ESP32_FS_SDMMC_PINS
#define ELOQUENT_EXTRA_ESP32_FS_SDMMC_PINS

namespace Eloquent {
    namespace Extra {
        namespace Esp32 {
            namespace Fs {
                /**
                 * Define SDMMC pins for common boards
                 */
                class Pinout {
                    public:
                        struct {
                            uint8_t clk;
                            uint8_t cmd;
                            uint8_t d0;
                            uint8_t d1;
                            uint8_t d2;
                            uint8_t d3;
                        } pins;

                        /**
                         * 
                         */
                        Pinout() {
                            pins.clk = 0;
                            pins.cmd = 0;
                            pins.d0  = 0;
                            pins.d1  = 0;
                            pins.d2  = 0;
                            pins.d3  = 0;
                        }

                        /**
                         * 
                         */
                        void freenove_s3() {
                            clk(39);
                            cmd(38);
                            d0(40);
                        }

                        /**
                         * Set CLK pin
                         */
                        void clk(uint8_t clk) {
                            pins.clk = clk;
                        }

                        /**
                         * Set CMD pin
                         */
                        void cmd(uint8_t cmd) {
                            pins.cmd = cmd;
                        }

                        /**
                         * Set D0 pin
                         */
                        void d0(uint8_t d0) {
                            pins.d0 = d0;
                        }

                        /**
                         * Set D1 pin
                         */
                        void d1(uint8_t d1) {
                            pins.d1 = d1;
                        }

                        /**
                         * Set D2 pin
                         */
                        void d2(uint8_t d2) {
                            pins.d2 = d2;
                        }

                        /**
                         * Set D3 pin
                         */
                        void d3(uint8_t d3) {
                            pins.d3 = d3;
                        }
                };
            }
        }
    }
}

#endif