#ifndef ELOQUENT_ESP32CAM_JPEG_YCBCR_H
#define ELOQUENT_ESP32CAM_JPEG_YCBCR_H 1

#include <stdio.h>
#include <stdlib.h>
#include "../camera/camera.h"
#include "./picojpeg.h"
#include "../extra/exception.h"
#include "../extra/time/benchmark.h"

using eloq::camera;
using Eloquent::Error::Exception;
using Eloquent::Extra::Time::Benchmark;

namespace eloq {
    namespace internals {
        namespace picojpeg {
            uint16_t offset = 0;

            unsigned char consume(unsigned char* dest, unsigned char chunkSize, unsigned char *read, void *data) {
                uint16_t len = camera.frame->len;

                if (!len || offset > len) {
                    ESP_LOGE("JPEG", "Either length is 0 or decoding offset > length");
                    *read = 0;
                    return PJPG_STREAM_READ_ERROR;
                }

                uint16_t n = len - offset > chunkSize ? chunkSize : len - offset;

                memcpy(dest, camera.frame->buf + offset, n);
                *read = n;
                offset += n;

                return 0;
            }
        }
    }
}


namespace Eloquent {
    namespace Esp32cam {
        namespace JPEG {
            /**
             * Decode JPEG into Y, Cb, Cr
             */
            class YCbCrDecoder {
                public:
                    Exception exception;
                    Benchmark benchmark;
                    pjpeg_image_info_t jpeg;
                    struct {
                        uint8_t *pixels = NULL;
                        uint16_t length = 0;
                        uint16_t width = 0;
                        uint16_t height = 0;
                    } y, cb, cr;

                    /**
                     * Constructor
                     */
                    YCbCrDecoder() :
                        exception("YCbCr") {
                        }

                    Exception& decode() {
                        eloq::internals::picojpeg::offset = 0;

                        if (!camera.hasFrame())
                            return exception.set("No frame to decode");

                        const uint16_t width = camera.resolution.getWidth() / 8;
                        const uint16_t height = camera.resolution.getHeight() / 8;
                        const uint16_t length = width * height;

                        if (y.length != length)
                            allocate(width, height, length);

                        benchmark.start();

                        // decode
                        camera.mutex.threadsafe([this]() {
                            int status;

                            if (status = pjpeg_decode_init(&jpeg, eloq::internals::picojpeg::consume, NULL, 1)) {
                                benchmark.stop();
                                exception.set(String("PicoJPEG decode error 0x") + status);
                                return;
                            }

                            consume();
                        });

                        benchmark.stop();

                        if (!exception.isOk())
                            return exception;

                        return exception.clear();

                    }

                    /**
                     * 
                     */
                    template<typename Writer>
                    void toJSON(Writer& writer) {
                        if (!y.length) {
                            ESP_LOGE("YCbCr", "Cannot convert to JSON empty data");
                            return;
                        }

                        writer.print("{\"y\":");
                        toJSON(writer, y.pixels);
                        writer.print(", \"cb\":");
                        toJSON(writer, cb.pixels);
                        writer.print(", \"cr\":");
                        toJSON(writer, cr.pixels);
                        writer.print(", \"width\":");
                        writer.print(y.width);
                        writer.print(", \"height\":");
                        writer.print(y.height);
                        writer.println("}");
                    }

                protected:

                    /**
                     * Allocate required memory
                    */
                    void allocate(const uint16_t width, const uint16_t height, const uint16_t length) {
                        ESP_LOGI("YCbCr", "(Re)Allocating 3x%d bytes for JPEG decoding", length);
                            
                            y.pixels  = (uint8_t*) realloc((void*) y.pixels, length * sizeof(uint16_t));
                            cb.pixels = (uint8_t*) realloc((void*) cb.pixels, length * sizeof(uint16_t));
                            cr.pixels = (uint8_t*) realloc((void*) cr.pixels, length * sizeof(uint16_t));

                            y.width = width;
                            cb.width = width;
                            cr.width = width;

                            y.height = height;
                            cb.height = height;
                            cr.height = height;

                            y.length = length;
                            cb.length = length;
                            cr.length = length;
                    }

                    /**
                     * Consume JPEG decoded data
                     */
                    void consume() {
                        int status;
                        uint16_t i = 0;

                        while ((status = pjpeg_decode_mcu()) != PJPG_NO_MORE_BLOCKS) {
                            switch (jpeg.m_scanType) {
                                case PJPG_GRAYSCALE:
                                    y.pixels[i]  = jpeg.m_pMCUBufR[0];
                                    cb.pixels[i] = jpeg.m_pMCUBufR[0];
                                    cr.pixels[i] = jpeg.m_pMCUBufR[0];
                                    i++;
                                    break;
                                case PJPG_YH1V1:
                                    y.pixels[i]  = jpeg.m_pMCUBufR[0];
                                    cb.pixels[i] = jpeg.m_pMCUBufG[0];
                                    cr.pixels[i] = jpeg.m_pMCUBufB[0];
                                    i++;
                                    break;
                                case PJPG_YH1V2:
                                    y.pixels[i]  = jpeg.m_pMCUBufR[0];
                                    cb.pixels[i] = jpeg.m_pMCUBufG[0];
                                    cr.pixels[i] = jpeg.m_pMCUBufB[0];
                                    i++;
                                    y.pixels[i]  = jpeg.m_pMCUBufR[128];
                                    cb.pixels[i] = jpeg.m_pMCUBufG[128];
                                    cr.pixels[i] = jpeg.m_pMCUBufB[128];
                                    i++;
                                    break;
                                case PJPG_YH2V1:
                                    y.pixels[i]  = jpeg.m_pMCUBufR[0];
                                    cb.pixels[i] = jpeg.m_pMCUBufG[0];
                                    cr.pixels[i] = jpeg.m_pMCUBufB[0];
                                    i++;
                                    y.pixels[i]  = jpeg.m_pMCUBufR[64];
                                    cb.pixels[i] = jpeg.m_pMCUBufG[64];
                                    cr.pixels[i] = jpeg.m_pMCUBufB[64];
                                    i++;
                                    break;
                                case PJPG_YH2V2:
                                    y.pixels[i]  = jpeg.m_pMCUBufR[0];
                                    cb.pixels[i] = jpeg.m_pMCUBufG[0];
                                    cr.pixels[i] = jpeg.m_pMCUBufB[0];
                                    i++;
                                    y.pixels[i]  = jpeg.m_pMCUBufR[64];
                                    cb.pixels[i] = jpeg.m_pMCUBufG[64];
                                    cr.pixels[i] = jpeg.m_pMCUBufB[64];
                                    i++;
                                    y.pixels[i]  = jpeg.m_pMCUBufR[128];
                                    cb.pixels[i] = jpeg.m_pMCUBufG[128];
                                    cr.pixels[i] = jpeg.m_pMCUBufB[128];
                                    i++;
                                    y.pixels[i]  = jpeg.m_pMCUBufR[192];
                                    cb.pixels[i] = jpeg.m_pMCUBufG[192];
                                    cr.pixels[i] = jpeg.m_pMCUBufB[192];
                                    i++;
                                    break;
                            }
                        }

                        if (i != y.length)
                            exception.set(
                                String("JPEG size mismatch while decoding. Exptected")
                                + y.length
                                + " pixels, found "
                                + i
                            );
                    }

                    /**
                     * Convert single pixels array to JSON
                     */
                    template<typename Writer>
                    void toJSON(Writer& writer, uint8_t* pixels) {
                        writer.print("[");
                        writer.print(pixels[0]);

                        for (int i = 1; i < y.length; i++) {
                            writer.print(",");
                            writer.print(pixels[i]);
                        }

                        writer.print("]");
                    }

            };
        }
    }
}

namespace eloq {
    namespace jpeg {
        static Eloquent::Esp32cam::JPEG::YCbCrDecoder ycbcr;
    }
}

#endif