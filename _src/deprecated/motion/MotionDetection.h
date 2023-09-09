//
// Created by Simone on 10/05/23.
//

#ifndef ELOQUENTESP32CAM_MOTION_H
#define ELOQUENTESP32CAM_MOTION_H


#include "../../esp32camera/components/BitArray.h"
#include "../../esp32camera/Cam.h"

#ifndef MAX_RESOLUTION_WIDTH
#define MAX_RESOLUTION_WIDTH 1024
#endif

#ifndef MAX_RESOLUTION_HEIGHT
#define MAX_RESOLUTION_HEIGHT 768
#endif

#define MAX_RESOLUTION_WIDTH_ONE_EIGHT (MAX_RESOLUTION_WIDTH / 8)
#define MAX_RESOLUTION_HEIGHT_ONE_EIGHT (MAX_RESOLUTION_HEIGHT / 8)
#define NUM_PIXELS_ONE_EIGHT (MAX_RESOLUTION_WIDTH_ONE_EIGHT * MAX_RESOLUTION_HEIGHT_ONE_EIGHT)



namespace Eloquent {
    namespace Esp32cam {
        namespace Motion {
            /**
             * Motion detection
             */
            class MotionDetection {
            public:
                size_t epoch;
                size_t trainEpochs;
                uint8_t diffThreshold;
                float alfa;
                float beta;
                float previous[NUM_PIXELS_ONE_EIGHT];
                Eloquent::Esp32cam::Components::BitArray<NUM_PIXELS_ONE_EIGHT> foregroundMask;

                /**
                 *
                 */
                MotionDetection() :
                    epoch(0),
                    trainEpochs(30),
                    diffThreshold(10),
                    alfa(0.97),
                    beta(0.99) {
                    for (size_t i = 0; i < NUM_PIXELS; i++) {
                        previous[i] = 0;
                        foregroundMask.set(i, false);
                    }
                }
                
                /**
                 * 
                 * @param epochs 
                 */
                void setTrainingEpochs(size_t epochs) {
                    trainEpochs = epochs;
                }

                /**
                 *
                 * @param threshold
                 */
                void setPixelDiffThreshold(uint8_t threshold) {
                    diffThreshold = threshold;
                }

                /**
                 *
                 * @param smooth
                 */
                void setBackgroundSmoothing(float smooth) {
                    alfa = constrain(smooth, 0, 1);
                }

                /**
                 *
                 * @param smooth
                 */
                void setForegroundSmoothing(float smooth) {
                    beta = constrain(smooth, 0, 1);
                }

                /**
                 *
                 * @param frame
                 * @return
                 */
                bool update() {
                    jpegDecoder.decode();

                    uint8_t *frame = jpegDecoder.gray.pixels;
                    epoch += 1;
                    blur(frame);

                    // training phase
                    if (epoch == 1) {
                        for (uint16_t i = 0; i < NUM_PIXELS; i++)
                            previous[i] = frame[i];

                        return false;
                    }

                    if (epoch <= trainEpochs) {
                        updateAll(frame);
                        return false;
                    }

                    for (size_t i = 0; i < NUM_PIXELS; i++) {
                        const int16_t curr = frame[i];
                        const int16_t old = previous[i];
                        const uint8_t delta = abs(curr - old);

                        foregroundMask.set(i, delta > diffThreshold);
                    }

                    morph();
                    updateMasked(frame);

                    return true;
                }

                /**
                 * Update all pixels
                 */
                void updateAll(uint8_t *frame) {
                    for (size_t i = 0; i < NUM_PIXELS; i++)
                        previous[i] = alfa * previous[i] + (1 - alfa) * frame[i];
                }

                /**
                 * Update pixels based on mask
                 */
                void updateMasked(uint8_t *frame) {
                    for (size_t i = 0; i < NUM_PIXELS; i++) {
                        const float a = foregroundMask.get(i) ? beta : alfa;

                        previous[i] = a * previous[i] + (1 - a) * frame[i];
                    }
                }

                /**
                 * Naive, fast blur
                 */
                void blur(uint8_t *frame) {
                    for (uint16_t y = 0; y < jpegDecoder.gray.height - 1; y += 2) {
                        const uint16_t y_offset = y * jpegDecoder.gray.width;

                        for (uint16_t x = 0; x < jpegDecoder.gray.width - 1; x += 2) {
                            const uint16_t offset0 = y_offset + x;
                            const uint16_t offset1 = offset0 + jpegDecoder.gray.width;
                            const uint8_t a = frame[offset0 + 0];
                            const uint8_t b = frame[offset0 + 1];
                            const uint8_t c = frame[offset1 + 0];
                            const uint8_t d = frame[offset1 + 1];
                            const uint8_t m = max(a, max(b, max(c, d)));

                            frame[offset0 + 0] = m;
                            frame[offset0 + 1] = m;
                            frame[offset1 + 0] = m;
                            frame[offset1 + 1] = m;
                        }
                    }
                }

                /**
                 * Apply morphological operators to foreground mask
                 */
                void morph() {
                    const size_t height = jpegDecoder.gray.height;
                    const size_t width = jpegDecoder.gray.width;
                    
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
                    for (uint16_t y = 0; y < height - 3; y++) {
                        const uint16_t y_offset = y * width;

                        for (uint16_t x = 0; x < width - 2; x++) {
                            const uint16_t offset = y_offset + x;
                            uint8_t sum = 0;

                            for (uint8_t k = 0; k < 3; k++) {
                                const uint16_t offset_k = offset + k * width;

                                sum +=
                                    foregroundMask.get(offset_k + 0) +
                                    foregroundMask.get(offset_k + 1) +
                                    foregroundMask.get(offset_k + 2);

                                // early stopping
                                if (sum > 4)
                                    break;
                            }

                            foregroundMask.set(offset, sum > 4);
                        }
                    }
                }
            };
        }
    }
}

static Eloquent::Esp32cam::Motion::MotionDetection motionDetection;

#endif //ELOQUENTESP32CAM_MOTION_H
