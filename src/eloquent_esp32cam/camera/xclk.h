#ifndef ELOQUENT_ESP32CAM_CAMERA_XCLK
#define ELOQUENT_ESP32CAM_CAMERA_XCLK


namespace Eloquent {
    namespace Esp32cam {
        namespace Camera {
            /**
             * Configure camera XCLK freq
             */
            class XCLK {
            public:
                size_t freq;

                /**
                 * Constructor
                 */
                XCLK() {
                    fast();
                }

                /**
                 * Freq = 20 MHz
                 */
                void fast() {
                    freq = 20000000ULL;
                }

                /**
                 * Freq = 10 MHz
                 */
                void slow() {
                    freq = 10000000ULL;
                }
            };
        }
    }
}

#endif
