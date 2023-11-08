#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_IMAGE_H
#define ELOQUENT_ESP32CAM_EDGEIMPULSE_IMAGE_H

#include <esp_camera.h>
#include "./classifier.h"

using namespace eloq;

#define _EI_RGB_ (EI_CLASSIFIER_NN_INPUT_FRAME_SIZE > EI_CLASSIFIER_RAW_SAMPLE_COUNT)


namespace Eloquent {
    namespace Esp32cam {
        namespace EdgeImpulse {
            /**
             * Edge Impulse image classification
             */
            class ImageClassification : public Classifier {
                public:
                    String label;
                    uint8_t ix;
                    float proba;

                    /**
                     * 
                     */
                    ImageClassification() :
                        Classifier(),
                        label(""),
                        ix(0),
                        proba(0) {
                            signal.get_data = [this](size_t offset, size_t length, float *out) {
                                ESP_LOGI("get_data", "get_data");

                                if (_buf == NULL)
                                    return 0;

                                size_t i = 0;
                                const size_t end = min((int) EI_CLASSIFIER_RAW_SAMPLE_COUNT, (int) (offset + length));
                                        
                                for (int y = 0; y < EI_CLASSIFIER_INPUT_HEIGHT; y++) {
                                    const size_t offset_y = ((int)(y * _dy)) * EI_CLASSIFIER_INPUT_WIDTH;

                                    for (int x = 0; x < EI_CLASSIFIER_INPUT_WIDTH; x++, i++) {
                                        if (i < offset)
                                            continue;

                                        if (i >= end)
                                            return 0;

                                        const size_t j = offset_y + ((int)(x * _dx));

                                        if (j >= _len)
                                            return 0;

                                        ESP_LOGI("get_data", "%d", j);
                                        continue;

                                        #if _EI_RGB_
                                            out[i - offset] = getRGB(_buf[j], _buf[j + 1]);
                                        #else
                                            out[i - offset] = getGray(_buf[j], _buf[j + 1]);
                                        #endif
                                    }
                                }

                                return 0;
                            };
                        }

                protected:
                    uint8_t *_buf;
                    size_t _len;
                    float _dx;
                    float _dy;

                    /**
                     * 
                     */
                    bool beforeClassification() {
                        if (camera.pixformat.isRGB565()) {
                            _buf = camera.frame->buf;
                            _len = camera.frame->len;
                        }
                        else if (camera.pixformat.isJpeg()) {
                            _buf = (uint8_t*) camera.rgb565.data;
                            _len = camera.rgb565.length;
                        }
                        else {
                            _buf = NULL;
                            _len = 0;
                            exception.set("Unknown image format. Only jpeg and RGB565 are allowed");
                            return false;
                        }

                        _dx = ((float) camera.resolution.getWidth()) / EI_CLASSIFIER_INPUT_WIDTH;
                        _dy = ((float) camera.resolution.getHeight()) / EI_CLASSIFIER_INPUT_HEIGHT;

                        return true;
                    }

                    /**
                     * 
                     */
                    void afterClassification() {
                        // find most probable class
                        for (uint8_t i = 0; i < EI_CLASSIFIER_LABEL_COUNT; i++) {
                            const float value = result.classification[i].value;

                            if (value > proba) {
                                ix = i;
                                proba = value;
                                label = ei_classifier_inferencing_categories[i];
                            } 
                        }
                    }

                    /**
                     * Convert high/low RGB565 bytes to R, G, B
                    */
                    void getRGBComponents(uint8_t h, uint8_t l, uint32_t *r, uint16_t *g, uint8_t *b) {
                        *r = (h & 0b11111000);
                        *g = ((h & 0b111) << 3) | ((l >> 5));
                        *b = (l & 0b11111) << 3;
                    }

                    /**
                     * Convert high/low RGB565 bytes to RGB 24bit
                     */
                    uint32_t getRGB(uint8_t h, uint8_t l) {
                        uint32_t r;
                        uint16_t g;
                        uint8_t b;

                        getRGBComponents(h, l, &r, &g, &b);

                        return (r << 16) | (g << 8) | b;
                    }

                    /**
                     * Convert high/low RGB565 bytes to RGB 24bit
                     */
                    uint32_t getGray(uint8_t h, uint8_t l) {
                        uint32_t r;
                        uint16_t g;
                        uint8_t b;

                        getRGBComponents(h, l, &r, &g, &b);
                        const uint32_t gray = (r + g + b) / 3;

                        return (gray << 16) | (gray << 8) | gray;
                    }
            };
        }
    }
}

#if EI_CLASSIFIER_OBJECT_DETECTION == 0
namespace eloq {
    namespace ei {
        static Eloquent::Esp32cam::EdgeImpulse::ImageClassification image_class;
    }
}
#endif

#endif