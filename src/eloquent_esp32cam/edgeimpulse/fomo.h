#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO_H
#define ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO_H

#if defined(EI_CLASSIFIER_OBJECT_DETECTION)
    #include <edge-impulse-sdk/dsp/image/image.hpp>
    #include "../extra/exception.h"
    #include "../extra/time/benchmark.h"
    #include "./bbox.h"

    #define _FOMO_IS_RGB_ (EI_CLASSIFIER_NN_INPUT_FRAME_SIZE > EI_CLASSIFIER_RAW_SAMPLE_COUNT)
    
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
                    bbox_t first;

                    /**
                     *
                     */
                    FOMO() :
                        exception("FOMO"),
                        fb(NULL),
                        first(0, 0, 0, 0, 0),
                        _isDebugEnabled(false) {
                            signal.total_length = EI_CLASSIFIER_RAW_SAMPLE_COUNT;
                            signal.get_data = [this](size_t offset, size_t length, float *out) {
                                if (fb == NULL)
                                    return 0;

                                #if EI_CLASSIFIER_INPUT_WIDTH == 49
                                    get_data_48x48(offset, length, out);
                                #else
                                    get_data_96x96(offset, length, out);
                                #endif

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
                    Exception& detectObjects(camera_fb_t *fb) {
                        if (EI_CLASSIFIER_INPUT_WIDTH != 48 || EI_CLASSIFIER_INPUT_WIDTH != 96)
                            return exception.set("You must use FOMO models trained on 48x48 or 96x96 images");

                        if (camera.resolution.getWidth() != 96)
                            return exception.set("FOMO only works with camera.resolution.yolo()");

                        if (camera.resolution.getHeight() != 96)
                            return exception.set("FOMO only works with camera.resolution.yolo()");

                        if (!camera.pixformat.isRGB565())
                            return exception.set("FOMO only works with camera.pixformat.rgb565()");

                        if (!fb->len)
                            return exception.set("Cannot run FOMO on empty image");

                        Serial.printf("EI raw sample count = %d\n", EI_CLASSIFIER_RAW_SAMPLE_COUNT);
                        Serial.printf("EI frame size = %d\n", EI_CLASSIFIER_NN_INPUT_FRAME_SIZE);

                        benchmark.start();
                        this->fb = fb;
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

                private:
                    camera_fb_t *fb;

                    /**
                     * Convert high/low RGB565 bytes to R, G, B
                    */
                    void get_r_g_b(uint8_t h, uint8_t l, uint32_t *r, uint16_t *g, uint8_t *b) {
                        *r = (h & 0b11111000);
                        *g = ((h & 0b111) << 3) | ((l >> 5));
                        *b = (l & 0b11111) << 3;
                    }

                    /**
                     * Convert high/low RGB565 bytes to RGB 24bit
                     */
                    uint32_t get_rgb(uint8_t h, uint8_t l) {
                        uint32_t r;
                        uint16_t g;
                        uint8_t b;

                        get_r_g_b(h, l, &r, &g, &b);

                        return (r << 16) | (g << 8) | b;
                    }

                    /**
                     * Convert high/low RGB565 bytes to RGB 24bit
                     */
                    uint32_t get_gray(uint8_t h, uint8_t l) {
                        uint32_t r;
                        uint16_t g;
                        uint8_t b;

                        get_r_g_b(h, l, &r, &g, &b);
                        const uint32_t gray = (r + g + b) / 3;

                        return (gray << 16) | (gray << 8) | gray;
                    }

                    /**
                     * 
                     */
                    size_t get_data_48x48(size_t offset, size_t length, float *out) {
                        size_t i = 0;
                        const size_t end = min((int) EI_CLASSIFIER_RAW_SAMPLE_COUNT, (int) (offset + length));
                                
                        for (int y = 0; y < 48; y++) {
                            // skip 1 in 2 rows
                            const size_t offset_y = y * 2 * 96;

                            for (int x = 0; x < 48; x++, i++) {
                                if (i < offset)
                                    continue;

                                if (i >= end)
                                    return 0;

                                const size_t j = 2 * (offset_y + x + x);

                                #if _FOMO_IS_RGB_
                                    out[i - offset] = get_rgb(fb->buf[j], fb->buf[j + 1]);
                                #else
                                    out[i - offset] = get_gray(fb->buf[j], fb->buf[j + 1]);
                                #endif
                            }
                        }
                    }

                    /**
                     * 
                     */
                    size_t get_data_96x96(size_t offset, size_t length, float *out) {
                        for (size_t i = 0, off = offset * 2; i < length; i++, off += 2) {
                            #if _FOMO_IS_RGB_
                                out[i] = get_rgb(fb->buf[off], fb->buf[off + 1]);
                            #else
                                out[i] = get_gray(fb->buf[off], fb->buf[off + 1]);
                            #endif
                        }
                    }
                };
            }
        }
    }

#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO_S3_H
    namespace eloq {
        static Eloquent::Esp32cam::EdgeImpulse::FOMO fomo;
    }
#endif

#else
#error "EdgeImpulse FOMO library not found"
#endif

#endif //ELOQUENT_ESP32CAM_EDGEIMPULSE_FOMO
