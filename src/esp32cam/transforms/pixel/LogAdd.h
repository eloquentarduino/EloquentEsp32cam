//
// Created by Simone on 06/01/23.
//

#ifndef ELOQUENTESP32CAM_LOGADD_H
#define ELOQUENTESP32CAM_LOGADD_H

#include "./PixelMapper.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Transforms {
            namespace Pixel {
                /**
                 * @papersWithCode "A MATHEMATICAL MODEL FOR LOGARITHMIC IMAGE PROCESSING"
                 */
                class LogAdd : public PixelMapper {
                public:

                    /**
                     *
                     * @param v
                     */
                    LogAdd(float v) {
                        float m = 9999;
                        float M = -9999;

                        if (v >= 1)
                            v /= 1000;

                        // create lookup table
                        for (uint16_t p = 1; p < 256; p += 2) {
                            float x = (v + p) / (v * p + 1);

                            _lookup[p >> 1] = x;

                            if (x < m) m = x;
                            if (x > M) M = x;
                        }

                        // normalize in range 0-255
                        float scale = 255.0f / (M - m);

                        for (uint8_t p = 0; p < (256 >> 1); p++)
                            _lookup[p] = constrain((_lookup[p] - m) * scale, 0, 255);
                    }

                    /**
                     *
                     * @param channel
                     * @param i
                     * @param pixel
                     * @return
                     */
                    inline uint8_t transform(uint8_t channel, size_t i, uint8_t pixel) {
                        return _lookup[pixel >> 1];
                    }

                protected:
                    float _lookup[256 >> 1];
                };
            }
        }
    }
}

#endif //ELOQUENTESP32CAM_LOGADD_H
