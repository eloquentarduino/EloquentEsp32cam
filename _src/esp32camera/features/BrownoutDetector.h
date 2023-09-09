//
// Created by Simone on 01/05/23.
//

#ifndef ELOQUENTESP32CAM_DISABLESBROWNOUTDETECTOR_H
#define ELOQUENTESP32CAM_DISABLESBROWNOUTDETECTOR_H


#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Features {
            class BrownoutDetector {
            public:

                /**
                 *
                 */
                BrownoutDetector() :
                    disableAutomatically(true) {

                }

                /**
                 * Don't disable automatically
                 */
                void shouldntDisable() {

                }

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

                /**
                 *
                 */
                void begin() {
                    if (disableAutomatically)
                        disable();
                }

            protected:
                bool disableAutomatically;
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_DISABLESBROWNOUTDETECTOR_H
