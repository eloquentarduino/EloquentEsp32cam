#ifndef ELOQUENTESP32CAM_SETCLOCKSPEED_H
#define ELOQUENTESP32CAM_SETCLOCKSPEED_H


namespace Eloquent {
    namespace Esp32cam {
        namespace Features {
            /**
             * Configure quality of jpeg compression
             */
            class SetsClockFreq {
            public:
                size_t freq;

                /**
                 * Constructor
                 */
                SetsClockFreq() {
                    fast();
                }


                /**
                 * Freq = 20 MHz
                 */
                void fast() {
                    freq = 20000000;
                }

                /**
                 * Freq = 10 MHz
                 */
                void slow() {
                    freq = 10000000;
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_SETCLOCKSPEED_H
