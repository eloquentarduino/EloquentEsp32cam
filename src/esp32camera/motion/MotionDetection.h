//
// Created by Simone on 04/07/23.
//

#ifndef ELOQUENTESP32CAM_MOTIONCLASS_H
#define ELOQUENTESP32CAM_MOTIONCLASS_H

#include "../../traits/HasErrorMessage.h"
#include "../features/Benchmark.h"
#include "../components/BitMatrix.h"


namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             * Motion detector
             */
            class MotionDetection : public HasErrorMessage {
            public:
                uint8_t *background;
                Components::BitMatrix foregroundMask;
                Features::Benchmark benchmark;

                /**
                 *
                 */
                MotionDetection() :
                    foregroundMask(0, 0),
                    _trainEpochs(30),
                    _pixThresh(10),
                    _countThresh(0.2),
                    _alpha(0.98),
                    _shouldBlur(true),
                    _shouldMorph(true),
                    _onlyBackground(true) {
                }

                /**
                 * The larger the value, the more weight
                 * past frames will have
                 *
                 * @param weight
                 */
                void setHistoryWeight(float weight) {
                    _alpha = weight;
                }

                /**
                 * Set how much the new pixel must differ from the old
                 * to mark it as "foreground"
                 * @param thresh
                 */
                void setPixelDeltaThreshold(uint8_t thresh) {
                    _pixThresh = thresh;
                }

                /**
                 * Set how many pixels should be marked as foreground
                 * for motion to be triggered.
                 * If in range [0, 1[, it is interpreted as a percent
                 * of the total number of pixels
                 *
                 * @param thresh
                 */
                void setCountThreshold(float thresh) {
                    _countThresh = thresh;
                }

                /**
                 * If true, applies a naive blurring to reduce
                 * noise sensitivity
                 *
                 * @param should
                 */
                void shouldBlurBeforeDetection(bool should) {
                    _shouldBlur = should;
                }

                /**
                 * If true, applies a naive closing to reduce
                 * noise sensitivity
                 *
                 * @param should
                 */
                void shouldSmoothDetectionNoise(bool should) {
                    _shouldMorph = should;
                }

                /**
                 * If true (recommended), only updates background
                 * pixels (so moving objects doesn't interfere with
                 * background modelling)
                 *
                 * @param should
                 */
                void shouldOnlyUpdateBackground(bool should) {
                    _onlyBackground = should;
                }

                /**
                 * Test if model is still training
                 *
                 * @return
                 */
                inline bool isTraining() {
                    return _epoch <= _trainEpochs;
                }

                /**
                 * Decode JPEG (required) and update background model
                 */
                bool decodeAndUpdate() {
                    if (!jpeg.decode())
                        return setErrorMessage(jpeg.getErrorMessage());

                    return update();
                }

                /**
                 *
                 * @return
                 */
                bool update() {
                    if (!jpeg.gray.length)
                        return setErrorMessage("No JPEG decoded frame to perform motion detection on");

                    if (!allocate())
                        return setErrorMessage("Cannot allocate memory for background modelling");

                    _epoch += 1;

                    if (_shouldBlur)
                        blur();

                    // first frame, just copy the current frame
                    if (_epoch == 1) {
                        memcpy(background, jpeg.gray.pixels, getPixCount());

                        return false;
                    }

                    // training
                    if (isTraining()) {
                        updateAllPixels();

                        return false;
                    }

//                    const size_t width = foregroundMask.getRows();
//                    const size_t height = foregroundMask.getCols();
//
//                    for (int i = 0; i < getPixCount(); i++) {
//                        Serial.print(background[i]);
//                        Serial.print(',');
//
//                        if (i % width == width - 1)
//                            Serial.println();
//                    }
//
//                    delay(5000);

                    // running
                    benchmark.start();

                    for (size_t i = 0, pixCount = getPixCount(); i < pixCount; i++) {
                        const uint8_t curr = jpeg.gray.pixels[i];
                        const uint8_t old = background[i];
                        const bool foreground = isForeground(i, curr, old);

                        foregroundMask.set(i, foreground);
                    }

                    if (_shouldMorph)
                        morph();

                    if (_onlyBackground)
                        updateMasked();
                    else
                        updateAllPixels();

                    benchmark.stop();

                    return true;
                }

                /**
                 * Test if motion happened
                 * @return
                 */
                bool detected() {
                    const size_t width = foregroundMask.getRows();
                    const size_t height = foregroundMask.getCols();
                    const size_t sum = foregroundMask.sum();
                    const size_t thresh = getAbsCountThresh();

                    Serial.printf("%lu forground over %lu\n", sum, thresh);

                    return sum > thresh;
                }

                void debug() {
                    const size_t width = foregroundMask.getRows();
                    const size_t height = foregroundMask.getCols();

                    for (int i = 0; i < getPixCount(); i++) {
                        Serial.print(background[i]);
                        Serial.print(',');

                        if (i % width == width - 1)
                            Serial.println();
                    }

                    Serial.println("\n\n");

                    for (int i = 0; i < getPixCount(); i++) {
                        Serial.print(foregroundMask.get(i) ? 255 : 0);
                        Serial.print(',');

                        if (i % width == width - 1)
                            Serial.println();
                    }
                }

            protected:
                size_t _epoch;
                size_t _trainEpochs;
                uint8_t _pixThresh;
                float _countThresh;
                float _alpha;
                bool _shouldBlur;
                bool _shouldMorph;
                bool _onlyBackground;

                /**
                 * Allocate memory for background modelling
                 * @return
                 */
                bool allocate() {
                    const size_t jpegWidth = jpeg.getDecodedWidth();
                    const size_t jpegHeight = jpeg.getDecodedHeight();

                    if (foregroundMask.getRows() != jpegWidth
                        || foregroundMask.getCols() != jpegHeight) {

                        if (!foregroundMask.resize(jpegWidth, jpegHeight))
                            return setErrorMessage("Cannot allocate memory for foreground mask");

                        free(background);
                        background = (uint8_t *) calloc(jpegWidth * jpegHeight, sizeof(uint8_t));

                        return background != NULL;
                    }

                    return true;
                }

                /**
                 * Get number of pixels
                 *
                 * @return
                 */
                inline const size_t getPixCount() {
                    return jpeg.getDecodedWidth() * jpeg.getDecodedHeight();
                }

                /**
                 * Get number of pixels that need to change for motion
                 * to be triggered
                 *
                 * @return
                 */
                inline const size_t getAbsCountThresh() {
                    return _countThresh < 1 ? _countThresh * getPixCount() : _countThresh;
                }

                /**
                 *
                 */
                void updateAllPixels() {
                    const float a = isTraining() ? 0.5 : _alpha;

                    for (size_t i = 0, pixCount = getPixCount(); i < pixCount; i++)
                        background[i] = a * background[i] + (1 - a) * jpeg.gray.pixels[i];
                }

                /**
                 * Update pixels based on mask
                 */
                void updateMasked() {
                    for (size_t i = 0, pixCount = getPixCount(); i < pixCount; i++) {
                        const float a = foregroundMask.get(i) ? sqrt(_alpha) : _alpha;

                        background[i] = a * background[i] + (1 - a) * jpeg.gray.pixels[i];
                    }
                }

                /**
                 * Test is current pixel must be marked as foreground
                 *
                 * @param i
                 * @param current
                 * @param old
                 * @return
                 */
                inline bool isForeground(const size_t i, const int16_t current, const int16_t old) {
                    return abs(current - old) > _pixThresh;
                }

                /**
                 * Naive blur
                 */
                void blur() {
                    const size_t height = foregroundMask.getCols();
                    const size_t width = foregroundMask.getRows();

                    for (uint16_t y = 0; y < height - 1; y += 2) {
                        const uint16_t y_offset = y * width;

                        for (uint16_t x = 0; x < width - 1; x += 2) {
                            const uint16_t offset0 = y_offset + x;
                            const uint16_t offset1 = offset0 + width;
                            const uint8_t a = jpeg.gray.pixels[offset0 + 0];
                            const uint8_t b = jpeg.gray.pixels[offset0 + 1];
                            const uint8_t c = jpeg.gray.pixels[offset1 + 0];
                            const uint8_t d = jpeg.gray.pixels[offset1 + 1];
                            const uint8_t m = max(a, max(b, max(c, d)));

                            jpeg.gray.pixels[offset0 + 0] = m;
                            jpeg.gray.pixels[offset0 + 1] = m;
                            jpeg.gray.pixels[offset1 + 0] = m;
                            jpeg.gray.pixels[offset1 + 1] = m;
                        }
                    }
                }

                /**
                 * Apply (approx) morphological operators to foreground mask
                 * to smooth out detection noise
                 */
                void morph() {
                    const size_t height = foregroundMask.getCols();
                    const size_t width = foregroundMask.getRows();
                    
                    // approx. erosion
                    for (uint16_t y = 0; y < height - 2; y++) {
                        const uint16_t y_offset = y * width;

                        for (uint16_t x = 0; x < width - 2; x++) {
                            const uint16_t offset0 = y_offset + x;
                            const uint16_t offset1 = offset0 + width;
                            const uint16_t offset2 = offset1 + width;

                            if (!foregroundMask.get(offset0))
                                continue;

                            const uint8_t sum = 1
                                                + foregroundMask.get(offset0 + 1)
                                                + foregroundMask.get(offset0 + 2)
                                                + foregroundMask.get(offset1 + 0)
                                                + foregroundMask.get(offset1 + 1)
                                                + foregroundMask.get(offset1 + 2)
                                                + foregroundMask.get(offset2 + 0)
                                                + foregroundMask.get(offset2 + 1)
                                                + foregroundMask.get(offset2 + 2);

                            foregroundMask.set(offset0, sum > 4);
                        }
                    }

                    // approx. dilation
                    for (uint16_t y = 0; y < height - 5; y++) {
                        const uint16_t y_offset = y * width;

                        for (uint16_t x = 0; x < width - 2; x++) {
                            const uint16_t offset = y_offset + x;
                            uint8_t sum = 0;

                            for (uint8_t k = 0; k < 5; k++) {
                                const uint16_t offset_k = offset + k * width;

                                sum +=
                                        foregroundMask.get(offset_k + 0) +
                                        foregroundMask.get(offset_k + 1) +
                                        foregroundMask.get(offset_k + 2);

                                // early stopping
                                if (sum > 7)
                                    break;
                            }

                            foregroundMask.set(offset, sum > 7);
                        }
                    }
                }

            };
        }
    }
}


static Eloquent::Esp32cam::Motion::MotionDetection motionDetection;

#endif //ELOQUENTESP32CAM_MOTIONCLASS_H
