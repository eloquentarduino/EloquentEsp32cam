#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO_H
#define ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO_H

#if defined(EI_CLASSIFIER_OBJECT_DETECTION)
    #include <edge-impulse-sdk/dsp/image/image.hpp>
    #include "../extra/exception.h"
    #include "../extra/time/benchmark.h"
    #include "./bbox.h"

    using Eloquent::Extra::Exception;
    using Eloquent::Extra::Time::Benchmark;
    using namespace eloq;

    namespace Eloquent {
        namespace Esp32cam {
            namespace EdgeImpulse {
                /**
                 * Run Edge Impulse FOMO model
                 */
                class FOMO {
                public:
                    ei::signal_t signal;
                    ei_impulse_result_t result;
                    EI_IMPULSE_ERROR error;
                    Exception exception;
                    Benchmark benchmark;
                    uint16_t input[EI_CLASSIFIER_RAW_SAMPLE_COUNT];
                    bbox_t first;

                    /**
                     *
                     */
                    FOMO() :
                        exception("FOMO"),
                        first(0, 0, 0, 0, 0),
                        _isDebugEnabled(false) {
                            signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
                            signal.get_data = [this](size_t offset, size_t length, float *out) {
                                const size_t i = 0;
                                const size_t end = offset + length;

                                for (int y = 0; y < EI_CLASSIFIER_INPUT_HEIGHT; y++) {
                                    const size_t offset_y = y * 2 * EI_CLASSIFIER_INPUT_WIDTH;

                                    for (int x = 0; x < EI_CLASSIFIER_INPUT_WIDTH; x++) {
                                        if (i >= end)
                                            break;

                                        if (i >= offset) {
                                            const uint16_t pixel = input[offset_y + x + x];
                                            const uint16_t r = (pixel & 0b1111100000000000) >> 11;
                                            const uint16_t b = (pixel & 0b111111000000) >> 6; // 5 bit also for green
                                            const uint16_t g = (pixel & 0b11111);
                                            const uint32_t gray = (r + g + b) / 3;

                                            out[i] = (gray << 16) | (gray << 8) | gray;
                                        }

                                        i += 1;
                                    }
                                }

                                return 0;
                            };
                    }


                    /**
                     *
                     * @param enabled
                     */
                    void debug(bool enabled = true) {
                        _isDebugEnabled = enabled;
                    }

                    /**
                     *
                     * @return
                     */
                    size_t getDspTime() {
                        return result.timing.dsp;
                    }

                    /**
                     *
                     * @return
                     */
                    size_t getClassificationTime() {
                        return result.timing.classification;
                    }

                    /**
                     *
                     */
                    size_t getTotalTime() {
                        return getDspTime() + getClassificationTime();
                    }

                    /**
                     *
                     * @return
                     */
                    template<typename Frame>
                    Exception& detectObjects(Frame& frame) {
                        if (camera.resolution.getWidth() != 96)
                            return exception.set("FOMO only works with camera.resolution.yolo()");

                        if (camera.resolution.getHeight() != 96)
                            return exception.set("FOMO only works with camera.resolution.yolo()");

                        if (!camera.pixformat.isRGB565())
                            return exception.set("FOMO only works with camera.pixformat.rgb565()");

                        if (!frame.length)
                            return exception.set("Cannot run FOMO on empty image");

                        benchmark.start();
                        crop();
                        error = run_classifier(&signal, &result, _isDebugEnabled);
                        benchmark.stop();

                        if (error != EI_IMPULSE_OK)
                            return exception.set(String("Failed to run classifier with error code ") + error);

                        return exception.clear();
                    }

                    /**
                     * Check if objects were found
                     */
                    bool found() {
                        return result.bounding_boxes[0].value != 0;
                    }

                    /**
                     * Check if objects were found
                     */
                    bool foundAnyObject() {
                        return found();
                    }

                    /**
                     * Run function on each bounding box found
                     */
                    template<typename Callback>
                    void forEach(Callback callback) {
                        for (size_t ix = 0, i = 0; ix < result.bounding_boxes_count; ix++) {
                            auto bb = result.bounding_boxes[ix];
                            bbox_t bbox(
                                bb.value,
                                bb.x,
                                bb.y,
                                bb.width,
                                bb.height
                            );

                            if (bbox.value != 0)
                                callback(i++, bbox);
                        }
                    }

                    /**
                     *
                     */
                    ei_impulse_result_bounding_box_t at(size_t ix) {
                        return result.bounding_boxes[ix];
                    }

                    /**
                     * Get count of (non background) bounding boxes
                     */
                    size_t count() {
                        size_t count = 0;

                        for (size_t ix = 0, i = 0; ix < result.bounding_boxes_count; ix++) {
                            auto bb = result.bounding_boxes[ix];

                            if (bb.value != 0)
                                count++;
                        }

                        return count;
                    }

                protected:
                    bool _isDebugEnabled;

                    /**
                     * Allow overriding
                     */
                    virtual void crop() {}
                };
            }
        }
    }

    namespace eloq {
        static Eloquent::Esp32cam::EdgeImpulse::FOMO fomo;
    }

#else
#error "EdgeImpulse FOMO library not found"
#endif

#endif //ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO
