#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO
#define ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO

#if defined(EI_CLASSIFIER_OBJECT_DETECTION)
    #include <edge-impulse-sdk/dsp/image/image.hpp>
    #include "../extra/exception.h"
    #include "../extra/benchmark.h"

    using Eloquent::Extra::Exception;
    using Eloquent::Extra::Benchmark;
    using namespace e;

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
                    uint32_t data[EI_CLASSIFIER_RAW_SAMPLE_COUNT];

                    /**
                     *
                     */
                    FOMO() :
                        exception("FOMO"),
                        isDebugEnabled(false) {
                            isRGB = (EI_CLASSIFIER_NN_INPUT_FRAME_SIZE / EI_CLASSIFIER_RAW_SAMPLE_COUNT) > 1;
                            signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
                            signal.get_data = [this](size_t offset, size_t length, float *out) {
                                for (size_t i = 0; i < length; i++)
                                    out[i] = data[offset + i];

                                return 0;
                            };
                    }


                    /**
                     *
                     * @param enabled
                     */
                    void debug(bool enabled = true) {
                        isDebugEnabled = enabled;
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
                    Exception& detectObjects() {
                        if (!camera.hasFrame())
                            return exception.set("Cannot predict from empty frame");

                        if (!camera.rgb565.convert().isOk())
                            return exception.set("Cannot convert to RGB565");

                        if (!isRGB)
                            camera.rgb565.toGrayscale();

                        benchmark.start();
                        crop();
                        error = run_classifier(&signal, &result, isDebugEnabled);
                        benchmark.stop();

                        if (error != EI_IMPULSE_OK)
                            return exception.set(String("Failed to run classifier. Error code ") + error);

                        return exception.clear();
                    }

                    /**
                     * Check if objects were found
                     */
                    bool hasObjects() {
                        return result.bounding_boxes[0].value != 0;
                    }

                    /**
                     * Run function on each bounding box found
                     */
                    template<typename Callback>
                    void forEach(Callback callback) {
                        for (size_t ix = 0, i = 0; ix < result.bounding_boxes_count; ix++) {
                            auto bb = result.bounding_boxes[ix];

                            if (bb.value != 0)
                                callback(i++, bb);
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
                    bool isDebugEnabled;
                    bool isRGB;

                    /**
                     * 
                     */
                    void crop() {
                        const float src_width = camera.rgb565.width;
                        const float src_height = camera.rgb565.height;
                        const size_t limit = (src_width - 1) * (src_height * 1);
                        const float dx = src_width / EI_CLASSIFIER_INPUT_WIDTH;
                        const float dy = src_height / EI_CLASSIFIER_INPUT_HEIGHT;

                        for (size_t y = 0; y < EI_CLASSIFIER_INPUT_HEIGHT; y++) {
                            const size_t dest_y = y * EI_CLASSIFIER_INPUT_WIDTH;
                            const size_t src_y = constrain(dy * y, 0, src_height - 1);
                            const size_t offset_y = src_y * src_width;
                            
                            for (size_t x = 0; x < EI_CLASSIFIER_INPUT_WIDTH; x++) {
                                const size_t pixel_offset = offset_y + constrain(x * dy, 0, src_width - 1);
                                const uint32_t pixel = camera.rgb565.as888(pixel_offset);

                                data[dest_y + x] = pixel;
                            }
                        }
                    }
                };
            }
        }
    }

    namespace e {
        static Eloquent::Esp32cam::EdgeImpulse::FOMO fomo;
    }

#else
#error "EdgeImpulse FOMO library not found"
#endif

#endif //ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO
