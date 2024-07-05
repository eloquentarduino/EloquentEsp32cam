#ifndef ELOQUENT_ESP32CAM_JPEG_DECODERGRAY
#define ELOQUENT_ESP32CAM_JPEGDECODERGRAY

#include <stdio.h>
#include <stdlib.h>
#include "../camera/Camera.h"
#include "./picojpeg.h"
#include "../extra/exception.h"
#include "../extra/benchmark.h"

using Eloquent::Error::Exception;
using Eloquent::Extra::Benchmark;
using namespace eloq;

struct JpegDecoding {
    uint16_t offset;
};

/**
 *
 * @param dest
 * @param chunkSize
 * @param read
 * @param data
 * @return
 */
unsigned char _pjpegConsumeGray(unsigned char* dest, unsigned char chunkSize, unsigned char *read, void *data) {
    JpegDecoding *decoding = (JpegDecoding*) data;

    if (!camera.hasFrame()) {
        ESP_LOGE("JPEG", "Cannot decode empty frame");
        *read = 0;
        return PJPG_STREAM_READ_ERROR;
    }

    uint16_t len = camera.frame->len;
    uint16_t *offset = &decoding->offset;

    if (!len || (*offset) > len) {
        ESP_LOGE("JPEG", "Either length is 0 or decoding offset > length");
        *read = 0;
        return PJPG_STREAM_READ_ERROR;
    }

    uint16_t n = len - (*offset) > chunkSize ? chunkSize : len - (*offset);

    memcpy(dest, camera.frame->buf + (*offset), n);
    *read = n;
    *offset += n;

    return 0;
}


namespace Eloquent {
    namespace Esp32cam {
        namespace Jpeg {
            /**
             * Decode Jpeg in Y, Cb, Cr components
             */
            class JpegDecoderGray {
            public:
                Exception exception;
                Benchmark benchmark;
                pjpeg_image_info_t jpeg;
                struct {
                    uint8_t *pixels;
                    uint16_t length = 0;
                    uint16_t width = 0;
                    uint16_t height = 0;
                } gray;

                /**
                 * Constructor
                 */
                JpegDecoderGray() :
                    exception("JPEG") {
                        gray.pixels = NULL;
                    }
                
                /**
                 * 
                 * @return 
                 */
                Exception& decode() {
                    int status;
                    uint16_t i = 0;
                    JpegDecoding decoding = {
                        .offset = 0
                    };

                    // skip repeated decoding of the same image
                    if (!camera.hasFrame())
                        return exception.set("Can't decode empty frame");

                    if (gray.pixels == NULL) {
                        ESP_LOGI("JPEG", "Allocating memory for decoding");
                        gray.pixels = (uint8_t*) ps_malloc(camera.resolution.getWidth() / 8 * camera.resolution.getHeight() / 8);
                    }

                    benchmark.start();

                    if (status = pjpeg_decode_init(&jpeg, _pjpegConsumeGray, (void *) &decoding, 1)) {
                        benchmark.stop();
                        return exception.set(String("JPEG decode error: ") + status);
                    }

                    while ((status = pjpeg_decode_mcu()) != PJPG_NO_MORE_BLOCKS) {
                        //if (i > MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64)
                        //    continue;

                        switch (jpeg.m_scanType) {
                            case PJPG_GRAYSCALE:
                                gray.pixels[i++] = jpeg.m_pMCUBufR[0];
                                break;
                            case PJPG_YH1V1:
                                gray.pixels[i++] = jpeg.m_pMCUBufR[0];
                                break;
                            case PJPG_YH1V2:
                                gray.pixels[i++] = jpeg.m_pMCUBufR[0];
                                gray.pixels[i++] = jpeg.m_pMCUBufR[128];
                                break;
                            case PJPG_YH2V1:
                                gray.pixels[i++] = jpeg.m_pMCUBufR[0];
                                gray.pixels[i++] = jpeg.m_pMCUBufR[64];
                                break;
                            case PJPG_YH2V2:
                                gray.pixels[i++] = jpeg.m_pMCUBufR[0];
                                gray.pixels[i++] = jpeg.m_pMCUBufR[64];
                                gray.pixels[i++] = jpeg.m_pMCUBufR[128];
                                gray.pixels[i++] = jpeg.m_pMCUBufR[192];
                                break;
                        }
                    }

                    const uint16_t w = getWidth() / 8;
                    const uint16_t h = getHeight() / 8;

                    gray.length = i;
                    gray.width = w;
                    gray.height = h;
                    benchmark.stop();

                    if (gray.length != getWidth() * getHeight() / 64)
                        return exception.set("Size mismatch");

                    return exception.clear();
                }

                /**
                 * Get image width
                 *
                 * @return
                 */
                inline uint16_t getWidth() {
                    return jpeg.m_width;
                }

                /**
                 * Get image height
                 *
                 * @return
                 */
                inline uint16_t getHeight() {
                    return jpeg.m_height;
                }

                /**
                 * Get pixel at (x, y)
                 * @param x
                 * @param y
                 * @return
                 */
                inline uint8_t at(size_t x, size_t y) {
                    return gray.pixels[y * gray.width + x];
                }

                /**
                 *
                 * @tparam Printer
                 * @param printer
                 * @param format
                 */
                template<typename Printer>
                void printTo(Printer& printer, uint8_t format = DEC) {
                    printer.print(gray.pixels[0], format);

                    for (uint16_t i = 1; i < gray.length; i++) {
                        printer.print(',');
                        printer.print(gray.pixels[i], format);
                    }

                    printer.print('\n');
                }

            protected:
                uint16_t _offset;
            };
        }
    }
}

namespace eloq {
    static Eloquent::Esp32cam::Jpeg::JpegDecoderGray jpeg;
}

#endif //ELOQUENTESP32CAM_JPEGDECODERGRAY_H
