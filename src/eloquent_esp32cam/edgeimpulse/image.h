#ifndef ELOQUENT_ESP32CAM_EDGEIMPULSE_IMAGE_H
#define ELOQUENT_ESP32CAM_EDGEIMPULSE_IMAGE_H

#include <esp_camera.h>
#include <edge-impulse-sdk/dsp/image/image.hpp>
#include "../extra/pubsub.h"
#include "./classifier.h"

using namespace eloq;
using eloq::camera;
using ei::signal_t;
#if defined(ELOQUENT_EXTRA_PUBSUB_H)
using Eloquent::Extra::PubSub;
#endif


#define _EI_RGB_ (EI_CLASSIFIER_NN_INPUT_FRAME_SIZE > EI_CLASSIFIER_RAW_SAMPLE_COUNT)


namespace Eloquent {
    namespace Esp32cam {
        namespace EdgeImpulse {
            /**
             * Edge Impulse image classification
             */
            class ImageClassifier : public Classifier {
                public:
                    String label;
                    uint8_t ix;
                    float proba;
                    size_t srcWidth;
                    size_t srcHeight;
                    #if defined(ELOQUENT_EXTRA_PUBSUB_H)
                    PubSub<ImageClassifier> mqtt;
                    #endif

                    /**
                     * 
                     */
                    ImageClassifier() :
                        Classifier(),
                        label(""),
                        ix(0),
                        proba(0),
                        #if defined(ELOQUENT_EXTRA_PUBSUB_H)
                        mqtt(this),
                        #endif
                        _buf(NULL),
                        _len(0) {

                            signal.get_data = [this](size_t offset, size_t length, float *out) {
                                return getData(offset, length, out);
                            };
                        }

                    /**
                     * Detect object from camera frame
                     */
                    virtual Exception& run() {
                        // run EI model
                        benchmark.benchmark([this]() {
                            camera.mutex.threadsafe([this]() {
                                if (!beforeClassification())
                                    return;

                                error = run_classifier(&signal, &result, _isDebugEnabled);
                            });
                        });

                        if (!exception.isOk())
                            return exception;

                        if (!camera.mutex.isOk())
                            return exception.set("Cannot acquire mutex for camera frame");

                        if (error != EI_IMPULSE_OK)
                            return exception.set(String("Failed to run classifier with error code 0x") + error);

                        afterClassification();
                        breakTiming();
                        
                        return exception.clear();
                    }

                    /**
                     * Convert to JSON string
                     */
                    virtual String toJSON() {
                        return String("{\"label\":\"") + label + "\",\"proba\":" + proba + "}";
                    }

                    /**
                     * Test if a MQTT payload is available
                     */
                    virtual bool shouldPub() {
                        return true;
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
                        if (!camera.hasFrame())
                            return exception.set("Cannot run EI model on empty frame").isOk();

                        if (camera.pixformat.isRGB565()) {
                            _buf = camera.frame->buf;
                            _len = camera.frame->len;
                            srcWidth = camera.resolution.getWidth();
                            srcHeight = camera.resolution.getHeight();

                            // rgb 565 bytes are swapped!
                            for (int i = 0; i < _len; i += 2) {
                                const uint8_t a = _buf[i];
                                const uint8_t b = _buf[i + 1];
                                
                                _buf[i] = b;
                                _buf[i + 1] = a;
                            }
                        }
                        else if (camera.pixformat.isJpeg()) {
                            const uint8_t ratioW = camera.resolution.getWidth() / EI_CLASSIFIER_INPUT_WIDTH;
                            const uint8_t ratioH = camera.resolution.getHeight() / EI_CLASSIFIER_INPUT_HEIGHT;
                            const uint8_t logScaleW = log(ratioW);
                            const uint8_t logScaleH = log(ratioH);
                            const uint8_t logScale  = min(logScaleW, logScaleH) / log(2);
                            const uint8_t scale = pow(2, logScale);
                            const uint16_t newWidth = camera.resolution.getWidth() / scale;
                            const uint16_t newHeight = camera.resolution.getHeight() / scale;
                            const uint16_t newSize = newWidth * newHeight;

                            if (_len != newSize * 2) {
                                ESP_LOGI(
                                    "EI", 
                                    "(Re)Allocating %d bytes for JPEG decoding into image of size %dx%d. Log scale = %d", 
                                    newSize * 2, 
                                    newWidth, 
                                    newHeight, 
                                    logScale
                                );
                                _len = newSize * 2;
                                srcWidth = newWidth;
                                srcHeight = newHeight;

                                if (_buf == NULL) _buf = (uint8_t*) ps_malloc(newSize * 2);
                                else _buf = (uint8_t*) ps_realloc(_buf, newSize * 2);
                            }

                            if (_buf == NULL)
                                return exception.set("Cannot allocate buffer memory");

                            if (!jpg2rgb565(camera.frame->buf, camera.frame->len, _buf, (jpg_scale_t) logScale))
                                return exception.set("Cannot convert frame from JPEG to RGB565").isOk();
                        }
                        else {
                            _buf = NULL;
                            _len = 0;
                            return exception.set("Unknown image format. Only jpeg and RGB565 are allowed").isOk();
                        }

                        if (_buf == NULL)
                            return exception.set("Error allocating memory buffer");

                        if (srcWidth < EI_CLASSIFIER_INPUT_WIDTH || srcHeight < EI_CLASSIFIER_INPUT_HEIGHT)
                            return exception.set("Cannot run EI model on resolution lower than model").isOk();

                        _dx = ((float) srcWidth) / EI_CLASSIFIER_INPUT_WIDTH;
                        _dy = ((float) srcHeight) / EI_CLASSIFIER_INPUT_HEIGHT;

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
                     * Get image data as RGB 24 bit
                     */
                    int getData(size_t offset, size_t length, float *out) {
                        size_t i = 0;
                        const size_t end = min((int) EI_CLASSIFIER_RAW_SAMPLE_COUNT, (int) (offset + length));

                        for (uint16_t y = 0; y < EI_CLASSIFIER_INPUT_HEIGHT; y++) {
                            const size_t offsetY = ((size_t) (y * _dy)) * srcWidth;

                            for (uint16_t x = 0; x < EI_CLASSIFIER_INPUT_WIDTH; x++, i++) {
                                if (i < offset)
                                    continue;

                                if (i >= end)
                                    return 0;

                                const size_t j = offsetY + (x * _dx);
                                const uint16_t pixel = ((uint16_t*) _buf)[j];
                                uint32_t r;
                                uint16_t g;
                                uint8_t b;

                                toRGB(pixel, &r, &g, &b);

                                #if _EI_RGB_
                                    out[i - offset] = (r << 16) | (g << 8) | b;
                                #else
                                    const uint32_t gray = constrain((r * 38 + g * 75 + b * 15) >> 7, 0, 255);
                                    out[i - offset] = (gray << 16) | (gray << 8) | gray;
                                #endif
                            }
                        }

                        return 0;
                    }

                    /**
                     * Convert high/low RGB565 bytes to R, G, B
                    */
                    void toRGB(const uint16_t pixel, uint32_t *r, uint16_t *g, uint8_t *b) {
                        *r = (pixel >> 8) & 0b11111000;
                        *g = (pixel & 0b11111100000) >> 3;
                        *b = (pixel & 0b11111) << 3;
                    }
            };
        }
    }
}

#if EI_CLASSIFIER_OBJECT_DETECTION == 0
namespace eloq {
    namespace ei {
        static Eloquent::Esp32cam::EdgeImpulse::ImageClassifier image_class;
    }
}
#endif

#endif