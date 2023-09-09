//
// Created by Simone on 19/11/22.
//

#ifndef ELOQUENTESP32CAM_SINGLE_GAUSSIAN_H
#define ELOQUENTESP32CAM_SINGLE_GAUSSIAN_H

#include "./Algorithm.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             *
             */
            class SingleGaussian : public Algorithm {
            public:

                /**
                 *
                 */
                SingleGaussian() :
                    _updateBackgroundOnly(false),
                    _alpha(0.95),
                    _k(2.5) {

                }

                /**
                 * Set smoothing factor
                 * @param alpha
                 */
                inline void smooth(float alpha) {
                    _alpha = alpha;
                }

                /**
                 * Set k
                 * @param k
                 */
                inline void k(float k) {
                    _k = k;
                }

                /**
                 *
                 * @param onlyUpgradeBackground
                 */
                inline void onlyUpdateBackground(bool onlyUpgradeBackground = true) {
                    _updateBackgroundOnly = onlyUpgradeBackground;
                }

                /**
                 *
                 * @param pixels
                 * @param length
                 */
                void initialize(uint8_t *pixels, size_t length) {
                    memcpy(_pixels, pixels, length);
                    memset(_variance, 10, length);
                }

                /**
                 *
                 * @param pixels
                 * @param length
                 */
                void train(uint8_t *pixels, size_t length) {
                    // no specific training
                    for (size_t i = 0; i < length; i++)
                        update(i, pixels[i], false);
                }

                /**
                 *
                 * @param offset
                 * @param pixel
                 * @param isForeground
                 */
                void update(size_t offset, uint8_t pixel, bool isForeground) {
                    if (!_updateBackgroundOnly || !isForeground) {
                        const uint8_t old = _pixels[offset];
                        const float diff = pixel > old ? pixel - old : old - pixel;

                        _pixels[offset] = (1 - _alpha) * pixel + _alpha * old;
                        _variance[offset] = (1 - _alpha) * diff + _alpha * _variance[offset];
                    }
                }

                /**
                 *
                 * @param offset
                 * @param pixel
                 * @return
                 */
                bool isForeground(size_t offset, uint8_t pixel) {
                    const uint8_t old = _pixels[offset];
                    const uint8_t diff = pixel > old ? pixel - old : old - pixel;

                    return diff > _k * _variance[offset];
                }

                /**
                 *
                 * @param param
                 * @param value
                 */
                void set(const String& param, const float value) {
                    if (param == "only-background")
                        onlyUpdateBackground(value > 0);
                    else if (param == "k")
                        k(value);
                    else if (param == "smooth")
                        smooth(value);
                }

                /**
                 * Get current configuration as string
                 * @return
                 */
                String getCurrentConfig() {
                    return String("smooth=")
                        + _alpha
                        + ", k="
                        + _k
                        + ", only-background="
                        + (_updateBackgroundOnly ? "yes" : "no");
                }

            protected:
                bool _updateBackgroundOnly;
                float _alpha;
                float _k;
                uint8_t _pixels[MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64];
                uint8_t _variance[MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64];
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_SINGLE_GAUSSIAN_H
