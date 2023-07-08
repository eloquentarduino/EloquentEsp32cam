//
// Created by Simone on 14/11/22.
//

#ifndef ELOQUENTESP32CAM_JPEGDECODERGRAY_H
#define ELOQUENTESP32CAM_JPEGDECODERGRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "./Cam.h"
#include "../libs/picojpeg.h"
#include "../traits/HasErrorMessage.h"
#include "../traits/BenchmarksCode.h"


struct JpegDecoding {
    const Eloquent::Esp32cam::Cam *cam;
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
unsigned char pjpegConsume(unsigned char* dest, unsigned char chunkSize, unsigned char *read, void *data) {
    JpegDecoding *decoding = (JpegDecoding*) data;

    if (decoding->cam->frame == NULL) {
        *read = 0;
        return PJPG_STREAM_READ_ERROR;
    }

    uint16_t len = decoding->cam->frame->len;
    uint16_t *offset = &decoding->offset;

    if (!len || (*offset) > len) {
        *read = 0;
        Serial.println("exit...");
        return PJPG_STREAM_READ_ERROR;
    }

    uint16_t n = len - (*offset) > chunkSize ? chunkSize : len - (*offset);

    memcpy(dest, decoding->cam->frame->buf + (*offset), n);
    *read = n;
    *offset += n;

    return 0;
}


namespace Eloquent {
    namespace Esp32cam {
        /**
         * Decode Jpeg in Y, Cb, Cr components
         */
        class JpegDecoderGray : public HasErrorMessage, public BenchmarksCode {
        public:
            pjpeg_image_info_t jpeg;
            struct {
                uint8_t pixels[MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64] = {0};
                uint16_t length = 0;
                uint16_t width = 0;
                uint16_t height = 0;
            } gray;
            
            /**
             * 
             * @param cam 
             * @return 
             */
            bool decode(const Cam& cam) {
                int status;
                uint16_t i = 0;
                JpegDecoding decoding = {
                    .cam = &cam,
                    .offset = 0
                };

                // skip repeated decoding of the same image
                if (cam.frame != NULL && cam.frame->len == _lastSize)
                    return true;

                startBenchmark();

                if (pjpeg_decode_init(&jpeg, pjpegConsume, (void *) &decoding, 1)) {
                    stopBenchmark();

                    return this->setErrorMessage("Jpeg decode error");
                }

                while ((status = pjpeg_decode_mcu()) != PJPG_NO_MORE_BLOCKS) {
                    if (i > MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64)
                        continue;

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
                _lastSize = cam.frame->len;
                stopBenchmark();

                if (gray.length != getWidth() * getHeight() / 64)
                    return setErrorMessage("Size mismatch");

                return true;
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
            size_t _lastSize = 0;
        };
    }
}

#endif //ELOQUENTESP32CAM_JPEGDECODERGRAY_H
