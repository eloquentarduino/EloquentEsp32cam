//
// Created by Simone on 26/11/22.
//

#ifndef ELOQUENTESP32CAM_MOTION_LOCALIZER_H
#define ELOQUENTESP32CAM_MOTION_LOCALIZER_H

#include "../Detector.h"

namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            namespace Localization {

                /**
                 * Abstract class for motion localizers
                 */
                class Localizer {
                public:
                    uint16_t x;
                    uint16_t y;
                    uint16_t width;
                    uint16_t height;
                    float coverage;
                    Detector *detector;

                    /**
                     * Convert to JSON
                     * @return
                     */
                    virtual String toJson() {
                        return String("{\"x\":")
                            + x
                            + ", \"y\":"
                            + y
                            + ", \"w\":"
                            + width
                            + ", \"h\":"
                            + height
                            + ", \"coverage\":"
                            + coverage
                            + "}";
                    }

                    /**
                     * Abstract localization algorithm
                     * @return
                     */
                    virtual bool localize() = 0;
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_MOTION_LOCALIZER_H
