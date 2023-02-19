//
// Created by Simone on 14/11/22.
//

#ifndef ELOQUENTESP32CAM_JPEGDECODER_H
#define ELOQUENTESP32CAM_JPEGDECODER_H

#include <stdio.h>
#include <stdlib.h>
#include "./Cam.h"
#include "../libs/picojpeg.h"
#include "../traits/HasErrorMessage.h"
#include "../traits/BenchmarksCode.h"
#include "./transforms/pixel/PixelMapper.h"


struct JpegDecoding {
    Eloquent::Esp32cam::Cam *cam;
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
unsigned char pjegConsume(unsigned char* dest, unsigned char chunkSize, unsigned char *read, void *data) {
    JpegDecoding *decoding = (JpegDecoding*) data;

    if (decoding->cam->frame == NULL) {
        *read = 0;
        return PJPG_STREAM_READ_ERROR;
    }

    uint16_t len = decoding->cam->frame->len;
    uint16_t *offset = &decoding->offset;

    if (!len || (*offset) > len) {
        *read = 0;
        return PJPG_STREAM_READ_ERROR;
    }

    uint16_t n = len - (*offset) > chunkSize ? chunkSize : len - (*offset);

    memcpy(dest, decoding->cam->frame->buf + (*offset), n);
    *read = n;
    *offset += chunkSize;

    return 0;
}


namespace Eloquent {
    namespace Esp32cam {
        /**
         * Decode Jpeg in Y, Cb, Cr components
         */
        class JpegDecoder : public HasErrorMessage, public BenchmarksCode {
        public:
            pjpeg_image_info_t jpeg;
            struct {
                uint8_t pixels[MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64] = {0};
                uint16_t length = 0;
                uint16_t width = 0;
                uint16_t height = 0;
            } luma, cb, cr;
            
            /**
             * 
             * @param cam 
             * @return 
             */
            bool decode(Cam& cam) {
                int status;
                uint16_t i = 0;
                JpegDecoding decoding = {
                        .cam = &cam,
                        .offset = 0
                };

                startBenchmark();

                if (pjpeg_decode_init(&jpeg, pjegConsume, (void *) &decoding, 1)) {
                    stopBenchmark();

                    return this->setErrorMessage("Jpeg decode error");
                }

                while ((status = pjpeg_decode_mcu()) != PJPG_NO_MORE_BLOCKS) {
                    if (i > MAX_RESOLUTION_WIDTH * MAX_RESOLUTION_HEIGHT / 64)
                        continue;

                    switch (jpeg.m_scanType) {
                        case PJPG_GRAYSCALE:
                            luma.pixels[i] = jpeg.m_pMCUBufR[0];
                            cb.pixels[i] = 0;
                            cr.pixels[i++] = 0;
                            break;
                        case PJPG_YH1V1:
                            luma.pixels[i] = jpeg.m_pMCUBufR[0];
                            cb.pixels[i] = jpeg.m_pMCUBufG[0];
                            cr.pixels[i++] = jpeg.m_pMCUBufB[0];
                            break;
                        case PJPG_YH1V2:
                            luma.pixels[i] = jpeg.m_pMCUBufR[0];
                            cb.pixels[i] = jpeg.m_pMCUBufG[0];
                            cr.pixels[i++] = jpeg.m_pMCUBufB[0];
                            luma.pixels[i] = jpeg.m_pMCUBufR[128];
                            cb.pixels[i] = jpeg.m_pMCUBufG[128];
                            cr.pixels[i++] = jpeg.m_pMCUBufB[128];
                            break;
                        case PJPG_YH2V1:
                            luma.pixels[i] = jpeg.m_pMCUBufR[0];
                            cb.pixels[i] = jpeg.m_pMCUBufG[0];
                            cr.pixels[i++] = jpeg.m_pMCUBufB[0];
                            luma.pixels[i] = jpeg.m_pMCUBufR[64];
                            cb.pixels[i] = jpeg.m_pMCUBufG[64];
                            cr.pixels[i++] = jpeg.m_pMCUBufB[64];
                            break;
                        case PJPG_YH2V2:
                            luma.pixels[i] = jpeg.m_pMCUBufR[0];
                            cb.pixels[i] = jpeg.m_pMCUBufG[0];
                            cr.pixels[i++] = jpeg.m_pMCUBufB[0];
                            luma.pixels[i] = jpeg.m_pMCUBufR[64];
                            cb.pixels[i] = jpeg.m_pMCUBufG[64];
                            cr.pixels[i++] = jpeg.m_pMCUBufB[64];
                            luma.pixels[i] = jpeg.m_pMCUBufR[128];
                            cb.pixels[i] = jpeg.m_pMCUBufG[128];
                            cr.pixels[i++] = jpeg.m_pMCUBufB[128];
                            luma.pixels[i] = jpeg.m_pMCUBufR[192];
                            cb.pixels[i] = jpeg.m_pMCUBufG[192];
                            cr.pixels[i++] = jpeg.m_pMCUBufB[192];
                            break;
                    }
                }

                const uint16_t w = getWidth() / 8;
                const uint16_t h = getHeight() / 8;

                luma.length = i;
                luma.width = w;
                luma.height = h;
                cb.length = i;
                cb.width = w;
                cb.height = h;
                cr.length = i;
                cr.width = w;
                cr.height = h;
                stopBenchmark();

                if (luma.length != getWidth() * getHeight() / 64)
                    return setErrorMessage("Size mismatch");

                mapPixels(0, luma.pixels, luma.length);
                mapPixels(1, cb.pixels, cb.length);
                mapPixels(2, cr.pixels, cr.length);

                return true;
            }

            /**
             *
             * @param f
             */
            void transformPixels(Transforms::Pixel::PixelMapper *f) {
                if (pixelTransforms == NULL)
                    pixelTransforms = f;
                else
                    pixelTransforms->chain(f);
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

        protected:
            uint16_t _offset;
            Transforms::Pixel::PixelMapper *pixelTransforms = NULL;

            /**
             * Apply a function to each pixel
             *
             * @param mapper
             */
            void mapPixels(uint8_t channel, uint8_t *pixels, size_t length) {
                Transforms::Pixel::PixelMapper *mapper = pixelTransforms;

                while (mapper != NULL) {
                    for (size_t i = 0; i < length; i++)
                        pixels[i] = mapper->transform(channel, i, pixels[i]);

                    mapper = mapper->next;
                }
            }
        };
    }
}

#endif //ELOQUENTESP32CAM_JPEGDECODER_H
