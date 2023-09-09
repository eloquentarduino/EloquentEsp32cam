#ifndef ELOQUENT_ESP32CAM_CAMERA_BROWNOUT
#define ELOQUENT_ESP32CAM_CAMERA_BROWNOUT


#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Camera {
            /**
             * Disable brownout detector
             * (often triggers on AiThinker cam)
             */
            class Brownout {
            public:
                /**
                 * Disable detector
                 */
                void disable() {
                    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
                }

                /**
                 * Enable detector
                 */
                void enable() {
                    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 1);
                }
            };
        }
    }
}

#endif
