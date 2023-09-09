//
// Created by Simone on 09/06/23.
//

#ifndef ELOQUENTESP32CAM_MOTION_H
#define ELOQUENTESP32CAM_MOTION_H


#include "../../esp32camera/Cam.h"
#include "../../esp32camera/components/BitMatrix.h"
#include "../../traits/HasErrorMessage.h"


namespace Eloquent {
    namespace Esp32Cam {
        namespace Motion {

            /**
             * Perform motion detection
             */
            template<uint16_t width, uint16_t height>
            class Motion : public HasErrorMessage {
            public:
                BitMatrix<height, width> mask;
                uint8_t frame[height * width];

                /**
                 *
                 */
                Motion() :
                    epoch(0),
                    trainEpochs(10),
                    retrainAfter(0),
                    onlyBackground(true),
                    alfa(0.95) {

                }


                /**
                 *
                 * @param epochs
                 */
                inline void setTrainingEpochs(size_t epochs) {
                    trainEpochs = epochs;
                }

                /**
                 * Set smoothing factor (in the range 0-1)
                 * The higher the value, the more the current frame
                 * is taken into account.
                 * Recommended value is 0.05-0.1
                 * @param a
                 */
                inline void setSmoothingFactor(float a) {
                    alfa = 1 - constrain(a, 0, 1);
                }

                /**
                 *
                 * @return
                 */
                inline bool isTraining() {
                    return epoch <= trainEpochs;
                }

                /**
                 * Retrain algorithm after given number of epochs
                 *
                 * @param epochs
                 */
                inline void retrainAfter(size_t epochs) {
                    retrainAfter = epochs;
                }

                /**
                 * Restart training
                 */
                inline void retrain() {
                    epoch = 0;
                }

                /**
                 *
                 * @param only
                 */
                inline void onlyUpdateBackground(bool only) {
                    onlyBackground = only;
                }

                /**
                 * Update background model and run detection
                 */
                bool update() {
                    if (!camera.captured())
                        return setErrorMessage("No frame to update motion background model");

                    // try to decode jpeg, if not already
                    if (!jpeg.decoded())
                        jpeg.decode();

                    if (!jpeg.decoded())
                        return setErrorMessage("Can't decode JPEG frame");

                    if (epoch > retrainAfter)
                        retrain();

                    if (epoch == 1) {
                        updateBackgroundModel(0);

                        return setErrorMessage("Initializing...");
                    }

                    if (isTraining()) {
                        updateBackgroundModel(0.75);

                        return setErrorMessage("Training...");
                    }

                    detectForeground();
                    updateBackgroundModel(alfa);

                    return clearError();
                }

            protected:
                size_t epoch;
                size_t trainEpochs;
                size_t retrainAfter;
                bool onlyBackground;
                float alfa;


                /**
                 * Update background model (exponential smoothing)
                 */
                void updateBackgroundModel(float alfa) {
                    size_t i = 0;

                    for (uint16_t y = 0; y < height; y++) {
                        for (uint16_t x = 0; x < width; x++) {
                            if (!onlyBackground || isTraining() || mask.get(i)) {
                                //const uint8_t pixel = blur(x, y, i);
                                const uint8_t pixel = jpeg.gray.pixels[i];

                                frame[i] = alfa * frame[i] + (1 - alfa) * pixel;
                            }

                            i += 1;
                        }
                    }
                }

                /**
                 * Detect foreground mask
                 */
                void detectForeground() {
                    for (size_t i = 0; i < width * height; i++) {
                        int8_t delta = abs(((int16_t) frame[i]) - jpeg.gray.pixels[i]);

                        mask.set(i, delta > thresh);
                    }

                    morph();
                }

                /**
                 * Fast "blurring"
                 */
                uint8_t blur(uint16_t x, uint16_t y, size_t i) {
                    if (x == width - 1 || y == height - 1)
                        return jpeg.gray.pixels[i];

                    const uint8_t *pixels = jpeg.gray.pixels;
                    const uint8_t a = frame[i];
                    const uint8_t b = frame[i + 1];
                    const uint8_t c = frame[i + width];
                    const uint8_t d = frame[i + width + 1];

                    return max(a, max(b, max(c, d)));
                }

                /**
                 * Apply morphological operators to foreground mask
                 */
                void morph() {
                    // approx. erosion
                    for (uint16_t y = 0; y < height - 2; y++) {
                        const uint16_t y_offset = y * width;

                        for (uint16_t x = 0; x < width - 2; x++) {
                            const uint16_t offset0 = y_offset + x;
                            const uint16_t offset1 = offset0 + width;
                            const uint16_t offset2 = offset1 + width;

                            if (!mask.get(offset0))
                                continue;

                            const uint8_t sum = 1
                                                + mask.get(offset0 + 1)
                                                + mask.get(offset0 + 2)
                                                + mask.get(offset1 + 0)
                                                + mask.get(offset1 + 1)
                                                + mask.get(offset1 + 2)
                                                + mask.get(offset2 + 0)
                                                + mask.get(offset2 + 1)
                                                + mask.get(offset2 + 2);

                            mask.set(offset0, sum > 4);
                        }
                    }

                    // approx. dilation
                    for (uint16_t y = 0; y < HEIGHT - 5; y++) {
                        const uint16_t y_offset = y * WIDTH;

                        for (uint16_t x = 0; x < WIDTH - 2; x++) {
                            const uint16_t offset = y_offset + x;
                            uint8_t sum = 0;

                            for (uint8_t k = 0; k < 5; k++) {
                                const uint16_t offset_k = offset + k * WIDTH;

                                sum +=
                                        mask.get(offset_k + 0) +
                                        mask.get(offset_k + 1) +
                                        mask.get(offset_k + 2);

                                // early stopping
                                if (sum > 7)
                                    break;
                            }

                            mask.set(offset, sum > 7);
                        }
                    }
                }
            };
        }
    }
}

#endif //ELOQUENTESP32CAM_MOTION_H
