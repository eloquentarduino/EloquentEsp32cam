//
// Created by Simone on 01/05/23.
//

#ifndef ELOQUENTESP32CAM_COLORJPEG_H
#define ELOQUENTESP32CAM_COLORJPEG_H


#include <stdio.h>
#include <stdlib.h>
#include "../../libs/picojpeg.h"
#include "../../traits/HasErrorMessage.h"
#include "../Cam.h"
#include "../features/Benchmark.h"


size_t pjpegOffset;


/**
 * On-demand JPEG decoding
 *
 * @param dest
 * @param chunkSize
 * @param read
 * @param data
 * @return
 */
unsigned char pjpegConsume(unsigned char* dest, unsigned char chunkSize, unsigned char *read, void *data) {
    if (!camera.frame || !camera.frame->len) {
        *read = 0;
        return PJPG_STREAM_READ_ERROR;
    }

    size_t length = camera.frame->len;

    if (!length || pjpegOffset >= length) {
        *read = 0;
        return PJPG_STREAM_READ_ERROR;
    }

    uint16_t n = length - pjpegOffset > chunkSize ? chunkSize : length - pjpegOffset;

    memcpy(dest, camera.frame->buf + pjpegOffset, n);
    *read = n;
    pjpegOffset += n;

    return 0;
}


namespace Eloquent {
    namespace Esp32cam {
        namespace Components {
            /**
             * Decode JPEG frame into YCbCr components
             */
            class ColorJpeg : public HasErrorMessage {
            public:

                pjpeg_image_info_t image;

                struct {
                    uint8_t *pixels;
                    size_t allocated;
                    size_t length;
                    size_t width;
                    size_t height;
                } gray, blue, red;
                Features::Benchmark benchmark;

                /**
                 *
                 */
                ColorJpeg() : lastDecodedAt(0) {
                    gray.allocated = 0;
                    gray.length = 0;
                    gray.width = 0;
                    gray.height = 0;
                    blue.allocated = 0;
                    blue.length = 0;
                    blue.width = 0;
                    blue.height = 0;
                    red.allocated = 0;
                    red.length = 0;
                    red.width = 0;
                    red.height = 0;

                    gray.pixels = NULL;
                    blue.pixels = NULL;
                    red.pixels = NULL;
                }

                /**
                 * Get image width
                 *
                 * @return
                 */
                inline size_t getWidth() {
                    return image.m_width;
                }

                /**
                 * Get image height
                 *
                 * @return
                 */
                inline size_t getHeight() {
                    return image.m_height;
                }

                /**
                 * Get decoded image width
                 *
                 * @return
                 */
                inline size_t getDecodedWidth() {
                    return image.m_width / 8;
                }

                /**
                 * Get decoded image height
                 *
                 * @return
                 */
                inline size_t getDecodedHeight() {
                    return image.m_height / 8;
                }


                /**
                 * Decode jpeg
                 * @return
                 */
                bool decode() {
                    int status;

                    if (!camera.frame || !camera.frame->len)
                        return setErrorMessage("No frame to decode", "ColorJpeg");

                    size_t requiredBytes = camera.resolution.getWidth() * camera.resolution.getHeight() / 64;

                    if (gray.allocated != requiredBytes) {
                        ESP_LOGD("ColorJpeg", "Allocating %lu x 3 bytes for JPEG decoding", (unsigned long) requiredBytes);
                        free(gray.pixels);

                        // try to reduce memory fragmentation
                        gray.pixels = (uint8_t *) malloc(requiredBytes * 3);
                        blue.pixels = gray.pixels + requiredBytes;
                        red.pixels = blue.pixels + requiredBytes;
                        gray.allocated = requiredBytes;
                        blue.allocated = requiredBytes;
                        red.allocated = requiredBytes;
                    }

                    // reset offset at every decoding
                    pjpegOffset = 0;
                    benchmark.start();

                    if ((status = pjpeg_decode_init(&image, pjpegConsume, NULL, 1))) {
                        benchmark.stop();

                        return setErrorMessage(String("JPEG decode error (") + status + ")", "ColorJpeg");
                    }

                    const size_t length = consume();
                    const uint16_t width = getWidth() / 8;
                    const uint16_t height = getHeight() / 8;

                    if (getWidth() != camera.resolution.getWidth())
                        return setErrorMessage("Image size mismatch", "ColorJpeg");

                    if (getHeight() != camera.resolution.getHeight())
                        return setErrorMessage("Image size mismatch", "ColorJpeg");

                    gray.length = length;
                    blue.length = length;
                    red.length = length;

                    gray.width = width;
                    blue.width = width;
                    red.width = width;

                    gray.height = height;
                    blue.height = height;
                    red.height = height;

                    benchmark.stop();
                    lastDecodedAt = millis();

                    return clearError();
                }

                /**
                 * Test if decoding was successful
                 *
                 * @return
                 */
                inline bool decoded() {
                    return isOk() && (millis() - lastDecodedAt) < 10;
                }

            protected:
                size_t lastDecodedAt;

                /**
                 * Consume JPEG bytes
                 */
                size_t consume() {
                    int status;
                    size_t i = 0;

                    while ((status = pjpeg_decode_mcu()) != PJPG_NO_MORE_BLOCKS) {
                        switch (image.m_scanType) {
                            case PJPG_GRAYSCALE:
                                gray.pixels[i] = image.m_pMCUBufR[0];
                                blue.pixels[i] = 0;
                                red.pixels[i] = 0;
                                i++;
                                break;
                            case PJPG_YH1V1:
                                gray.pixels[i] = image.m_pMCUBufR[0];
                                blue.pixels[i] = image.m_pMCUBufG[0];
                                red.pixels[i] = image.m_pMCUBufB[0];
                                i++;
                                break;
                            case PJPG_YH1V2:
                                gray.pixels[i] = image.m_pMCUBufR[0];
                                blue.pixels[i] = image.m_pMCUBufG[0];
                                red.pixels[i] = image.m_pMCUBufB[0];
                                i++;
                                gray.pixels[i] = image.m_pMCUBufR[128];
                                blue.pixels[i] = image.m_pMCUBufG[128];
                                red.pixels[i] = image.m_pMCUBufB[128];
                                i++;
                                break;
                            case PJPG_YH2V1:
                                gray.pixels[i] = image.m_pMCUBufR[0];
                                blue.pixels[i] = image.m_pMCUBufG[0];
                                red.pixels[i] = image.m_pMCUBufB[0];
                                i++;
                                gray.pixels[i] = image.m_pMCUBufR[64];
                                blue.pixels[i] = image.m_pMCUBufG[64];
                                red.pixels[i] = image.m_pMCUBufB[64];
                                i++;
                                break;
                            case PJPG_YH2V2:
                                gray.pixels[i] = image.m_pMCUBufR[0];
                                blue.pixels[i] = image.m_pMCUBufG[0];
                                red.pixels[i] = image.m_pMCUBufB[0];
                                i++;
                                gray.pixels[i] = image.m_pMCUBufR[64];
                                blue.pixels[i] = image.m_pMCUBufG[64];
                                red.pixels[i] = image.m_pMCUBufB[64];
                                i++;
                                gray.pixels[i] = image.m_pMCUBufR[128];
                                blue.pixels[i] = image.m_pMCUBufG[128];
                                red.pixels[i] = image.m_pMCUBufB[128];
                                i++;
                                gray.pixels[i] = image.m_pMCUBufR[192];
                                blue.pixels[i] = image.m_pMCUBufG[192];
                                red.pixels[i] = image.m_pMCUBufB[192];
                                i++;
                                break;
                        }
                    }

                    return i;
                }
            };
        }
    }
}


static Eloquent::Esp32cam::Components::ColorJpeg jpeg;

#endif //ELOQUENTESP32CAM_COLORJPEG_H
