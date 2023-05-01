//
// Created by Simone on 15/11/22.
//

#ifndef ELOQUENTESP32CAM_SIMPLECHANGE_H
#define ELOQUENTESP32CAM_SIMPLECHANGE_H

#include "./Algorithm.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            class SimpleChange : public Algorithm {
            public:

                /**
                 *
                 */
                SimpleChange() :
                    _updateBackgroundOnly(false),
                    _alpha(0.95),
                    _diff(5) {

                }

                /**
                 * Set smoothing factor
                 * @param alpha
                 */
                inline void smooth(float alpha) {
                    _alpha = alpha;
                }

                /**
                 * Set min pixel value diff
                 * @param diff
                 */
                inline void differBy(uint8_t diff) {
                    _diff = diff;
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
                    if (!_updateBackgroundOnly || !isForeground)
                        _pixels[offset] = (1 - _alpha) * pixel + _alpha * _pixels[offset];
                }

                /**
                 *
                 * @param offset
                 * @param pixel
                 * @return
                 */
                bool isForeground(size_t offset, uint8_t pixel) {
                    uint8_t old = _pixels[offset];
                    uint8_t diff = pixel > old ? pixel - old : old - pixel;

                    return diff >= _diff;
                }

                /**
                 *
                 * @param param
                 * @param value
                 */
                void set(const String& param, const float value) {
                    if (param == "only-background")
                        onlyUpdateBackground(value > 0);
                    else if (param == "differ-by")
                        differBy(value);
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
                        + ", differ-by="
                        + _diff
                        + ", only-background="
                        + (_updateBackgroundOnly ? "yes" : "no");
                }

            protected:
                bool _updateBackgroundOnly;
                float _alpha;
                uint8_t _diff;
                uint8_t _pixels[MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64];
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_SIMPLECHANGE_H
